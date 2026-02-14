# Pokémon (monster) data

MONSTER_DIR = $(DATA_ASM_SUBDIR)/monster
MONSTER_DATA = $(MONSTER_DIR)/monster_data.inc

# Headers included by tools/dungeonjson/dungeonjson.cpp:generate_monster_data_text
MONSTER_DATA_INC = \
	include/constants/ability.h \
	include/constants/evolve_type.h \
	include/constants/friend_area.h \
	include/constants/item.h \
	include/constants/monster.h \
	include/constants/type.h \
	include/constants/walkable_tile.h

data_monster: $(MONSTER_DATA);

$(MONSTER_DATA): $(MONSTER_DIR)/monster_data.json $(MONSTER_DATA_INC)
	$(DUNGEONJSON) monster pmd-red $<


LEARNSET_DIR = $(MONSTER_DIR)/learnset
LEARNSET_DATA = $(LEARNSET_DIR)/learnset_data.inc
LEARNSET_PTRS = $(LEARNSET_DIR)/learnset_ptrs.s

data_learnset: $(LEARNSET_DATA);

$(LEARNSET_DATA): $(LEARNSET_DIR)/learnset_data.json
	$(DUNGEONJSON) learnset pmd-red $<

data_learnset_ptrs: ${LEARNSET_DATA};
	echo '.4byte 0' > $(LEARNSET_PTRS)
	echo '.4byte 0' >> $(LEARNSET_PTRS)
	cat $(LEARNSET_DATA) | grep .global >> $(LEARNSET_PTRS)
ifeq ($(shell uname -s), Darwin)
	sed -i '' 's/global/4byte/g' $(LEARNSET_PTRS)
else
	sed -i 's/global/4byte/g' $(LEARNSET_PTRS)
endif

LVMP_DIR = $(MONSTER_DIR)/lvmp
LVMP_JSON_SRC ?= $(LVMP_DIR)/lvmp.json
LVMP_JSON = $(LVMP_DIR)/lvmp.json
LVMAP_DATA_DIR = $(MONSTER_DIR)/lvmap_data
LVMAP_DATA_INC = $(MONSTER_DIR)/lvmap_data.inc

data_lvmp: $(LVMAP_DATA_INC);

$(LVMP_JSON):
	@mkdir -p $(LVMP_DIR)
	@if [ "$(LVMP_JSON_SRC)" != "$(LVMP_JSON)" ]; then cp $(LVMP_JSON_SRC) $(LVMP_JSON); fi

$(LVMAP_DATA_INC): $(LVMP_JSON) tools/scripts/lvmp_build.py
	python3 tools/scripts/lvmp_build.py --json $(LVMP_JSON) --out-inc $(LVMAP_DATA_INC) --out-dir $(LVMAP_DATA_DIR)
