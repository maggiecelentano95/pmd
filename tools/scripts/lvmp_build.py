#!/usr/bin/env python3
import argparse
import json
import os
import struct
from collections import defaultdict

MAX_LEVELS = 100
RECORD_SIZE = 12
MAX_AT4P_SIZE = 0xFFFF
# Worst-case compressed size is roughly raw + raw/8 + header.
MAX_RAW_CHUNK = 8184


def _find_best_match(data, pos, candidates):
    best_len = 0
    best_dist = 0
    n = len(data)
    for cand in reversed(candidates):
        dist = pos - cand
        if dist <= 0 or dist > 0x1000:
            continue
        max_len = min(18, n - pos)
        if max_len <= best_len:
            continue
        cur_len = 0
        while cur_len < max_len and data[cand + cur_len] == data[pos + cur_len]:
            cur_len += 1
        if cur_len > best_len:
            best_len = cur_len
            best_dist = dist
            if best_len == 18:
                break
    return best_len, best_dist


def _lzss_tokens(data):
    n = len(data)
    pos_lists = defaultdict(list)
    tokens = []

    def add_pos(p):
        if p + 2 >= n:
            return
        key = data[p:p + 3]
        lst = pos_lists[key]
        lst.append(p)
        if len(lst) > 64:
            del lst[0]

    i = 0
    while i < n:
        if i + 2 < n:
            key = data[i:i + 3]
            candidates = pos_lists.get(key, [])
            best_len, best_dist = _find_best_match(data, i, candidates)
        else:
            best_len, best_dist = 0, 0

        if best_len >= 3:
            tokens.append(("m", best_len, best_dist))
            for k in range(best_len):
                add_pos(i + k)
            i += best_len
        else:
            tokens.append(("l", data[i]))
            add_pos(i)
            i += 1

    return tokens


def _encode_tokens(tokens):
    out = bytearray()
    i = 0
    while i < len(tokens):
        chunk = tokens[i:i + 8]
        flag = 0
        for tok in chunk:
            flag = (flag << 1) | (1 if tok[0] == "l" else 0)
        flag <<= (8 - len(chunk))
        out.append(flag & 0xFF)
        for tok in chunk:
            if tok[0] == "l":
                out.append(tok[1])
            else:
                length, dist = tok[1], tok[2]
                offset = 0x1000 - dist
                if length < 3 or length > 18:
                    raise ValueError(f"Invalid match length {length}")
                if offset < 0 or offset > 0xFFF:
                    raise ValueError(f"Invalid offset {offset:#x}")
                out.append(((length - 3) << 4) | ((offset >> 8) & 0xF))
                out.append(offset & 0xFF)
        i += 8
    return bytes(out)


def compress_at4p(raw):
    tokens = _lzss_tokens(raw)
    payload = _encode_tokens(tokens)

    header = bytearray()
    header += b"AT4P"
    header += b"X"  # compressed mode (anything other than 'N')
    header += b"\x00\x00"  # length placeholder
    header += bytes([0xFF] * 9)  # flags; avoid special-case remaps
    header += struct.pack("<H", len(raw) & 0xFFFF)

    data = header + payload
    total_len = len(data)
    if total_len > MAX_AT4P_SIZE:
        raise ValueError(f"AT4P file too large: {total_len} bytes")
    data[5] = total_len & 0xFF
    data[6] = (total_len >> 8) & 0xFF
    return bytes(data)


def build_raw_lvmp(entries, count):
    by_id = {e["id"]: e for e in entries}
    missing = [i for i in range(1, count + 1) if i not in by_id]
    if missing:
        raise ValueError(f"Missing lvmp entries for ids: {missing[:10]}...")

    out = bytearray()
    for i in range(1, count + 1):
        entry = by_id[i]
        levels = entry.get("levels", [])
        if len(levels) != MAX_LEVELS:
            raise ValueError(f"Entry {entry.get('name')} has {len(levels)} levels")
        for lv in levels:
            exp_required = int(lv["expRequired"])
            gain_hp = int(lv["gainHP"])
            gain_atk = int(lv["gainAtk"])
            gain_sp_atk = int(lv["gainSpAtk"])
            gain_def = int(lv["gainDef"])
            gain_sp_def = int(lv["gainSpDef"])
            fill_a = int(lv.get("fillA", 0))
            out += struct.pack(
                "<iHBBBBH",
                exp_required,
                gain_hp,
                gain_atk,
                gain_sp_atk,
                gain_def,
                gain_sp_def,
                fill_a,
            )
    return bytes(out)


def chunk_and_compress(raw, out_dir):
    os.makedirs(out_dir, exist_ok=True)
    chunks = []
    offset = 0
    idx = 0
    total = len(raw)

    while offset < total:
        chunk_size = min(MAX_RAW_CHUNK, total - offset)
        while chunk_size > 0:
            chunk_raw = raw[offset:offset + chunk_size]
            at4p = compress_at4p(chunk_raw)
            if len(at4p) <= MAX_AT4P_SIZE:
                break
            chunk_size -= 1024
        if chunk_size <= 0:
            raise RuntimeError("Failed to fit chunk into AT4P size limit")

        name = f"chunk_{idx:03d}.at4p"
        path = os.path.join(out_dir, name)
        with open(path, "wb") as f:
            f.write(at4p)
        chunks.append(
            {
                "index": idx,
                "offset": offset,
                "size": chunk_size,
                "path": path.replace("\\", "/"),
                "at4p_size": len(at4p),
            }
        )
        offset += chunk_size
        idx += 1

    return chunks


def write_inc(out_inc, chunks, total_entries):
    total_size = total_entries * MAX_LEVELS * RECORD_SIZE
    lines = []
    lines.append("@ auto-generated by lvmp_build.py")
    lines.append(".global gLvmapDataHeader")
    lines.append("gLvmapDataHeader:")
    lines.append(".byte 0x4c, 0x56, 0x4d, 0x50 @ 'LVMP'")
    lines.append(f".4byte {total_entries}")
    lines.append(f".4byte {MAX_LEVELS}")
    lines.append(f".4byte {RECORD_SIZE}")
    lines.append(f".4byte {total_size}")
    lines.append(f".4byte {len(chunks)}")

    for c in chunks:
        lines.append(f".4byte {c['offset']}")
        lines.append(f".4byte {c['size']}")
        lines.append(f".4byte {c['at4p_size']}")
        lines.append(f".4byte LvmapChunk_{c['index']:03d}")

    lines.append(".align 2,0")
    for c in chunks:
        lines.append(f"LvmapChunk_{c['index']:03d}:")
        lines.append(f".incbin \"{c['path']}\"")

    os.makedirs(os.path.dirname(out_inc), exist_ok=True)
    with open(out_inc, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json", required=True, help="lvmp.json source of truth")
    ap.add_argument("--out-inc", required=True, help="Output .inc path for lvmap_data")
    ap.add_argument("--out-dir", required=True, help="Output directory for chunked AT4P files")
    args = ap.parse_args()

    with open(args.json, "r", encoding="utf-8") as f:
        data = json.load(f)

    entries = data.get("entries", [])
    count = int(data.get("count", len(entries)))
    raw = build_raw_lvmp(entries, count)
    chunks = chunk_and_compress(raw, args.out_dir)
    write_inc(args.out_inc, chunks, count)
    print(f"Wrote {len(chunks)} chunks and {args.out_inc}")


if __name__ == "__main__":
    main()
