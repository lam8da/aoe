#ifndef AOE_LANGX_H
#define AOE_LANGX_H

#define STR_HELP 1003
#define STR_LOAD_GAME 1101
#define STR_INIT_SCENARIO 1102
#define STR_LOAD_SCENARIO 1103
#define STR_WAIT_PLAYERS 1108
#define STR_READY 1200
#define STR_COMPUTER 1201
#define STR_EMPTY 1202
#define STR_LOAD 1203
#define STR_WAIT 1204
#define STR_UNKNOWN 1205
#define STR_SAVE_SCENARIO 1206
#define STR_COMM_PLAYER_IDLE 1210
#define STR_ERROR 2001
#define STR_ERR_INIT 2002
#define STR_ERR_GFX 2003
#define STR_ERR_SFX 2004
#define STR_ERR_DIRECTX 2005
#define STR_ERR_COMM 2005
#define STR_ERR_OPENGL STR_ERR_DIRECTX
#define STR_ERR_CREATE_GAME 2006
#define STR_ERR_LOAD_DATA 2007
#define STR_ERR_NO_CD 2008
#define STR_ERR_MULTI_CD 2009
#define STR_ERR_NO_CD_SCENARIO 2010
#define STR_ERR_PAL 2011
#define STR_ERR_SYNC 2012
#define STR_ERR_DX 2013
#define STR_ERR_LOWOS 2014
#define STR_ERR_LOWDISK 2015
#define STR_ERR_NO_ORIG 2016
#define STR_ERR_EXPIRED 2017
#define STR_FAIL_START 2401
#define STR_FAIL_SAVE_GAME 2402
#define STR_FAIL_SAVE_SCENARIO 2403
#define STR_FAIL_LOAD_GAME 2404
#define STR_FAIL_LOAD_SCENARIO 2405
#define STR_FAIL_SYNC 2406
#define STR_LOW_FOOD 3001
#define STR_LOW_WOOD 3002
#define STR_LOW_STONE 3003
#define STR_LOW_GOLD 3004
#define STR_LOW_POP 3005
#define STR_SHIP_TRADE 3006
#define STR_SHIP_LAND 3007
#define STR_SHIP_UNLOAD 3008
#define STR_FAIL_PLACE 3010
#define STR_FAIL_TRADE 3011
#define STR_FAIL_REPAIR 3018
#define STR_LOW_POP_MAX 3046
#define STR_CONFIRM_LOAD 3100
#define STR_CONFIRM_QUIT 3101
#define STR_CONFIRM_RESTART 3102
#define STR_CONFIRM_RESIGN 3103
#define STR_ACT_WORK 4101
#define STR_ACT_MOVE 4102
#define STR_ACT_BUILD 4103
#define STR_ACT_TRADE 4104
#define STR_ACT_STOP 4105
#define STR_ACT_UNSELECT 4106
#define STR_ACT_UNLOAD 4107
#define STR_ACT_GROUP 4108
#define STR_ACT_UNGROUP 4109
#define STR_ACT_FORMATION 4110
#define STR_ACT_CANCEL 4111
#define STR_ACT_NEXT 4112
#define STR_ACT_CHAT 4113
#define STR_ACT_DIPLOMACY 4114
#define STR_ACT_MENU 4115
#define STR_ACT_TRADE_WITH 4116
#define STR_ACT_RESEARCH 4117
#define STR_ACT_CREATE 4118
#define STR_ACT_BUILD2 4119
#define STR_ACT_BUILD_ABORT 4120
#define STR_ACT_HELP 4121
#define STR_ACT_STANCE_GROUND 4122
#define STR_ACT_STANCE_ATTACK 4123
#define STR_ACT_HEAL 4124
#define STR_ACT_CONVERT 4125
#define STR_ACT_ATTACK 4126
#define STR_ACT_REPAIR 4127
#define STR_ACT_TRADE_FOOD 4128
#define STR_ACT_TRADE_WOOD 4129
#define STR_ACT_TRADE_STONE 4130
#define STR_ACT_QUEUE_INC 4133
#define STR_ACT_QUEUE_DEC 4134
#define STR_AGE_STONE 4201
#define STR_AGE_TOOL 4202
#define STR_AGE_BRONZE 4203
#define STR_AGE_IRON 4204
#define STR_AGE_IRON2 4205
#define STR_AGE_NOMAD 4206
#define STR_FOOD 4301
#define STR_WOOD 4302
#define STR_STONE 4303
#define STR_GOLD 4304
#define STR_ATTR_HEALTH 4305
#define STR_ATTR_ARMOR 4306
#define STR_ATTR_ATTACK 4307
#define STR_ATTR_LOAD 4308
#define STR_ATTR_RESEARCH 4309
#define STR_ATTR_TRAIN 4310
#define STR_ATTR_BUILD 4311
#define STR_ATTR_SIGHT 4312
#define STR_ATTR_POP 4313
#define STR_ATTR_RANGE 4314
#define STR_GOODS 4315
#define STR_ATTR_GOODS 4315
#define STR_ATTR_SPEED 4316
#define STR_CIV_EGYPT 10231
#define STR_CIV_GREEK 10232
#define STR_CIV_BABYLONIAN 10233
#define STR_CIV_ASSYRIAN 10234
#define STR_CIV_MINOAN 10235
#define STR_CIV_HITTITE 10236
#define STR_CIV_PHOENICIAN 10237
#define STR_CIV_SUMERIAN 10238
#define STR_CIV_PERSIAN 10239
#define STR_CIV_SHANG 10240
#define STR_CIV_YAMATO 10241
#define STR_CIV_CHOSON 10242
#define STR_CIV_UNKNOWN 10244
#define STR_CIV_TRIBE 10245
#define STR_CIV_ROMAN 10246
#define STR_CIV_CARTHAGINIAN 10247
#define STR_CIV_MACEDONIAN 10248
#define STR_CIV_PALMYRIAN 10249
#define STR_CIV_RANDOM 10250

int loadstr(unsigned id, char *str, unsigned n);

#endif
