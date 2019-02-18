/*
  Stick-Knights-Online
  Designed   by    Nate Howard / Cody Bygd
  Programmed by    Nate Howard
  Networking by    Nate Howard / Todd Selaku
  C++, SDL, OpenGL, SDL_net (originally winsock2, but ported for Linux)
  3/31/2016 setup in Visual
*/

#define HAVE_STRUCT_TIMESPEC

#include "operating_system.h"
#include <cstdio>
#include <ctime>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cctype>
#include <thread>
#include <chrono>
#include "OPI_Sleep.h"
#include "OPI_Clock.h"

#ifdef WINDOWS_OS
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengl.h"
#include "SDL_mixer.h"
#include "SDL_main.h"
#include "SDL_opengl.h"
#elif defined MAC_OS
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_main.h>
#include <OpenGL/gl.h>
#elif defined LINUX_OS
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_main.h>
#include <GL/gl.h>
#endif

#include "OPI_Text.h"
#include "SKO_Player.h"
#include "SKO_Enemy.h"
#include "SKO_Item.h"
#include "SKO_ItemObject.h"
#include "SKO_NPC.h"
#include "KE_Socket.h"
#include "KE_Timestep.h"
#include "InputBox.h"
#include "SKO_Stall.h"
#include "SKO_Shop.h"
#include "SKO_Map.h"
#include "SKO_item_defs.h"
#include "SKO_Sprite.h"
#include "INIReader.h"
#include "OPI_Image.h"
#include "md5.h"
#include "OPI_Hasher.h"
#include "SKO_Network.h"

// Define global variables to be used externally (in networking clasS)
#include "Global.h"

// Maximum number of clients allowed to connect
// #define MAX_CLIENTS 16

// HIT_LOOPS is maximum times to gravitate toward the ground (snap to ground) when a falling collision happens
#define HIT_LOOPS 100

// Player walking speed
const float WALK_SPEED = 3;

// Debug flag for cheating to find bugs
bool DEBUG_FLAG = false;

// Loaded yet?
bool loaded = false;
bool contentLoaded = false;

// Login and out
OPI_Hasher *hasher;
void TryToLogin();

// Maps and stuff ! :)
const char NUM_MAPS = 6;
SKO_Map *map[NUM_MAPS];
int current_map = 2;

// Sprites
SKO_Sprite EnemySprite[7];
SKO_Sprite NpcSprite[1];

// Enemy respawn default, defined in global.h now
// #define ENEMY_DEAD_X -1000
// #define ENEMY_DEAD_Y -1000

static void physics();
static int numDigits(int num);

// Screen surface
SDL_Surface *screen;
SDL_Event event = SDL_Event();

// Center of screen for player
#define PLAYER_CAMERA_X 480
#define PLAYER_CAMERA_Y 300

// Networking
SKO_Network Client;
std::string  SERVER_IP;
int SERVER_PORT;
int MyID = -1;

// Junk
bool done = false;
bool lclick = false;
bool rclick = false;
unsigned long long int cosmeticTicker;
unsigned long long int cosmeticTime = 100;
bool connectError = false;
bool versionError = false;
bool fatalNetworkError = false;

// Sounds
bool enableMUS = true;
bool enableSFX = true;
bool enableSND = true;
int lastSfx = 0;//random sounds :)

// Default Options
bool enableSIGN = true;

// Exit Points
void Disconnect();
void Kill();

//Connecting clientto server
bool tryingToConnect = true;
bool ConnectToSKOServer();

InputBox inputBox = InputBox();
unsigned char requestedPlayer = 0;
int TILES_WIDE = 0, TILES_TALL = 0;
double back_offsetx[4];
double back_offsety[4];
unsigned int offerIncrement = 1;
int bankScroll = 0;

// Text

short int tSeek = 0, uSeek = 0, pSeek = 0, iSeek = 0;

const int NAME_PASS_MAX = 36;

char tMessage[MAX_T_MESSAGE];
char uMessage[NAME_PASS_MAX];
char pMessage[NAME_PASS_MAX];
char iMessage[20];
char ppMessage[NAME_PASS_MAX];
bool SHIFT;

// Current Menu
const char STATE_TITLE = 0;
const char STATE_LOGIN = 1;
const char STATE_CREATE = 2;
const char STATE_PLAY = 4;
const char STATE_LOADING = 5;
const char STATE_DISCONNECT = 6;
int menu = STATE_TITLE;


//what popup menu
int popup_menu = 0;
//none = 0
//inventory = 1
//stats = 2
//options = 3
//gui input = 4

//what gui popup
int popup_gui_menu = 0;
//drop item popup = 1
//interact user popup = 2
//confirm trade popup = 3
//trade window = 4
// bank window = 5
// shop window = 6
// equipment window = 7
// confirrm party = 8
// confirm quit window = 9
// make clan window = 10
// confirm clan window = 11

//what chat you are on
int chat_box = 3;
//username == 1
//password == 2
//chatting == 3
//gaming   == 4
//popup gui == 5

//what sign you're reading
bool popup_sign = false;
SKO_Sign current_sign;
SKO_Sign temp_sign;

//what NPC you're talking to
bool popup_npc = false;
SKO_NPC current_npc;
int current_page = 0;
int current_npc_action = -1;
#define FINAL_PAGE 999

//only handle one at a time.
bool guiHit = false;

//are you hovering a shop item?
int hoveredShopItemX = -1;
int hoveredShopItemY = -1;

//players
SKO_Player Player[MAX_CLIENTS];



//items
SKO_Item Item[256];

//controls
bool LEFT = false;
bool RIGHT = false;
unsigned long long int keyTicker = 0;
unsigned long long int keyRepeatTicker = 0;
int keyRepeat = 0;
bool actionKeyDown = false;


//gravity
const float GRAVITY = 0.17;

//coords
float camera_x = 0, camera_y = 0;
int sky_x = 0;


const int NUM_TEXT = 180;

//hold all the text
OPI_Text Message[NUM_TEXT];

//chat
#define NUM_CHAT_LINES 12
std::string *chat_line[NUM_CHAT_LINES];

//animation
//int animation_speed = 90;
//went from 8 frames to 6 frames
float animation_speed = 90;
unsigned long long int attack_speed = 40;

//music
Mix_Music *music = NULL;
Mix_Chunk *login_noise = NULL;
Mix_Chunk *item_pickup_noise = NULL;
Mix_Chunk *items_drop_noise = NULL;
Mix_Chunk *grunt_noise = NULL;
Mix_Chunk *logout_noise = NULL;
Mix_Chunk *attack_noise = NULL;
Mix_Chunk *hit1 = NULL, *hit2 = NULL, *hit3 = NULL;

//inventory
int selectedInventoryItem = 0;
int hoveredInventoryItem = 0;
int hoverItemX = 0;
int hoverItemY = 0;

//equipment
unsigned char equipmentSlot = 0;

int selectedLocalItem = 0;
int selectedRemoteItem = 0;

int selectedBankItem = 0;
int currentShop = 0;
int selectedShopItem = 0;
bool shopBuyMode = true; //true=buy false=sell


bool draw_gui = true, draw_chat = true, draw_chat_back = true;

#define OK 0
#define FAIL 1

bool doneUpdating = false;

std::string toLower(std::string);
void inventory();


#include <iostream>
#include <string>

void replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return;
    str.replace(start_pos, from.length(), to);
}

std::string getBuildDate()
{
	// Compile-time date.
	std::string date = __DATE__;

	// Format single-digit date such as: "Jan  8" to "Jan 8"
	replace(date, "  ", " ");

	// Insert comma
	date.insert(date.find_last_of(" "), ",");

	// Return formatted date
	return date;
}

void setTitle()
{

	std::stringstream title;
	title << "Stick Knights Online "
		<< (int)VERSION_MAJOR << "."
		<< (int)VERSION_MINOR << "."
		<< (int)VERSION_PATCH << " ("
		<< getBuildDate() << ")     "
		<< SERVER_IP << ":" << SERVER_PORT
		;//<< " {Dev Version: 1.1.2A}";

	SDL_WM_SetCaption(title.str().c_str(), "SKO");
}

SDL_Surface *load_image_basic(std::string filename)
{
	//The image that's loaded
	SDL_Surface* loadedImage = NULL;

	//The optimized image that will be used
	SDL_Surface* optimizedImage = NULL;

	//Load the image using SDL_image
	loadedImage = IMG_Load(filename.c_str());

	//If the image loaded
	if (loadedImage != NULL)
	{
		//Create an optimized image
		optimizedImage = SDL_DisplayFormat(loadedImage);

		//Free the old image
		SDL_FreeSurface(loadedImage);
	}

	//Return the optimized image
	return optimizedImage;
}

#define ICON_SMALL          0
#define ICON_BIG            1


#ifdef WINDOWS_OS
#include <windows.h>
#endif

void screenOptions()
{
	//setTitle();
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, 1024, 600);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0f, 1024, 600, 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	SDL_WM_SetIcon(IMG_Load("sko.png"), NULL);

	//glDisable(GL_DEPTH_TEST);
}


void Text(int x, int y, std::string index, int R, int G, int B)
{
	//use the oldest message
	//if there are no unplaced messages
	int open_object = 0;

	//but check for an unplaced message first
	for (int i = 0; i < NUM_TEXT; i++)
	{
		if (!Message[i].used)
		{
			open_object = i;
			break;
		}
	}

	//make the message regardless
	Message[open_object].SetText(index);
	Message[open_object].used = true;
	Message[open_object].pos_x = x;
	Message[open_object].pos_y = y;
	Message[open_object].length = index.length();
	Message[open_object].R = R / 255.0f;
	Message[open_object].G = G / 255.0f;
	Message[open_object].B = B / 255.0f;
}

void SetUsername(int i)
{
	Player[i].clantag = OPI_Text();
	Player[i].nametag = OPI_Text();

	Player[i].clantag.used = true;
	Player[i].nametag.used = true;

	printf("SetUsername(%i): %s %s\n", i, Player[i].Nick.c_str(), Player[i].str_clantag.c_str());
	Player[i].clantag.SetText(Player[i].str_clantag);
	Player[i].nametag.SetText(Player[i].Nick);
}

void inventory()
{
	if (draw_gui && menu == STATE_PLAY && popup_menu != 4 && (popup_gui_menu == 0 || popup_gui_menu == 7))
	{
		if (popup_menu == 1)
		{
			popup_menu = 0;
			Client.saveInventory(Player[MyID].inventory);
		}
		else
		{
			popup_menu = 1;
			Client.saveInventory(Player[MyID].inventory);
		}
	}

}


//OPI_Images

//sign image
OPI_Image sign_bubble;
OPI_Image next_page;

//sprite sheet
OPI_Image stickman_sprite_img, skeleton_sprite_img;

//target images
OPI_Image target_img[3];

//hover a bank item
OPI_Image hover;


//system OPI_Images
OPI_Image background, font, back_img[4], options_img, loading_img, disconnect_img;
//map OPI_Images
OPI_Image tile_img[256];
//game OPI_Images
OPI_Image weapon_img[17];
OPI_Image hat_img[15];
OPI_Image trophy_img[6];

//shadow OPI_Image
OPI_Image shadow;
//chat bar
OPI_Image cred_bar;
//chat cursor
OPI_Image chatcursor;
//GUI
OPI_Image stats_gui, hud_button_img, xp_filler, hp_filler, inventory_gui, stats_popup, options_popup, inventorySelectorBox, greyedImg, chatBackImg;
OPI_Image equip_show_gui, equip_img;
OPI_Image hud_icon[4];

//Items
OPI_Image Item_img[256];
//Enemuy hp
OPI_Image enemy_hp;
//popup gui
OPI_Image popup_gui_input_img, popup_gui_request_img, popup_gui_confirm_img, trade_gui_img, mass_trade_selector, bank_gui_img, shop_gui_img;
//main menu GUI
OPI_Image login_button_img, create_button_img, banner_img, play_button_img, back_button_img, blank_button_img, quit_button_img;
//sound toggle
OPI_Image soundon, soundoff, option_selector;


/* party things */
OPI_Image buddy_container;
OPI_Image leave_party_img;
//arrows for pointing to people
OPI_Image arrow[8];
//hp xp bar fillers for party
OPI_Image party_filler[2];




void saveOptions()
{
	//dump all the memory into a file
	std::ofstream optionFile("DAT/options.dat", std::ios::out | std::ios::binary);

	if (optionFile.is_open())
	{
		unsigned char a = (unsigned char)(int)enableSND;
		unsigned char b = (unsigned char)(int)enableSFX;
		unsigned char c = (unsigned char)(int)enableMUS;
		unsigned char d = (unsigned char)(int)enableSIGN;

		//spit out each of the bytes
		optionFile << a << b << c << d;
		printf("save SND[%i] SFX[%i] MUS[%i] SIGN[%i]\n", a, b, c, d);
	}
	optionFile.close();
}



int letter_w = 8;
int letter_h = 11;
float xadj = 2.0;
float yadj = 2.0;



void drawText(OPI_Text t)
{

	//only draw if it is used
	if (t.used)
	{
		glBindTexture(GL_TEXTURE_2D, font.texture);

		//tint
		glColor3f(t.R, t.G, t.B);

		//go through all the letters in each message
		for (int ii = 0; ii < t.length; ii++)
		{

			float
				screen_x = t.pos_x + ii*letter_w + xadj,
				screen_y = t.pos_y + yadj,
				offset_x = t.letter_x[ii],
				offset_y = t.letter_y[ii],
				selection_width = letter_w,
				selection_height = letter_h,
				end_x = t.pos_x + letter_w + letter_w*ii + xadj,
				end_y = t.pos_y + letter_h + yadj;

			float right = (offset_x + selection_width) / (float)font.w;
			float bottom = (offset_y + selection_height) / (float)font.h;
			float left = (offset_x) / (float)font.w;
			float top = (offset_y) / (float)font.h;

			glBegin(GL_QUADS);
			//Top-left vertex (corner)
			glTexCoord2f(left, top);
			glVertex2f(screen_x, screen_y);

			//Bottom-left vertex (corner)
			glTexCoord2f(left, bottom);
			glVertex2f(screen_x, end_y);

			//Bottom-right vertex (corner)
			glTexCoord2f(right, bottom);
			glVertex2f(end_x, end_y);

			//Top-right vertex (corner)
			glTexCoord2f(right, top);
			glVertex2f(end_x, screen_y);
			glEnd();

		} //for
		glColor3f(1.0f, 1.0f, 1.0f);
	}//if used
	glLoadIdentity();
}


SDL_Rect getSpriteRect(int frame, int y)
{
	SDL_Rect rect;

	//where in the sprite sheet is this stickman?
	rect.y = y;
	rect.x = frame * 64;

	//how large is the stickman sprite?
	rect.w = 64;
	rect.h = 64;

	return rect;
}

SDL_Rect getSpriteWalk(int frame)
{
	return getSpriteRect(frame, 0);
}

SDL_Rect getSpriteWalkArmed(int frame)
{
	return getSpriteRect(frame, 64);
}
SDL_Rect getSpriteAttackArmed(int frame)
{
	return getSpriteRect(frame, 128);
}
SDL_Rect getSpriteAttack(int frame)
{
	return getSpriteRect(frame, 192);
}


void DrawFrameR(float screen_x, float screen_y, OPI_Image spriteSheet, SDL_Rect selection)
{
	glBindTexture(GL_TEXTURE_2D, spriteSheet.texture);


	float
		offset_x = selection.x,
		offset_y = selection.y,
		end_x = screen_x + selection.w,
		end_y = screen_y + selection.h;

	float right = (offset_x + selection.w) / (float)spriteSheet.w;
	float bottom = (offset_y + selection.h) / (float)spriteSheet.h;
	float left = (offset_x) / (float)spriteSheet.w;
	float top = (offset_y) / (float)spriteSheet.h;

	glBegin(GL_QUADS);
	//Top-left vertex (corner)
	glTexCoord2f(left, top);
	glVertex2f(screen_x, screen_y);

	//Bottom-left vertex (corner)
	glTexCoord2f(left, bottom);
	glVertex2f(screen_x, end_y);

	//Bottom-right vertex (corner)
	glTexCoord2f(right, bottom);
	glVertex2f(end_x, end_y);

	//Top-right vertex (corner)
	glTexCoord2f(right, top);
	glVertex2f(end_x, screen_y);
	glEnd();


	glLoadIdentity();
}
void DrawFrameL(float screen_x, float screen_y, OPI_Image spriteSheet, SDL_Rect selection)
{
	glBindTexture(GL_TEXTURE_2D, spriteSheet.texture);


	float
		offset_x = selection.x,
		offset_y = selection.y,
		end_x = screen_x + selection.w,
		end_y = screen_y + selection.h;

	float left = (offset_x + selection.w) / (float)spriteSheet.w;
	float bottom = (offset_y + selection.h) / (float)spriteSheet.h;
	float right = (offset_x) / (float)spriteSheet.w;
	float top = (offset_y) / (float)spriteSheet.h;

	glBegin(GL_QUADS);
	//Top-left vertex (corner)
	glTexCoord2f(left, top);
	glVertex2f(screen_x, screen_y);

	//Bottom-left vertex (corner)
	glTexCoord2f(left, bottom);
	glVertex2f(screen_x, end_y);

	//Bottom-right vertex (corner)
	glTexCoord2f(right, bottom);
	glVertex2f(end_x, end_y);

	//Top-right vertex (corner)
	glTexCoord2f(right, top);
	glVertex2f(end_x, screen_y);
	glEnd();


	glLoadIdentity();
}


void drawText(int i)
{
	if (i > NUM_TEXT)
		printf("It broke. Sorry!\n");
	else
		drawText(Message[i]);
}

void DrawImagefuncL(float x, float y, OPI_Image img)
{
	float rotation = 180.0;
	glTranslatef(x + img.w / 2, y + img.h / 2, 0);

	//rotate the image
	glRotatef(rotation, 0.0, 1.0, 0.0);


	//bind rotated image
	glBindTexture(GL_TEXTURE_2D, img.texture);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);


	glBegin(GL_QUADS);
	//Top-left vertex (corner)
	glTexCoord2f(0, 0);
	glVertex3f(-img.w / 2, -img.h / 2, 0);


	//Top-right vertex (corner)
	glTexCoord2f(1, 0);
	glVertex3f(img.w / 2, -img.h / 2, 0);


	//Bottom-right vertex (corner)
	glTexCoord2f(1, 1);
	glVertex3f(img.w / 2, img.h / 2, 0);


	//Bottom-left vertex (corner)
	glTexCoord2f(0, 1);
	glVertex3f(-img.w / 2, img.h / 2, 0);
	glEnd();

	glLoadIdentity();
}
void DrawImagefunc(float x, float y, OPI_Image img)
{
	//pixel perfect drawing
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);


	glBegin(GL_QUADS);
	//Top-left vertex (corner)
	glTexCoord2f(0, 0);
	glVertex3f(x, y, 0);

	//Bottom-left vertex (corner)
	glTexCoord2f(0, 1);
	glVertex3f(x, y + img.h, 0);

	//Bottom-right vertex (corner)
	glTexCoord2f(1, 1);
	glVertex3f(x + img.w, y + img.h, 0);

	//Top-right vertex (corner)
	glTexCoord2f(1, 0);
	glVertex3f(x + img.w, y, 0);
	glEnd();
	//glLoadIdentity();
}

void DrawImagef(float x, float y, OPI_Image img)
{

	glBindTexture(GL_TEXTURE_2D, img.texture);

	DrawImagefunc(x, y, img);

}
void DrawImage(float x, float y, OPI_Image img)
{
	DrawImagef((int)x, (int)y, img);
}
void DrawImagefL(float x, float y, OPI_Image img)
{
	glBindTexture(GL_TEXTURE_2D, img.texture);
	DrawImagefuncL(x, y, img);
}

void authenticateUser()
{
	if (menu != STATE_CREATE && menu != STATE_LOGIN)
		return;

	guiHit = true;
	Message[0].SetText("Hashing password...");
	Message[1].SetText("");
	drawText(0);
	drawText(1);

	//store and send a salted hash of the password and username, instead of clear text password
	//this salted hash of the username and password will also be 
	hasher->storePassword(uMessage, pMessage);
		
	if (menu == STATE_CREATE) //creating account
	{
		Client.createAccount(hasher->getUsername(), hasher->getPasswordHash());
	}
	if (menu == STATE_LOGIN)//logging in
	{
		TryToLogin();
	}
}
void DrawImagefRotatedL(float x, float y, OPI_Image img, float rotation)
{
	glTranslatef(x + img.w / 2, y + img.h / 2, 0);

	//flip
	glRotatef(180.0, 0.0, 1.0, 0.0);

	//rotate the image
	glRotatef(rotation, 0.0, 0.0, 1.0);


	//bind rotated image
	glBindTexture(GL_TEXTURE_2D, img.texture);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);


	glBegin(GL_QUADS);
	//Top-left vertex (corner)
	glTexCoord2f(0, 0);
	glVertex3f(-img.w / 2, -img.h / 2, 0);


	//Top-right vertex (corner)
	glTexCoord2f(1, 0);
	glVertex3f(img.w / 2, -img.h / 2, 0);


	//Bottom-right vertex (corner)
	glTexCoord2f(1, 1);
	glVertex3f(img.w / 2, img.h / 2, 0);


	//Bottom-left vertex (corner)
	glTexCoord2f(0, 1);
	glVertex3f(-img.w / 2, img.h / 2, 0);
	glEnd();

	glLoadIdentity();

}

void DrawImagefRotated(float x, float y, OPI_Image img, float rotation)
{

	glTranslatef(x + img.w / 2, y + img.h / 2, 0);

	//rotate the image
	glRotatef(rotation, 0.0, 0.0, 1.0);


	//bind rotated image
	glBindTexture(GL_TEXTURE_2D, img.texture);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);


	glBegin(GL_QUADS);
	//Top-left vertex (corner)
	glTexCoord2f(0, 0);
	glVertex3f(-img.w / 2, -img.h / 2, 0);


	//Top-right vertex (corner)
	glTexCoord2f(1, 0);
	glVertex3f(img.w / 2, -img.h / 2, 0);


	//Bottom-right vertex (corner)
	glTexCoord2f(1, 1);
	glVertex3f(img.w / 2, img.h / 2, 0);


	//Bottom-left vertex (corner)
	glTexCoord2f(0, 1);
	glVertex3f(-img.w / 2, img.h / 2, 0);
	glEnd();

	glLoadIdentity();

}

//The button
class Button
{
private:
	//The attributes of the button
	SDL_Rect box;

public:
	//Initialize the variables
	Button();
	Button(int x, int y, int w, int h);

	//Handles events and set the button's sprite region
	void handle_events(int ID);
	void setBounds(int x, int y, int w, int h);

	//Shows the button on the screen
	void show();
};

Button::Button()
{
	box.x = 0;
	box.y = 0;
	box.w = 0;
	box.h = 0;
}

Button::Button(int x, int y, int w, int h)
{
	//Set the button's attributes
	box.x = x;
	box.y = y;
	box.w = w;
	box.h = h;
}

void Button::setBounds(int x, int y, int w, int h)
{
	//Set the button's attributes
	box.x = x;
	box.y = y;
	box.w = w;
	box.h = h;

}

void scroll_sky()
{

	/* horizontal */
	back_offsetx[1] -= 0.02;

	if (back_offsetx[1] > back_img[1].w)
		back_offsetx[1] -= back_img[1].w;

	if (back_offsetx[1] < -back_img[1].w)
		back_offsetx[1] += back_img[1].w;

	back_offsetx[2] -= 0.05;

	if (back_offsetx[2] > back_img[2].w)
		back_offsetx[2] -= back_img[2].w;

	if (back_offsetx[2] < -back_img[2].w)
		back_offsetx[2] += back_img[2].w;

	back_offsetx[3] -= 0.1;

	if (back_offsetx[3] > back_img[3].w)
		back_offsetx[3] -= back_img[3].w;

	if (back_offsetx[3] < -back_img[3].w)
		back_offsetx[3] += back_img[3].w;

	/* vertical */
	back_offsety[1] -= 0.01;
	back_offsety[2] += 0.015;
	back_offsety[3] -= 0.01;


	if (back_offsety[1] < -back_img[1].h)
		back_offsety[1] += back_img[1].h;


	if (back_offsety[2] > back_img[2].h)
		back_offsety[2] -= back_img[2].h;


	if (back_offsety[3] < -back_img[3].h)
		back_offsety[3] += back_img[3].h;
}

void Button::handle_events(int ID)
{

	//printf("handle_events(%i) guiHit: %i\n", ID, (int)guiHit);
	if (guiHit) return;

	//don't let people click buttons if user could not connect or is trying to connect
	if ((connectError || tryingToConnect) && ID != 8)
		return;

	//The mouse offsets
	int x, y;

	//If a mouse button was pressed
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		//Get the mouse offsets
		x = event.button.x;
		y = event.button.y;

		if ((!lclick && event.button.button == SDL_BUTTON_LEFT) || (!rclick && event.button.button == SDL_BUTTON_RIGHT))
		{
			bool clickWasRight = false;

			//If the left mouse button was pressed
			if (event.button.button == SDL_BUTTON_RIGHT)
			{
				clickWasRight = true;
			}

			//Get the mouse offsets
			x = event.button.x;
			y = event.button.y;

			//If the mouse is over the button
			if ((x > box.x) && (x < box.x + box.w) && (y > box.y) && (y < box.y + box.h))
			{
				//printf("inside my button\n");


			  //inventory items
				int itmx = x - 11, itmy = y - 253;
				//shop
				int btmx = x - 779, btmy = y - 275;
				switch (ID)
				{
					//[login]
				case 1:
					if (menu == STATE_TITLE)
					{
						guiHit = true;
						menu = STATE_LOGIN;
						chat_box = 1;


						//clear message in corner
						Message[0].SetText("");
						Message[1].SetText("");
						drawText(0);
						drawText(1);
					}
					break;



					//[create]
				case 2:
					if (menu == STATE_TITLE)
					{
						guiHit = true;
						menu = STATE_CREATE;
						chat_box = 1;
						//clear message in corner
						Message[0].SetText("");
						Message[1].SetText("");
						drawText(0);
						drawText(1);
					}

					break;

					// quit button
				case 8:
					if (menu == STATE_TITLE)
					{
						saveOptions();
						Kill();
					}
					break;



					//[back]
				case 3:
					if (menu != STATE_PLAY && menu != STATE_TITLE)
					{
						guiHit = true;
						menu = STATE_TITLE;

						//clear message in corner
						Message[0].SetText("");
						Message[1].SetText("");
						drawText(0);
						drawText(1);
					}
					break;


					//[ok]
				case 4:
					authenticateUser();
					break;

					//[username]
				case 5:
					if (menu == STATE_LOGIN || menu == STATE_CREATE)
						chat_box = 1;
					break;

					//[password]
				case 6:
					if (menu == STATE_LOGIN || menu == STATE_CREATE)
						chat_box = 2;
					break;

					//chatting
				case 7:
					if (menu == STATE_PLAY && chat_box != 5)
						chat_box = 3;
					break;


					//inventory_button
				case 9:
					inventory();
					break;

					//stats_button
				case 10:
					if (draw_gui && menu == STATE_PLAY && popup_menu != 4 && popup_gui_menu == 0)
					{
						guiHit = true;
						if (popup_menu == 2)
							popup_menu = 0;
						else
							popup_menu = 2;
					}
					break;

				case 11:
					if (draw_gui && menu == STATE_PLAY && popup_menu == 2)
					{
						// Increase HP stat
						guiHit = true;
						Client.allocateStatPoint("health");

					}
					break;
				case 12:
					if (draw_gui && menu == STATE_PLAY && popup_menu == 2)
					{
						// Increase Strength stat
						guiHit = true;
						Client.allocateStatPoint("strength");
					}
					break;
				case 13:
					if (draw_gui && menu == STATE_PLAY && popup_menu == 2)
					{
						// Increase Defense stat
						guiHit = true;
						Client.allocateStatPoint("defense");
					}
					break;

					//inventory items or bank items
				case 14:

					if (draw_gui && menu == STATE_PLAY && popup_menu == 1)
					{
						//if within the inventory box
						if (x > 8 && x < 246 && y > 252 && y < 473)
						{
							guiHit = true;
							//calculate which item
							itmx = itmx / 39;
							itmy = itmy / 56;
							itmx += 6 * itmy;

							if (!clickWasRight)
							{
								//select item
								selectedInventoryItem = itmx;
								hoveredInventoryItem = -1;
							}

							//trade items - OFFER MORE!!! :D
							if (clickWasRight && popup_gui_menu == 4 && Player[MyID].inventory[itmx][1] > 0)
							{
								int item = Player[MyID].inventory[itmx][0];
								unsigned int amount = 0;


								//find if there's an item: om nom nom the amount
								for (unsigned int i = 0; i < 24; i++)
									if (Player[MyID].localTrade[i][0] == (unsigned int)item && Player[MyID].localTrade[i][1] > 0)
										amount = Player[MyID].localTrade[i][1];
								amount += offerIncrement;

								Client.setTradeItemOffer(item, amount);
							}
							//bank items - OFFER MORE!!! :D
							if (clickWasRight && popup_gui_menu == 5 && Player[MyID].inventory[itmx][1] > 0)
							{
								int item = Player[MyID].inventory[itmx][0];
								unsigned int amount = 0;


								//find if there's an item: om nom nom the amount
								for (unsigned int i = 0; i < 24; i++)
									if (Player[MyID].localTrade[i][0] == (unsigned int)item && Player[MyID].localTrade[i][1] > 0)
										amount = Player[MyID].localTrade[i][1];


								//offer this amount to the bank
								amount = offerIncrement;

								Client.depositBankItem(item, amount);
							}
						}


						//local trade window (OFFFER ONE LESS BRO)
						if (x > 224 + 264 && x < 224 + 502 && y > 252 && y < 473 && popup_gui_menu == 4)
						{
							guiHit = true;
							//calculate which item
							itmx = itmx - (224 + 256);//offset

							itmx = itmx / 39;
							itmy = itmy / 56;
							itmx += 6 * itmy;

							if (!clickWasRight)
							{
								selectedLocalItem = itmx;
							}

							//trade items
							if (clickWasRight && Player[MyID].localTrade[itmx][1] > 0)
							{
								int item = Player[MyID].localTrade[itmx][0];
								unsigned int amount = 0;


								//find if there's an item: om nom nom the amount
								for (unsigned int i = 0; i < 24; i++)
									if (Player[MyID].localTrade[i][0] == (unsigned int)item && Player[MyID].localTrade[i][1] > 0)
										amount = Player[MyID].localTrade[i][1];


								//offer one less
								if (amount - offerIncrement >= 0)
									amount -= offerIncrement;

								Client.setTradeItemOffer(item, amount);
							}
						}
						//remote trade window
						if (x > 776 && x < 1014 && y > 252 && y < 473 && popup_gui_menu == 4)
						{
							guiHit = true;
							//calculate which item
							itmx = itmx - 768;//offset

							itmx = itmx / 39;
							itmy = itmy / 56;
							itmx += 6 * itmy;

							if (!clickWasRight)
							{
								selectedRemoteItem = itmx;
							}
						}

					} // you have invy. open
					if (menu == STATE_PLAY && popup_gui_menu == 5)
					{//you have bank open

						//if within the bank box
						if (x > 776 && x < 1014 && y > 274 && y < 439)
						{
							guiHit = true;

							//calculate which item
							btmx = btmx / 39;
							btmy = btmy / 56;
							btmx += 6 * btmy;

							//bank scrolling
							btmx += 6 * bankScroll;

							//select item
							selectedBankItem = btmx;

							// TODO
							//bank items withdrawl
							if (clickWasRight && popup_gui_menu == 5 && Player[MyID].bank[btmx] > 0)
							{
								unsigned int amount = Player[MyID].bank[btmx];

								if (amount >= offerIncrement)
									amount = offerIncrement;

								Client.withdrawalBankItem(btmx, amount);
							}
						}
					} // you have bank open
					if (menu == STATE_PLAY && popup_gui_menu == 6)
					{//you have shop open
						//if within the bank box
						if (x > 776 && x < 1014 && y > 274 && y < 439)
						{
							guiHit = true;
							//calculate which item
							btmx = btmx / 39;
							btmy = btmy / 42;
							btmx += 6 * btmy;

							//select item
							selectedShopItem = btmx;
						}
					} // you have shop open

					break;

					//options button
				case 15:
					if (draw_gui && menu == STATE_PLAY && popup_gui_menu == 0)
					{
						guiHit = true;
						if (popup_menu == 3)
							popup_menu = 0;
						else
							popup_menu = 3;
					}
					break;

					//enable music
				case 16:
					if (draw_gui && popup_menu == 3)
					{
						guiHit = true;
						//only play if it's not playing
						if (enableMUS == false) {
							//Play the music
							if (enableSND && Mix_PausedMusic() == 1) {
								//Resume the music
								Mix_ResumeMusic();
							}
							enableMUS = true;
						}
					}

					break;

					//disable music
				case 17:
					if (draw_gui && popup_menu == 3)
					{
						guiHit = true;
						//only stop if it's not playing
						if (enableMUS == true) {
							//Pause the music
							if (Mix_PausedMusic() != 1) {
								//Resume the music
								Mix_PauseMusic();
							}
							enableMUS = false;
						}
					}
					break;

				case 18: // drop button
					if (draw_gui && menu == STATE_PLAY && popup_gui_menu == 0 && popup_menu == 1)
					{
						guiHit = true;
						inputBox.outputText->SetText("Drop how many items?");
						inputBox.okayText->SetText("Drop");
						inputBox.cancelText->SetText("Cancel");
						inputBox.inputText->SetText("");
						popup_gui_menu = 1;
						chat_box = 5;
					}
					break;


				case 19: // use button
					if (draw_gui && menu == STATE_PLAY && (popup_gui_menu == 0 || popup_gui_menu == 7) && popup_menu == 1)
					{
						guiHit = true;
						Client.useItem(Player[MyID].inventory[selectedInventoryItem][0]);
					}
					break;

					//drop items
				case 20:
					if (draw_gui)
					{
						//okay
						if (x > 305 && x < 358 && y < 476 && y > 459)
						{
							unsigned int amount;
							std::string clanTag;

							switch (popup_gui_menu)
							{
								//drop items
							case 1:
								guiHit = true;

								if (Player[MyID].inventory[selectedInventoryItem][1] > 0)
								{
									unsigned char itemId = Player[MyID].inventory[selectedInventoryItem][0];
									//parse the amount the user typed into the GUI textbox
									amount = atoi(iMessage);

									Client.dropItem(itemId, amount);
								}
								break;


								//clan buy
							case 10:
								guiHit = true;
								// Create a clan
								clanTag = (std::string)iMessage;
								Client.createClan((std::string)iMessage);
								//create clan attempt packet
								break;
							}
						}

						//cancel
						if (x > 409 && x < 462 && y < 476 && y > 459)
						{
							guiHit = true;
							popup_gui_menu = 0;
							chat_box = 4;

							for (int i = 0; i < 20; i++)
							{
								iMessage[i] = 0;
								iSeek = 0;
							}
						}
					}
					break;


					//world interactions
				case 21:
					//just chillin as far as menus go
					if (!guiHit && menu == STATE_PLAY)
					{
						printf("CLICK AT (%i,%i)\n", (int)(x + camera_x), (int)(y + camera_y));

						//check for interaction with player
						for (int i = 0; i < MAX_CLIENTS; i++)
						{
							if (Player[i].Status && i != MyID && current_map == Player[i].current_map)
							{
								float x1 = Player[i].x + 25 - camera_x;
								float x2 = Player[i].x + 38 - camera_x;
								float y1 = Player[i].y + 13 - camera_y;
								float y2 = Player[i].y + 64 - camera_y;

								//check if you clicked on a player
								if (x > x1 && x < x2 && y > y1 && y < y2)
								{
									std::stringstream ss;
									ss << "Invite ";
									ss << Player[i].Nick;
									ss << " to...";
									Message[89].SetText(ss.str());
									requestedPlayer = i;
									//position it
									int xOffset = (Player[i].Nick.length() * 9) / 2;
									Message[89].pos_x = 325 - xOffset;


									popup_gui_menu = 2;
									return;
								}

							}
						}
						//check for interaction with NPC
						for (int i = 0; i < map[current_map]->num_npcs; i++)
						{
							float x1 = map[current_map]->NPC[i].x + 25 - camera_x;
							float x2 = map[current_map]->NPC[i].x + 38 - camera_x;
							float y1 = map[current_map]->NPC[i].y + 13 - camera_y;
							float y2 = map[current_map]->NPC[i].y + 64 - camera_y;

							//check if you clicked on an NPC
							if (x > x1 && x < x2 && y > y1 && y < y2)
							{
								printf("npc!\n");
								popup_menu = 0;
								popup_npc = true;
								current_npc = map[current_map]->NPC[i];
								current_page = 0;
								return;
							}
						}
						//check for interaction with a stall
						for (int i = 0; i < map[current_map]->num_stalls; i++)
						{
							float x1 = map[current_map]->Stall[i].x - camera_x;
							float x2 = map[current_map]->Stall[i].x + map[current_map]->Stall[i].w - camera_x;
							float y1 = map[current_map]->Stall[i].y - camera_y;
							float y2 = map[current_map]->Stall[i].y + map[current_map]->Stall[i].h - camera_y;

							//check if you clicked on a stall
							if (popup_gui_menu != 5 && x > x1 && x < x2 && y > y1 && y < y2)
							{
								printf("shop\n");
								Client.openStall(i);
								return;
							}
						}
						//check for interaction with a sign

						for (int i = 0; i < map[current_map]->num_signs; i++)
						{
							float x1 = map[current_map]->Sign[i].x - camera_x;
							float x2 = map[current_map]->Sign[i].x + map[current_map]->Sign[i].w - camera_x;
							float y1 = map[current_map]->Sign[i].y - camera_y;
							float y2 = map[current_map]->Sign[i].y + map[current_map]->Sign[i].h - camera_y;

							//check if you clicked on a sign
							if (popup_gui_menu != 5 && x > x1 && x < x2 && y > y1 && y < y2)
							{
								printf("sign!\n");
								popup_menu = 0;
								popup_sign = true;
								popup_npc = false;
								current_sign = map[current_map]->Sign[i];
								return;
							}






						}


					}

					break;

					//player request gui / confirm boxes
				case 22:
					//trade, clan, or party?
					if (draw_gui && popup_gui_menu == 2)
					{
						guiHit = true;

						//trade
						if (x > 322 && x < 449 && y > 401 && y < 416)
						{
							//initiate trade
							Client.sendTradeInvite(requestedPlayer);

							//close menus and wait for response.
							popup_gui_menu = 0;
							chat_box = 4;
						}
						else
							//party
							if (x > 322 && x < 449 && y > 425 && y < 440)
							{
								//initiate party
								Client.sendPartyInvite(requestedPlayer);

								//close menus and wait for response...
								popup_gui_menu = 0;
								chat_box = 4;
							}
							else
								//clan
								if (x > 322 && x < 449 && y > 449 && y < 464)
								{
									Client.sendClanInvite(requestedPlayer);

									//close menus and wait for response...
									popup_gui_menu = 0;
									chat_box = 4;
								}
								else
									//cancel
									if (x > 322 && x < 449 && y > 473 && y < 488)
									{
										//close menus...
										popup_gui_menu = 0;
										chat_box = 4;
									}
					}
					// trade with this person?
					if (popup_gui_menu == 3)
					{
						//okay
						if (x > 305 && x < 358 && y < 476 && y > 459)
						{
							Client.acceptTradeInvite();
							//I want to trade. Close menus and ...
							popup_gui_menu = 0;
							chat_box = 4;
						}


						//cancel
						if (x > 409 && x < 462 && y < 476 && y > 459)
						{
							Client.cancelTrade();

							//don't want to trade. Close menus and ...
							popup_gui_menu = 0;
							chat_box = 4;
						}
					}
					// party with this person?
					if (popup_gui_menu == 8)
					{
						//okay
						if (x > 305 && x < 358 && y < 476 && y > 459)
						{
							//I want to party. Close menus and ...
							popup_gui_menu = 0;
							chat_box = 4;

							//send accept packet
							Client.acceptPartyInvite();
						}


						//cancel
						if (x > 409 && x < 462 && y < 476 && y > 459)
						{
							//don't want to trade. Close menus and ...
							popup_gui_menu = 0;
							chat_box = 4;

							//send deny packet
							Client.cancelParty();
						}

					}
					// clan with this person?
					if (popup_gui_menu == 11)
					{
						//okay
						if (x > 305 && x < 358 && y < 476 && y > 459)
						{
							Client.acceptClanInvite();

							//I want to party. Close menus and ...
							popup_gui_menu = 0;
							chat_box = 4;
						}

						//cancel
						if (x > 409 && x < 462 && y < 476 && y > 459)
						{
							Client.acceptClanInvite();

							//don't want to trade. Close menus and ...
							popup_gui_menu = 0;
							chat_box = 4;
						}
					}

					//exit the game?
					if (popup_gui_menu == 9) {
						//okay
						if (x > 305 && x < 358 && y < 476 && y > 459)
						{
							//disconnect
							saveOptions();
							Kill();
						}


						//cancel
						if (x > 409 && x < 462 && y < 476 && y > 459)
						{
							//don't want to trade. Close menus and ...
							popup_gui_menu = 0;
							chat_box = 4;

						}


					}
					break;

					//submit trade
				case 23:
					if (popup_gui_menu == 4)
					{
						guiHit = true;
						Client.confirmTrade();
					}
					break;

					//call off trade
				case 24:
					if (popup_gui_menu == 4)
					{
						guiHit = true;
						Client.cancelTrade();
					}
					break;

					//mass trading
				case 25:
					if (popup_gui_menu == 4)
					{
						guiHit = true;
						if (x < 224 + 457)
							offerIncrement = 1;
						else if (x < 224 + 485)
							offerIncrement = 10;
						else if (x < 224 + 513)
							offerIncrement = 100;
						else if (x < 224 + 541)
							offerIncrement = 1000;
						else if (x < 224 + 569)
							offerIncrement = 10000;
						else if (x < 224 + 597)
							offerIncrement = 100000;
						else if (x < 224 + 625)
							offerIncrement = 1000000;
					}
					break;

					//mass banking
				case 26:
					if (popup_gui_menu == 5)
					{
						guiHit = true;
						if (x < 224 + 599)
							offerIncrement = 1;
						else if (x < 224 + 627)
							offerIncrement = 10;
						else if (x < 224 + 655)
							offerIncrement = 100;
						else if (x < 224 + 683)
							offerIncrement = 1000;
						else if (x < 224 + 711)
							offerIncrement = 10000;
						else if (x < 224 + 739)
							offerIncrement = 100000;
						else if (x < 224 + 767)
							offerIncrement = 1000000;
					}
					break;

					//bank take all
				case 27:

					//if in bank and you have that item
					if (popup_gui_menu == 5)
					{
						guiHit = true;
						int item = selectedBankItem;
						unsigned int amount = Player[MyID].bank[selectedBankItem];

						if (amount > 0)
						{
							Client.withdrawalBankItem(item, amount);
						}
					}
					break;

					//bank cancel
				case 28:
					if (popup_gui_menu == 5)
					{
						guiHit = true;
						popup_gui_menu = 0;
						{
							Client.closeBank();
						}
					}
					break;

					//bank scroll
				case 29:
					if (popup_gui_menu == 5)//if bank is open
					{
						guiHit = true;
						if (x < 224 + 673)
						{//go up
							if (bankScroll > 0)
								bankScroll--;
						}
						else
						{//go down
							if (bankScroll < 36)
								bankScroll++;
						}

						//re-make the text
						int i = bankScroll * 6;
						int t = 0;
						for (int y = 0; y < 3; y++)
						{
							for (int x = 0; x < 6; x++)
							{
								//re-draw the text
								if (i < NUM_ITEMS)
								{
									std::stringstream amt;
									amt << Player[MyID].bank[i];

									if (Player[MyID].bank[i] > 0)
										Message[144 + t].SetText(amt.str());
									else
										Message[144 + t].SetText("none");

								}
								else
								{
									Message[144 + t].SetText("");
								}

								i++; t++;
							}
						}//done redraw text

					}//done bank scroll
					break;

				case 30:
					//mass shopping
					if (popup_gui_menu == 6)
					{
						guiHit = true;
						if (x < 224 + 599)
							offerIncrement = 1;
						else if (x < 224 + 627)
							offerIncrement = 10;
						else if (x < 224 + 655)
							offerIncrement = 100;
						else if (x < 224 + 683)
							offerIncrement = 1000;
						else if (x < 224 + 711)
							offerIncrement = 10000;
						else if (x < 224 + 739)
							offerIncrement = 100000;
						else if (x < 224 + 767)
							offerIncrement = 1000000;
					}
					break;


					//shop cancel
				case 31:
					if (popup_gui_menu == 6)
					{
						guiHit = true;
						popup_gui_menu = 0;
						{
							Client.closeShop();
						}
					}
					break;

				case 32:
					if (popup_gui_menu == 6)
					{
						guiHit = true;
						shopBuyMode = !shopBuyMode;
					}
					break;

				case 33:
					//accept shop transaction!! BUY/SELL
					if (popup_gui_menu == 6)
					{
						guiHit = true;

						unsigned char item = 0;
						unsigned int amount = 0;

						//find item position
						//buy items!
						if (shopBuyMode)
						{
							//find the item & amount
							item = selectedShopItem;
						}
						else // SELL ! :D
						{
							//request sell mode
							int i = 0;
							for (int y = 0; y < 4; y++)
							{
								for (int x = 0; x < 6; x++)
								{
									if (i == selectedInventoryItem)
										break;
									else
										i++;
								}
							}

							//find the item
							item = Player[MyID].inventory[i][0];
						}
						amount = offerIncrement;

						//Send to SKO Network
						if (shopBuyMode)
							Client.buyItem(item, amount);
						else
							Client.sellItem(item, amount);
					}
					break;

				case 34: //logout
					if (draw_gui && menu == STATE_PLAY)
					{
						guiHit = true;


						//confirm you want to trade pop up
						inputBox.outputText->SetText("Quit Stick Knights Online?");
						inputBox.okayText->SetText("Quit");
						inputBox.cancelText->SetText("Cancel");
						//position it
						Message[85].pos_x = 290;

						popup_gui_menu = 9;
						Message[138].pos_x = 10;
						Message[139].pos_x = 140;
						Message[138].pos_y = 452;
						Message[139].pos_y = 452;



					}
					break;

				case 35:
					if (draw_gui)
					{
						guiHit = true;
						//sound toggle button
						if (enableSND) //sound is ON =====> turn it OFF
						{
							//Pause the music
							if (Mix_PausedMusic() != 1) {
								//Resume the music
								Mix_PauseMusic();
							}
						}
						else //sound is OFF =====> turn it ON
						{
							//turn it on!
							if (enableMUS && Mix_PausedMusic() == 1) {
								//Resume the music
								Mix_ResumeMusic();
							}
						}

						//flip the boolean.
						enableSND = !enableSND;
					}
					break;


					//master sound
					//enable
				case 36:
					if (popup_menu == 3)
					{
						guiHit = true;
						enableSND = true;

						//only play if it's not playing
						if (enableMUS && enableSND && Mix_PausedMusic() == 1)
						{
							//Resume the music
							Mix_ResumeMusic();
						}
					}
					break;
					//disable
				case 37:
					if (popup_menu == 3)
					{
						guiHit = true;
						enableSND = false;

						//only stop if it's playing
						if (enableMUS && Mix_PausedMusic() != 1)
						{
							//Pause the music
							Mix_PauseMusic();
						}
					}
					break;

					//sound effects
					//enable
				case 38:
					if (popup_menu == 3)
					{
						guiHit = true;
						enableSFX = true;
					}
					break;
					//disable
				case 39:
					if (popup_menu == 3)
					{
						guiHit = true;
						enableSFX = false;
					}
					break;



					//equipment toggle
				case 40:
					//if inventory is open
					if (popup_menu == 1)
					{
						guiHit = true;
						//handle different ways for hiding and showing
						if (popup_gui_menu == 0)
						{
							popup_gui_menu = 7;
						}
						else if (popup_gui_menu == 7)
						{
							popup_gui_menu = 0;
						}
					}
					break;

					//equipment window
				case 41:
					printf("popup_menu = %i popup_gui_menu = %i\n",
						popup_menu, popup_gui_menu);
					if (popup_menu == 1 && popup_gui_menu == 7)
					{

						//select hat
						if (x > 459 && x < 496 && y > 273 && y < 306)
						{
							guiHit = true;
							equipmentSlot = 1;
						}
						else //select sword
							if (x > 459 && x < 496 && y > 347 && y < 380)
							{
								guiHit = true;
								equipmentSlot = 0;
							}
							else //select trophy 327, 317
								if (x > 327 && x < 359 && y > 317 && y < 349)
								{
									guiHit = true;
									equipmentSlot = 2;
								}
								else
									if (x > 429 && x < 501 && y > 480 && y < 495)
									{
										guiHit = true;
										Client.unequipItem(equipmentSlot);
									}
					}
					break;

				case 42:
					if (Player[MyID].party >= 0)
					{
						Client.cancelParty();
					}
					break;

				case 43:
					if (popup_sign) {
						popup_sign = false;
						current_sign.hasBeenClosed = true;
					}
					if (popup_npc)
						popup_npc = false;
					current_sign.triggered = true;
					break;

				case 44:

					if (popup_npc)
					{
						guiHit = true;
						current_page++;
						if (current_page >= current_npc.num_pages)
						{
							current_page = FINAL_PAGE;
							std::string npcLine = current_npc.final;

							switch (current_npc.quest)
							{
								//Clan
							case 0:
								inputBox.outputText->SetText("Enter Desired Clan Name");
								inputBox.okayText->SetText("Buy");
								inputBox.cancelText->SetText("Cancel");
								inputBox.inputText->SetText("");
								popup_gui_menu = 10;
								chat_box = 5;
								inputBox.numeric = false;
								break;


								break;

								//Stat Rest
							case 1:
								break;

								//this NPC is done talking
							default:
								break;
							}

							//note: it never resets back to the beginning of the dialogue unless you reset it elsewhere
							//what I mean is that if you have talked to a dialogue only-npc then you cant talk to it again..
							//aswell as the fact that you cant reset your stats ever again if you forget to reset this variable
							current_npc_action = current_npc.quest;//TODO make this more than 1 int so there can be multiple paths for the _handler s
							return;
						}
					}

					break;

				case 45:
					//drop items
					if (draw_gui)
					{
						guiHit = true;
						//okay
						if (x > 305 && x < 358 && y < 476 && y > 459)
						{
							unsigned int amount = 0;
							unsigned char itemId = 0;

							switch (popup_gui_menu)
							{
								//drop items
							case 1:
								//parse the amount the user typed into the GUI textbox
								amount = atoi(iMessage);
								itemId = Player[MyID].inventory[selectedInventoryItem][0];

								Client.dropItem(itemId, amount);
								break;

								//clan buy
							case 10:
								printf("Clan make [%s]\n", "test123");
								break;
							}

						}

						//cancel
						if (x > 409 && x < 462 && y < 476 && y > 459)
						{
							popup_gui_menu = 0;
							chat_box = 4;

							for (int i = 0; i < 20; i++)
							{
								iMessage[i] = 0;
								iSeek = 0;
							}
						}
					}


					break;

					//enable or disable auto sign reading
				case 46:
					(enableSIGN) = (!enableSIGN);

					break;

					//something unknown, but this shouldn't happen
				default:
					break;
				} // switch

				printf("button id: %i guiHit %i\n", ID, (int)guiHit);

			} //over box
		} //!click
	}//button down
	if (event.type == SDL_MOUSEBUTTONUP)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			//Get the mouse offsets
			x = event.button.x;
			y = event.button.y;

			//If the mouse is over the button
			if ((x > box.x) && (x < box.x + box.w) && (y > box.y) && (y < box.y + box.h))
			{
				int itmx = x - 11, itmy = y - 253;
				switch (ID)
				{
				case 14:

					//if within the inventory box
					if (x > 8 && x < 246 && y > 252 && y < 473)
					{
						//calculate which item
						itmx = itmx / 39;
						itmy = itmy / 56;
						itmx += 6 * itmy;

						if (menu == 4 && popup_menu)
						{

							//swap items
							int tempA = Player[MyID].inventory[itmx][0];
							int tempB = Player[MyID].inventory[itmx][1];
							Player[MyID].inventory[itmx][0] = Player[MyID].inventory[selectedInventoryItem][0];
							Player[MyID].inventory[itmx][1] = Player[MyID].inventory[selectedInventoryItem][1];
							Player[MyID].inventory[selectedInventoryItem][0] = tempA;
							Player[MyID].inventory[selectedInventoryItem][1] = tempB;

							/*update text*/
						   //selected
							{
								unsigned int amount = Player[MyID].inventory[selectedInventoryItem][1];
								std::stringstream ss_am;
								std::string s_am;
								int tamount = amount;
								//10K
								if (tamount >= 10000000)
								{
									tamount = tamount / 1000000;
									ss_am << tamount;
									s_am = "" + ss_am.str() + "M";
								}
								else if (tamount >= 1000000)
								{
									tamount = tamount / 1000000;
									ss_am << tamount << "." << ((tamount)-(amount / 1000000));
									s_am = "" + ss_am.str() + "M";
								}
								else if (tamount >= 10000)
								{
									tamount = tamount / 1000;
									ss_am << tamount;
									s_am = "" + ss_am.str() + "K";
								}
								else if (tamount > 0)
								{
									ss_am << tamount;
									s_am = "" + ss_am.str();
								}
								else {
									s_am = "";
								}

								Message[55 + selectedInventoryItem].SetText(s_am);
							}
							//hovered
							{
								unsigned int amount = Player[MyID].inventory[itmx][1];
								std::stringstream ss_am;
								std::string s_am;
								int tamount = amount;

								//10K
								if (tamount >= 10000000)
								{
									tamount = tamount / 1000000;
									ss_am << tamount;
									s_am = "" + ss_am.str() + "M";
								}
								else if (tamount >= 1000000)
								{
									tamount = tamount / 1000000;
									ss_am << tamount << "." << ((tamount)-(amount / 1000000));
									s_am = "" + ss_am.str() + "M";
								}
								else if (tamount >= 10000)
								{
									tamount = tamount / 1000;
									ss_am << tamount;
									s_am = "" + ss_am.str() + "K";
								}
								else if (tamount > 0)
								{
									ss_am << tamount;
									s_am = "" + ss_am.str();
								}
								else {
									s_am = "";
								}

								Message[55 + itmx].SetText(s_am);
							}


							//show swapped item box over new one
							selectedInventoryItem = itmx;
						} // end pop up menu
					}//end inside inventory click
					break;


				default:
					break;

				} // switch
			}// inside box
		} //left button
	}//mouse button up

} // method


//buttons

Button login_button, create_button, okay_button, back_button, quit_button;
Button username_box, password_box, chatting_box;
Button inventory_button, options_button, stats_button, logout_button, hp_button, str_button, def_button, inv_item_button;
Button equip_toggle_button, equip_button;
Button leave_party_button;

Button mute_sound_button, unmute_sound_button;
Button mute_music_button, unmute_music_button;
Button mute_effects_button, unmute_effects_button;
Button disable_auto_signs_button, enable_auto_signs_button;


Button use_button, drop_button;
Button inputBoxButton;
Button worldInteractButton, playerRequestButton;
Button tradeAcceptButton, tradeCancelButton;
Button tradeIncrementButton;
Button bankIncrementButton;
Button shopIncrementButton, shopCancelButton, shopToggleButton, shopAcceptButton;
Button bankAcceptButton, bankCancelButton;
Button bankScrollButton;
Button soundToggleButton;

//chat exit button
Button closeChatButton;

//next page button
Button nextPageButton;

void DrawRect(SDL_Rect rect)
{
	glBegin(GL_LINE_LOOP);
	glVertex2f(rect.x, rect.y);
	glVertex2f(rect.x + rect.w, rect.y);
	glVertex2f(rect.x + rect.w, rect.y + rect.h);
	glVertex2f(rect.x, rect.y + rect.h);
	glEnd();
}


float getWeaponAttackRotate(int frame) {
	switch (frame)
	{
	case 0: return 350.0f; break;
	case 1: return 355.0f; break;
	case 2: return 0.0f; break;
	case 3: return 70.0f; break;
	case 4: return 110.0f; break;
	case 5: return 12.0f; break;
	}
	return 0.0f;
}

float getWeaponAttackOffsetX(int frame) {
	switch (frame)
	{
	case 0: return 5.0f; break;
	case 1: return 13.0f; break;
	case 2: return 16.0f; break;
	case 3: return 42.0f; break;
	case 4: return 42.0f; break;
	case 5: return 22.5f; break;
	}
	return 0.0f;
}

float getWeaponAttackOffsetY(int frame) {
	switch (frame)
	{
	case 0: return -21.0f; break;
	case 1: return -25.0f; break;
	case 2: return -34.0f; break;
	case 3: return -11.0f; break;
	case 4: return 16.0f; break;
	case 5: return -21.0f; break;

	}
	return 0.0f;
}

float getWeaponWalkRotate(int frame) {
	switch (frame)
	{
	case 0: return 310.0f; break;
	case 1: return 310.0f; break;
	case 2: return 310.0f; break;
	case 3: return 310.0f; break;
	case 4: return 310.0f; break;
	case 5: return 310.0f; break;
	case 6: return   5.0f; break;
	}
	return 0.0f;
}

float getWeaponWalkOffsetX(int frame) {
	switch (frame)
	{
	case 0: return -9.0f; break;
	case 1: return -9.0f; break;
	case 2: return -9.0f; break;
	case 3: return -9.0f; break;
	case 4: return -9.0f; break;
	case 5: return -9.0f; break;
	case 6: return  14.0f; break;
	}
	return 0.0f;
}

float getWeaponWalkOffsetY(int frame) {
	switch (frame)
	{
	case 0: return -11.0f; break;
	case 1: return -11.0f; break;
	case 2: return -11.0f; break;
	case 3: return -11.0f; break;
	case 4: return -11.0f; break;
	case 5: return -11.0f; break;
	case 6: return -43.0f; break;

	}
	return 0.0f;
}

int getHatWalkOffsetY(int frame)
{
	switch (frame)
	{
	case 0:	return -11; break;
	case 1:	return -12; break;
	case 2:	return -11; break;
	case 3:	return -10; break;
	case 4:	return -11; break;
	case 5:	return -12; break;
	case 6:	return -12; break;
	}
	return 0;
}

int getHatWalkOffsetX(int frame)
{
	return -1;
}

int getHatAttackOffsetY(int frame)
{
	return -11;
}

int getHatUnarmedAttackOffsetY(int frame)
{
	return getHatAttackOffsetY(frame);
}

int getHatAttackOffsetX(int frame)
{
	switch (frame)
	{
	case 0:	return  -1; break;
	case 1:	return  -1; break;
	case 2:	return  -1; break;
	case 3:	return  1; break;
	case 4:	return  3; break;
	case 5:	return  1; break;
	case 6:	return  1; break;
	}
	return 0;
}

int getHatUnarmedAttackOffsetX(int frame)
{

	switch (frame)
	{
	case 0:	return -1; break;
	case 1:	return -1; break;
	case 2:	return -3; break;
	case 3:	return -1; break;
	case 4:	return  2; break;
	case 5:	return  0; break;
	case 6:	return  0; break;
	}
	return 60;
}


int getTrophyAttackOffsetY(int frame)
{
	switch (frame)
	{
	case 0:	return  48; break;
	case 1:	return  48; break;
	case 2:	return  48; break;
	case 3:	return  48; break;
	case 4:	return  49; break;
	case 5:	return  49; break;
	case 6:	return  49; break;
	}
	return 0;
}

int getTrophyUnarmedAttackOffsetY(int frame)
{
	switch (frame)
	{
	case 0:	return  42; break;
	case 1:	return  35; break;
	case 2:	return  31; break;
	case 3:	return  31; break;
	case 4:	return  31; break;
	case 5:	return  45; break;
	case 6:	return  49; break;
	}
	return 0;
}

int getTrophyAttackOffsetX(int frame)
{
	switch (frame)
	{
	case 0:	return  4; break;
	case 1:	return  4; break;
	case 2:	return  4; break;
	case 3:	return  7; break;
	case 4:	return  6; break;
	case 5:	return  5; break;
	case 6:	return  6; break;
	}
	return 0;
}

int getTrophyUnarmedAttackOffsetX(int frame)
{
	switch (frame)
	{
	case 0:	return   1; break;
	case 1:	return   0; break;
	case 2:	return  -1; break;
	case 3:	return  -9; break;
	case 4:	return -17; break;
	case 5:	return -15; break;
	case 6:	return  -3; break;
	}
	return 0;
}

int getTrophyWalkOffsetX(int frame)
{
	switch (frame)
	{
	case 0:	return   4; break;
	case 1:	return   7; break;
	case 2:	return   4; break;
	case 3:	return  -2; break;
	case 4:	return  -7; break;
	case 5:	return   2; break;
	case 6:	return   0; break;
	}
	return 0;
}

int getTrophyWalkOffsetY(int frame)
{
	switch (frame)
	{
	case 0:	return  48; break;
	case 1:	return  48; break;
	case 2:	return  48; break;
	case 3:	return  48; break;
	case 4:	return  48; break;
	case 5:	return  48; break;
	case 6:	return  15; break;
	}
	return 0;
}

#define PLAYER_TEST false
#define ENEMY_TEST false

void
construct_frame()
{
	if (!contentLoaded)
		return;

	if (MyID > -1)
		current_map = Player[MyID].current_map;

	//printf("camera_x : %i\n", (int)camera_x);
	//printf("camera_y : %i\n", (int)camera_y);

	glClearColor(0.00f, 0.00f, 0.00f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (loaded)
	{
		//printf("debug opportunity here.");
		;
	}
	else
	{
		camera_x = 0;
		camera_y = 50;
	}


	int tcam_x = (int)(camera_x + 0.5);
	int tcam_y = (int)(camera_y + 0.5);

	// SKY
	DrawImagef(0, 0, back_img[0]);

	// FAR CLOUDS
	for (int x = -1; x <= TILES_WIDE; x++)
		for (int y = -1; y <= TILES_TALL; y++)
			DrawImagef((x*back_img[1].w + back_offsetx[1]), (y*back_img[1].h + back_offsety[1]), back_img[1]);

	// MED CLOUDS
	for (int x = -1; x <= TILES_WIDE; x++)
		for (int y = -1; y <= TILES_TALL; y++)
			DrawImagef((x*back_img[2].w + back_offsetx[2]), (y*back_img[2].h + back_offsety[2]), back_img[2]);

	// CLOSE CLOUDS
	for (int x = -1; x <= TILES_WIDE; x++)
		for (int y = -1; y <= TILES_TALL; y++)
			DrawImagef((x*back_img[3].w + back_offsetx[3]), (y*back_img[3].h + back_offsety[3]), back_img[3]);

	//draw map
	//draw tiles, only on screen
	for (int i = 0; i < map[current_map]->number_of_tiles; i++)
	{

		int draw_x = (int)(map[current_map]->tile_x[i] - tcam_x);
		int draw_y = (int)(map[current_map]->tile_y[i] - tcam_y);

		if (draw_x >= 0 - tile_img[map[current_map]->tile[i]].w &&
			draw_x < 1024 &&
			draw_y < 600 &&
			draw_y >= 0 - tile_img[map[current_map]->tile[i]].h)
			DrawImagef(draw_x, draw_y, tile_img[map[current_map]->tile[i]]);

		//printf("\nDraw_x=%i draw_y=%i tile[i]=%i i=%i\n", draw_x, draw_y, map[current_map]->tile[i], i);

	}

	//menu gui
	if (draw_gui && menu < 4 && menu != STATE_DISCONNECT)
	{
		//draw logo banner
		DrawImagef(168, 100, banner_img);

		if (menu == STATE_TITLE)
		{
			DrawImagef(428, 220, blank_button_img);
			DrawImagef(428, 220, login_button_img);

			DrawImagef(428, 310, blank_button_img);
			DrawImagef(428, 310, create_button_img);

			DrawImagef(428, 400, blank_button_img);
			DrawImagef(428, 400, quit_button_img);
		}

		if (draw_gui && (menu == STATE_LOGIN || menu == STATE_CREATE))
		{

			//draw credentials bars
			DrawImagef(364, 223, cred_bar);
			DrawImagef(364, 277, cred_bar);

			if (chat_box == 1)
			{
				DrawImagef(uSeek * 8 + 366, 224, chatcursor);
			}
			if (chat_box == 2)
			{
				DrawImagef(pSeek * 8 + 366, 278, chatcursor);
			}

			//[PLAY]
			DrawImagef(428, 310, blank_button_img);
			DrawImagef(428, 310, play_button_img);

			//[BACK]
			DrawImagef(428, 400, blank_button_img);
			DrawImagef(428, 400, back_button_img);

		}

		//draw vignette shadow
		DrawImagef(0, 0, shadow);

	}
	else if (menu == STATE_PLAY)
	{


		//Items
		for (int i = 0; i < 256; i++)
		{
			if (map[current_map]->ItemObj[i].status)
			{
				DrawImagef(map[current_map]->ItemObj[i].x - camera_x,
					map[current_map]->ItemObj[i].y - camera_y,
					Item_img[(int)map[current_map]->ItemObj[i].itemID]);
			}
		}

		//Targets
		for (int i = 0; i < map[current_map]->num_targets; i++)
		{
			SKO_Target target = map[current_map]->Target[i];

			if (target.active)
				DrawImagef(target.x - camera_x, target.y - camera_y, target_img[target.pic]);
		}

		//Enemies
		for (int i = 0; i < map[current_map]->num_enemies; i++)
		{
			//Enemy
			SKO_Enemy  enemy = map[current_map]->Enemy[i];
			SKO_Sprite sprite = EnemySprite[enemy.sprite];

			if (enemy.hit)
				glColor4f(1.0f, 0.5f, 0.5f, 0.5f);
			else
				glColor3f(1.0f, 1.0f, 1.0f);

			if (!enemy.ground)
				enemy.current_frame = 6;

			if (enemy.attacking)
			{
				//where to place the hat?
			   //hint: your head movesin the animation now
				int hat_offset_x = getHatAttackOffsetX(enemy.current_frame);
				int hat_offset_y = getHatAttackOffsetY(enemy.current_frame);

				if (enemy.facing_right)
				{
					//draw enemy weapon
					if (sprite.weapon >= 0) {
						DrawImagefRotated(enemy.x - camera_x +
							getWeaponAttackOffsetX(enemy.current_frame),
							enemy.y - camera_y +
							getWeaponAttackOffsetY(enemy.current_frame),
							weapon_img[sprite.weapon],
							getWeaponAttackRotate(enemy.current_frame));
						//draw enemy sprite
						DrawFrameR(enemy.x - camera_x,
							enemy.y - camera_y,
							sprite.spriteSheet,
							getSpriteAttackArmed(enemy.current_frame));
					}
					else {
						//draw enemy sprite
						DrawFrameR(enemy.x - camera_x,
							enemy.y - camera_y,
							sprite.spriteSheet,
							getSpriteAttack(enemy.current_frame));
					}

					//draw hat
					if (sprite.hat >= 0)
						DrawImagef(enemy.x - camera_x + hat_offset_x,
							enemy.y - camera_y + hat_offset_y,
							hat_img[sprite.hat]);
				}
				else
				{
					//draw enemy weapon
					if (sprite.weapon >= 0) {
						DrawImagefRotatedL(enemy.x - camera_x +
							-getWeaponAttackOffsetX(enemy.current_frame),
							enemy.y - camera_y +
							getWeaponAttackOffsetY(enemy.current_frame),
							weapon_img[sprite.weapon],
							getWeaponAttackRotate(enemy.current_frame));
						//draw enemy sprite
						DrawFrameL(enemy.x - camera_x,
							enemy.y - camera_y,
							sprite.spriteSheet,
							getSpriteAttackArmed(enemy.current_frame));
					}
					else {
						//draw enemy sprite
						DrawFrameL(enemy.x - camera_x,
							enemy.y - camera_y,
							sprite.spriteSheet,
							getSpriteAttackArmed(enemy.current_frame));
					}
					//draw hat
					if (sprite.hat >= 0)
						DrawImagefL(enemy.x - camera_x + -hat_offset_x,
							enemy.y - camera_y + hat_offset_y,
							hat_img[sprite.hat]);
				}
			}
			else // not attacking
			{
				//where to place the hat?
			   //hint: your head movesin the animation now
				int hat_offset_x = getHatWalkOffsetX(enemy.current_frame);
				int hat_offset_y = getHatWalkOffsetY(enemy.current_frame);

				if (enemy.facing_right)
				{

					//draw enemy weapon
					if (sprite.weapon >= 0) {
						DrawImagefRotated(enemy.x - camera_x +
							getWeaponWalkOffsetX(enemy.current_frame),
							enemy.y - camera_y +
							getWeaponWalkOffsetY(enemy.current_frame),
							weapon_img[sprite.weapon],
							getWeaponWalkRotate(enemy.current_frame));

						//draw enemy sprite
						DrawFrameR(enemy.x - camera_x,
							enemy.y - camera_y,
							sprite.spriteSheet,
							getSpriteWalkArmed(enemy.current_frame));
					}
					else {
						//draw enemy sprite
						DrawFrameR(enemy.x - camera_x,
							enemy.y - camera_y,
							sprite.spriteSheet,
							getSpriteWalk(enemy.current_frame));
					}

					//draw hat
					if (sprite.hat >= 0)
						DrawImagef(enemy.x - camera_x + hat_offset_x,
							enemy.y - camera_y + hat_offset_y,
							hat_img[sprite.hat]);
				}
				else
				{
					//draw enemy weapon
					if (sprite.weapon >= 0) {
						DrawImagefRotatedL(enemy.x - camera_x +
							-getWeaponWalkOffsetX(enemy.current_frame),
							enemy.y - camera_y +
							getWeaponWalkOffsetY(enemy.current_frame),
							weapon_img[sprite.weapon],
							getWeaponWalkRotate(enemy.current_frame));
						//draw enemy sprite
						DrawFrameL(enemy.x - camera_x,
							enemy.y - camera_y,
							sprite.spriteSheet,
							getSpriteWalkArmed(enemy.current_frame));
					}
					else {
						//draw enemy sprite
						DrawFrameL(enemy.x - camera_x,
							enemy.y - camera_y,
							sprite.spriteSheet,
							getSpriteWalk(enemy.current_frame));
					}
					//draw hat
					if (sprite.hat >= 0)
						DrawImagefL(enemy.x - camera_x + -hat_offset_x,
							enemy.y - camera_y + hat_offset_y,
							hat_img[sprite.hat]);
				}
			}

			if (enemy.hp_ticker > OPI_Clock::milliseconds())
			{
				//draw hp above their heads, centered
				float barX = enemy.x +
					(sprite.x2 -
						sprite.x1) -
					enemy.hp_draw / 2;
				float barY = enemy.y - 14;


				//printf("enemy.x:%i enemy.y:%i barX:%i barY:%i\n", (int)enemy.x, (int)enemy.y, (int)barX, (int)barY);
				glColor3f(1.0f, 1.0f, 1.0f);

				//iterate health bar
				for (int a = 0; a < enemy.hp_draw; a++)
					DrawImagef(barX + a - camera_x, barY - camera_y, enemy_hp);
			}

		}//end enemy loop


		//NPC's
		for (int i = 0; i < map[current_map]->num_npcs; i++)
		{
			//NPC
			SKO_NPC npc = map[current_map]->NPC[i];
			SKO_Sprite sprite = NpcSprite[npc.sprite];


			if (!npc.ground)
				npc.current_frame = 6;
			{
				//where to place the hat?
			   //hint: your head movesin the animation now
				int hat_offset_x = getHatWalkOffsetX(npc.current_frame);
				int hat_offset_y = getHatWalkOffsetY(npc.current_frame);

				if (npc.facing_right)
				{

					//draw npc weapon
					if (sprite.weapon >= 0) {
						DrawImagefRotated(npc.x - camera_x +
							getWeaponWalkOffsetX(npc.current_frame),
							npc.y - camera_y +
							getWeaponWalkOffsetY(npc.current_frame),
							weapon_img[sprite.weapon],
							getWeaponWalkRotate(npc.current_frame));

						//draw npc sprite
						DrawFrameR(npc.x - camera_x,
							npc.y - camera_y,
							sprite.spriteSheet,
							getSpriteWalkArmed(npc.current_frame));
					}
					else {
						//draw npc sprite
						DrawFrameR(npc.x - camera_x,
							npc.y - camera_y,
							sprite.spriteSheet,
							getSpriteWalk(npc.current_frame));
					}

					//draw hat
					if (sprite.hat >= 0)
						DrawImagef(npc.x - camera_x + hat_offset_x,
							npc.y - camera_y + hat_offset_y,
							hat_img[sprite.hat]);
				}
				else
				{
					//draw npc weapon
					if (sprite.weapon >= 0) {
						DrawImagefRotatedL(npc.x - camera_x +
							-getWeaponWalkOffsetX(npc.current_frame),
							npc.y - camera_y +
							getWeaponWalkOffsetY(npc.current_frame),
							weapon_img[sprite.weapon],
							getWeaponWalkRotate(npc.current_frame));
						//draw npc sprite
						DrawFrameL(npc.x - camera_x,
							npc.y - camera_y,
							sprite.spriteSheet,
							getSpriteWalkArmed(npc.current_frame));
					}
					else {
						//draw npc sprite
						DrawFrameL(npc.x - camera_x,
							npc.y - camera_y,
							sprite.spriteSheet,
							getSpriteWalk(npc.current_frame));
					}
					//draw hat
					if (sprite.hat >= 0)
						DrawImagefL(npc.x - camera_x + -hat_offset_x,
							npc.y - camera_y + hat_offset_y,
							hat_img[sprite.hat]);
				}
			}
		}//end npc loop

		//draw players
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			//if this player is online
			if (Player[i].Status && Player[i].current_map == current_map)
			{
				camera_x = Player[MyID].x - PLAYER_CAMERA_X;
				camera_y = Player[MyID].y - PLAYER_CAMERA_Y;

				//don't show edge of map
				if (camera_x < 0)
					camera_x = 0;
				if (camera_y < 0)
					camera_y = 0;

				int p_x = (int)Player[i].x - (int)camera_x;
				int p_y = (int)Player[i].y - (int)camera_y;

				//username
				Player[i].nametag.pos_x = p_x - (Player[i].Nick.length()*4.5) + 32;
				Player[i].nametag.pos_y = p_y - 25;



				//clan
				Player[i].clantag.pos_x = p_x - (Player[i].clantag.length*4.5) + 32;
				Player[i].clantag.pos_y = p_y - 36;


				//getting hit flash
				if (Player[i].hit)
					glColor4f(1.0f, 0.5f, 0.5f, 0.5f);
				else
					glColor3f(1.0f, 1.0f, 1.0f);


				if (!Player[i].ground)
					Player[i].current_frame = 6;

				//where yto place the trophy?
				//hint: your hand moves in the animation..
				int trophy_offset_y = 0;
				int trophy_offset_x = 0;

				//attacking
				if (Player[i].attacking)
				{



					if (Player[i].equip[0] != 0)
					{
						//with weapon
						trophy_offset_x = getTrophyAttackOffsetX(Player[i].current_frame);
						trophy_offset_y = getTrophyAttackOffsetY(Player[i].current_frame);
					}
					else {
						//without weapon
						trophy_offset_x = getTrophyUnarmedAttackOffsetX(Player[i].current_frame);
						trophy_offset_y = getTrophyUnarmedAttackOffsetY(Player[i].current_frame);
					}




					//always draw trophies! :D
					if (Player[i].equip[2] != 0)
					{
						//draw walk
						if (Player[i].facing_right)
						{
							DrawImagef(p_x + 32 - trophy_offset_x - Item[Player[i].equipI[2]].w / 2,
								p_y + trophy_offset_y - Item[Player[i].equipI[2]].h / 4,
								trophy_img[Player[i].equip[2] - 1]);
						}
						else
						{
							DrawImagef(p_x + 32 + trophy_offset_x - Item[Player[i].equipI[2]].w / 2,
								p_y + trophy_offset_y - Item[Player[i].equipI[2]].h / 4,
								trophy_img[Player[i].equip[2] - 1]);
						}
					}

					if (Player[i].equip[0] == 0) //no weapon
					{

						if (Player[i].facing_right)
						{
							DrawFrameR(p_x, p_y,
								stickman_sprite_img,
								getSpriteAttack(Player[i].current_frame));
						}
						else
						{
							DrawFrameL(p_x, p_y,
								stickman_sprite_img,
								getSpriteAttack(Player[i].current_frame));
						}
					}
					else // yes weapon
					{
						if (Player[i].facing_right)
						{
							DrawImagefRotated(p_x + getWeaponAttackOffsetX(Player[i].current_frame),
								p_y + getWeaponAttackOffsetY(Player[i].current_frame),
								weapon_img[Player[i].equip[0] - 1],
								getWeaponAttackRotate(Player[i].current_frame));
							DrawFrameR(p_x, p_y,
								stickman_sprite_img,
								getSpriteAttackArmed(Player[i].current_frame));
						}
						else
						{
							DrawImagefRotatedL(p_x + -getWeaponAttackOffsetX(Player[i].current_frame),
								p_y + getWeaponAttackOffsetY(Player[i].current_frame),
								weapon_img[Player[i].equip[0] - 1],
								getWeaponAttackRotate(Player[i].current_frame));
							DrawFrameL(p_x, p_y,
								stickman_sprite_img,
								getSpriteAttackArmed(Player[i].current_frame));
						}
					}

				}
				else //walking
				{
					trophy_offset_x = getTrophyWalkOffsetX(Player[i].current_frame);
					trophy_offset_y = getTrophyWalkOffsetY(Player[i].current_frame);

					//always draw trophies! :D
					if (Player[i].equip[2] != 0)
					{
						//draw walk
						if (Player[i].facing_right)
						{
							DrawImagef(p_x + 32 - trophy_offset_x - Item[Player[i].equipI[2]].w / 2,
								p_y + trophy_offset_y - Item[Player[i].equipI[2]].h / 4,
								trophy_img[Player[i].equip[2] - 1]);
						}
						else
						{
							DrawImagef(p_x + 32 + trophy_offset_x - Item[Player[i].equipI[2]].w / 2,
								p_y + trophy_offset_y - Item[Player[i].equipI[2]].h / 4,
								trophy_img[Player[i].equip[2] - 1]);
						}
					}

					if (Player[i].equip[0] == 0)
					{
						//draw walk
						if (Player[i].facing_right)
						{
							DrawFrameR(p_x, p_y,
								stickman_sprite_img,
								getSpriteWalk(Player[i].current_frame));
						}
						else
						{
							DrawFrameL(p_x, p_y,
								stickman_sprite_img,
								getSpriteWalk(Player[i].current_frame));
						}
					}
					else //some weapon is held
					{
						//draw walk
						if (Player[i].facing_right)
						{
							DrawImagefRotated(p_x + getWeaponWalkOffsetX(Player[i].current_frame),
								p_y + getWeaponWalkOffsetY(Player[i].current_frame),
								weapon_img[Player[i].equip[0] - 1],
								getWeaponWalkRotate(Player[i].current_frame));
							DrawFrameR(p_x, p_y,
								stickman_sprite_img,
								getSpriteWalkArmed(Player[i].current_frame));

						}
						else
						{
							DrawImagefRotatedL(p_x + -getWeaponWalkOffsetX(Player[i].current_frame),
								p_y + getWeaponWalkOffsetY(Player[i].current_frame),
								weapon_img[Player[i].equip[0] - 1],
								getWeaponWalkRotate(Player[i].current_frame));
							DrawFrameL(p_x, p_y,
								stickman_sprite_img,
								getSpriteWalkArmed(Player[i].current_frame));
						}

					}
				}//end walking


				//always draw hats! :D
				if (Player[i].equip[1] != 0)
				{
					//where yto place the hat?
					//hint: your head movesin the animation now
					int hat_offset_y = 0;
					int hat_offset_x = 0;

					if (Player[i].attacking)
					{
						//Armed attack
						if (Player[i].equip[0] != 0)
						{
							hat_offset_x = getHatAttackOffsetX(Player[i].current_frame);
							hat_offset_y = getHatAttackOffsetY(Player[i].current_frame);
						}
						///Unarmed Attack
						else
						{
							hat_offset_x = getHatUnarmedAttackOffsetX(Player[i].current_frame);
							hat_offset_y = getHatUnarmedAttackOffsetY(Player[i].current_frame);
						}
					}
					else
					{
						hat_offset_x = getHatWalkOffsetX(Player[i].current_frame);
						hat_offset_y = getHatWalkOffsetY(Player[i].current_frame);
					}

					//draw walk
					if (Player[i].facing_right)
					{
						DrawImagef(p_x + hat_offset_x,
							p_y + hat_offset_y,
							hat_img[Player[i].equip[1] - 1]);
					}
					else
					{
						DrawImagefL(p_x - hat_offset_x,
							p_y + hat_offset_y,
							hat_img[Player[i].equip[1] - 1]);
					}
				}

				//go back to white (turn off hit effect [red flash])
				glColor3f(1.0f, 1.0f, 1.0f);

				//draw grey box for it
				for (int nameSlot = 0; (std::size_t)nameSlot < Player[i].Nick.length(); nameSlot++)
					DrawImagef(Player[i].nametag.pos_x + nameSlot * 8 + 1, Player[i].nametag.pos_y + 1, chatcursor);

				for (unsigned int nameSlot = 0; (std::size_t)nameSlot < (std::size_t)Player[i].clantag.length; nameSlot++)
					DrawImagef(Player[i].clantag.pos_x + nameSlot * 8 + 1, Player[i].clantag.pos_y + 1, chatcursor);


			}//if player.status and map
			else
			{
				Player[i].nametag.pos_x = Player[i].clantag.pos_x = 1024;

			}
			drawText(Player[i].nametag);
			drawText(Player[i].clantag);
		}//for all clients



		camera_x = Player[MyID].x - PLAYER_CAMERA_X;
		camera_y = Player[MyID].y - PLAYER_CAMERA_Y;

		if (camera_x < 0)
			camera_x = 0;
		if (camera_y < 0)
			camera_y = 0;

		//draw fringe tiles
		for (int i = 0; i < map[current_map]->number_of_fringe; i++)
		{

			int draw_x = (int)(map[current_map]->fringe_x[i] - tcam_x);
			int draw_y = (int)(map[current_map]->fringe_y[i] - tcam_y);

			if (draw_x >= 0 - tile_img[map[current_map]->fringe[i]].w &&
				draw_x < 1024 &&
				draw_y < 600 &&
				draw_y >= 0 - tile_img[map[current_map]->fringe[i]].h)
				DrawImagef(draw_x, draw_y, tile_img[map[current_map]->fringe[i]]);
			//printf("\nDraw_x=%i draw_y=%i tile[i]=%i i=%i\n", draw_x, draw_y, tile[i], i);

		}

		//draw vignette shadow
		DrawImagef(0, 0, shadow);

		//GUI
		if (draw_gui)
		{

			//draw party things
			int current_buddy = 0;
			bool first = true;
			for (int i = 0; i < MAX_CLIENTS; i++)
			{

				//if this player is online and in party
				if (i != MyID && Player[i].Status && Player[i].party > -1 && Player[i].party == Player[MyID].party)
				{
					if (first)
					{
						DrawImagef(224 + 652, 45, leave_party_img);
					}
					first = false;

					DrawImagef(858, 81 + current_buddy * 65, buddy_container);
					drawText(current_buddy + 166);

					//drawXP bar
					for (unsigned int x = 0; x < Player[i].xp; x++)
					{
						DrawImagef(224 + 637 + x * 2, 103 + current_buddy * 65, party_filler[0]);
					}

					//draw HP bar
					for (int h = 0; h < Player[i].hp; h++)
					{
						DrawImagef(224 + 637 + h * 2, 115 + current_buddy * 65, party_filler[1]);
					}

					//draw direction arrow
					DrawImagef(224 + 637, 125 + current_buddy * 65, arrow[Player[i].arrow]);

					drawText(current_buddy + 171);

					//go to next buddy
					current_buddy++;
				}
			}


			if (draw_chat_back)
				DrawImagef(0, 0, chatBackImg);

			//TODO stats gui
			DrawImagef(0, 500, stats_gui);
			for (int i = 0; i < 4; i++)
			{
				DrawImagef(700 + 81 * i, 515, hud_button_img);
				DrawImagef(708 + 81 * i, 515, hud_icon[i]);

			}
			for (int i = 0; i < ((Player[MyID].hp / (float)Player[MyID].max_hp)*(209)); i++)
			{
				DrawImagef(41 + 2 * i, 543, hp_filler);
			}


			//XP bar
			for (int i = 0; i < ((Player[MyID].xp / (float)Player[MyID].max_xp)*(209)); i++)
			{
				DrawImagef(41 + 2 * i, 515, xp_filler);
			}
			drawText(51);//xp
			drawText(52);//hp
			drawText(53);//level
			drawText(54); //stats

		   //chat bar
			if (chat_box == 3) {
				DrawImagef(tSeek * 8 + 2, 584, chatcursor);
			}

			//sign
			if (popup_sign) {
				DrawImagef(150, 372, sign_bubble);
				for (int i = 0; i < current_sign.NUM_LINES; i++)
					drawText(current_sign.line[i]);
			}
			//npc dialogue
			if (popup_npc) {


				//if not on final
				if (current_page != FINAL_PAGE)
				{
					DrawImagef(150, 372, sign_bubble);
					for (int i = 0; i < current_npc.NUM_LINES; i++)
						drawText(*current_npc.line[current_page][i]);
					DrawImagef(860, 372, next_page);
				}
				else {
					// _handler for quests
					switch (current_npc.quest)
					{
						//quest 0 is the Clan creen
					case 0:
						//this is already handled by inputBox code.
						break;

						//quest 1 is the Stat Reset Screen
					case 1:

						break;

						//this npc does nothing, he is done talking. (usually -1)
					default:
						break;

					}
				}//end final page or not

			}//end popup
		}//end if draw gui

		glColor3f(1.0f, 1.0f, 1.0f);

	}

	//draw text:
	switch (menu)
	{
	case 0:
		for (int i = 0; i < 2; i++)
			drawText(i);
		break;

	case 1: case 2:
		for (int i = 0; i < 2; i++)
			drawText(i);
		for (int i = 16; i < 18; i++)
			drawText(i);
		break;
	case 3:
		for (int i = 0; i < 2; i++)
			drawText(i);
		break;

	case 4:
		//username font
		for (int i = 0; i < MAX_CLIENTS; i++)
			if (Player[i].Status)
				drawText(i + 18);

		//chat
		if (draw_chat)
		{
			for (int i = 2; i < 15; i++)
				drawText(i);
		}
		if (draw_gui)
		{
			//ping
			drawText(15);
			//coordinates
			drawText(50);
		}
		break;

	default:
		break;
	}



	if (draw_gui)
	{
		//GUI pop ups
		if (popup_menu == 1)
		{
			//inventory window
			DrawImagef(0, 244, inventory_gui);
			DrawImagef(256, 244, equip_show_gui);


			//items
			int i = 0;
			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 6; x++)
				{


					int item = Player[MyID].inventory[i][0];
					unsigned int amount = Player[MyID].inventory[i][1];

					int offset_x = (32 - Item[item].w) / 2;
					int offset_y = (32 - Item[item].h) / 2;
					if (amount > 0)
						DrawImagef(14 + offset_x + 39 * x, 256 + offset_y + 56 * y, Item_img[item]);

					//selector box
					if (i == selectedInventoryItem)
					{
						DrawImagef(11 + 39 * x, 255 + 56 * y, inventorySelectorBox);


						std::stringstream ss;
						ss << amount;

						if (amount > 0)
						{
							Message[83].SetText(Item[item].descr);
							Message[84].SetText(ss.str());

							drawText(83);

							//position it
							Message[84].pos_x = 155 - (numDigits(amount) - 1) * 8;
							drawText(84);
						}


					}
					else if (i == hoveredInventoryItem)
					{
						DrawImagef(11 + 39 * x, 255 + 56 * y, inventorySelectorBox);
					}


					drawText(55 + i);

					//dragging an item
					if (i == selectedInventoryItem && hoveredInventoryItem >= 0 && lclick)
					{
						int itemAmount;
						itemAmount = Player[MyID].inventory[i][1];

						//center the item over your mouse
						if (itemAmount > 0) {
							offset_x -= 16;
							offset_y -= 16;
							DrawImagef(hoverItemX + offset_x, hoverItemY + offset_y, Item_img[item]);
						}
					}

					i++;


				}
			}

			//equipment window
			if (popup_gui_menu == 7)
			{
				//draw the window
				DrawImagef(256, 244, equip_img);

				//draw the sword
				int eq = Player[MyID].equipI[0];
				if (eq > 0)
				{
					//draw the icon
					DrawImagef(465 + (32 - Item_img[eq].w) / 2, 348 + (32 - Item_img[eq].h) / 2, Item_img[eq]);
				}

				//draw the hat
				eq = Player[MyID].equipI[1];
				if (eq > 0)
				{
					//draw the icon
					DrawImagef(466 + (32 - Item_img[eq].w) / 2, 274 + (32 - Item_img[eq].h) / 2, Item_img[eq]);
				}


				//draw the trophy
				eq = Player[MyID].equipI[2];
				if (eq > 0)
				{
					//draw the icon
					DrawImagef(330 + (32 - Item_img[eq].w) / 2, 321 + (32 - Item_img[eq].h) / 2, Item_img[eq]);
				}



				//where to draw the selector
				switch (equipmentSlot)
				{
				case 0://sword
					DrawImagef(459, 347, inventorySelectorBox);
					break;
				case 1://hat
					DrawImagef(459, 273, inventorySelectorBox);
					break;
				case 2://trophy
					DrawImagef(325, 316, inventorySelectorBox);
					break;

				default:
					break;
				}

				drawText(162);
				drawText(163);
				drawText(164);
				drawText(165);
			}

		}
		else if (popup_menu == 2)
		{

			DrawImagef(0, 358, stats_popup);


			//stats text
			drawText(79);
			drawText(80);
			drawText(81);
			drawText(82);

		}
		else if (popup_menu == 3)
		{
			DrawImagef(0, 244, options_popup);

			/* draw little options selectors */
			//Sign enabling
			if (enableSIGN)
				DrawImagef(169, 393, option_selector);
			else
				DrawImagef(206, 393, option_selector);
			//Master sound
			if (enableSND)
				DrawImagef(169, 417, option_selector);
			else
				DrawImagef(206, 417, option_selector);
			//Sound Effects
			if (enableSFX)
				DrawImagef(169, 441, option_selector);
			else
				DrawImagef(206, 441, option_selector);
			//GameMusic
			if (enableMUS)
				DrawImagef(169, 465, option_selector);
			else
				DrawImagef(206, 465, option_selector);


		}


		//gui input box pop up
		if (popup_gui_menu == 1 || popup_gui_menu == 10)
		{
			DrawImagef(256, 372, popup_gui_input_img);
			//input gui pop up
			if (chat_box == 5) {
				DrawImagef(iSeek * 8 + 308, 429, chatcursor);
			}
			drawText(85);
			drawText(86);
			drawText(87);
			drawText(88);
		}
		else
			//gui player request popup
			if (popup_gui_menu == 2)
			{
				DrawImagef(256, 372, popup_gui_request_img);
				drawText(89);
			}
			else
				if (popup_gui_menu == 3 || popup_gui_menu == 8 || popup_gui_menu == 9 || popup_gui_menu == 11)
				{
					DrawImagef(256, 372, popup_gui_confirm_img);
					drawText(85);
					drawText(87);
					drawText(88);
				}
				else
					if (popup_gui_menu == 4)
					{//trade


						DrawImagef(224 + 256, 225, trade_gui_img);


						//selector for increment mass trading
						switch (offerIncrement)
						{
						case 1:         DrawImagef(224 + 429, 232, mass_trade_selector); break;
						case 10:        DrawImagef(224 + 457, 232, mass_trade_selector); break;
						case 100:       DrawImagef(224 + 485, 232, mass_trade_selector); break;
						case 1000:      DrawImagef(224 + 513, 232, mass_trade_selector); break;
						case 10000:     DrawImagef(224 + 541, 232, mass_trade_selector); break;
						case 100000:    DrawImagef(224 + 569, 232, mass_trade_selector); break;
						case 1000000:   DrawImagef(224 + 597, 232, mass_trade_selector); break;
						default: break;
						}

						//ready/waiting text
						drawText(142);
						drawText(143);


						//items
						int i = 0;
						for (int y = 0; y < 4; y++)
							for (int x = 0; x < 6; x++)
							{
								//
								// LOCAL
								//
								int item = Player[MyID].localTrade[i][0];
								unsigned int amount = Player[MyID].localTrade[i][1];


								if (amount > 0)
									DrawImagef(224 + 270 + (32 - Item[item].w) / 2 + 39 * x, 256 + (32 - Item[item].h) / 2 + 56 * y, Item_img[item]);

								//selector box
								if (i == selectedLocalItem)
								{
									DrawImagef(224 + 267 + 39 * x, 255 + 56 * y, inventorySelectorBox);

									std::stringstream ss;
									ss << amount;

									if (amount > 0)
									{
										Message[138].SetText(Item[item].descr);
										Message[139].SetText(ss.str());

										drawText(138);

										//position it
										Message[139].pos_x = 224 + 411 - (numDigits(amount) - 1) * 8;
										drawText(139);
									}
								}



								//
								// REMOTE
								//
								item = Player[MyID].remoteTrade[i][0];
								amount = Player[MyID].remoteTrade[i][1];


								if (amount > 0)
									DrawImagef(782 + (32 - Item[item].w) / 2 + 39 * x, 256 + (32 - Item[item].h) / 2 + 56 * y, Item_img[item]);

								//selector box
								if (i == selectedRemoteItem)
								{
									DrawImagef(779 + 39 * x, 255 + 56 * y, inventorySelectorBox);

									std::stringstream ss;
									ss << amount;

									if (amount > 0)
									{
										Message[140].SetText(Item[item].descr);
										Message[141].SetText(ss.str());

										drawText(140);

										//position it
										Message[141].pos_x = 1004 - (numDigits(amount) - 1) * 8;
										drawText(141);
									}
								}

								drawText(90 + i);
								drawText(114 + i);

								i++;
							}
					}//end trade
					else if (popup_gui_menu == 5)
					{//bank!
						DrawImagef(768, 244, bank_gui_img);

						//selector for increment mass trading
						switch (offerIncrement)
						{
						case 1:         DrawImagef(224 + 573, 254, mass_trade_selector); break;
						case 10:        DrawImagef(224 + 601, 254, mass_trade_selector); break;
						case 100:       DrawImagef(224 + 629, 254, mass_trade_selector); break;
						case 1000:      DrawImagef(224 + 657, 254, mass_trade_selector); break;
						case 10000:     DrawImagef(224 + 685, 254, mass_trade_selector); break;
						case 100000:    DrawImagef(224 + 713, 254, mass_trade_selector); break;
						case 1000000:   DrawImagef(224 + 741, 254, mass_trade_selector); break;
						default: break;
						}

						//items
						int i = bankScroll * 6;
						int t = 0;
						for (int y = 0; y < 3 && i < NUM_ITEMS; y++)
							for (int x = 0; x < 6 && i < NUM_ITEMS; x++)
							{
								//
								// bank items :)
								//
								int item = i;
								unsigned int amount = Player[MyID].bank[i];


								// if (amount > 0)
								DrawImagef(782 + (32 - Item[item].w) / 2 + 39 * x, 278 + (32 - Item[item].h) / 2 + 56 * y, Item_img[item]);
								if (amount == 0)
								{
									//draw a "greyed out" look over the image
									DrawImagef(780 + 39 * x, 278 + 56 * y, greyedImg);
								}

								//selector box
								if (i == selectedBankItem)
								{
									DrawImagef(779 + 39 * x, 277 + 56 * y, inventorySelectorBox);

									std::stringstream ss;
									ss << amount;


									Message[138].SetText(Item[item].descr);
									Message[139].SetText(ss.str());

									drawText(138);

									//position it

									Message[139].pos_x = 964 - (numDigits(amount) - 1) * 8;
									drawText(139);

								}

								//item descriptor and amount
								drawText(90 + t);
								drawText(114 + t);

								//amount under icon
								drawText(144 + t);

								i++;
								t++;
							}
					}//end bank
					else if (popup_gui_menu == 6)
					{//shop!
						DrawImagef(768, 244, shop_gui_img);

						//selector for increment mass trading
						switch (offerIncrement)
						{
						case 1:         DrawImagef(224 + 573, 254, mass_trade_selector); break;
						case 10:        DrawImagef(224 + 601, 254, mass_trade_selector); break;
						case 100:       DrawImagef(224 + 629, 254, mass_trade_selector); break;
						case 1000:      DrawImagef(224 + 657, 254, mass_trade_selector); break;
						case 10000:     DrawImagef(224 + 685, 254, mass_trade_selector); break;
						case 100000:    DrawImagef(224 + 713, 254, mass_trade_selector); break;
						case 1000000:   DrawImagef(224 + 741, 254, mass_trade_selector); break;
						default: break;
						}



						//items
						int i = 0;
						for (int y = 0; y < 4; y++)
							for (int x = 0; x < 6; x++)
							{
								//
								// shop
								//
								int item = 0;
								unsigned int amount = 0;

								if (shopBuyMode)
								{
									item = map[current_map]->Shop[currentShop].item[x][y][0];
									amount = 1;
								}
								else
								{
									item = Player[MyID].inventory[selectedInventoryItem][0];
									amount = Player[MyID].inventory[selectedInventoryItem][1];
								}

								int price = 0;

								/* Buy or sell price? */
								if (shopBuyMode)
									price = map[current_map]->Shop[currentShop].item[x][y][1];
								else
									price = Item[item].price;

								// if buying, only draw items that are for sale. OR
								// if selling, only draw in the first item slot of the shop
								// and only draw it if a legit inventory slot is selected.
								if ((price > 0 && amount > 0 && shopBuyMode)
									|| (x == 0 && y == 0 && amount > 0 && item > 0 && !shopBuyMode))
									DrawImagef(782 + (32 - Item[item].w) / 2 + 39 * x, 278 + (32 - Item[item].h) / 2 + 42 * y, Item_img[item]);


								//selector box
								if (i == selectedShopItem)
								{
									DrawImagef(779 + 39 * x, 277 + 42 * y, inventorySelectorBox);

									if (price > 0 && amount > 0)
									{
										std::stringstream ss;
										ss << price;

										std::stringstream ss2;
										ss2 << Item[item].descr;

										if (shopBuyMode)
											ss2 << " costs:";
										else
											ss2 << " sells for:";

										//but if its free dont offer at all.. nothing in life is free :)


										Message[138].SetText(ss2.str());
										Message[139].SetText(ss.str());

										drawText(138);

										//position it

										Message[139].pos_x = 1004 - (numDigits(price) - 1) * 8;
										drawText(139);
									}
								}


								i++;
							}

						//draw hover thing to show item stats when shoppin on amazon for fur coats
						if (hoveredShopItemX > -1 && hoveredShopItemY > -1 && shopBuyMode)
						{
							//-1 is disabled else it's the item X :)
							DrawImagef(768 - 89, 244, hover);

							//draw these values, woot!
							drawText(176);
							drawText(177);
							drawText(178);
							drawText(179);
						}


					}//end shop

	}//end if draw_gui

	//draw sound toggle
	if (draw_gui)
	{
		if (enableSND)
			DrawImagef(982, 10, soundon);
		else
			DrawImagef(982, 10, soundoff);
	}

	if (menu == STATE_LOADING) {
		DrawImagef(0, 0, shadow);
		DrawImagef(0, 0, loading_img);
	}

	if (menu == STATE_DISCONNECT) {
		DrawImagef(0, 0, background);
	}
}//end draw construct_frame

bool blocked(float box1_x1, float box1_y1, float box1_x2, float box1_y2)
{
	for (int r = 0; r < map[current_map]->number_of_rects; r++)
	{
		float box2_x1 = map[current_map]->collision_rect[r].x;
		float box2_y1 = map[current_map]->collision_rect[r].y;
		float box2_x2 = map[current_map]->collision_rect[r].x + map[current_map]->collision_rect[r].w;
		float box2_y2 = map[current_map]->collision_rect[r].y + map[current_map]->collision_rect[r].h;


		if (box1_x2 > box2_x1 && box1_x1 < box2_x2 && box1_y2 > box2_y1 && box1_y1 < box2_y2)
		{
			// printf("collision with rect[%i]\n\tme:\t{%i,%i,%i,%i}\n\tit:\t{%i,%i,%i,%i}\n", r, box1_x1, box1_y1, box1_x2, box1_y2, box2_x1, box2_y1, box2_x2, box2_y2);
			// printf("%i > %i && %i < %i && %i > %i && %i < %i\n\n", box1_x2, box2_x1, box1_x1, box2_x2, box1_y2, box2_y1, box1_y1, box2_y2);
			return true;

		}
	}
	for (int r = 0; r < map[current_map]->num_targets; r++)
	{
		if (!map[current_map]->Target[r].active)
			continue;
		float box2_x1 = map[current_map]->Target[r].x;
		float box2_y1 = map[current_map]->Target[r].y;
		float box2_x2 = map[current_map]->Target[r].x + map[current_map]->Target[r].w;
		float box2_y2 = map[current_map]->Target[r].y + map[current_map]->Target[r].h;


		if (box1_x2 > box2_x1 && box1_x1 < box2_x2 && box1_y2 > box2_y1 && box1_y1 < box2_y2)
			return true;
	}

	return false;
}

int pressKey(int key)
{
	int returnKey = 0;

	//movement
	if (menu == 4)//game
		switch (key)
		{

		case SDLK_ESCAPE:
			popup_menu = 0;
			break;

		case '4':
		case 'o':
			if (chat_box == 4 && draw_gui && menu == STATE_PLAY && popup_gui_menu == 0)
			{
				if (popup_menu == 3)
					popup_menu = 0;
				else
					popup_menu = 3;
			}
			break;

		case '3':
		case 'e':
			if (chat_box == 4)
			{
				//if inventory is open
				if (popup_menu != 1)
					inventory();

				//handle different ways for hiding and showing
				if (popup_gui_menu == 0)
				{
					popup_gui_menu = 7;
				}
				else if (popup_gui_menu == 7)
				{
					popup_gui_menu = 0;
				}
			}
			break;

		case '2':
		case 'p':
			if (chat_box == 4 && draw_gui && menu == STATE_PLAY && popup_menu != 4 && popup_gui_menu == 0)
			{
				if (popup_menu == 2)
					popup_menu = 0;
				else
					popup_menu = 2;
			}
			break;

		case 'i':
		case '1':
			if (chat_box == 4)
				inventory();
			break;


		case 'r':
			// Check to see if the action key ('r') has been released before throwing again
			if (!actionKeyDown)
			{
				actionKeyDown = true;
				if (chat_box == 4 && !Player[MyID].attacking)
				{
					Client.castSpell();
				}
			}
			break;

		case SDLK_LEFT:
		case 'a':
			if (chat_box == 4 && !Player[MyID].attacking && Player[MyID].x_speed != -2)
			{
				//Send action to server
				Client.playerAction("left", Player[MyID].x, Player[MyID].y);

				//Client prediction physics
				LEFT = true;
				RIGHT = false;

				Player[MyID].x_speed = -WALK_SPEED;
				Player[MyID].facing_right = false;
			}
			break;

		case SDLK_RIGHT:
		case 'd':
			if (chat_box == 4 && !Player[MyID].attacking && Player[MyID].x_speed != 2)
			{
				//send action to SKO network
				Client.playerAction("right", Player[MyID].x, Player[MyID].y);

				//Client prediction physics
				RIGHT = true;
				LEFT = false;
				Player[MyID].x_speed = WALK_SPEED;
				Player[MyID].facing_right = true;
			}

			break;


		case 305:
		case 306:
		case 307:
		case 308:
		case 's':
			if (chat_box == 4 && Player[MyID].ground && !Player[MyID].attacking)
			{
				//send action to SKO network
				Client.playerAction("attack", Player[MyID].x, Player[MyID].y);

				if (enableSND && enableSFX)
					Mix_PlayChannel(-1, attack_noise, 0);

				Player[MyID].attacking = true;
				Player[MyID].current_frame = 0;
				Player[MyID].x_speed = 0;
			}
			break;
		default:
			//printf("event.key.keysym.sym: %i\n", event.key.keysym.sym);
			break;
		case SDLK_SPACE:
		case 'w':
			if (chat_box == 4 && !Player[MyID].attacking)

				//verical collision detection
				if (blocked(Player[MyID].x + 25, Player[MyID].y + Player[MyID].y_speed + 0 + 0.15, Player[MyID].x + 38, Player[MyID].y + Player[MyID].y_speed + 64 + 0.15))
				{
					//Send action to SKO network
					Client.playerAction("jump", Player[MyID].x, Player[MyID].y);

					//fly
					Player[MyID].y_speed = -6;
				}
			break;
		}

	//menus..
	switch (key)
	{

	case SDLK_F1:
		draw_gui = !draw_gui;
		break;

	case SDLK_F2:
		draw_chat = !draw_chat;
		break;

	case SDLK_F3:
		draw_chat_back = !draw_chat_back;
		break;

	case SDLK_LSHIFT: case SDLK_RSHIFT:
		SHIFT = true;
		key = '@'; //fake
		break;

	case 256: key = '0'; break;
	case 257: key = '1'; break;
	case 258: key = '2'; break;
	case 259: key = '3'; break;
	case 260: key = '4'; break;
	case 261: key = '5'; break;
	case 262: key = '6'; break;
	case 263: key = '7'; break;
	case 264: key = '8'; break;
	case 265: key = '9'; break;
	case 266: key = '.'; break;
	case 267: key = '/'; break;
	case 268: key = '*'; break;
	case 269: key = '-'; break;
	case 270: key = '+'; break;
	case 271:
		key = SDLK_RETURN;
		break;

	case SDLK_RIGHT:
	case SDLK_LEFT:
	case SDLK_UP:
	case SDLK_DOWN:
	case SDLK_CAPSLOCK:
	case 2:
	case 16:
	case 19:
	case 27:
	case 127:
		/*
		   numpad
		*/
	case 277:
	case 278:
	case 279:
	case 280:
	case 281:
		//case 282:   //SDLK_F1
		//case 283:   //SDLK_F2
		//case 284:   //SDLK_F3
	case 285:
	case 286:
	case 287:
	case 288:
	case 289:
	case 290:
	case 291:
	case 292:
	case 293: //SDLK_F12
	case 316:
	case 302:
	case 300:
	case 305:
	case 306:
	case 307:
	case 308:
	case 311:
	case 312:
	case 319:
		key = '@';
		break;

	case SDLK_TAB:
		key = '&';
		break;

	default:
		break;

	}



	if (key != SDLK_RETURN    &&
		key != SDLK_BACKSPACE &&
		!SHIFT)
	{
		if (key != '@' && key != '&')
		{
			returnKey = key;

			//key input
			if (chat_box == 1)
			{
				switch (key)
				{
					//normal letters
				case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':
					//numbers
				case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':

					uMessage[uSeek] = key;
					//seek forward one but don't go off the screen
					uSeek++;
					if (uSeek >= NAME_PASS_MAX)
						uSeek = NAME_PASS_MAX - 1;
					break;

				default: break;
				}
			}
			else
				if (chat_box == 2)
				{
					pMessage[pSeek] = key;
					ppMessage[pSeek] = '*';
					//seek forward one but don't go off the screen
					pSeek++;
					if (pSeek >= NAME_PASS_MAX)
						pSeek = NAME_PASS_MAX - 1;
				}
				else
					if (chat_box == 3)
					{

						tMessage[tSeek] = key;
						//seek forward one but don't go off the screen
						tSeek++;
						int max = MAX_T_MESSAGE - 2;//76 - Player[MyID].Nick.length();
						if (tSeek > max)
							tSeek = max;

						//chat_box = 0;
					}
					else
						if (chat_box == 5) {
							if (inputBox.numeric)
							{
								switch (key)
								{
									//numbers
								case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':

									iMessage[iSeek] = key;
									//seek forward one but don't go off the screen
									iSeek++;
									if (iSeek > 9)
										iSeek = 9;
									break;

								default: break;
								}
							}//only numeric
							else
							{
								iMessage[iSeek] = key;

								//seek forward one but don't go off the screen
								iSeek++;

								if (iSeek > 18)
									iSeek = 18;

							}//alphanumeric
						}//chat_box == 5, inputBox gui pop up
		}//not a break key
		else if (key == '&')
		{
			if (chat_box == 1)
				chat_box = 2;
			else if (chat_box == 2)
				chat_box = 1;

		} //tab
	}
	//if you hit enter, send the message
	else if (key == SDLK_RETURN)
	{
		if (chat_box == 3)
		{
			if (tMessage[0] == '&')
				DEBUG_FLAG = true;
			else if (tMessage[0] > 0)
				Client.sendChat(tMessage);

			//clear chat bar
			//TODO: this was here, but leaves a dirty character:  
			//      for (int i = 0; i < MAX_T_MESSAGE - 2; i++)
			// TODO should this be - 0?
			for (int i = 0; i < MAX_T_MESSAGE-1; i++)
				tMessage[i] = 0;

			tSeek = 0;
			chat_box = 4;
		}
		else if (chat_box == 4)
		{
			chat_box = 3;
		}

		if (menu == STATE_LOGIN)
		{
				authenticateUser();
		}
	}
	else if (key == SDLK_BACKSPACE)
	{
		returnKey = key;

		if (chat_box == 1)
		{
			//do we move the cursor back?
			bool goBack = (uMessage[uSeek] == 0);

			//erase this spot for sure
			uMessage[uSeek] = 0;

			//if marked to go back, scoot the cursor back
			if (goBack && uSeek)
				uSeek--;

			//clear the spot the cursor is in
			uMessage[uSeek] = 0;
		}
		if (chat_box == 2)
		{
			//do we move the cursor back?
			bool goBack = (pMessage[pSeek] == 0);

			//erase this spot for sure
			pMessage[pSeek] = 0;
			ppMessage[pSeek] = 0;

			//if marked to go back, scoot the cursor back
			if (goBack && pSeek)
				pSeek--;

			//clear the spot the cursor is in
			pMessage[pSeek] = 0;
			ppMessage[pSeek] = 0;
		}
		if (chat_box == 3)
		{
			//do we move the cursor back?
			bool goBack = (tMessage[tSeek] == 0);

			//erase this spot for sure
			tMessage[tSeek] = 0;

			//if marked to go back, scoot the cursor back
			if (goBack && tSeek)
				tSeek--;

			//clear the spot the cursor is in
			tMessage[tSeek] = 0;
		}
		if (chat_box == 5)
		{
			//do we move the cursor back?
			bool goBack = (iMessage[iSeek] == 0);

			//erase this spot for sure
			iMessage[iSeek] = 0;

			//if marked to go back, scoot the cursor back
			if (goBack && iSeek)
				iSeek--;

			//clear the spot the cursor is in
			iMessage[iSeek] = 0;
		}
	}

	else if (SHIFT == true && key != '@')
	{
		returnKey = key;
		char symbol = ' ';

		//capitalize the keys
		switch (key)
		{
			//normal letters
		case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':
			symbol = key - 32;

			break;

			//odd symbols
		case '\'':
			symbol = '\"';
			break;
		case '\\':
			symbol = '|';
			break;
		case ';':
			symbol = ':';
			break;

		case ',':
			symbol = '<';
			break;

		case '.':
			symbol = '>';
			break;

		case '/':
			symbol = '?';
			break;
		case '`':
			symbol = '~';
			break;
		case '-':
			symbol = '_';
			break;
		case '=':
			symbol = '+';
			break;
		case '[':
			symbol = '{';
			break;
		case ']':
			symbol = '}';
			break;

			//numbers
		case '1':
			symbol = '!';
			break;
		case '2':
			symbol = '@';
			break;
		case '3':
			symbol = '#';
			break;
		case '4':
			symbol = '$';
			break;
		case '5':
			symbol = '%';
			break;
		case '6':
			symbol = '^';
			break;
		case '7':
			symbol = '&';
			break;
		case '8':
			symbol = '*';
			break;
		case '9':
			symbol = '(';
			break;
		case '0':
			symbol = ')';
			break;


			//spacebar-or-error
		default:

			break;
		}

		if (chat_box == 2)
		{
			pMessage[pSeek] = symbol;
			ppMessage[pSeek] = '*';
		}
		if (chat_box == 3)
		{
			tMessage[tSeek] = symbol;
		}
		if (chat_box == 1)
		{
			//only allowed certain ones
			switch (symbol)
			{
				//caps
			case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z':
				//only allowed symbols
			case '_':
				uMessage[uSeek] = symbol;

				//seek forward one but don't go off the screen
				uSeek++;
				if (uSeek >= 19)
					uSeek = 19;
				break;

			default:

				break;
			}
		}
		if (chat_box == 2)
		{
			//seek forward one but don't go off the screen
			pSeek++;
			if (pSeek >= NAME_PASS_MAX)
				pSeek = NAME_PASS_MAX;
		}
		if (chat_box == 3)
		{
			//seek forward one but don't go off the screen
			tSeek++;
			int max = MAX_T_MESSAGE - 2;// - Player[MyID].Nick.length();
			if (tSeek > max)
				tSeek = max;
		}
		if (chat_box == 5 && !inputBox.numeric)
		{
			//do we move the cursor back?
			bool goBack = (iMessage[iSeek] == 0);

			//erase this spot for sure
			iMessage[iSeek] = 0;

			//if marked to go back, scoot the cursor back
			if (goBack && iSeek)
				iSeek--;

			//clear the spot the cursor is in
			iMessage[iSeek] = 0;
		}

	}

	//update the little string at the bottom of the screen where you are typing
	if (chat_box == 1)
		Message[16].SetText(uMessage);
	if (chat_box == 2)
		Message[17].SetText(ppMessage);
	if (chat_box == 3 || chat_box == 4)
		Message[2].SetText(tMessage);
	if (chat_box == 5)
		inputBox.inputText->SetText(iMessage);

	return returnKey;
}
void HandleUI()
{
	//When a user is typing and holds down a key, repeat it
	if (keyRepeat && OPI_Clock::milliseconds() - keyTicker > 500)
	{
		if (OPI_Clock::milliseconds() - keyRepeatTicker > 20) {
			pressKey(keyRepeat);
			keyRepeatTicker = OPI_Clock::milliseconds();
		}
	}

	//if there are no SDL events then return to main loop
	while (SDL_PollEvent(&event))
	{
		int x = event.button.x;
		int y = event.button.y;

		switch (event.type)
		{
			//mouse clickies
		case SDL_MOUSEBUTTONDOWN:
			if (!lclick) {
				//buttons!!
				guiHit = false;
				login_button.handle_events(1);
				create_button.handle_events(2);
				back_button.handle_events(3);
				okay_button.handle_events(4);
				username_box.handle_events(5);
				password_box.handle_events(6);
				chatting_box.handle_events(7);
				quit_button.handle_events(8);
				inventory_button.handle_events(9);
				stats_button.handle_events(10);
				hp_button.handle_events(11);
				str_button.handle_events(12);
				def_button.handle_events(13);
				inv_item_button.handle_events(14);
				options_button.handle_events(15);

				unmute_music_button.handle_events(16);
				mute_music_button.handle_events(17);

				drop_button.handle_events(18);
				use_button.handle_events(19);
				inputBoxButton.handle_events(20);
				playerRequestButton.handle_events(22);
				tradeAcceptButton.handle_events(23);
				tradeCancelButton.handle_events(24);
				tradeIncrementButton.handle_events(25);
				bankIncrementButton.handle_events(26);
				bankAcceptButton.handle_events(27);
				bankCancelButton.handle_events(28);
				bankScrollButton.handle_events(29);
				shopIncrementButton.handle_events(30);
				shopCancelButton.handle_events(31);
				shopToggleButton.handle_events(32);
				shopAcceptButton.handle_events(33);
				logout_button.handle_events(34);
				soundToggleButton.handle_events(35);

				unmute_sound_button.handle_events(36);
				mute_sound_button.handle_events(37);

				unmute_effects_button.handle_events(38);
				mute_effects_button.handle_events(39);

				equip_toggle_button.handle_events(40);
				equip_button.handle_events(41);

				leave_party_button.handle_events(42);

				closeChatButton.handle_events(43);

				nextPageButton.handle_events(44);

				disable_auto_signs_button.handle_events(46);
				enable_auto_signs_button.handle_events(46);
				//do world interactions after all gui
				worldInteractButton.handle_events(21);
			}
			if (event.button.button == SDL_BUTTON_LEFT)
				lclick = true;
			if (event.button.button == SDL_BUTTON_RIGHT)
				rclick = true;
			break;

		case SDL_MOUSEBUTTONUP:
			guiHit = false;
			inv_item_button.handle_events(14);
			if (event.button.button == SDL_BUTTON_LEFT)
				lclick = false;
			if (event.button.button == SDL_BUTTON_RIGHT)
				rclick = false;
			break;

		case SDL_MOUSEMOTION:
			//hover inventory items
			if (menu == STATE_PLAY && popup_menu)
			{


				int itmx = x - 11, itmy = y - 253;//calculate which item

				//only if within box
				if (x > 8 && x < 246 && y > 252 && y < 473)
				{
					itmx = itmx / 39;
					itmy = itmy / 56;
					itmx += 6 * itmy;

					//show swapped item
					hoveredInventoryItem = itmx;


					//draw item on your cursor
					hoverItemX = x;
					hoverItemY = y;
				}
				else
				{
					//reset this. Only draw when user is hovering this item with the mouse.
					hoveredInventoryItem = -1;
				}

			}
			//hover shop items
			if (menu == STATE_PLAY && popup_gui_menu == 6)
			{

				int btmx = x - 779, btmy = y - 275;

				//only if within box
				if (x > 776 && x < 1014 && y > 274 && y < 439)
				{


					//calculate which item
					btmx = btmx / 39;
					btmy = btmy / 42;

					//if it did change:
					if (hoveredShopItemX != btmx || hoveredShopItemY != btmy)
					{
						int item, str, def, hp, sellPrice;
						std::stringstream ss1, ss2, ss3, ss4;

						//convert btmx to a SKO_Item ID
						item = map[current_map]->Shop[currentShop].item[btmx][btmy][0];

						//get item stats and apply to text objects

						//health bonus
						hp = Item[item].hp;
						ss3 << "+" << hp;
						Message[176].SetText(ss3.str());

						//strength bonus
						str = Item[item].sp;
						ss1 << "+" << str;
						Message[177].SetText(ss1.str());

						//defense bonus
						def = Item[item].dp;
						ss2 << "+" << def;
						Message[178].SetText(ss2.str());


						//sell price
						sellPrice = Item[item].price;
						ss4 << "" << sellPrice;
						Message[179].SetText(ss4.str());


					}//end selected new item

					//select item
					hoveredShopItemX = btmx;
					hoveredShopItemY = btmy;

				}
				else
				{
					//reset this. Only draw when user is hovering this item with the mouse.
					hoveredShopItemX = -1;
					hoveredShopItemY = -1;
				}

			}

			break;



			//keys
		case SDL_KEYDOWN:
			keyTicker = OPI_Clock::milliseconds();
			keyRepeat = pressKey(event.key.keysym.sym);

			//disable space on jumping
			if (keyRepeat == SDLK_SPACE && chat_box == 4)//4 is game
				keyRepeat = 0;

			//printf("Key is: %i\n", event.key.keysym.sym);
			break;


		case SDL_KEYUP:
			keyTicker = OPI_Clock::milliseconds();
			keyRepeat = 0;

			switch (event.key.keysym.sym)
			{
			case SDLK_LSHIFT: case SDLK_RSHIFT:
				SHIFT = false;
				break;

			case SDLK_LEFT:
			case 'a':
				LEFT = false;
				if (chat_box == 4 && !RIGHT && !Player[MyID].attacking)
				{
					//send action to SKO network
					Client.playerAction("stop", Player[MyID].x, Player[MyID].y);
				}
				break;
			case SDLK_RIGHT:
			case 'd':
				RIGHT = false;
				if (chat_box == 4 && !LEFT && !Player[MyID].attacking)
				{
					//send action to SKO network
					Client.playerAction("stop", Player[MyID].x, Player[MyID].y);
				}
				break;
			case 'r':
				// Set the action key ('r') as released, so that objects can be thrown again
				actionKeyDown = false;
				break;
			default: break;
			}
			break;

			//if a mouse button was released
			if (event.type == SDL_MOUSEBUTTONUP)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
					lclick = false;
				if (event.button.button == SDL_BUTTON_RIGHT)
					rclick = false;
			}

		case SDL_QUIT:
			//printf("QUIT.\n");
		//printf("MyID is: %i\n", MyID);
			//printf("MY Username is: %s\n", Player[MyID].Nick);
			saveOptions();
			Kill();

			break;
		default: break;
		}
	}
}//end handle ui

void Network()
{
	//connect normally
	connectError = ConnectToSKOServer();
	tryingToConnect = false;

	// If client could not connect upon opening the game
	// patiently reconnect until a connection is made or the user exists the game.
	for (int i = 0; connectError && i < 10; i++)
	{
		if (Client.TryReconnect(1000))
		{
			connectError = false;
			Client.sendVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
		}
		else
		{
			std::string message = "    Trying to reconnect for 10 seconds";
			for (int dot = 0; dot < i; dot++)
				message += "...";
			Message[1].SetText(message);
		}
	}

	if (connectError)
	{
		Message[1].SetText("Try again later? Use www.StickKnightsOnline.com/Chat for help!");
	}

	while (!done && !connectError && !versionError)
	{
		// If the client disconnects during gameplay
		// Try reconnecting and displaying 
		if (!Client.isConnected())
		{
			if (menu != STATE_DISCONNECT)
				Disconnect();
			continue;
		}

		Client.receivePacket(connectError);
		Client.checkPing();
		OPI_Sleep::microseconds(100);
	}

	//Close connection gracefully
	Client.disconnect();
}

void Graphics();


void PhysicsLoop()
{
	KE_Timestep *timestep = new KE_Timestep(60);

	while (!done)
	{
		timestep->Update();

		bool draw = false;
		while (timestep->Check())
		{
			physics();
			HandleUI();
			draw = true;
		}

		if (draw)
			Graphics();

		OPI_Sleep::microseconds(1);
	}
}


/* loadContent - this is both a place to store
 * 	hard-coded content as a "TODO" to fix in the future
 * 	load all content from files
 *
 */
void loadContent()
{
	//music
	music = Mix_LoadMUS("SND/menu.ogg");

	//sound effects
	item_pickup_noise = Mix_LoadWAV("SND/item_pickup.wav");
	items_drop_noise = Mix_LoadWAV("SND/items_drop.wav");
	grunt_noise = Mix_LoadWAV("SND/grunt.wav");
	login_noise = Mix_LoadWAV("SND/login.wav");
	logout_noise = Mix_LoadWAV("SND/logout.wav");
	hit1 = Mix_LoadWAV("SND/hit1.wav");
	hit2 = Mix_LoadWAV("SND/hit2.wav");
	hit3 = Mix_LoadWAV("SND/hit3.wav");
	attack_noise = Mix_LoadWAV("SND/attack.wav");

	//TODO move this
	std::string mapConfigLoc = "MAP";

	//load maps and all they contain
	//targets, enemies, stalls, shops
	for (int mp = 0; mp < NUM_MAPS; mp++)
	{
		std::stringstream ss1;
		ss1 << "map" << mp;
		std::string mapFile = ss1.str();
		printf("mapFile: %s\n", mapFile.c_str());
		map[mp] = new SKO_Map(mapConfigLoc, mapFile);   //
	}
	////
	//tile images for the map
	for (int i = 0; i < 256; i++)//check if file exists, etc.
	{
		char szFilename[24];
		sprintf(szFilename, "IMG/TILE/tile%i.png", i);
		std::ifstream checker(szFilename);

		if (checker.is_open())
		{
			checker.close();
			tile_img[i].setImage(szFilename);
		}
		else
		{
			printf("Loaded %i tile images.\n", i);
			break;
		}
	}

	//content is now loaded so draw
	contentLoaded = true;
}

#ifdef WINDOWS_OS

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)

{

#else

int main(int argc, char *argv[])
{

#endif
	printf("Initial value of blank player party: %i\n", Player[0].party);

	hasher = new OPI_Hasher();

	hasher->storePassword("username", "password");
	printf("manual test print of hash result: %s\n", hasher->getPasswordHash().c_str());
	if (!hasher->verifyHash(toLower("pASsWoRD"), "Quq6He1Ku8vXTw4hd0cXeEZAw0nqbpwPxZn50NcOVbk="))
		return 1;

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		SDL_Quit();
		return 1;
	}
	std::ifstream optionFile("DAT/options.dat", std::ios::in | std::ios::binary | std::ios::ate);

	if (optionFile.is_open())
	{
		std::ifstream::pos_type size;
		//allocate memory
		size = optionFile.tellg();
		char * memblock = (char *)malloc(size);

		//load the file into memory
		optionFile.seekg(0, std::ios::beg);
		optionFile.read(memblock, size);
		//close file
		optionFile.close();

		//build an int from 4 bytes
		enableSND = (bool)(int)memblock[0];
		enableSFX = (bool)(int)memblock[1];
		enableMUS = (bool)(int)memblock[2];
		enableSIGN = (bool)(int)memblock[3];

		//fix memory leak
		free(memblock);
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("ERROR! COULD NOT INIT SOUND!\n");
		enableSND = false;
		enableMUS = false;
		enableSFX = false;
	}

	//chat line buffer
	for (int i = 0; i < NUM_CHAT_LINES; i++)
		chat_line[i] = new std::string("");

	//read config.ini file and set all options
	INIReader configFile("DAT/config.ini");
	if (configFile.ParseError() < 0) {

		if (configFile.ParseError() == -1)
			printf("file open error. TIP: Line endings must be CRLF!");

		if (configFile.ParseError() == -2)
			printf("memory allocation error.");

		printf("error: Can't load 'config.ini'\n");
		return 1;
	}
	SERVER_IP = configFile.Get("server", "hostname", "optimuspi.us");
	SERVER_PORT = configFile.GetInteger("server", "port", 1337);
	bool FULLSCREEN = configFile.GetBoolean("graphics", "fullscreen", false);

	printf("Connecting to %s:%i\n\n", SERVER_IP.c_str(), SERVER_PORT);

	setTitle();

	if (FULLSCREEN)
	{
		screen = SDL_SetVideoMode(1024, 600, 32, SDL_OPENGL | SDL_FULLSCREEN);
	}
	else
	{
		screen = SDL_SetVideoMode(1024, 600, 32, SDL_OPENGL);
	}

	screenOptions();

	if (screen == NULL) {
		SDL_Quit();
		exit(2);
	}

	loading_img.setImage("IMG/GUI/loading.png");
	background.setImage("IMG/GUI/loading.png");
	Graphics();

	//stickman sprite sheet
	stickman_sprite_img.setImage("IMG/SPRITES/stickman.png");
	skeleton_sprite_img.setImage("IMG/SPRITES/skeleton.png");

	//target imges
	target_img[TARGET_BULLSEYE].setImage("IMG/TARGET/bullseye.png");
	target_img[TARGET_KEG].setImage("IMG/TARGET/keg.png");
	target_img[TARGET_CRATE].setImage("IMG/TARGET/crate.png");


	closeChatButton.setBounds(687 + 150, 4 + 372, 10, 10);
	nextPageButton.setBounds(860, 372, 92, 43);
	leave_party_button.setBounds(224 + 652, 45, 148, 32);

	enable_auto_signs_button.setBounds(169, 393, 34, 17);
	disable_auto_signs_button.setBounds(206, 393, 34, 17);

	unmute_sound_button.setBounds(169, 417, 34, 17);
	mute_sound_button.setBounds(206, 417, 34, 17);

	unmute_effects_button.setBounds(169, 441, 34, 17);
	mute_effects_button.setBounds(206, 441, 34, 17);

	unmute_music_button.setBounds(169, 465, 34, 17);
	mute_music_button.setBounds(206, 465, 34, 17);


	login_button.setBounds(400, 220, 167, 65); nextPageButton.setBounds(874, 372, 92, 43);
	create_button.setBounds(400, 310, 167, 65);
	quit_button.setBounds(400, 400, 167, 65);
	okay_button.setBounds(428, 320, 167, 65);
	back_button.setBounds(428, 410, 167, 65);

	username_box.setBounds(362, 219, 300, 20);
	password_box.setBounds(362, 274, 200, 20);
	chatting_box.setBounds(0, 588, 1024, 11);

	equip_toggle_button.setBounds(256, 244, 16, 256);
	equip_button.setBounds(272, 244, 240, 256);

	inventory_button.setBounds(700, 515, 81, 64);
	stats_button.setBounds(700 + 81, 515, 81, 64);
	options_button.setBounds(700 + 81 + 81, 515, 81, 64);
	logout_button.setBounds(700 + 81 + 81 + 81, 515, 81, 64);

	hp_button.setBounds(8, 405, 53, 18);
	str_button.setBounds(8, 434, 53, 18);
	def_button.setBounds(8, 464, 53, 18);
	inv_item_button.setBounds(18, 253, 1024, 220);

	worldInteractButton.setBounds(0, 0, 1024, 600);
	playerRequestButton.setBounds(320, 400, 129, 90);

	//trade buttons
	tradeAcceptButton.setBounds(224 + 425, 481, 101, 14);
	tradeCancelButton.setBounds(224 + 528, 481, 101, 14);
	tradeIncrementButton.setBounds(224 + 430, 233, 194, 13);

	//inventory buttons
	drop_button.setBounds(168, 481, 38, 14);
	use_button.setBounds(207, 481, 38, 14);

	//pop up gui input box
	inputBoxButton.setBounds(256, 372, 256, 128);

	//bank buttons and that
	bankIncrementButton.setBounds(224 + 573, 254, 193, 13);
	bankAcceptButton.setBounds(224 + 553, 478, 101, 14);
	bankCancelButton.setBounds(224 + 700, 478, 101, 14);
	bankScrollButton.setBounds(224 + 653, 475, 40, 19);

	//shop buttons and that
	shopIncrementButton.setBounds(224 + 573, 254, 193, 13);
	shopCancelButton.setBounds(224 + 700, 478, 101, 14);
	shopToggleButton.setBounds(224 + 650, 477, 45, 16);
	shopAcceptButton.setBounds(224 + 553, 478, 101, 14);

	//sound toggle
	soundToggleButton.setBounds(992, 0, 32, 32);


	//enemy sprite
	EnemySprite[0] = SKO_Sprite("enemySprites", "bandit");
	EnemySprite[1] = SKO_Sprite("enemySprites", "hobo");
	EnemySprite[2] = SKO_Sprite("enemySprites", "banditboss");
	EnemySprite[3] = SKO_Sprite("enemySprites", "skeleton");
	EnemySprite[4] = SKO_Sprite("enemySprites", "skeletonguard");
	EnemySprite[5] = SKO_Sprite("enemySprites", "skeletonboss");
	EnemySprite[6] = SKO_Sprite("enemySprites", "skeletonhaunted");

	//npc sprite
	NpcSprite[0] = SKO_Sprite("npcSprites", "guard");

	//main menu images
	back_button_img.setImage("IMG/GUI/back_text.png");
	play_button_img.setImage("IMG/GUI/play_text.png");
	blank_button_img.setImage("IMG/GUI/blank_button.png");
	login_button_img.setImage("IMG/GUI/login_text.png");
	create_button_img.setImage("IMG/GUI/create_text.png");
	quit_button_img.setImage("IMG/GUI/quit_text.png");
	banner_img.setImage("IMG/GUI/banner.png");

	//sign bubble
	sign_bubble.setImage("IMG/GUI/sign_bubble.png");
	next_page.setImage("IMG/GUI/next_page.png");

	//char bar for username
	cred_bar.setImage("IMG/GUI/cred_bar.png");

	shadow.setImage("IMG/MISC/shadow.png");
	font.setImage("IMG/MISC/font.png");
	chatcursor.setImage("IMG/MISC/chatcursor.png");


	//hud
	stats_gui.setImage("IMG/GUI/stats_gui.png");
	hud_button_img.setImage("IMG/GUI/hud_button.png");
	hud_icon[0].setImage("IMG/GUI/inventory_button.png");
	hud_icon[1].setImage("IMG/GUI/stats_button.png");
	hud_icon[2].setImage("IMG/GUI/options_button.png");
	hud_icon[3].setImage("IMG/GUI/logout_button.png");

	options_img.setImage("IMG/GUI/credits.png");
	disconnect_img.setImage("IMG/GUI/disconnected.png");
	inventorySelectorBox.setImage("IMG/GUI/inventory_selector.png");
	mass_trade_selector.setImage("IMG/GUI/mass_trade_selector.png");
	option_selector.setImage("IMG/GUI/option_selector.png");
	bank_gui_img.setImage("IMG/GUI/bank_window.png");
	greyedImg.setImage("IMG/GUI/greyedImg.png");
	shop_gui_img.setImage("IMG/GUI/shop_window.png");
	chatBackImg.setImage("IMG/GUI/chat_back.png");
	equip_show_gui.setImage("IMG/GUI/equipment_show.png");
	equip_img.setImage("IMG/GUI/equipment.png");
	hover.setImage("IMG/GUI/hover.png");


	//hats n helmets
	hat_img[0].setImage("IMG/EQUIP/HAT/blue_party_hat.png");
	hat_img[1].setImage("IMG/EQUIP/HAT/santa_hat.png");
	hat_img[2].setImage("IMG/EQUIP/HAT/gold_party_hat.png");
	hat_img[3].setImage("IMG/EQUIP/HAT/nerd_glasses.png");
	hat_img[4].setImage("IMG/EQUIP/HAT/bunny_ears.png");
	hat_img[5].setImage("IMG/EQUIP/HAT/skull_mask.png");
	hat_img[6].setImage("IMG/EQUIP/HAT/bandana.png");
	hat_img[7].setImage("IMG/EQUIP/HAT/sunglasses.png");
	hat_img[8].setImage("IMG/EQUIP/HAT/bandana_mask.png");
	hat_img[9].setImage("IMG/EQUIP/HAT/halloween_mask.png");
	hat_img[10].setImage("IMG/EQUIP/HAT/guard_helm.png");
	hat_img[11].setImage("IMG/EQUIP/HAT/white_party_hat.png");
	hat_img[12].setImage("IMG/EQUIP/HAT/skeleton_helm.png");
	hat_img[13].setImage("IMG/EQUIP/HAT/training_helmet.png");
	hat_img[14].setImage("IMG/EQUIP/HAT/purple_party_hat.png");


	//trophies to show off in the player's off-hand. Can be thrown at other players.
	trophy_img[0].setImage("IMG/ITEM/cherry_pi.png");
	trophy_img[1].setImage("IMG/ITEM/easter_egg.png");
	trophy_img[2].setImage("IMG/ITEM/pumpkin.png");
	trophy_img[3].setImage("IMG/ITEM/icecream.png");
	trophy_img[4].setImage("IMG/ITEM/jack-o-lantern.png");
	trophy_img[5].setImage("IMG/ITEM/snowball.png");

	//weapon
	//todo make this better
	//but for now the equip id is on the server
	//and the id is here in brackets -1
	// so sword-training is equipid 1 and index [0]
	weapon_img[0].setImage("IMG/EQUIP/WEAPON/sword-training.png");
	weapon_img[1].setImage("IMG/EQUIP/WEAPON/dagger.png");
	weapon_img[2].setImage("IMG/EQUIP/WEAPON/sword-rusted.png");
	weapon_img[3].setImage("IMG/EQUIP/WEAPON/sword-steel.png");
	weapon_img[4].setImage("IMG/EQUIP/WEAPON/sword-gold.png");
	weapon_img[5].setImage("IMG/EQUIP/WEAPON/sword-crystal.png");
	weapon_img[6].setImage("IMG/EQUIP/WEAPON/axe-rusted.png");
	weapon_img[7].setImage("IMG/EQUIP/WEAPON/axe-steel.png");
	weapon_img[8].setImage("IMG/EQUIP/WEAPON/axe-gold.png");
	weapon_img[9].setImage("IMG/EQUIP/WEAPON/axe-crystal.png");
	weapon_img[10].setImage("IMG/EQUIP/WEAPON/hammer-rusted.png");
	weapon_img[11].setImage("IMG/EQUIP/WEAPON/hammer-steel.png");
	weapon_img[12].setImage("IMG/EQUIP/WEAPON/hammer-gold.png");
	weapon_img[13].setImage("IMG/EQUIP/WEAPON/hammer-crystal.png");
	weapon_img[14].setImage("IMG/EQUIP/WEAPON/scythe.png");
	weapon_img[15].setImage("IMG/EQUIP/WEAPON/reaper_scythe.png");
	weapon_img[16].setImage("IMG/EQUIP/WEAPON/candy_cane.png");


	//GUI elements
	xp_filler.setImage("IMG/GUI/xp_bar_filler.png");
	hp_filler.setImage("IMG/GUI/hp_bar_filler.png");
	inventory_gui.setImage("IMG/GUI/inventory.png");
	stats_popup.setImage("IMG/GUI/stats.png");
	options_popup.setImage("IMG/GUI/options.png");

	//Items
	Item[ITEM_GOLD] = SKO_Item(8, 8, 0, "Gold");
	Item[ITEM_GOLD_PHAT] = SKO_Item(10, 13, 0, "Gold Party Hat");
	Item[ITEM_MYSTERY_BOX] = SKO_Item(16, 15, 0, "Mystery Box");
	Item[ITEM_CHEESE] = SKO_Item(13, 20, 1, "Cheese");
	Item[ITEM_CARROT] = SKO_Item(15, 17, 1, "Carrot");
	Item[ITEM_BEER] = SKO_Item(9, 31, 2, "Beer");
	Item[ITEM_WINE] = SKO_Item(9, 31, 4, "Wine");
	Item[ITEM_POTION] = SKO_Item(10, 16, 5, "Health Potion");
	Item[ITEM_DIAMOND] = SKO_Item(15, 14, 100, "Diamond");
	Item[ITEM_SWORD_TRAINING] = SKO_Item(11, 31, 5, "Training Sword", 0, 1, 0);
	Item[ITEM_BLUE_PHAT] = SKO_Item(10, 13, 0, "Blue Party Hat");
	Item[ITEM_SANTA_HAT] = SKO_Item(17, 10, 0, "Santa Hat");
	Item[ITEM_NERD_GLASSES] = SKO_Item(21, 13, 0, "Nerd Glasses");
	Item[ITEM_CHERRY_PI] = SKO_Item(32, 28, 0, "Cherry Pi");
	Item[ITEM_BUNNY_EARS] = SKO_Item(14, 10, 0, "Bunny Ears");
	Item[ITEM_EASTER_EGG] = SKO_Item(8, 10, 0, "Easter Egg");
	Item[ITEM_SKULL_MASK] = SKO_Item(13, 18, 0, "Skull Mask");
	Item[ITEM_PUMPKIN] = SKO_Item(21, 23, 0, "Pumpkin");
	Item[ITEM_ICECREAM] = SKO_Item(9, 16, 0, "Icecream Cone");
	Item[ITEM_SUNGLASSES] = SKO_Item(21, 13, 0, "Sunglasses");
	Item[ITEM_SWORD_RUSTED] = SKO_Item(11, 32, 15, "Rusty Sword", 0, 2, 1);
	Item[ITEM_SWORD_STEEL] = SKO_Item(11, 32, 400, "Steel Sword", 0, 4, 2);
	Item[ITEM_SWORD_GOLD] = SKO_Item(11, 32, 8600, "Golden Sword", 0, 11, 3);
	Item[ITEM_SWORD_CRYSTAL] = SKO_Item(11, 32, 100300, "Crystal Sword", 0, 22, 5);
	Item[ITEM_AXE_RUSTED] = SKO_Item(12, 31, 15, "Rusty Hatchet", 0, 3, 1);
	Item[ITEM_AXE_STEEL] = SKO_Item(12, 31, 425, "Steel Hatchet", 0, 5, 1);
	Item[ITEM_AXE_GOLD] = SKO_Item(21, 32, 9600, "Golden Axe", 0, 13, 2);
	Item[ITEM_AXE_CRYSTAL] = SKO_Item(24, 32, 98525, "Crystal Axe", 0, 24, 3);
	Item[ITEM_HAMMER_RUSTED] = SKO_Item(17, 32, 10, "Rusty Hammer", 0, 4, 0);
	Item[ITEM_HAMMER_STEEL] = SKO_Item(17, 32, 90, "Steel Hammer", 0, 7, 0);
	Item[ITEM_HAMMER_GOLD] = SKO_Item(17, 32, 6120, "Golden Hammer", 0, 16, 2);
	Item[ITEM_HAMMER_CRYSTAL] = SKO_Item(29, 32, 99085, "Crystal Hammer", 0, 25, 3);
	Item[ITEM_SCYTHE] = SKO_Item(23, 25, 900, "Scythe", 0, 3, 3);
	Item[ITEM_SCYTHE_REAPER] = SKO_Item(27, 32, 15, "Reaper's Scythe", 10, 10, 10);
	Item[ITEM_HALLOWEEN_MASK] = SKO_Item(18, 21, 0, "Halloween Mask");
	Item[ITEM_GUARD_HELM] = SKO_Item(18, 21, 2500, "Guard Helmet", 10, 1, 5);
	Item[ITEM_JACK_OLANTERN] = SKO_Item(21, 23, 0, "Jack O' Lantern");
	Item[ITEM_WHITE_PHAT] = SKO_Item(10, 13, 0, "White Party Hat");
	Item[ITEM_CANDY_CANE] = SKO_Item(16, 31, 0, "Candy Cane", 3, 3, 3);
	Item[ITEM_SKELETON_HELM] = SKO_Item(18, 21, 500, "Skeleton Helm", 1, 2, 1);
	Item[ITEM_TRAINING_HELM] = SKO_Item(18, 21, 75, "Training Helm", 0, 0, 1);
	Item[ITEM_PURPLE_PHAT] = SKO_Item(10, 13, 0, "Purple Party Hat");
	Item[ITEM_SNOW_BALL] = SKO_Item(12, 12, 0, "Snowball");

	Item_img[ITEM_GOLD].setImage("IMG/ITEM/gold.png");
	Item_img[ITEM_GOLD_PHAT].setImage("IMG/ITEM/gold_party_hat.png");
	Item_img[ITEM_MYSTERY_BOX].setImage("IMG/ITEM/mystery_box.png");
	Item_img[ITEM_CHEESE].setImage("IMG/ITEM/cheese.png");
	Item_img[ITEM_CARROT].setImage("IMG/ITEM/carrot.png");
	Item_img[ITEM_BEER].setImage("IMG/ITEM/beer.png");
	Item_img[ITEM_WINE].setImage("IMG/ITEM/wine.png");
	Item_img[ITEM_POTION].setImage("IMG/ITEM/potion.png");
	Item_img[ITEM_DIAMOND].setImage("IMG/ITEM/diamond.png");
	Item_img[ITEM_SWORD_TRAINING].setImage("IMG/ITEM/sword-training.png");
	Item_img[ITEM_BLUE_PHAT].setImage("IMG/ITEM/blue_party_hat.png");
	Item_img[ITEM_SANTA_HAT].setImage("IMG/ITEM/santa_hat.png");
	Item_img[ITEM_NERD_GLASSES].setImage("IMG/ITEM/nerd_glasses.png");
	Item_img[ITEM_CHERRY_PI].setImage("IMG/ITEM/cherry_pi.png");
	Item_img[ITEM_BUNNY_EARS].setImage("IMG/ITEM/bunny_ears.png");
	Item_img[ITEM_EASTER_EGG].setImage("IMG/ITEM/easter_egg.png");
	Item_img[ITEM_SKULL_MASK].setImage("IMG/ITEM/skull_mask.png");
	Item_img[ITEM_PUMPKIN].setImage("IMG/ITEM/pumpkin.png");
	Item_img[ITEM_ICECREAM].setImage("IMG/ITEM/icecream.png");
	Item_img[ITEM_SUNGLASSES].setImage("IMG/ITEM/sunglasses.png");

	//added 10-30-2013
	Item_img[ITEM_SWORD_RUSTED].setImage("IMG/ITEM/sword-rusted.png");
	Item_img[ITEM_SWORD_STEEL].setImage("IMG/ITEM/sword-steel.png");
	Item_img[ITEM_SWORD_GOLD].setImage("IMG/ITEM/sword-gold.png");
	Item_img[ITEM_SWORD_CRYSTAL].setImage("IMG/ITEM/sword-crystal.png");

	Item_img[ITEM_AXE_RUSTED].setImage("IMG/ITEM/axe-rusted.png");
	Item_img[ITEM_AXE_STEEL].setImage("IMG/ITEM/axe-steel.png");
	Item_img[ITEM_AXE_GOLD].setImage("IMG/ITEM/axe-gold.png");
	Item_img[ITEM_AXE_CRYSTAL].setImage("IMG/ITEM/axe-crystal.png");

	Item_img[ITEM_HAMMER_RUSTED].setImage("IMG/ITEM/hammer-rusted.png");
	Item_img[ITEM_HAMMER_STEEL].setImage("IMG/ITEM/hammer-steel.png");
	Item_img[ITEM_HAMMER_GOLD].setImage("IMG/ITEM/hammer-gold.png");
	Item_img[ITEM_HAMMER_CRYSTAL].setImage("IMG/ITEM/hammer-crystal.png");

	///halloween event items
	Item_img[ITEM_SCYTHE].setImage("IMG/ITEM/scythe.png");
	Item_img[ITEM_SCYTHE_REAPER].setImage("IMG/ITEM/scythe_reaper.png");
	Item_img[ITEM_HALLOWEEN_MASK].setImage("IMG/ITEM/halloween_mask.png");
	Item_img[ITEM_GUARD_HELM].setImage("IMG/ITEM/guard_helmet.png");
	Item_img[ITEM_JACK_OLANTERN].setImage("IMG/ITEM/jack-o-lantern.png");

	//some more in jterm 2014 added (finally. quit slacking)
	Item_img[ITEM_WHITE_PHAT].setImage("IMG/ITEM/white_party_hat.png");
	Item_img[ITEM_CANDY_CANE].setImage("IMG/ITEM/candy_cane.png");
	Item_img[ITEM_SKELETON_HELM].setImage("IMG/ITEM/skeleton_helmet.png");


	// added on 2/10/2015
	Item_img[ITEM_TRAINING_HELM].setImage("IMG/ITEM/training_helmet.png");

	// added on 2/22/2015
	Item_img[ITEM_PURPLE_PHAT].setImage("IMG/ITEM/purple_party_hat.png");
	Item_img[ITEM_SNOW_BALL].setImage("IMG/ITEM/snowball.png");

	//gui popup
	popup_gui_input_img.setImage("IMG/GUI/popup_gui_amount.png");
	popup_gui_request_img.setImage("IMG/GUI/popup_gui_request.png");
	popup_gui_confirm_img.setImage("IMG/GUI/popup_gui_confirm.png");
	trade_gui_img.setImage("IMG/GUI/trade_window.png");

	//sound toggle
	soundon.setImage("IMG/MISC/soundon.png");
	soundoff.setImage("IMG/MISC/soundoff.png");


	//party buddy container
	buddy_container.setImage("IMG/GUI/buddy_container.png");
	leave_party_img.setImage("IMG/GUI/leave_party.png");

	//todo really? make a rotating image.
	//arrows
	arrow[0].setImage("IMG/GUI/arrow_ul.png");
	arrow[1].setImage("IMG/GUI/arrow_u.png");
	arrow[2].setImage("IMG/GUI/arrow_ur.png");
	arrow[3].setImage("IMG/GUI/arrow_r.png");
	arrow[4].setImage("IMG/GUI/arrow_dr.png");
	arrow[5].setImage("IMG/GUI/arrow_d.png");
	arrow[6].setImage("IMG/GUI/arrow_dl.png");
	arrow[7].setImage("IMG/GUI/arrow_l.png");

	//hp xp fillers for party
	party_filler[0].setImage("IMG/GUI/party_xp_bar_filler.png");
	party_filler[1].setImage("IMG/GUI/party_hp_bar_filler.png");

	//Text
	for (int i = 0; i < NUM_TEXT; i++) {
		Message[i] = OPI_Text();
	}

	// enemy hp bars
	enemy_hp.setImage("IMG/MISC/enemy_hp.png");

	// tiled background
	back_img[0].setImage("IMG/MISC/sky_back_new.png");
	back_img[1].setImage("IMG/MISC/far_back.png");
	back_img[2].setImage("IMG/MISC/med_back.png");
	back_img[3].setImage("IMG/MISC/close_back.png");

	TILES_WIDE = (1024 / back_img[1].w) + 2;
	TILES_TALL = (600 / back_img[1].h) + 2;

	//Message[0],[1]
	Text(370, 530, "", 240, 250, 250);
	Text(345, 555, "", 240, 250, 250);
	Message[0].SetText("Loading...");
	Message[1].SetText("Attempting to connect to the server...");

	construct_frame();
	SDL_GL_SwapBuffers();


	//Message[2] what you type
	Text(2, 585, "", 0, 0, 0);
	//Message[2].SetText(tMessage);
	tMessage[0] = ' ';
	uMessage[0] = ' ';
	pMessage[0] = ' ';
	//Message[3],[4],[5],[6], [7], [8], [9], [10], [11], [12], [13], [14] data from the server
	Text(6, 8, "", 245, 255, 250);
	Text(6, 19, "", 245, 255, 250);
	Text(6, 30, "", 245, 255, 250);
	Text(6, 41, "", 245, 255, 250);
	Text(6, 52, "", 245, 255, 250);
	Text(6, 63, "", 245, 255, 250);
	Text(6, 74, "", 245, 255, 250);
	Text(6, 85, "", 245, 255, 250);
	Text(6, 96, "", 245, 255, 250);
	Text(6, 107, "", 245, 255, 250);
	Text(6, 118, "", 245, 255, 250);
	Text(6, 129, "", 245, 255, 250);


	//ping ([15])
	Text(840, 20, "Ping:", 180, 210, 100);

	//username ([16])
	Text(364, 223, " ", 250, 250, 255);
	//password ([17])
	Text(364, 278, " ", 250, 250, 255);



	//username tags[18]-[49]
	for (int i = 0; i < 32; i++) {
		Text(20, 100, "", 255, 255, 255);
	}

	// Coords ([50])
	Text(840, 7, "Coords: (0,0)", 190, 210, 100);

	//xp[51], hp[52]
	Text(224 + 135, 520, "0/10", 255, 255, 255);
	Text(224 + 135, 548, "0/10", 255, 255, 255);

	//level[53]
	Text(750, 7, "Level:  0", 50, 225, 225);
	//stats[54]
	Text(750, 20, "Points: 0", 158, 5, 160);

	//inventory [55]-[78]
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 6; x++)
			Text(12 + x * 39, 291 + y * 56, "", 255, 255, 255);

	//stats_popup [79]-[82]
	Text(61, 378, "0", 255, 255, 255);
	Text(61, 407, "0", 255, 255, 255);
	Text(61, 436, "0", 255, 255, 255);
	Text(61, 466, "0", 255, 255, 255);

	//inventory selected item [83], [84]
	Text(10, 482, "0", 255, 255, 255);
	Text(155, 482, "0", 255, 255, 255);


	//popup gui input text box [85], [86], [87], [88]
	Text(300, 392, "0", 255, 255, 255); //message
	Text(308, 427, "0", 255, 255, 255); //input
	Text(407, 461, "cancel", 255, 255, 255); // cancel
	Text(309, 461, "okay", 255, 255, 255); // okay

	inputBox.outputText = &Message[85];
	inputBox.inputText = &Message[86];
	inputBox.cancelText = &Message[87];
	inputBox.okayText = &Message[88];

	//popup gui player action request [89]
	Text(300, 380, "Invite X to...", 255, 255, 255);


	//local trade
	//inventory [90]-[113]
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 6; x++)
			Text(224 + 268 + x * 39, 291 + y * 56, "", 255, 255, 255);

	//remote trade
	//inventory [114]-[137]
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 6; x++)
			Text(224 + 556 + x * 39, 291 + y * 56, "", 255, 255, 255);



	//local trade descriptor [138], [139]
	Text(224 + 266, 482, "0", 255, 255, 255);
	Text(224 + 411, 482, "0", 255, 255, 255);
	//remote trade descriptor [140], [141]
	Text(224 + 635, 482, "0", 255, 255, 255);
	Text(224 + 780, 482, "0", 255, 255, 255);

	//local ready/accept text and remote [142], [143]
	Text(224 + 340, 235, "Waiting...", 255, 255, 255);
	Text(224 + 628, 235, "Waiting...", 255, 255, 255);

	//bank item amounts
   //bank [144]-[161]
	for (int y = 0; y < 3; y++)
		for (int x = 0; x < 6; x++)
			Text(224 + 556 + x * 39, 313 + y * 56, "none", 255, 255, 255);

	//equipment window
	// hp, sp, dp [162-164]
	Text(333, 380, "+", 20, 160, 20);
	Text(333, 408, "+", 20, 160, 20);
	Text(333, 438, "+", 20, 160, 20);

	//descriptor [165]
	Text(294, 482, "???", 255, 255, 255);

	//buddy name [166-170]
	for (int b = 0; b < 5; b++)
		Text(224 + 638, 86 + 65 * b, "", 0, 0, 0);

	//buddy coords [171-175]
	for (int b = 0; b < 5; b++)
		Text(224 + 652, 127 + 65 * b, "", 0, 0, 0);

	//item hover window [176][177][178][179]
	Text(768 - 89 + 30, 256, "+0", 0, 0, 0);
	Text(768 - 89 + 30, 284, "+100", 0, 0, 0);
	Text(768 - 89 + 30, 314, "+10", 0, 0, 0);
	Text(768 - 89 + 30, 338, "142009", 0, 0, 0);

	//initialize all the clients
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		Player[i] = SKO_Player();
	}
	printf("After SKO_Player(): Player[0] party is: %i\n", Player[0].party);

	loadContent();

	if (enableSND && enableMUS)
	{
		if (music != NULL && Mix_PlayMusic(music, -1) == -1)
			printf("ERROR PLAYING MUSIC!!");
		else
			printf("music played ok.");
	}

	printf("Starting network thread . . . \n");
	std::thread networkThread(Network);

	OPI_Sleep::milliseconds(100);

	printf("Starting input and graphics . . . \n");
	PhysicsLoop();

	printf("done == true. joining threads.\r\n");
	networkThread.join();

	printf("Quitting Stick Knights Online.");
	SDL_Quit();
	return 0;
}//end main

void Graphics()
{
	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//graphics
	construct_frame();
	SDL_GL_SwapBuffers();
}

bool ConnectToSKOServer()
{
	if (Client.init(SERVER_IP, SERVER_PORT) == "error")
	{
		printf("FATAL: Client.init(SERVER_IP, SERVER_PORT) == \"error\"\n");
		Message[0].SetText("There may be a problem with your internet connection.");
		Message[1].SetText(" Join chat for help. www.StickKnightsOnline.com/chat");
		Graphics();
		fatalNetworkError = true;
		return true;
	}

	if (Client.connect() == "error")
	{
		printf("FATAL: Client.connect() == \"error\"\n");
		Message[0].SetText("There was an error connecting to the server!");
		Message[1].SetText("");
		Graphics();
		return true;
	}

	Message[0].SetText("Connected!");
	Message[1].SetText("Attempting to verify client version...");

	Client.sendVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
	return false;
}

int numDigits(int num)
{
	if (num == 0)
		return 1;// 0 is still 1 digit

	int count = 0;
	while (num)
	{
		num /= 10;
		++count;
	}
	return count;
}

void physics()
{
	//always scroll sky
	if (menu < 4)
		scroll_sky();

	if (menu != STATE_PLAY)
		return;

	//enemies
	for (int i = 0; i < map[current_map]->num_enemies; i++)
	{
		//turn off flash effect
		if (map[current_map]->Enemy[i].hit && OPI_Clock::milliseconds() - map[current_map]->Enemy[i].hit_ticker > 100)
		{
			map[current_map]->Enemy[i].hit = false;
		}

		//fall
		if (map[current_map]->Enemy[i].y_speed < 10)
			map[current_map]->Enemy[i].y_speed += GRAVITY;

		map[current_map]->Enemy[i].ground = true;

		//verical collision detection
		bool block_y = blocked(
			map[current_map]->Enemy[i].x +
			EnemySprite[map[current_map]->Enemy[i].sprite].x1,

			map[current_map]->Enemy[i].y +
			map[current_map]->Enemy[i].y_speed +
			EnemySprite[map[current_map]->Enemy[i].sprite].y1,

			map[current_map]->Enemy[i].x +
			EnemySprite[map[current_map]->Enemy[i].sprite].x2,

			map[current_map]->Enemy[i].y +
			map[current_map]->Enemy[i].y_speed +
			EnemySprite[map[current_map]->Enemy[i].sprite].y2);

		//vertical movement
		if (!block_y)
		{//not blocked, fall
			//animation
			map[current_map]->Enemy[i].ground = false;
			map[current_map]->Enemy[i].y += map[current_map]->Enemy[i].y_speed;
		}
		else
		{  //blocked, stop
			if (map[current_map]->Enemy[i].y_speed > 0)
			{
				map[current_map]->Enemy[i].y_speed = 1;

				//todo see if if or while is better
				for (int loopVar = 0; loopVar < HIT_LOOPS && (!blocked(map[current_map]->Enemy[i].x +
					EnemySprite[map[current_map]->Enemy[i].sprite].x1,
					map[current_map]->Enemy[i].y +
					map[current_map]->Enemy[i].y_speed +
					EnemySprite[map[current_map]->Enemy[i].sprite].y1,
					map[current_map]->Enemy[i].x +
					EnemySprite[map[current_map]->Enemy[i].sprite].x2,
					map[current_map]->Enemy[i].y +
					map[current_map]->Enemy[i].y_speed +
					EnemySprite[map[current_map]->Enemy[i].sprite].y1 +
					EnemySprite[map[current_map]->Enemy[i].sprite].y2)); loopVar++)
					map[current_map]->Enemy[i].y +=
					map[current_map]->Enemy[i].y_speed;

				map[current_map]->Enemy[i].y = (int)(map[current_map]->Enemy[i].y);
			}
			if (map[current_map]->Enemy[i].y_speed < 0)
			{
				map[current_map]->Enemy[i].ground = false;
				map[current_map]->Enemy[i].y_speed = -1;

				//todo test if while or if is better
				for (int loopVar = 0; loopVar < HIT_LOOPS && (!blocked(map[current_map]->Enemy[i].x +
					EnemySprite[map[current_map]->Enemy[i].sprite].x1,
					map[current_map]->Enemy[i].y +
					map[current_map]->Enemy[i].y_speed +
					EnemySprite[map[current_map]->Enemy[i].sprite].y1,
					map[current_map]->Enemy[i].x +
					EnemySprite[map[current_map]->Enemy[i].sprite].x2,
					map[current_map]->Enemy[i].y +
					map[current_map]->Enemy[i].y_speed +
					EnemySprite[map[current_map]->Enemy[i].sprite].y1 +
					EnemySprite[map[current_map]->Enemy[i].sprite].y2)); loopVar++)
					map[current_map]->Enemy[i].y += map[current_map]->Enemy[i].y_speed;
			}

			map[current_map]->Enemy[i].y_speed = 1;

		}

		//horizontal collision detection
		bool block_x = blocked(
			map[current_map]->Enemy[i].x +
			map[current_map]->Enemy[i].x_speed +
			EnemySprite[map[current_map]->Enemy[i].sprite].x1,
			map[current_map]->Enemy[i].y +
			EnemySprite[map[current_map]->Enemy[i].sprite].y1,
			map[current_map]->Enemy[i].x +
			map[current_map]->Enemy[i].x_speed +
			EnemySprite[map[current_map]->Enemy[i].sprite].x2,
			map[current_map]->Enemy[i].y +
			EnemySprite[map[current_map]->Enemy[i].sprite].y2);

		//horizontal movement
		if (!block_x)
		{//not blocked, walk
			map[current_map]->Enemy[i].x += map[current_map]->Enemy[i].x_speed;
		}
		else
			map[current_map]->Enemy[i].x_speed = 0;



		//enemy animation
		//animationmap[current_map]->Enemy[i]
		if ((map[current_map]->Enemy[i].x_speed != 0 && map[current_map]->Enemy[i].ground) || map[current_map]->Enemy[i].attacking)
		{
			//if it is time to change the frame
			if (!map[current_map]->Enemy[i].attacking && OPI_Clock::milliseconds() - map[current_map]->Enemy[i].animation_ticker >= animation_speed)
			{
				map[current_map]->Enemy[i].current_frame++;

				if (map[current_map]->Enemy[i].current_frame >= 6)
					map[current_map]->Enemy[i].current_frame = 0;

				map[current_map]->Enemy[i].animation_ticker = OPI_Clock::milliseconds();
			}
			//if it is time to change the frame
			if (map[current_map]->Enemy[i].attacking && OPI_Clock::milliseconds() - map[current_map]->Enemy[i].attack_ticker >= attack_speed / 2.0)
			{
				map[current_map]->Enemy[i].current_frame++;

				if (map[current_map]->Enemy[i].current_frame >= 6)
				{
					map[current_map]->Enemy[i].current_frame = 0;
					if (map[current_map]->Enemy[i].attacking)
						map[current_map]->Enemy[i].attacking = false;
				}
				map[current_map]->Enemy[i].attack_ticker = OPI_Clock::milliseconds();
			}
		}
		else
			map[current_map]->Enemy[i].current_frame = 0;
	}
	//end enemies

	//npcs
	for (int i = 0; i < map[current_map]->num_npcs; i++)
	{
		//fall
		if (map[current_map]->NPC[i].y_speed < 10)
			map[current_map]->NPC[i].y_speed += GRAVITY;

		map[current_map]->NPC[i].ground = true;

		//verical collision detection
		bool block_y = blocked(
			map[current_map]->NPC[i].x +
			NpcSprite[map[current_map]->NPC[i].sprite].x1,

			map[current_map]->NPC[i].y +
			map[current_map]->NPC[i].y_speed +
			NpcSprite[map[current_map]->NPC[i].sprite].y1,

			map[current_map]->NPC[i].x +
			NpcSprite[map[current_map]->NPC[i].sprite].x2,

			map[current_map]->NPC[i].y +
			map[current_map]->NPC[i].y_speed +
			NpcSprite[map[current_map]->NPC[i].sprite].y2);

		//vertical movement
		if (!block_y)
		{//not blocked, fall

		   //animation
			map[current_map]->NPC[i].ground = false;

			map[current_map]->NPC[i].y += map[current_map]->NPC[i].y_speed;

		}
		else
		{  //blocked, stop
			if (map[current_map]->NPC[i].y_speed > 0)
			{
				map[current_map]->NPC[i].y_speed = 1;

				//todo see if if or while is better
				for (int loopVar = 0; loopVar < HIT_LOOPS && (!blocked(map[current_map]->NPC[i].x +
					NpcSprite[map[current_map]->NPC[i].sprite].x1,
					map[current_map]->NPC[i].y +
					map[current_map]->NPC[i].y_speed +
					NpcSprite[map[current_map]->NPC[i].sprite].y1,
					map[current_map]->NPC[i].x +
					NpcSprite[map[current_map]->NPC[i].sprite].x2,
					map[current_map]->NPC[i].y +
					map[current_map]->NPC[i].y_speed +
					NpcSprite[map[current_map]->NPC[i].sprite].y1 +
					NpcSprite[map[current_map]->NPC[i].sprite].y2)); loopVar++)
					map[current_map]->NPC[i].y +=
					map[current_map]->NPC[i].y_speed;

				map[current_map]->NPC[i].y = (int)(map[current_map]->NPC[i].y);
			}
			if (map[current_map]->NPC[i].y_speed < 0)
			{
				map[current_map]->NPC[i].ground = false;
				map[current_map]->NPC[i].y_speed = -1;

				//todo test if while or if is better
				for (int loopVar = 0; loopVar < HIT_LOOPS && (!blocked(map[current_map]->NPC[i].x +
					NpcSprite[map[current_map]->NPC[i].sprite].x1,
					map[current_map]->NPC[i].y +
					map[current_map]->NPC[i].y_speed +
					NpcSprite[map[current_map]->NPC[i].sprite].y1,
					map[current_map]->NPC[i].x +
					NpcSprite[map[current_map]->NPC[i].sprite].x2,
					map[current_map]->NPC[i].y +
					map[current_map]->NPC[i].y_speed +
					NpcSprite[map[current_map]->NPC[i].sprite].y1 +
					NpcSprite[map[current_map]->NPC[i].sprite].y2)); loopVar++)
					map[current_map]->NPC[i].y += map[current_map]->NPC[i].y_speed;
			}

			map[current_map]->NPC[i].y_speed = 1;

		}

		//horizontal collision detection
		bool block_x = blocked(
			map[current_map]->NPC[i].x +
			map[current_map]->NPC[i].x_speed +
			NpcSprite[map[current_map]->NPC[i].sprite].x1,
			map[current_map]->NPC[i].y +
			NpcSprite[map[current_map]->NPC[i].sprite].y1,
			map[current_map]->NPC[i].x +
			map[current_map]->NPC[i].x_speed +
			NpcSprite[map[current_map]->NPC[i].sprite].x2,
			map[current_map]->NPC[i].y +
			NpcSprite[map[current_map]->NPC[i].sprite].y2);

		//horizontal movement
		if (!block_x)
		{//not blocked, walk
			map[current_map]->NPC[i].x += map[current_map]->NPC[i].x_speed;
		}
		else
			map[current_map]->NPC[i].x_speed = 0;




		//NPC animation
		//animationmap[current_map]->NPC[i]
		if ((map[current_map]->NPC[i].x_speed != 0 && map[current_map]->NPC[i].ground))
		{
			//if it is time to change the frame
			if (OPI_Clock::milliseconds() - map[current_map]->NPC[i].animation_ticker >= animation_speed)
			{
				map[current_map]->NPC[i].current_frame++;

				if (map[current_map]->NPC[i].current_frame >= 6)
					map[current_map]->NPC[i].current_frame = 0;

				map[current_map]->NPC[i].animation_ticker = OPI_Clock::milliseconds();
			}

		}
		else
			map[current_map]->NPC[i].current_frame = 0;
	}
	//end npcs

	//players
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (Player[i].Status && Player[i].current_map == current_map)
		{
			//turn off flash effect
			if (Player[i].hit && OPI_Clock::milliseconds() - Player[i].hit_ticker > 100)
			{
				Player[i].hit = false;
			}

			//fall
			if (Player[i].y_speed < 10) {
				Player[i].y_speed += GRAVITY;
			}

			Player[i].ground = true;

			//verical collision detection
			bool block_y = blocked(Player[i].x + 25, Player[i].y + Player[i].y_speed + 13, Player[i].x + 38, Player[i].y + Player[i].y_speed + 64);


			//vertical movement
			if (!block_y)
			{//not blocked, fall

			   //animation
				Player[i].ground = false;

				Player[i].y += Player[i].y_speed;

				if (i == MyID) {
					//move tiles background
					if (camera_y > 0)
					{
						back_offsety[1] -= (Player[i].y_speed) / 3.3;
						back_offsety[2] -= (Player[i].y_speed) / 2.2;
						back_offsety[3] -= (Player[i].y_speed) / 1.75;
						back_offsety[1] -= 0.01;
					}

					if (back_offsety[1] > back_img[1].h)
						back_offsety[1] -= back_img[1].h;

					if (back_offsety[1] < -back_img[1].h)
						back_offsety[1] += back_img[1].h;

					back_offsety[2] += 0.015;

					if (back_offsety[2] > back_img[2].h)
						back_offsety[2] -= back_img[2].h;

					if (back_offsety[2] < -back_img[2].h)
						back_offsety[2] += back_img[2].h;

					back_offsety[3] -= 0.01;

					if (back_offsety[3] > back_img[3].h)
						back_offsety[3] -= back_img[3].h;

					if (back_offsety[3] < -back_img[3].h)
						back_offsety[3] += back_img[3].h;
				}

			}
			else
			{  //blocked, stop
				if (Player[i].y_speed > 0)
				{
					Player[i].y_speed = 1;

					//todo see if while or if is better
					for (int loopVar = 0; loopVar < HIT_LOOPS &&
						(!blocked(Player[i].x + 25, Player[i].y + Player[i].y_speed + 13, Player[i].x + 38, Player[i].y + Player[i].y_speed + 64))
						; loopVar++)
						Player[i].y += Player[i].y_speed;

					Player[i].y = (int)(Player[i].y);
				}
				else if (Player[i].y_speed < 0)
				{
					Player[i].ground = false;
					Player[i].y_speed = -1;

					//todo see if while or if is better
					//trying out for 3 loops
					for (int loopVar = 0; loopVar < HIT_LOOPS &&
						(!blocked(Player[i].x + 25, Player[i].y + Player[i].y_speed + 13, Player[i].x + 38, Player[i].y + Player[i].y_speed + 64))
						; loopVar++)
						Player[i].y += Player[i].y_speed;
				}
				Player[i].y_speed = 1;
			}

			//horizontal collision detection
			bool block_x = blocked(Player[i].x + Player[i].x_speed + 23, Player[i].y + 13, Player[i].x + Player[i].x_speed + 40, Player[i].y + 64);

			//horizontal movement
			if (!block_x)
			{//not blocked, walk
				Player[i].x += (Player[i].x_speed);

				//move tiles background
				if (i == MyID) {

					back_offsetx[1] -= 0.02;
					back_offsetx[2] -= 0.05;
					back_offsetx[3] -= 0.1;

					if (camera_x > 0)
					{

						back_offsetx[1] -= (Player[i].x_speed) / 3.3;
						back_offsetx[2] -= (Player[i].x_speed) / 2.2;
						back_offsetx[3] -= (Player[i].x_speed) / 1.75;
					}

					if (back_offsetx[1] > back_img[1].w)
						back_offsetx[1] -= back_img[1].w;

					if (back_offsetx[1] < -back_img[1].w)
						back_offsetx[1] += back_img[1].w;

					if (back_offsetx[2] > back_img[2].w)
						back_offsetx[2] -= back_img[2].w;

					if (back_offsetx[2] < -back_img[2].w)
						back_offsetx[2] += back_img[2].w;


					if (back_offsetx[3] > back_img[3].w)
						back_offsetx[3] -= back_img[3].w;

					if (back_offsetx[3] < -back_img[3].w)
						back_offsetx[3] += back_img[3].w;
				}
			}

			//yourself
			if (i == MyID)
			{
				//if walking and player performs another action such as 
				if (LEFT && !Player[MyID].attacking && Player[MyID].x_speed != -2)
				{
					//send action to SKO network
					Client.playerAction("left", Player[MyID].x, Player[MyID].y);

					//Client prediction physics
					Player[MyID].x_speed = -WALK_SPEED;
					Player[MyID].facing_right = false;
				}
				if (RIGHT && !Player[MyID].attacking && Player[MyID].x_speed != 2)
				{
					//send action to SKO network
					Client.playerAction("right", Player[MyID].x, Player[MyID].y);

					//Client prediction physics
					Player[MyID].x_speed = WALK_SPEED;
					Player[MyID].facing_right = true;
				}

				if (LEFT == RIGHT)
					Player[i].x_speed = 0;

				//cosmetic updates
				if (OPI_Clock::milliseconds() - cosmeticTicker >= cosmeticTime)
				{
					//coordinates
					std::stringstream ss;
					ss << (int)current_map;
					ss << ": (";
					ss << (int)Player[i].x;
					ss << ",";
					ss << (int)Player[i].y;
					ss << ")";
					std::string cs = "Map " + ss.str();
					Message[50].SetText(cs);

					//hp, xp
					std::stringstream ss_hp, ss_xp;
					ss_hp << (int)Player[MyID].hp;
					ss_hp << "/";
					ss_hp << (int)Player[MyID].max_hp;
					ss_xp << (int)Player[MyID].xp;
					ss_xp << "/";
					ss_xp << (int)Player[MyID].max_xp;
					std::string s_hp = "" + ss_hp.str();
					std::string s_xp = "" + ss_xp.str();
					Message[51].SetText(s_xp);
					Message[52].SetText(s_hp);

					//stats
					//stat points
					std::stringstream ss0;
					std::string sp_string;
					ss0 << (unsigned int)Player[MyID].stat_points;
					sp_string = "" + ss0.str();
					Message[79].SetText(sp_string);

					std::stringstream ss00;
					ss00 << "Points: ";
					ss00 << (unsigned int)Player[MyID].stat_points;
					Message[54].SetText(ss00.str());

					//regen
					std::stringstream ss1;
					std::string hp_string;
					ss1 << (unsigned int)Player[MyID].regen;
					hp_string = "" + ss1.str();
					Message[80].SetText(hp_string);

					//attack
					std::stringstream ss2;
					std::string ap_string;
					ss2 << (unsigned int)Player[MyID].strength;
					ap_string = "" + ss2.str();
					Message[81].SetText(ap_string);

					//defend
					std::stringstream ss3;
					std::string dp_string;
					ss3 << (unsigned int)Player[MyID].defense;
					dp_string = "" + ss3.str();
					Message[82].SetText(dp_string);


					//correct placement

					Message[51].pos_x = 250 - numDigits((int)Player[MyID].xp) * 11;
					Message[52].pos_x = 250 - numDigits((int)Player[MyID].hp) * 11;

					//level
					std::stringstream ss_lev;
					ss_lev << "Level:  ";
					ss_lev << (int)Player[MyID].level;
					std::string s_lev = "" + ss_lev.str();
					Message[53].SetText(s_lev);


					if (popup_gui_menu == 7)
					{

						std::stringstream stat_str;

						//hp bonus stats
						stat_str.str("");
						stat_str << "+" << Item[Player[MyID].equipI[equipmentSlot]].hp;
						Message[162].SetText(stat_str.str());

						//sp bonus stats
						stat_str.str("");
						stat_str << "+" << Item[Player[MyID].equipI[equipmentSlot]].sp;
						Message[163].SetText(stat_str.str());


						//dp bonus stats
						stat_str.str("");
						stat_str << "+" << Item[Player[MyID].equipI[equipmentSlot]].dp;
						Message[164].SetText(stat_str.str());


						//descriptor [165]
						stat_str.str("");
						stat_str << Item[Player[MyID].equipI[equipmentSlot]].descr;
						Message[165].SetText(stat_str.str());

					}

					//party
					int buddy = 0;
					for (int pp = 0; pp < MAX_CLIENTS; pp++)
					{
						if (Player[pp].Status)
						{
							if (Player[MyID].party >= 0 && Player[pp].party == Player[MyID].party)
							{
								printf("Player party is: %i\n", Player[MyID].party);
								Player[MyID].nametag.R = 0.2;
								Player[MyID].nametag.G = 0.9;
								Player[MyID].nametag.B = 0.2;

								if (MyID != pp)
								{
									//set username in buddy container
									std::stringstream buddyName;
									buddyName << Player[pp].Nick;
									buddyName << " ";
									buddyName << (int)Player[pp].level;
									Message[166 + buddy].SetText(buddyName.str());

									//set user coords in buddy container
									std::stringstream buddyCoords;
									buddyCoords << "(";
									buddyCoords << (int)Player[pp].x;
									buddyCoords << ",";
									buddyCoords << (int)Player[pp].y;
									buddyCoords << ")";
									Message[171 + buddy].SetText(buddyCoords.str());

									/* set the arrow direction */
									int left = (int)(Player[MyID].x - Player[pp].x);
									int right = (int)(Player[pp].x - Player[MyID].x);
									int down = (int)(Player[pp].y - Player[MyID].y);
									int up = (int)(Player[MyID].y - Player[pp].y);
									int d = 0;

									if (up >= 0 && left >= 0)
									{
										d = 1;
										if (0.41421356237 * up - left < 0)
										{
											if (up < left)
												d = 7;
											else
												d = 0;
										}
									}
									if (up >= 0 && right >= 0)
									{
										d = 1;
										if (0.41421356237 * up - right < 0)
										{
											if (up < right)
												d = 2;
											else
												d = 3;
										}
									}
									if (down >= 0 && left >= 0)
									{
										d = 5;
										if (0.41421356237 * down - left < 0)
										{
											if (down < left)
												d = 7;
											else
												d = 6;
										}
									}
									if (down >= 0 && right >= 0)
									{
										d = 5;
										if (0.41421356237 * down - right < 0)
										{
											if (down < right)
												d = 3;
											else
												d = 4;
										}
									}


									Player[pp].arrow = d;
									/* done pointing arrow */

									buddy++;
								}
							}
							else //(not in party)
							{
								Player[MyID].nametag.R = 1;
								Player[MyID].nametag.G = 1;
								Player[MyID].nametag.B = 1;
							}
						}//end ident
					}


					cosmeticTicker = OPI_Clock::milliseconds();
				}
			}




			//animation
			if ((Player[i].x_speed != 0 && Player[i].ground) || Player[i].attacking)
			{
				//if it is time to change the frame
				if (!Player[i].attacking && OPI_Clock::milliseconds() - Player[i].animation_ticker >= animation_speed)
				{
					Player[i].current_frame++;

					if (Player[i].current_frame >= 6)
						Player[i].current_frame = 0;

					Player[i].animation_ticker = OPI_Clock::milliseconds();
				}

				//if it is time to change the frame
				if (Player[i].attacking && OPI_Clock::milliseconds() - Player[i].attack_ticker >= attack_speed)
				{
					Player[i].current_frame++;

					if (Player[i].current_frame >= 6)
					{
						Player[i].current_frame = 0;
						if (Player[i].attacking)
							Player[i].attacking = false;
					}
					Player[i].attack_ticker = OPI_Clock::milliseconds();
				}
			}
			else // reset to standing frame!
				Player[i].current_frame = 0;

		}//end (Player[i].Status && Player[i].current_map == current_map)
	}
	//end players

	//items
	for (int i = 0; i < 256; i++)
	{
		if (map[current_map]->ItemObj[i].status)
		{
			// printf("(%.2f,%.2f)\n", map[current_map]->ItemObj[i].x, map[current_map]->ItemObj[i].y);
				//horizontal collision detection
			bool block_x = blocked(
				map[current_map]->ItemObj[i].x + map[current_map]->ItemObj[i].x_speed,
				map[current_map]->ItemObj[i].y,
				map[current_map]->ItemObj[i].x + map[current_map]->ItemObj[i].x_speed + Item[(int)map[current_map]->ItemObj[i].itemID].w,
				map[current_map]->ItemObj[i].y + Item[(int)map[current_map]->ItemObj[i].itemID].h);

			if (map[current_map]->ItemObj[i].y_speed < 10)
				map[current_map]->ItemObj[i].y_speed += GRAVITY;

			//verical collision detection
			bool block_y = blocked(
				map[current_map]->ItemObj[i].x + map[current_map]->ItemObj[i].x_speed,
				map[current_map]->ItemObj[i].y + map[current_map]->ItemObj[i].y_speed,
				map[current_map]->ItemObj[i].x + Item[(int)map[current_map]->ItemObj[i].itemID].w,
				map[current_map]->ItemObj[i].y + map[current_map]->ItemObj[i].y_speed + Item[(int)map[current_map]->ItemObj[i].itemID].h);


			//vertical movement
			if (!block_y)
				map[current_map]->ItemObj[i].y += map[current_map]->ItemObj[i].y_speed;
			else {
				map[current_map]->ItemObj[i].y_speed = 0;
				map[current_map]->ItemObj[i].x_speed *= 0.5;
			}


			//horizontal movement
			if (!block_x)
			{//not blocked, move
				map[current_map]->ItemObj[i].x += map[current_map]->ItemObj[i].x_speed;
			}
			else {
				map[current_map]->ItemObj[i].x_speed = 0;
			}

		}
	}
	//end items

	//Sign reading collision detection
	if (enableSIGN && (Player[MyID].x_speed != 0 || Player[MyID].y_speed != 1))
	{
		for (int i = 0; i < map[current_map]->num_signs; i++)
		{
			float rangeX = (std::abs)(Player[MyID].x - map[current_map]->Sign[i].x + map[current_map]->Sign[i].w / 2.0);
			float rangeY = (std::abs)(Player[MyID].y - map[current_map]->Sign[i].y + map[current_map]->Sign[i].h / 2.0);
			float distance = sqrt((rangeX*rangeX) + (rangeY*rangeY));
			bool inRange;
			inRange = (distance < 32);
			if (inRange) {
				bool isNew = ((current_sign.hasBeenClosed) == (map[current_map]->Sign[i].hasBeenClosed));
				if (isNew) {
					if (popup_sign == false) {
						current_sign = map[current_map]->Sign[i];
					}
				}
				if (!current_sign.triggered && !(current_sign.hasBeenClosed)) {
					if (popup_sign == false) {
						current_sign.triggered = true;
						popup_menu = 0;
						popup_sign = true;
						popup_npc = false;
					}
				}
			}
		}

		float rangeX = (std::abs)(Player[MyID].x - current_sign.x + current_sign.w / 2.0);
		float rangeY = (std::abs)(Player[MyID].y - current_sign.y + current_sign.h / 2.0);
		float distance = sqrt((rangeX*rangeX) + (rangeY*rangeY));
		bool inRange = (distance < 48);
		if (!inRange && current_sign.triggered) {
			popup_sign = false;
			current_sign.hasBeenClosed = false;
		}
		if (distance > 48) {
			current_sign.triggered = false;
			current_sign.hasBeenClosed = false;
		}

	}
}

void Disconnect()
{
	//Disconnection screen
	popup_menu = -1;
	popup_gui_menu = -1;
	menu = STATE_DISCONNECT;
	Graphics();

	//Try reconnecting for 10 seconds
	if (Client.TryReconnect(10000))
	{
		if (loaded)
			TryToLogin();
	}
	else
	{
		Kill();
	}
}

void TryToLogin()
{
	printf("Client.sendLoginRequest(%s, %s);\n", hasher->getUsername().c_str(), hasher->getPasswordHash().c_str());
	Client.sendLoginRequest(hasher->getUsername(), hasher->getPasswordHash());
}



void Kill()
{
	printf("Kill();\n");
	done = true;
	Client.done = true;
}



std::string toLower(std::string myString)
{
	const int length = myString.length();
	for (int i = 0; i != length; ++i)
	{
		myString[i] = std::tolower(myString[i]);
	}
	return myString;
}