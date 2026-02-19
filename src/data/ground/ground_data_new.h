
static const struct ScriptCommand group0_sector0_station[] = 
{
    DEBUGINFO_O(10),
    SELECT_MAP(MAP_NEW),
    BGM_STOP,
    JUMP_SCRIPT(COMMON_ENTER),
};

static const ScriptRef group0_sector0_station_ref = { ENTER_CONTROL, SCRIPT_TYPE_01, NULL, group0_sector0_station };

static const struct ScriptCommand group1_sector0_station[] = 
{
    DEBUGINFO_O(37),
    { 0x3b, 0x39,  0x0001,  0x00000000,  0x00000000, NULL },
    BGM_STOP,
    { 0x23, 0x01,  0x0000,  0x00000000,  0x00000000, NULL },
    { 0x26, 0x01,  0x0000,  0x00000000,  0x00000000, NULL },
    SELECT_MAP(MAP_NEW),
    WAIT(100),
    MSG_ON_BG_AUTO(30, _("{CENTER_ALIGN}Grookey, I have to go.")),
    MSG_ON_BG_AUTO(30, _("{CENTER_ALIGN}I gotta go get milk")),
    MSG_ON_BG_AUTO(30, _("{CENTER_ALIGN}Please, don't come looking for me...")),
    TEXTBOX_CLEAR,
    WAIT(1),
    { 0x25, 0x01,  0x0000,  0x00000000,  0x00000000, NULL },
    MSG_NPC(-1, _(" ...")),
    MSG_NPC(-1, _(" ......")),
    MSG_NPC(-1, _(" ............")),
    MSG_VAR(2, PARTNER_TALK_KIND, -1),
    VARIANT_DEFAULT(_(" ...Excuse me.")),
    VARIANT_DEFAULT(_(" ...Please, wake up.{WAIT_PRESS}\nCome on, wake up.")),
    TEXTBOX_CLEAR,
    SELECT_ENTITIES(-1, 0),//this group, sector 0
    BGM_FADEIN(60, 101),
    { 0x22, 0x01,  0x003c,  0x00000000,  0x00000000, NULL },
    { 0x3b, 0x39,  0x0004,  0x00000000,  0x00000000, NULL },
    AWAIT_CUE(3),
    BGM_FADEOUT(30),
    NEXT_DUNGEON(30, SCRIPT_DUNGEON_TINY_WOODS),
    RET,
};

static const ScriptRef group1_sector0_station_ref = { EVENT_CONTROL, SCRIPT_TYPE_07, NULL, group1_sector0_station };



/*typedef struct GroundLivesData {
    u8 kind;
    u8 unk1;
    u8 width;
    u8 height;
    CompactPos pos;
    const ScriptCommand *scripts[4];
} GroundLivesData;*/


static const struct ScriptCommand group1_sector0_lives0_dlg[] = 
{
    //Player

    //---1---
    DEBUGINFO_O(76),
    UPDATE_VARINT(CALC_SET, MAP_LOCAL, -1), //set to nothing
    //ALERT_CUE(6),
    //AWAIT_CUE(5),
    CALL_SCRIPT(INIT_SLEEP_FUNC),
    { 0x2d, 0x01,  0x0000,  0x00000000,  0x00000000, NULL },
    { 0xdf, 0x00,  0x0000,  0x00000000,  0x00000000, NULL },
    PORTRAIT(PLACEMENT_LEFT_BOTTOM_2, 0x0000, 0x00000000),
    PORTRAIT(PLACEMENT_RIGHT_BOTTOM_FLIP, 0x0001, 0x00000000),
    CALL_SCRIPT(WAKEUP_FUNC),
    UPDATE_VARINT(CALC_SET, MAP_LOCAL, 1), //run LABEL 1 in partner
    ALERT_CUE(6), //jump to parter live
    AWAIT_CUE(5),
    //-------

    //---2---
    MSG_VAR(2, PARTNER_TALK_KIND, 1),
    VARIANT_DEFAULT(_(" You're finally awake! Great!")),
    TEXTBOX_CLEAR,
    WAIT(30),
    MSG_QUIET(0, _("(Wh-where is this...?)")),
    TEXTBOX_CLEAR,
    FANFARE_PLAY2(468),
    ROTATE_TO(4, DIR_TRANS_SPINLEFT1, DIRECTION_EAST),
    WAIT(20),
    FANFARE_PLAY2(468),
    ROTATE_TO(4, DIR_TRANS_SPINRIGHT1, DIRECTION_WEST),
    WAIT(20),
    FANFARE_PLAY2(468),
    ROTATE_TO(4, DIR_TRANS_SPINLEFT1, DIRECTION_EAST),
    WAIT(50),

    GET_RELATIONSHIP,
    MSG_VAR(2, RELATIONSHIP_STATUS, 1),
    VARIANT(/* == */  10, _(" MWAHAHA")),
    VARIANT(/* == */  3, _(" I'm {NAME_1}.\nHappy to meet you!")),
    VARIANT_DEFAULT(_(" I'm {NAME_1}.\nGlad to meet you!")),
    VARIANT_DEFAULT(_(" You were passed out here,\nhardly breathing!")),
    VARIANT_DEFAULT(_(" I'm glad to see you're not dead")),

    UPDATE_VARINT(CALC_ADD, RELATIONSHIP_STATUS, 2), 
    MSG_VAR(2, RELATIONSHIP_STATUS, 1),
    VARIANT(/* == */  11, _(" I like pancakes.")),
    VARIANT(/* == */  12, _(" I like hoagies.")),
    VARIANT_DEFAULT(_(" Ain't that neat?")),

    UPDATE_VARINT(CALC_SET, MAP_LOCAL, 2), //run LABEL 2 in partner
    ALERT_CUE(6),
    AWAIT_CUE(5),
    //-------

    //---3---
    PORTRAIT(PLACEMENT_RIGHT_BOTTOM_FLIP, 0x0001, 0x00000001),
    MSG_VAR(2, PARTNER_TALK_KIND, 1),
    VARIANT_DEFAULT(_(" I'm {NAME_1}.\nGlad to meet you!")),
    TEXTBOX_CLEAR,
    WAIT(30),
    PORTRAIT_REP(0x0001, 0x00000000),
    MSG_VAR(2, PARTNER_TALK_KIND, 1),
    ASK3_VAR( TRUE, /*default*/ -1, /* speaker */ 1, PARTNER_TALK_KIND),
    VARIANT_DEFAULT(_(" ...What're you doing here?")),
    CHOICE(/* label */  1, _("None of your business.")),
    CHOICE(/* label */  2, _("I can't remember.")),

    //---4.1---
  LABEL(1), 
    //Rude response
    TEXTBOX_CLEAR,
    UPDATE_VARINT(CALC_SET, MAP_LOCAL, 3), //Run Label 3 in partner
    ALERT_CUE(6), 
    AWAIT_CUE(5),
    PORTRAIT_REP(0x0001, 0x0000000c),
    MSG_VAR(2, PARTNER_TALK_KIND, 1),
    VARIANT_DEFAULT(_("Rude...")),
    PORTRAIT_REP(0x0001, 0x00000005),
    MSG_VAR(2, PARTNER_TALK_KIND, 1),
    VARIANT_DEFAULT(_(" I was just trying to be\nnice.")),
    TEXTBOX_CLEAR,
    WAIT(30),
    JUMP_LABEL(3),

    //---4.2---
  LABEL(2),
    TEXTBOX_CLEAR,
    UPDATE_VARINT(CALC_SET, MAP_LOCAL, 4), //Run Label 4 in partner
    ALERT_CUE(6), 
    AWAIT_CUE(5),
    PORTRAIT_REP(0x0001, 0x00000001),
    MSG_VAR(2, PARTNER_TALK_KIND, 1),
    VARIANT_DEFAULT(_("Can't remember!")),
    TEXTBOX_CLEAR,
    WAIT(30),
    JUMP_LABEL(3),
  LABEL(3),
    TEXTBOX_CLEAR,
    WAIT(4),
    PORTRAIT_REP(0x0001, 0x00000001),
    MSG_VAR(2, PARTNER_TALK_KIND, 1),
    VARIANT_DEFAULT(_("Well, I want to help.")),
    TEXTBOX_CLEAR,
    WAIT(30),
    TEXTBOX_CLEAR,
    WAIT(30),
    ALERT_CUE(3),
    HALT,
};

static const struct ScriptCommand group1_sector0_lives1_dlg[] = 
{
    //Partner LIVE
   LABEL(0),
    DEBUGINFO_O(273),
    SELECT_ANIMATION(2),
    //Update how portrait (textbox)? slot 1 is identified in dialogue UI.
    { 0x2d, 0x01,  0x0001,  0x00000000,  0x00000000, NULL },
    JUMP_LABEL(99),

    // MAP_LOCAL step IDs:
    // 1 = wake notice
    // 2 = partner intro mode
    // 3 = rude reaction
    // 4 = no-memory reaction
   LABEL(99), // wait for next handoff from lives0
    AWAIT_CUE(6),
    JUMPIF_EQUAL(MAP_LOCAL, 1, 1),
    JUMPIF_EQUAL(MAP_LOCAL, 2, 2),
    JUMPIF_EQUAL(MAP_LOCAL, 3, 3),
    JUMPIF_EQUAL(MAP_LOCAL, 4, 4),
    JUMPIF_EQUAL(MAP_LOCAL, 5, 5),
    JUMP_LABEL(99), // unknown step -> keep waiting



   LABEL(1),
    FANFARE_PLAY2(465),
    CALL_SCRIPT(NOTICE_FUNC),//notice animation/reaction
    WAIT(1),
    FANFARE_STOP2(465),
    ALERT_CUE(5),
    JUMP_LABEL(99),

   LABEL(2),
   //think this puts partner's name in textbox, as we know his name now
    { 0x2d, 0x07,  0x0001,  0x00000000,  0x00000000, NULL },
    ALERT_CUE(5),
    JUMP_LABEL(99),

   LABEL(3),
    SELECT_ANIMATION(2),
    ALERT_CUE(5),
    JUMP_LABEL(99),

   LABEL(4),
   ROTATE_TO(4, DIR_TRANS_SPINLEFT1, DIRECTION_EAST),
    WAIT(5),
    FANFARE_PLAY2(468),
    ROTATE_TO(4, DIR_TRANS_SPINRIGHT1, DIRECTION_WEST),
    WAIT(5),
    FANFARE_PLAY2(468),
    ROTATE_TO(4, DIR_TRANS_SPINLEFT1, DIRECTION_EAST),
    SELECT_ANIMATION(3),
    ALERT_CUE(5),
    JUMP_LABEL(99),

   LABEL(5),
    HALT,
};

static const struct GroundLivesData group1_sector0_lives[] = 
{ 
    /*  0 */ {   0,   0,   0,   0, {  25,  24, 0, CPOS_HALFTILE }, {
        [0] = group1_sector0_lives0_dlg,
    } },
    /*  1 */ {  34,   6,   0,   0, {  29,  24, 0, CPOS_HALFTILE }, {
        [0] = group1_sector0_lives1_dlg,
    } },
};


static const ScriptRef * const (sScripts[]) = 
{
    &group0_sector0_station_ref,
    &group1_sector0_station_ref
};

/*struct GroundScriptSector {
    u32 nLives;
    const GroundLivesData *lives;
    u32 nObjects;
    const GroundObjectData *objects;
    u32 nEffects;
    const GroundEffectData *effects;
    u32 nEvents;
    const GroundEventData *events;
    u32 hasStation;
    const ScriptRef * const *station;
};*/
static const struct GroundScriptSector group0_sectors[] = 
{ 
    { 0,NULL, 0,NULL, 0,NULL, 0,NULL, 1,&sScripts[0], },
};

static const struct GroundScriptSector group1_sectors[] = 
{
    { LPARRAY(group1_sector0_lives), 0,NULL, 0, NULL, 0,NULL, 1,&sScripts[1], },
};


static const struct GroundScriptGroup s_groups[] = 
{
    { LPARRAY(group0_sectors) },
    { LPARRAY(group1_sectors) },
};


static const struct GroundLink s_links[] = 
{
    {},
};

const GroundScriptHeader gGroundScript_new = { LPARRAY(s_groups), s_links };