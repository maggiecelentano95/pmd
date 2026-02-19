#ifndef GUARD_GROUND_MAP_CONVERSION_TABLE_H
#define GUARD_GROUND_MAP_CONVERSION_TABLE_H

#include "constants/ground_map.h"

// size: 0xC
typedef struct GroundConversionStruct
{
    /*5,6,7,8,9 uses one background init path (gUnknown_811733C) 
    0,11 uses dungeon-style mapping (GroundMap_SelectDungeon) 
    (like 1,2,3,4) use the default background path (gUnknown_8117324)
    prite/shadow behavior toggles based on this too: only unk0==5 or unk0==8 return false in sub_80A4D48
    Practical takeaway:
    1/2 are the common “normal map” values.
    9 is often used for special scene/cutscene-style maps.
    10/11 are dungeon-transition style map types.*/
    s16 unk0;
    s16 groundPlaceId;
    s16 mapFileTableId;
    s16 unk6;
    /* 0x8 */ const u8 *text;
} GroundConversionStruct;

extern const GroundConversionStruct gGroundMapConversionTable[MAP_COUNT];

#endif // GUARD_GROUND_MAP_CONVERSION_TABLE_H
