#ifndef __SKO_PACKETTYPES_
#define __SKO_PACKETTYPES_

#include "operating_system.h"

const unsigned char	

VERSION_CHECK = 255, 
LOADED = 254, 
SERVER_FULL = 253,
PONG = 252, 
DISCONNECT = 251, 

VERSION_MAJOR = 1, 
VERSION_MINOR = 3,
VERSION_PATCH = 1, 
VERSION_OS = MY_OS, 

PING = 0,
CHAT = 1,

INVITE = 1, CANCEL = 2, BUSY = 3, ACCEPT = 4, CONFIRM = 5,
OFFER = 6, READY = 7, 

LOGIN = 2, 
REGISTER = 3,
LOGIN_SUCCESS = 4, 
LOGIN_FAIL_DOUBLE = 5,
LOGIN_FAIL_NONE = 6, LOGIN_FAIL_BANNED = 7,
REGISTER_SUCCESS = 8, REGISTER_FAIL_DOUBLE = 9,
MOVE_LEFT = 10, MOVE_RIGHT = 11, MOVE_JUMP = 12,
MOVE_STOP = 13, JOIN = 14, EXIT = 15, VERSION_SUCCESS = 16,
VERSION_FAIL = 17, STAT_HP = 18, STAT_XP = 19,
STAT_LEVEL = 20, STAT_STR = 21, STAT_DEF = 22,
STATMAX_HP = 23, STATMAX_XP = 24, RESPAWN = 26,
SPAWN_ITEM = 27, DESPAWN_ITEM = 28, POCKET_ITEM = 29,
DEPOCKET_ITEM = 30, BANK_ITEM = 31, DEBANK_ITEM = 32,
STAT_POINTS = 33, ATTACK = 34, ENEMY_ATTACK = 35,
ENEMY_MOVE_LEFT = 36, ENEMY_MOVE_RIGHT = 37,
ENEMY_MOVE_STOP = 38, USE_ITEM = 39, EQUIP = 40,
TARGET_HIT = 41, STAT_REGEN = 42, DROP_ITEM = 43,
TRADE = 44, PARTY = 45, CLAN = 46, BANK = 47, SHOP = 48,
BUY = 49, SELL = 50, ENEMY_HP = 51, INVENTORY = 52,
BUDDY_XP = 53, BUDDY_HP = 54, BUDDY_LEVEL = 55, WARP = 56,
SPAWN_TARGET = 57, DESPAWN_TARGET = 58, NPC_MOVE_LEFT = 59,
NPC_MOVE_RIGHT = 60, NPC_MOVE_STOP = 61, NPC_TALK = 62,
MAKE_CLAN = 63, CAST_SPELL = 64

;

#endif