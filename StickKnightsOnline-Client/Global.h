// Temporary file until we get everything sorted

#ifdef WINDOWS_OS
	#include "SDL.h"
	#include "SDL_mixer.h"
#elif defined MAC_OS
	#include <SDL/SDL.h>
	#include <SDL/SDL_mixer.h> 
#elif defined LINUX_OS
	#include <SDL/SDL.h>
	#include <SDL/SDL_mixer.h>
	#include <GL/gl.h>
#endif



#include "OPI_Text.h"
#include "SKO_Player.h"
#include "SKO_Map.h"
#include "InputBox.h"
#include <iostream>
#include "OPI_Hasher.h"

#define MAX_CLIENTS 16

#define ENEMY_DEAD_X -1000
#define ENEMY_DEAD_Y -1000

extern const float WALK_SPEED;
extern OPI_Text Message[];
extern Mix_Music *music;
extern bool enableMUS;
extern bool enableSND;
extern bool enableSFX;
extern int menu;
extern const char STATE_TITLE;
extern const char STATE_LOGIN;
extern const char STATE_CREATE;
extern const char STATE_PLAY;
extern const char STATE_LOADING;
extern const char STATE_DISCONNECT;
extern int popup_menu;
extern Mix_Chunk *login_noise;
extern Mix_Chunk *logout_noise;
extern Mix_Chunk *grunt_noise;
extern Mix_Chunk *attack_noise;
extern Mix_Chunk *items_drop_noise;
extern Mix_Chunk *item_pickup_noise;
extern Mix_Chunk *hit1;
extern Mix_Chunk *hit2;
extern Mix_Chunk *hit3;
extern bool loaded;
extern SKO_Player Player[MAX_CLIENTS];
extern float camera_xf;
extern float camera_yf;
extern SKO_Map *map[];
extern int popup_gui_menu;
extern int bankScroll;
extern InputBox inputBox;
extern int lastSfx;
extern void SetUsername(int);
extern std::string *chat_line[];
extern int selectedLocalItem;
extern int selectedRemoteItem;
extern bool shopBuyMode;
extern int currentShop;
extern int MyID;
extern bool connectError;
extern bool versionError;
extern void TryToLogin();
extern int chat_box;
extern OPI_Hasher *hasher;
extern bool VersionCheckOK;


// Center of screen for player
#define PLAYER_CAMERA_X 480
#define PLAYER_CAMERA_Y 300
extern float camera_xspeed;
extern float camera_yspeed;

//Quit process
extern void Kill();