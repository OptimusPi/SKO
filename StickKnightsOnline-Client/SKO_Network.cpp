#include "SKO_Network.h"
#include "KE_Socket.h"
#include "hasher.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "Global.h"
#include <sstream>
#include "SDL_mixer.h"

SKO_Network::SKO_Network()
{

}

std::string SKO_Network::init(std::string server, unsigned short port)
{
	// Temporarily store a pointer to the PiSock object in main, whilst everything is migrated over to here
	// This is gonna be a nightmare...
	// When everything is migrated over I can move connect etc here and the socket can be a private variable of this class
	// instead.  Hopefully that makes sense.
	socket = new KE_Socket();

	if (!socket->Startup()) {
		socket->Cleanup();
		return "error";
	}

	//Set server connection details
	this->server = server;
	this->port = port;

	//TODO - move this to packet factory such as: PacketFactory.Create(PING) or something to be determined later
	pingPacket = "20";
	pingPacket[1] = PING;
	pingPacket[0] = pingPacket.length();

	return "connected";
}

//Open socket to this port
std::string SKO_Network::connect()
{
	if (!socket->Connect(this->server, this->port))
	{
		socket->Cleanup();
		return "error";
	}

	return "success";
}

//Send client version to validate newest software version
std::string SKO_Network::sendVersion(unsigned char major, unsigned char minor, unsigned char patch)
{
	std::string packet = "0";
	packet += VERSION_CHECK;
	packet += VERSION_MAJOR;
	packet += VERSION_MINOR;
	packet += VERSION_PATCH;
	packet += VERSION_OS;
	packet[0] = packet.length();
	socket->Send(packet);

	if (!isConnected())
		return "error";
	if (socket->BReceive() == -1)
		return "error";

	if (socket->Data[1] == VERSION_SUCCESS)
	{
		socket->Data = socket->Data.substr(socket->Data[0]);
		return "version correct";
	}

	if (socket->Data[1] == VERSION_FAIL)
		return "version error";

	if (socket->Data[1] == SERVER_FULL)
		return "server full";
}

bool SKO_Network::isConnected()
{
	return socket->Connected;
}

bool SKO_Network::TryReconnect(unsigned int retries, unsigned int sleep)
{
	int attempt = 0;
	do { 
		attempt++; 
		SDL_Delay(sleep); 
	} while (connect() != "success" && attempt < retries);

	socket->Data = "";
	return isConnected();
}

// Request to create an account
// Returns a string indicating success or error type
std::string SKO_Network::createAccount(std::string desiredUsername, std::string desiredPassword)
{
	Hasher hasher;
	std::string packet = "0";
	packet += REGISTER; // 3 is REGISTER
	packet += desiredUsername;
	packet += " ";
	std::transform(desiredUsername.begin(), desiredUsername.end(), desiredUsername.begin(), ::tolower);
	packet += hasher.Hash(desiredUsername + desiredPassword);
	packet[0] = packet.length();

	// Send the packet
	socket->Send(packet);

	// Check connection first
	if (!socket->Connected)
	{
		//reconnect for 10 attempts
		if (!TryReconnect(10, 200))
			return "error";
	}

	//Receive a packet from the server
	if (socket->BReceive() > 0)
	{
		int code = socket->Data[1];
		socket->Data = "";
		if (code == REGISTER_SUCCESS) // 8 is REGISTER_SUCCESS
		{
			// Register successfully, return successful to the client
			return "success";
		}
		else if (code == REGISTER_FAIL_DOUBLE) // 9 is REGISTER_DOUBLE_DAIL
		{
			// Registration failed due to duplicate username in the database
			return "username exists";
		}

		return "error";
	}
	else 
	{
		return "error";
	}
}

// Request to login
// Returns a string indicating success or error type
std::string SKO_Network::sendLoginRequest(std::string username, std::string password)
{
	std::string returnVal = "";
	Hasher hasher;

	//building the packet to send
	std::string Message1 = "0";
	Message1 += LOGIN;
	Message1 += username;
	Message1 += " ";
	std::transform(username.begin(), username.end(), username.begin(), ::tolower);
	Message1 += hasher.Hash(username + password);
	Message1[0] = Message1.length();
	socket->Send(Message1);


	// Check connection first
	if (!socket->Connected)
	{
		//reconnect for 10 attempts
		if (!TryReconnect(10, 200))
			return "error";
	}

	//Receive a packet from the server
	if (socket->BReceive() > 0)
	{
		int code = socket->Data[1];
		if (code == LOGIN_SUCCESS)
		{
			MyID = socket->Data[2];
			returnVal = "success";
		}
		else if (code == LOGIN_FAIL_DOUBLE)
		{
			returnVal = "already online";
		}
		else if (code == LOGIN_FAIL_NONE)
		{
			returnVal = "login failed";
		}
		else if (code == LOGIN_FAIL_BANNED)
		{
			returnVal = "banned";
		}

		socket->Data = "";
		return returnVal;
	}
	else 
	{
		return "error";
	}
}

// Send a clan creation request
void SKO_Network::createClan(std::string clanName)
{
	std::string Packet = "0";
	Packet += MAKE_CLAN;
	Packet += clanName;
	Packet[0] = Packet.length();

	socket->Send(Packet);
}

//Decline clan invite from another player
void SKO_Network::cancelClanInvite()
{
	std::string packet = "0";
	packet += CLAN;
	packet += CANCEL;
	packet[0] = packet.length();
	socket->Send(packet);
}

// Join clan of another player
void SKO_Network::acceptClanInvite()
{
	std::string packet = "0";
	packet += CLAN;
	packet += ACCEPT;
	packet[0] = packet.length();
	socket->Send(packet);
}

void SKO_Network::sendClanInvite(unsigned char requestedPlayer)
{
	std::string packet = "0";
	packet += CLAN;
	packet += INVITE;
	packet += requestedPlayer;
	packet[0] = packet.length();
	socket->Send(packet);
}

// Save the order of the inventory to allow players to re-arrange items
// Receives a point to Player.inventory
void SKO_Network::saveInventory(unsigned int inventory[24][2])
{
	std::string Packet = "0";
	Packet += INVENTORY;

	for (int i = 0; i < 24; i++)
		Packet += inventory[i][0];
	for (int i = 0; i < 24; i++)
		Packet += inventory[i][1];

	Packet[0] = Packet.length();
	socket->Send(Packet);
}

// Allocate an unnasigned stat point to the desired stat
void SKO_Network::allocateStatPoint(std::string desiredStat)
{
	std::string Packet = "0";
	if (desiredStat == "health")
		Packet += STAT_HP;
	else if (desiredStat == "strength")
		Packet += STAT_STR;
	else if (desiredStat == "defense")
		Packet += STAT_DEF;
	else return; // Something weird happened
	
	Packet[0] = Packet.length();
	socket->Send(Packet);
}

// This will unequip an item and put it in your inventory
// The value passed in is the selected equipment slot
void SKO_Network::unequipItem(unsigned char selectedEquipItem)
{
	std::string Packet = "0";
	Packet += EQUIP;
	Packet += selectedEquipItem;
	Packet[0] = Packet.length();
	socket->Send(Packet);
}

// This will attempt to use an item
// The value passed in is the integer value from the inventory slot that the player is attempting to use
void SKO_Network::useItem(unsigned int itemId)
{
	std::string Packet = "0";
	Packet += USE_ITEM;
	Packet += itemId;
	Packet[0] = Packet.length();
	socket->Send(Packet);
}

//Drop one or more items in a stack onto the ground
void SKO_Network::dropItem(unsigned char itemId, unsigned int amount)
{
	std::string packet = "0";
	packet += DROP_ITEM;
	packet += itemId;

	//how many to drop...could be lots
	packet += getPacketInt(amount);
	packet[0] = packet.length();

	socket->Send(packet);
}

// Cast a spell
// Currently only used for throwing trophies
void SKO_Network::castSpell()
{
	std::string Packet = "0";
	Packet += CAST_SPELL;
	Packet[0] = Packet.length();

	socket->Send(Packet);
}

void SKO_Network::sendPartyInvite(unsigned char playerId)
{
	std::string packet = "0";
	packet += PARTY;
	packet += INVITE;
	packet += playerId;
	packet[0] = packet.length();
	socket->Send(packet);
}

void SKO_Network::acceptPartyInvite()
{
	std::string packet = "0";
	//send accept packet
	packet += TRADE;
	packet += ACCEPT;
	packet[0] = packet.length();
	socket->Send(packet);
}

void SKO_Network::cancelParty()
{
	std::string packet = "0";
	packet += PARTY;
	packet += CANCEL;
	packet[0] = packet.length();
	socket->Send(packet);
}

void SKO_Network::acceptTradeInvite()
{
	std::string packet = "0";
	packet += TRADE;
	packet += ACCEPT;
	packet[0] = packet.length();
	socket->Send(packet);
}

//Cancel trade / reject trade invite
void SKO_Network::cancelTrade()
{
	std::string packet = "0";
	packet += TRADE;
	packet += CANCEL;
	packet[0] = packet.length();
	socket->Send(packet);
}

//Confirm and complete trade
void SKO_Network::confirmTrade()
{
	std::string packet = "0";
	packet += TRADE;
	packet += CONFIRM;
	packet[0] = packet.length();
	socket->Send(packet);
}

//request to trade with another player
void SKO_Network::sendTradeInvite(unsigned char playerId)
{
	std::string packet = "0";
	packet += TRADE;
	packet += INVITE;
	packet += playerId;
	packet[0] = packet.length();
	socket->Send(packet);
}

//Set the offer amount for an item
void SKO_Network::setTradeItemOffer(unsigned char item, unsigned int amount)
{
	//set the current offer for <item> to <amount>
	std::string tPacket = "0";
	tPacket += TRADE;
	tPacket += OFFER;
	tPacket += item;
	tPacket += getPacketInt(amount);
	tPacket[0] = tPacket.length();
	socket->Send(tPacket);
}

void SKO_Network::openShop(unsigned char shopId)
{
	std::string packet = "0";
	packet += SHOP;
	packet += INVITE;
	packet += shopId;
	packet[0] = packet.length();
	socket->Send(packet);
}

//Close the currently opened shop.
void SKO_Network::closeShop()
{
	std::string packet = "0";
	packet += SHOP;
	packet += CANCEL;
	packet[0] = packet.length();
	socket->Send(packet);
}

//Buy a shop item
void SKO_Network::buyItem(unsigned char itemSelectionId, unsigned int amount)
{
	std::string packet = "0";
	packet += SHOP;
	packet += BUY;
	packet += itemSelectionId;
	packet += getPacketInt(amount);
	packet[0] = packet.length();
	socket->Send(packet);
}

//Sell an inventory item to a shop
void SKO_Network::sellItem(unsigned char itemId, unsigned int amount)
{
	std::string packet = "0";
	packet += SHOP;
	packet += SELL;
	packet += itemId;
	packet += getPacketInt(amount);
	packet[0] = packet.length();
	socket->Send(packet);
}

void SKO_Network::depositBankItem(unsigned char itemId, unsigned int amount)
{
	std::string packet = "0";
	packet += BANK;
	packet += BANK_ITEM;
	packet += itemId;
	packet += getPacketInt(amount);
	packet[0] = packet.length();
	socket->Send(packet);
}

void SKO_Network::withdrawalBankItem(unsigned char itemId, unsigned int amount)
{
	std::string packet = "0";
	packet += BANK;
	packet += DEBANK_ITEM;
	packet += itemId;
	packet += getPacketInt(amount);
	packet[0] = packet.length();
	socket->Send(packet);
}

void SKO_Network::closeBank()
{
	std::string packet = "0";
	packet += BANK;
	packet += CANCEL;
	packet[0] = packet.length();
	socket->Send(packet);
}

///
/// Player Actions Function
///
void SKO_Network::playerAction(std::string actionType, float playerX, float playerY)
{
	std::string packet = "0";

	if (actionType == "left")
		packet += MOVE_LEFT;
	else if (actionType == "right")
		packet += MOVE_RIGHT;
	else if (actionType == "jump")
		packet += MOVE_JUMP;
	else if (actionType == "stop")
		packet += MOVE_STOP;
	else if (actionType == "attack")
		packet += ATTACK;

	packet += getPacketFloat(playerX);
	packet += getPacketFloat(playerY);
	packet[0] = packet.length();
	socket->Send(packet);
}

//Chat to other players or use /commands
void SKO_Network::sendChat(std::string message)
{
	std::string packet = "0";
	packet += CHAT;
	packet += message;
	packet[0] = message.length();
	socket->Send(packet);
}

///
/// Network Engine Functions
///

//Helper functions
std::string SKO_Network::getPacketFloat(float value)
{
	//convert 4-byte floating point value into a string of bytes 
	char * bytes = (char*)&value;

	//construct packet format string of bytes
	std::string packetBytes = "";
	packetBytes += bytes[0];
	packetBytes += bytes[1];
	packetBytes += bytes[2];
	packetBytes += bytes[3];

	//return a 4-byte string
	return packetBytes;
}
std::string SKO_Network::getPacketInt(unsigned int value)
{
	//convert 4-byte integer value into a string of bytes 
	char * bytes = (char*)&value;

	//construct packet format string of bytes
	std::string packetBytes = "";
	packetBytes += bytes[0];
	packetBytes += bytes[1];
	packetBytes += bytes[2];
	packetBytes += bytes[3];

	//return a 4-byte string
	return packetBytes;
}
std::string SKO_Network::getPacketShort(unsigned short value)
{
	//convert 2-byte short integer value into a string of bytes 
	char * bytes = (char*)&value;

	//construct packet format string of bytes
	std::string packetBytes = "";
	packetBytes += bytes[0];
	packetBytes += bytes[1];

	//return a 4-byte string
	return packetBytes;
}

//This will periodically check for the client ping to the server in milliseconds
void SKO_Network::checkPing()
{
	//get the ping
	if (menu == STATE_PLAY && (SDL_GetTicks() - pingRequestTicker) >= 950 && !pingWaiting)
	{
		unsigned int currentTime = SDL_GetTicks();

		if (socket->Connected) {
			socket->Send(pingPacket);
			pingWaiting = true;

			pingRequestTicker = currentTime;
			pingRequestTime = currentTime;
		}
	}
}

//Receive as much data that is ready, and process a single packet
void SKO_Network::receivePacket(bool connectErr)
{
	unsigned int currentTime;
	int code = 0;
	int data_len = 0;
	int pack_len = 0;
	int ping = 0;

	//get messages from the server
	if (socket->Data.length() == 0 || socket->Data.length() < (unsigned int)socket->Data[0])
	{
		socket->Receive();

		//              if (false && gotSomeData > 0){
		//                 printf("**BANG** recv length is: %i\n", gotSomeData);
		//                 printf("**BANG** data length is: %i\n", socket->Data.length());
		//                 printf("Data is: ");
		//                 for (unsigned int i = 0; i < socket->Data.length(); i++)
		//                     printf("[%i]", socket->Data[i]);
		//                 printf("\n");
		//
		//              }

	}

	// If the data holds a complete data
	data_len = socket->Data.length();
	pack_len = 0;
	currentTime = SDL_GetTicks();


	if (data_len > 0)
		pack_len = socket->Data[0];

	if (data_len >= pack_len && data_len > 0)
	{

		//printf("\n*************************\n");
		//printf("data_length %i\npack_length %i\n", data_len, pack_len);

		// If the data holds a complete data
		data_len = 0;
		pack_len = 0;

		data_len = socket->Data.length();

		if (data_len > 0)
			pack_len = socket->Data[0];



		std::string newPacket = "";

		if (data_len > pack_len)
			newPacket = socket->Data.substr(pack_len, data_len);

		socket->Data = socket->Data.substr(0, pack_len);




		//rip the command
		code = socket->Data[1];

		//      int check_a = socket->Data.length();
		//      int check_b = (int)socket->Data[0];

		//              if (check_a)
		//              {
		//                 printf("code is ---------------  %i\n", code);
		//                 printf("socket->Data.length() =   %i\n\n", socket->Data.length());
		//                 printf("The length of the packet should be: %i\n", (int)socket->Data[0]);
		//              }


		//respond to server's PING or die!
		if (code == PING)
		{
			std::string pongPacket = "0";
			pongPacket += PONG;
			pongPacket[0] = pongPacket.length();
			socket->Send(pongPacket);
		}
		else
			if (code == PONG)
			{

				//calculate the ping
				ping = currentTime - pingRequestTime;

				//show the ping
				std::stringstream ss;
				ss << "Ping:   ";
				// ss << (float)(ping+ping_old)/2.0;
				ss << ping;
				Message[15].SetText(ss.str());

				if (ping < 50) {
					Message[15].R = 150 / 255.0;
					Message[15].G = 255 / 255.0;
					Message[15].B = 150 / 255.0;
				}
				else
					if (ping < 100) {
						Message[15].R = 230 / 255.0;
						Message[15].G = 255 / 255.0;
						Message[15].B = 90 / 255.0;
					}
					else
						if (ping < 250) {
							Message[15].R = 255 / 255.0;
							Message[15].G = 200 / 255.0;
							Message[15].B = 150 / 255.0;
						}
						else {
							Message[15].R = 255 / 255.0;
							Message[15].G = 80 / 255.0;
							Message[15].B = 80 / 255.0;
						}

						//tell the counters that the ping came back
						pingWaiting = false;
			}
			else if (code == LOADED)
			{
				printf("loaded.\n");
				Mix_PauseMusic();
				music = Mix_LoadMUS("SND/game.ogg");

				if (music != NULL && Mix_PlayMusic(music, -1) == -1)
					printf("ERROR PLAYING MUSIC!!");
				else
					printf("music played ok.");

				if (!enableSND || !enableMUS)
				{
					printf("not playing music because either sound or music is mute.\n");
					if (Mix_PausedMusic() != 1)
					{
						//pause the music
						Mix_PauseMusic();
					}
				}

				printf("my id is %i and hp is %i and map is %i\n", MyID, Player[MyID].hp, Player[MyID].current_map);
				/* if not fully loaded */
				if (Player[MyID].level == 255)
				{
					printf("ERROR: Your character did not load, so closing the connection and client. just try again.");
					TryReconnect(10, 200);
				}


				//ask who is online
				std::string whom = "0";
				whom += CHAT;
				whom += "/who";
				whom[0] = whom.length();
				socket->Send(whom);

				menu = STATE_PLAY;//game
				popup_menu = 0;
				popup_gui_menu = 0;

				loaded = true;

				//Play the music
				if (!enableSND || !enableMUS)
				{
					Mix_PauseMusic();
				}

				if (enableSND && enableSFX)
					Mix_PlayChannel(-1, login_noise, 0);

				printf("LOADED packet operations successfully completed.");
			}
			else if (code == ATTACK)
			{


				float temp;
				int CurrSock = socket->Data[2];
				((char*)&temp)[0] = socket->Data[3];
				((char*)&temp)[1] = socket->Data[4];
				((char*)&temp)[2] = socket->Data[5];
				((char*)&temp)[3] = socket->Data[6];
				Player[CurrSock].x = temp;
				((char*)&temp)[0] = socket->Data[7];
				((char*)&temp)[1] = socket->Data[8];
				((char*)&temp)[2] = socket->Data[9];
				((char*)&temp)[3] = socket->Data[10];
				Player[CurrSock].y = temp;
				Player[CurrSock].attacking = true;
				Player[CurrSock].x_speed = 0;

				//if they are in sight
				float px = Player[CurrSock].x - camera_x;
				float py = Player[CurrSock].y - camera_y;

				if (px > -32 && px < 1024 && py > -32 && py < 600)
				{
					//play the swooshy sound
					if (enableSND && enableSFX)
						Mix_PlayChannel(-1, attack_noise, 0);
				}

			}
			else if (code == ENEMY_ATTACK)
			{
				float temp;
				int i = socket->Data[2];
				int cu = socket->Data[3];
				((char*)&temp)[0] = socket->Data[4];
				((char*)&temp)[1] = socket->Data[5];
				((char*)&temp)[2] = socket->Data[6];
				((char*)&temp)[3] = socket->Data[7];

				map[cu]->Enemy[i].x = temp;
				((char*)&temp)[0] = socket->Data[8];
				((char*)&temp)[1] = socket->Data[9];
				((char*)&temp)[2] = socket->Data[10];
				((char*)&temp)[3] = socket->Data[11];
				map[cu]->Enemy[i].y = temp;
				map[cu]->Enemy[i].attacking = true;
				map[cu]->Enemy[i].x_speed = 0;

				//printf("enemy attack. current map [%i] x:[%i] y:[%i]\n", cu,
				//		  (int)map[cu]->Enemy[i].x,
				//		  (int)map[cu]->Enemy[i].y);

			}
			else if (code == ENEMY_MOVE_RIGHT)
			{
				int npc_id = socket->Data[2];
				int current_map = socket->Data[3];
				float npc_x, npc_y;
				((char*)&npc_x)[0] = socket->Data[4];
				((char*)&npc_x)[1] = socket->Data[5];
				((char*)&npc_x)[2] = socket->Data[6];
				((char*)&npc_x)[3] = socket->Data[7];
				((char*)&npc_y)[0] = socket->Data[8];
				((char*)&npc_y)[1] = socket->Data[9];
				((char*)&npc_y)[2] = socket->Data[10];
				((char*)&npc_y)[3] = socket->Data[11];

				map[current_map]->Enemy[npc_id].x_speed = 2;
				map[current_map]->Enemy[npc_id].x = npc_x;
				map[current_map]->Enemy[npc_id].y = npc_y;
				map[current_map]->Enemy[npc_id].facing_right = true;

			}
			else if (code == ENEMY_MOVE_LEFT)
			{
				int npc_id = socket->Data[2];
				int current_map = socket->Data[3];
				float npc_x, npc_y;
				((char*)&npc_x)[0] = socket->Data[4];
				((char*)&npc_x)[1] = socket->Data[5];
				((char*)&npc_x)[2] = socket->Data[6];
				((char*)&npc_x)[3] = socket->Data[7];
				((char*)&npc_y)[0] = socket->Data[8];
				((char*)&npc_y)[1] = socket->Data[9];
				((char*)&npc_y)[2] = socket->Data[10];
				((char*)&npc_y)[3] = socket->Data[11];

				map[current_map]->Enemy[npc_id].x_speed = -2;
				map[current_map]->Enemy[npc_id].x = npc_x;
				map[current_map]->Enemy[npc_id].y = npc_y;
				map[current_map]->Enemy[npc_id].facing_right = false;
			}
			else if (code == ENEMY_MOVE_STOP)
			{
				int npc_id = socket->Data[2];
				int current_map = socket->Data[3];
				float npc_x, npc_y;
				((char*)&npc_x)[0] = socket->Data[4];
				((char*)&npc_x)[1] = socket->Data[5];
				((char*)&npc_x)[2] = socket->Data[6];
				((char*)&npc_x)[3] = socket->Data[7];
				((char*)&npc_y)[0] = socket->Data[8];
				((char*)&npc_y)[1] = socket->Data[9];
				((char*)&npc_y)[2] = socket->Data[10];
				((char*)&npc_y)[3] = socket->Data[11];

				map[current_map]->Enemy[npc_id].x_speed = 0;
				map[current_map]->Enemy[npc_id].y_speed = 0;
				map[current_map]->Enemy[npc_id].x = npc_x;
				map[current_map]->Enemy[npc_id].y = npc_y;

				if (loaded && npc_x == ENEMY_DEAD_X && npc_y == ENEMY_DEAD_Y)
				{
					if (enableSND && enableSFX)
						Mix_PlayChannel(-1, items_drop_noise, 0);
				}

				//                   printf("ENEMY_MOVE_STOP: map[%i].Enemy[%i] (%i, %i)\n",
				//                		   current_map,
				//						   npc_id,
				//						   (int)map[current_map]->Enemy[npc_id].x,
				//						   (int)map[current_map]->Enemy[npc_id].x);
			}
		//NPC packets
			else if (code == NPC_TALK)
			{
				//					float temp;
				//					int i = socket->Data[2];
				//					int cu = socket->Data[3];
				//					((char*)&temp)[0] = socket->Data[4];
				//					((char*)&temp)[1] = socket->Data[5];
				//					((char*)&temp)[2] = socket->Data[6];
				//					((char*)&temp)[3] = socket->Data[7];
				//
				//					map[cu]->NPC[i].x = temp;
				//					((char*)&temp)[0] = socket->Data[8];
				//					((char*)&temp)[1] = socket->Data[9];
				//					((char*)&temp)[2] = socket->Data[10];
				//					((char*)&temp)[3] = socket->Data[11];
				//					map[cu]->NPC[i].y = temp;
				//					map[cu]->NPC[i].attacking = true;
				//					map[cu]->NPC[i].x_speed = 0;
				//
				//					//printf("npc attack. current map [%i] x:[%i] y:[%i]\n", cu,
				//				  //		  (int)map[cu]->NPC[i].x,
				//				  //		  (int)map[cu]->NPC[i].y);

			}
			else if (code == NPC_MOVE_RIGHT)
			{
				int npc_id = socket->Data[2];
				int current_map = socket->Data[3];
				float npc_x, npc_y;
				((char*)&npc_x)[0] = socket->Data[4];
				((char*)&npc_x)[1] = socket->Data[5];
				((char*)&npc_x)[2] = socket->Data[6];
				((char*)&npc_x)[3] = socket->Data[7];
				((char*)&npc_y)[0] = socket->Data[8];
				((char*)&npc_y)[1] = socket->Data[9];
				((char*)&npc_y)[2] = socket->Data[10];
				((char*)&npc_y)[3] = socket->Data[11];

				map[current_map]->NPC[npc_id].x_speed = 2;
				map[current_map]->NPC[npc_id].x = npc_x;
				map[current_map]->NPC[npc_id].y = npc_y;
				map[current_map]->NPC[npc_id].facing_right = true;

			}
			else if (code == NPC_MOVE_LEFT)
			{
				int npc_id = socket->Data[2];
				int current_map = socket->Data[3];
				float npc_x, npc_y;
				((char*)&npc_x)[0] = socket->Data[4];
				((char*)&npc_x)[1] = socket->Data[5];
				((char*)&npc_x)[2] = socket->Data[6];
				((char*)&npc_x)[3] = socket->Data[7];
				((char*)&npc_y)[0] = socket->Data[8];
				((char*)&npc_y)[1] = socket->Data[9];
				((char*)&npc_y)[2] = socket->Data[10];
				((char*)&npc_y)[3] = socket->Data[11];

				map[current_map]->NPC[npc_id].x_speed = -2;
				map[current_map]->NPC[npc_id].x = npc_x;
				map[current_map]->NPC[npc_id].y = npc_y;
				map[current_map]->NPC[npc_id].facing_right = false;
			}
			else if (code == NPC_MOVE_STOP)
			{
				int npc_id = socket->Data[2];
				int current_map = socket->Data[3];
				float npc_x, npc_y;
				((char*)&npc_x)[0] = socket->Data[4];
				((char*)&npc_x)[1] = socket->Data[5];
				((char*)&npc_x)[2] = socket->Data[6];
				((char*)&npc_x)[3] = socket->Data[7];
				((char*)&npc_y)[0] = socket->Data[8];
				((char*)&npc_y)[1] = socket->Data[9];
				((char*)&npc_y)[2] = socket->Data[10];
				((char*)&npc_y)[3] = socket->Data[11];

				map[current_map]->NPC[npc_id].x_speed = 0;
				map[current_map]->NPC[npc_id].y_speed = 0;
				map[current_map]->NPC[npc_id].x = npc_x;
				map[current_map]->NPC[npc_id].y = npc_y;

			}
		//end NPC packets
			else if (code == MOVE_RIGHT)
			{
				// Declare message string
				std::string Message = "";

				//fill message with username and reason
				Message += socket->Data.substr(2, pack_len - 2);

				//printf("Message[%s]\n", Message);

				//put each byte into memory
				char b1 = (char)Message[1];
				char b2 = (char)Message[2];
				char b3 = (char)Message[3];
				char b4 = (char)Message[4];
				char b5 = (char)Message[5];
				char b6 = (char)Message[6];
				char b7 = (char)Message[7];
				char b8 = (char)Message[8];

				//hold the result...
				float numx;

				//build an int from 4 bytes
				((char*)&numx)[0] = b1;
				((char*)&numx)[1] = b2;
				((char*)&numx)[2] = b3;
				((char*)&numx)[3] = b4;

				//hold the result...
				float numy;

				//build an int from 4 bytes
				((char*)&numy)[0] = b5;
				((char*)&numy)[1] = b6;
				((char*)&numy)[2] = b7;
				((char*)&numy)[3] = b8;

				unsigned char a = Message[0];

				//move the player
				Player[a].x_speed = 2;
				Player[a].x = numx;
				Player[a].y = numy;
				Player[a].facing_right = true;

				//printf("Player[%i] numx=%inumy=%i\n", a, numx, numy);

			}
			else if (code == MOVE_LEFT)
			{

				// Declare message string
				std::string Message = "";

				//fill message with username and reason
				Message += socket->Data.substr(2, pack_len - 2);

				//printf("Message[%s]\n", Message);

				//put each byte into memory
				char b1 = (char)Message[1];
				char b2 = (char)Message[2];
				char b3 = (char)Message[3];
				char b4 = (char)Message[4];
				char b5 = (char)Message[5];
				char b6 = (char)Message[6];
				char b7 = (char)Message[7];
				char b8 = (char)Message[8];

				//hold the result...
				float numx;

				//build an int from 4 bytes
				((char*)&numx)[0] = b1;
				((char*)&numx)[1] = b2;
				((char*)&numx)[2] = b3;
				((char*)&numx)[3] = b4;

				//hold the result...
				float numy;

				//build an int from 4 bytes
				((char*)&numy)[0] = b5;
				((char*)&numy)[1] = b6;
				((char*)&numy)[2] = b7;
				((char*)&numy)[3] = b8;

				unsigned char a = Message[0];

				//move the player
				Player[a].x_speed = -2;
				Player[a].x = numx;
				Player[a].y = numy;
				Player[a].facing_right = false;


			}
			else if (code == MOVE_STOP)
			{

				// Declare message string
				std::string Message = "";

				//fill message with username and reason
				Message += socket->Data.substr(2, pack_len - 2);

				//printf("Message[%s]\n", Message);

				//put each byte into memory
				char b1 = (char)Message[1];
				char b2 = (char)Message[2];
				char b3 = (char)Message[3];
				char b4 = (char)Message[4];
				char b5 = (char)Message[5];
				char b6 = (char)Message[6];
				char b7 = (char)Message[7];
				char b8 = (char)Message[8];

				//hold the result...
				float numx;

				//build an int from 4 bytes
				((char*)&numx)[0] = b1;
				((char*)&numx)[1] = b2;
				((char*)&numx)[2] = b3;
				((char*)&numx)[3] = b4;

				//hold the result...
				float numy;

				//build an int from 4 bytes
				((char*)&numy)[0] = b5;
				((char*)&numy)[1] = b6;
				((char*)&numy)[2] = b7;
				((char*)&numy)[3] = b8;

				unsigned char a = Message[0];

				//move the player
				Player[a].x_speed = 0;
				Player[a].x = numx;
				Player[a].y = numy;

				//printf("Player[%i] numx=%inumy=%i\n", a, numx, numy);

			}
			else if (code == MOVE_JUMP)
			{
				// Declare message string
				std::string Message = "";

				//fill message with username and reason
				Message += socket->Data.substr(2, pack_len - 2);

				//printf("Message[%s]\n", Message);

				//put each byte into memory
				char b1 = (char)Message[1];
				char b2 = (char)Message[2];
				char b3 = (char)Message[3];
				char b4 = (char)Message[4];
				char b5 = (char)Message[5];
				char b6 = (char)Message[6];
				char b7 = (char)Message[7];
				char b8 = (char)Message[8];

				//hold the result...
				float numx;

				//build an int from 4 bytes
				((char*)&numx)[0] = b1;
				((char*)&numx)[1] = b2;
				((char*)&numx)[2] = b3;
				((char*)&numx)[3] = b4;

				//hold the result...
				float numy;

				//build an int from 4 bytes
				((char*)&numy)[0] = b5;
				((char*)&numy)[1] = b6;
				((char*)&numy)[2] = b7;
				((char*)&numy)[3] = b8;

				int a = Message[0];

				//move the player
				Player[a].y_speed = -6;
				Player[a].x = numx;
				Player[a].y = numy;


				//printf("Player[%i] numx=%inumy=%i\n", a, numx, numy);

			}
			else if (code == TARGET_HIT)
			{


				int type = socket->Data[2];
				int id = socket->Data[3];
				bool inSight = false;
				if (type == 0) // enemy
				{
					map[current_map]->Enemy[id].hit = true;
					map[current_map]->Enemy[id].hit_ticker = SDL_GetTicks();
					map[current_map]->Enemy[id].hp_ticker = SDL_GetTicks() + 1500;
					float px = map[current_map]->Enemy[id].x - camera_x;
					float py = map[current_map]->Enemy[id].y - camera_y;
					if (px > -32 && px < 1024 && py > -32 && py < 600)
						inSight = true;
				}
				else if (type == 1) // player
				{
					Player[id].hit = true;
					Player[id].hit_ticker = SDL_GetTicks();
					float px = Player[id].x - camera_x;
					float py = Player[id].y - camera_y;
					if (px > -32 && px < 1024 && py > -32 && py < 600)
						inSight = true;
				}

				//decide a sound effect to play
				if (enableSND && enableSFX && inSight)
				{
					int sf = 0;
					for (int trySfx = 0; trySfx < 10 && sf == lastSfx; trySfx++)
						sf = rand() % 3;
					lastSfx = sf;

					switch (sf)
					{
					case 0: Mix_PlayChannel(-1, hit1, 0); break;
					case 1: Mix_PlayChannel(-1, hit2, 0); break;
					case 2: Mix_PlayChannel(-1, hit3, 0); break;
					}
				}
			}
			else if (code == EQUIP)
			{
				int user = socket->Data[2];
				int slot = socket->Data[3];
				int equipId = socket->Data[4];
				int item = socket->Data[5];

				Player[user].equip[slot] = equipId;
				Player[user].equipI[slot] = item;
			}
			else if (code == JOIN)
			{
				if (loaded && enableSND && enableSFX)
					Mix_PlayChannel(-1, login_noise, 0);

				/*printf("socket->length() == %i and PiSock[0] == %i \n",socket->Data.length(), socket->Data[0]);

				for (int g = 0; g < socket->Data.length(); g++)
				{
				printf("byte #%i : [%c](%i)\n", g, socket->Data[g], socket->Data[g]);
				}*/
				//printf("\n");
				printf("JOIN !! \n");
				int a = socket->Data[2];

				//printf("join id (a) is [%i] and my id is [%i]\n*\n*\n", a, MyID);

				if (a != MyID)
				{
					Player[a] = SKO_Player();
				}

				//hold the result...
				float numx;


				//build a float from 4 bytes
				((char*)&numx)[0] = socket->Data[3];
				((char*)&numx)[1] = socket->Data[4];
				((char*)&numx)[2] = socket->Data[5];
				((char*)&numx)[3] = socket->Data[6];

				Player[a].x = numx;

				//hold the result...
				float numy;

				//build a float from 4 bytes
				((char*)&numy)[0] = socket->Data[7];
				((char*)&numy)[1] = socket->Data[8];
				((char*)&numy)[2] = socket->Data[9];
				((char*)&numy)[3] = socket->Data[10];

				Player[a].y = numy;

				//hold the result...
				float numxs;

				//build a float from 4 bytes
				((char*)&numxs)[0] = socket->Data[11];
				((char*)&numxs)[1] = socket->Data[12];
				((char*)&numxs)[2] = socket->Data[13];
				((char*)&numxs)[3] = socket->Data[14];

				Player[a].x_speed = numxs;

				//hold the result...
				float numys;

				//build a float from 4 bytes
				((char*)&numys)[0] = socket->Data[15];
				((char*)&numys)[1] = socket->Data[16];
				((char*)&numys)[2] = socket->Data[17];
				((char*)&numys)[3] = socket->Data[18];

				Player[a].y_speed = numys;

				//facing left or right
				Player[a].facing_right = (bool)socket->Data[19];

				//what map are they on?
				Player[a].current_map = (int)socket->Data[20];

				printf("player is on map: %i\n", Player[a].current_map);
				if (a == MyID)
					current_map = Player[MyID].current_map;

				std::string Username = "";
				std::string Message1 = socket->Data;


				printf("Message1:%s\n", Message1.c_str());

				int posLetter = 0;
				for (unsigned int a = 21; a < Message1.length(); a++)
				{
					if (Message1[a] == '|') {
						posLetter = a + 1;
						break;
					}

					Username += Message1[a];
					//printf("Message1[%i]: %c\n", a, Message1[a]);
				}

				// printf("Username: [%s]\n\n", Username);
				Player[a].Nick = Username;

				std::string clantag = "";
				//set username tag and clan tag
				for (unsigned int a = posLetter; a < Message1.length(); a++)
				{
					clantag += Message1[a];
					//printf("Message1[%i]: %c\n", a, Message1[a]);
				}

				printf("Clan: [%s]\n\n", clantag.c_str());
				Player[a].str_clantag = clantag;


				//               printf("Username: [%s]\n\n", Player[a].Nick);
				Player[a].Status = true;

				//printf("JOIN: a:%i x:%i y:%i x_speed:%i y_speed:%i username:%s\n", a, (int)numx, (int)numy, (int)numxs, (int)numys, Username);

				SetUsername(a);
				//printf("setting nick of [%i] to [%s]\nMyID is [%i]\n\n", a, Player[a].Nick.c_str(),MyID);


				//only if you're not first entering
				if (loaded)
				{

					//display they logged on
					std::string jMessage = "";
					jMessage += Player[a].Nick;
					jMessage += " has joined the game.";

					//scroll the stuff up
					chat_line[0] = chat_line[1];
					chat_line[1] = chat_line[2];
					chat_line[2] = chat_line[3];
					chat_line[3] = chat_line[4];
					chat_line[4] = chat_line[5];
					chat_line[5] = chat_line[6];
					chat_line[6] = chat_line[7];
					chat_line[7] = chat_line[8];
					chat_line[8] = chat_line[9];
					chat_line[9] = chat_line[10];
					chat_line[10] = chat_line[11];
					chat_line[11] = new std::string(jMessage);


					//display the new one
					for (int i = 0; i < 12; i++)
					{
						Message[i + 3].SetText(chat_line[i]->c_str());
					}

					//printf("JOIN [%i][%s]\n", a, jMessage.c_str() );
				}


			}
			else if (code == EXIT)
			{
				if (enableSND && enableSFX)
					Mix_PlayChannel(-1, logout_noise, 0);

				int a = socket->Data[2];

				//display they logged off
				std::string lMessage = "";
				lMessage += Player[a].Nick;
				lMessage += " has left the game.";

				//scroll the stuff up
				chat_line[0] = chat_line[1];
				chat_line[1] = chat_line[2];
				chat_line[2] = chat_line[3];
				chat_line[3] = chat_line[4];
				chat_line[4] = chat_line[5];
				chat_line[5] = chat_line[6];
				chat_line[6] = chat_line[7];
				chat_line[7] = chat_line[8];
				chat_line[8] = chat_line[9];
				chat_line[9] = chat_line[10];
				chat_line[10] = chat_line[11];
				chat_line[11] = new std::string(lMessage);


				//display the new one
				for (int i = 0; i < 12; i++)
				{
					Message[i + 3].SetText(chat_line[i]->c_str());
				}

				Player[a] = SKO_Player();
				SetUsername(a);

			}
			else if (code == STAT_HP)
			{
				Player[MyID].hp = socket->Data[2];
			}
			else if (code == STAT_REGEN)
			{
				Player[MyID].regen = socket->Data[2];
			}
			else if (code == STATMAX_HP)
			{
				Player[MyID].max_hp = socket->Data[2];
			}
			else if (code == STAT_POINTS)
			{
				Player[MyID].stat_points = socket->Data[2];
			}
			else if (code == STAT_XP)
			{
				unsigned int temp;
				((char*)&temp)[0] = socket->Data[2];
				((char*)&temp)[1] = socket->Data[3];
				((char*)&temp)[2] = socket->Data[4];
				((char*)&temp)[3] = socket->Data[5];
				Player[MyID].xp = temp;
			}
			else if (code == STAT_LEVEL)
			{
				Player[MyID].level = socket->Data[2];
			}
			else if (code == STAT_DEF)
			{
				Player[MyID].defence = socket->Data[2];
			}
			else if (code == STAT_STR)
			{
				Player[MyID].strength = socket->Data[2];
			}
			else if (code == STATMAX_XP)
			{
				unsigned int temp;
				((char*)&temp)[0] = socket->Data[2];
				((char*)&temp)[1] = socket->Data[3];
				((char*)&temp)[2] = socket->Data[4];
				((char*)&temp)[3] = socket->Data[5];
				Player[MyID].max_xp = temp;
			}
			else if (code == RESPAWN)
			{
				int pid = socket->Data[2];
				if (pid == MyID) {
					if (enableSND && enableSFX)
						Mix_PlayChannel(-1, grunt_noise, 0);
					Player[pid].hp = Player[pid].max_hp;
				}
				else {
					Player[pid].hp = 80;//cosmetic HP bar
				}

				//hold the result...
				float numx;

				//build a float from 4 bytes
				((char*)&numx)[0] = socket->Data[3];
				((char*)&numx)[1] = socket->Data[4];
				((char*)&numx)[2] = socket->Data[5];
				((char*)&numx)[3] = socket->Data[6];


				//hold the result...
				float numy;

				//build a float from 4 bytes
				((char*)&numy)[0] = socket->Data[7];
				((char*)&numy)[1] = socket->Data[8];
				((char*)&numy)[2] = socket->Data[9];
				((char*)&numy)[3] = socket->Data[10];

				//set their coords
				Player[pid].x = numx;
				Player[pid].y = numy;
				Player[pid].y_speed = 0;
			}
			else if (code == SPAWN_ITEM)
			{

				int current_item = (unsigned char)socket->Data[2];
				int c_map = socket->Data[3];
				int spawned_item = (unsigned char)socket->Data[4];

				//hold the result...
				float numx;

				//build a float from 4 bytes
				((char*)&numx)[0] = socket->Data[5];
				((char*)&numx)[1] = socket->Data[6];
				((char*)&numx)[2] = socket->Data[7];
				((char*)&numx)[3] = socket->Data[8];


				//hold the result...
				float numy;

				//build a float from 4 bytes
				((char*)&numy)[0] = socket->Data[9];
				((char*)&numy)[1] = socket->Data[10];
				((char*)&numy)[2] = socket->Data[11];
				((char*)&numy)[3] = socket->Data[12];


				//hold the result...
				float numxs;

				//build a float from 4 bytes
				((char*)&numxs)[0] = socket->Data[13];
				((char*)&numxs)[1] = socket->Data[14];
				((char*)&numxs)[2] = socket->Data[15];
				((char*)&numxs)[3] = socket->Data[16];


				//hold the result...
				float numys;

				//build a float from 4 bytes
				((char*)&numys)[0] = socket->Data[17];
				((char*)&numys)[1] = socket->Data[18];
				((char*)&numys)[2] = socket->Data[19];
				((char*)&numys)[3] = socket->Data[20];

				map[c_map]->ItemObj[current_item] = SKO_ItemObject(spawned_item, numx, numy, numxs, numys);
				//printf("ItemObj[%i] ITEM_[%i] x:%.2f y:%.2f xs:%.2f ys:%.2f\n", socket->Data[2], socket->Data[3], numx, numy, numxs, numys);
			}
			else if (code == DESPAWN_ITEM)
			{
				int theItemID = (unsigned char)socket->Data[2];
				int c_map = socket->Data[3];

				if (theItemID >= 0)
					map[c_map]->ItemObj[theItemID].remove();
				else {
					printf("\n\n ERROR the ItemID is [%i]\n", theItemID);
					printf("socket->Data is:\n");
					for (unsigned int i = 0; i<socket->Data.length(); i++)
						printf("[%i]", socket->Data[i]);
					TryReconnect(10, 200);
				}
			}
			else if (code == POCKET_ITEM)
			{
				unsigned char item = socket->Data[2];
				unsigned int amount = 0;

				//build an int from 4 bytes
				((char*)&amount)[0] = socket->Data[3];
				((char*)&amount)[1] = socket->Data[4];
				((char*)&amount)[2] = socket->Data[5];
				((char*)&amount)[3] = socket->Data[6];

				if (amount == 0)
				{
					//find the item in inventory already
					for (int i = 0; i < 24; i++)
						if (Player[MyID].inventory[i][0] == item) {
							Player[MyID].inventory[i][0] = 0;
							Player[MyID].inventory[i][1] = 0;
							Message[55 + i].SetText("");
						}
				}

				//play sound or no
				bool playSound = false;

				//find if there is an item
				bool itemFound = false;
				int i;
				for (i = 0; i < 24; i++)
					if (Player[MyID].inventory[i][0] == item && Player[MyID].inventory[i][1] > 0)
					{
						itemFound = true;
						break;
					}


				if (itemFound)//theres already an item, stack it
				{
					if (Player[MyID].inventory[i][1] < amount && loaded)
						playSound = true;

					Player[MyID].inventory[i][1] = amount;
					int tamount = amount;
					std::stringstream ss_am;
					std::string s_am;

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

					Message[55 + i].SetText(s_am);
				}
				else//find open slot
				{
					//play sound;
					if (loaded) playSound = true;
					for (int i = 0; i < 24; i++)
						if (Player[MyID].inventory[i][1] == 0)
						{
							Player[MyID].inventory[i][0] = item;
							Player[MyID].inventory[i][1] = amount;
							int tamount = amount;
							std::stringstream ss_am;
							std::string s_am;

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


							Message[55 + i].SetText(s_am);
							break;
						}
				}

				//play sound on item pickup
				if (playSound)
				{
					if (enableSND && enableSFX)
						Mix_PlayChannel(-1, item_pickup_noise, 0);
				}

			}//end POCKET_ITEM
			else if (code == BANK_ITEM)
			{
				unsigned char item = socket->Data[2];
				unsigned int amount = 0;

				//build an int from 4 bytes
				((char*)&amount)[0] = socket->Data[3];
				((char*)&amount)[1] = socket->Data[4];
				((char*)&amount)[2] = socket->Data[5];
				((char*)&amount)[3] = socket->Data[6];

				Player[MyID].bank[item] = amount;
				int tamount = amount;

				std::stringstream ss_am;
				std::string s_am;

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
					s_am = "none";
				}

				int msgNum = 144 + item - (bankScroll * 6);

				//do not change this. It's 162 because bank text ends at [161] :)
				//these are loaded adn the rest are ignored, but dont worry,
				// the text updates when you bank scroll.
				if (msgNum < 162)
					Message[msgNum].SetText(s_am);
				//                     else
				//                     	printf("OH noes it broke ... msgNum is %i\n", msgNum);

			}//end bank
			else if (code == PARTY)
			{
				//what kind of party action, yo?
				int action = socket->Data[2];
				int playerB;
				int xOffset;
				std::stringstream ss;
				switch (action)
				{

				case INVITE:
					playerB = socket->Data[3];


					ss << "Join party of ";
					ss << Player[playerB].Nick;
					ss << "?";

					//confirm you want to trade pop up
					inputBox.outputText->SetText((ss.str().c_str()));
					inputBox.okayText->SetText("Party");
					inputBox.cancelText->SetText("Cancel");
					inputBox.inputText->SetText("");
					Message[89].SetText(ss.str());
					//position it
					xOffset = (Player[playerB].Nick.length() * 9) / 2;
					Message[89].pos_x = 320 - xOffset;
					popup_gui_menu = 8;
					Message[138].pos_x = 10;
					Message[139].pos_x = 155;
					Message[138].pos_y = 452;
					Message[139].pos_y = 452;
					break;


					// just assign someone to a party
				case ACCEPT:
				{
					playerB = socket->Data[3];
					int party = socket->Data[4];
					Player[playerB].party = party;
					printf("Player %i joins party %i\n", playerB, party);
				}
				break;

				//close out of all parties
				case CANCEL:
					popup_gui_menu = 0;
					Player[MyID].party = -1;
					break;

				default:
					break;
				}//end switch action
			}//end code PARTY
			else if (code == CLAN)
			{
				//what kind of clan action, yo?
				int action = socket->Data[2];
				int playerB;
				int xOffset;
				std::stringstream ss;
				switch (action)
				{

				case INVITE:
					playerB = socket->Data[3];


					ss << "Join Clan ";
					ss << Player[playerB].str_clantag;
					ss << "?";

					//confirm you want to trade pop up
					inputBox.outputText->SetText((ss.str().c_str()));
					inputBox.okayText->SetText("Join");
					inputBox.cancelText->SetText("Cancel");
					inputBox.inputText->SetText("");
					Message[89].SetText(ss.str());
					//position it
					xOffset = (Player[playerB].Nick.length() * 9) / 2;
					Message[89].pos_x = 320 - xOffset;
					popup_gui_menu = 11;
					Message[138].pos_x = 10;
					Message[139].pos_x = 155;
					Message[138].pos_y = 452;
					Message[139].pos_y = 452;
					break;


					// just assign someone to a clan
				case ACCEPT:
				{
					playerB = socket->Data[3];
					int clan = socket->Data[4];
					Player[playerB].clan = clan;
					printf("Player %i joins clan %i\n", playerB, clan);
				}
				break;

				//close out of all parties
				case CANCEL:
					popup_gui_menu = 0;
					Player[MyID].clan = -1;
					break;

				default:
					break;
				}//end switch action
			}//end code CLAN
			else if (code == TRADE)
			{
				//what kind of trade action, yo?
				int action = socket->Data[2];
				int playerB;
				int xOffset;
				std::stringstream ss;
				switch (action)
				{

				case INVITE:
					playerB = socket->Data[3];


					ss << "Trade with ";
					ss << Player[playerB].Nick;
					ss << "?";

					//confirm you want to trade pop up
					inputBox.outputText->SetText((ss.str().c_str()));
					inputBox.okayText->SetText("Trade");
					inputBox.cancelText->SetText("Cancel");
					inputBox.inputText->SetText("");
					Message[89].SetText(ss.str());
					//position it
					xOffset = (Player[playerB].Nick.length() * 9) / 2;
					Message[89].pos_x = 330 - xOffset;
					popup_gui_menu = 3;
					Message[138].pos_x = 10;
					Message[139].pos_x = 155;
					Message[138].pos_y = 452;
					Message[139].pos_y = 452;
					break;

				case ACCEPT:
					popup_gui_menu = 4;
					popup_menu = 1;
					Message[138].pos_x = 488;
					Message[138].pos_y = 482;
					Message[139].pos_y = 482;
					break;

				case CANCEL:
					popup_gui_menu = 0;
					for (int i = 0; i < 24; i++)
					{
						Player[MyID].localTrade[i][0] = 0;
						Player[MyID].localTrade[i][1] = 0;
						Player[MyID].remoteTrade[i][0] = 0;
						Player[MyID].remoteTrade[i][1] = 0;
						selectedLocalItem = 0;
						selectedRemoteItem = 0;
						//clear texts
						Message[90 + i].SetText(" ");
						Message[114 + i].SetText(" ");

						std::string ready = "Waiting...";
						Message[142].SetText(ready);
						Message[143].SetText(ready);
					}

					break;

				case OFFER:
				{
					int remote = socket->Data[3];
					int item = socket->Data[4];
					int amount = 0;
					((char*)&amount)[0] = socket->Data[5];
					((char*)&amount)[1] = socket->Data[6];
					((char*)&amount)[2] = socket->Data[7];
					((char*)&amount)[3] = socket->Data[8];

					//local
					if (remote == 1)
					{
						//find open item
						if (amount == 0)
						{
							//find the item in inventory already
							for (int i = 0; i < 24; i++)
								if (Player[MyID].localTrade[i][0] == (unsigned int)item) {
									Player[MyID].localTrade[i][1] = 0;
								}
						}

						int i;

						//find if there is an item
						for (i = 0; i < 24; i++)
							if (Player[MyID].localTrade[i][0] == (unsigned int)item && Player[MyID].localTrade[i][1] > 0)
								break;


						if (i < 24)//theres already an item, stack it
						{
							Player[MyID].localTrade[i][1] = amount;
							int tamount = amount;
							std::stringstream ss_am;
							std::string s_am;

							//10K
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
								s_am = ss_am.str();
							}
							else {
								s_am = "";
							}
							Message[90 + i].SetText(s_am);

						}
						else//find open slot
						{
							for (int i = 0; i < 24; i++)
								if (Player[MyID].localTrade[i][1] == 0)
								{
									Player[MyID].localTrade[i][0] = item;
									Player[MyID].localTrade[i][1] = amount;
									std::stringstream ss_am;
									ss_am << amount;
									std::string s_am = "x" + ss_am.str();

									if (amount <= 0)
										s_am = "";
									Message[90 + i].SetText(s_am);
									break;
								}

						}


					}
					else
					{

						//find open item
						if (amount == 0)
						{
							//find the item in inventory already
							for (int i = 0; i < 24; i++)
								if (Player[MyID].remoteTrade[i][0] == (unsigned int)item) {
									Player[MyID].remoteTrade[i][1] = 0;
								}
						}

						int i;

						//find if there is an item
						for (i = 0; i < 24; i++)
							if (Player[MyID].remoteTrade[i][0] == (unsigned int)item && Player[MyID].remoteTrade[i][1] > 0)
								break;


						if (i < 24)//theres already an item, stack it
						{
							Player[MyID].remoteTrade[i][1] = amount;
							int tamount = amount;
							std::stringstream ss_am;
							std::string s_am;

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

							Message[114 + i].SetText(s_am);

						}
						else//find open slot
						{
							for (int i = 0; i < 24; i++)
								if (Player[MyID].remoteTrade[i][1] == 0)
								{
									Player[MyID].remoteTrade[i][0] = item;
									Player[MyID].remoteTrade[i][1] = amount;
									std::stringstream ss_am;
									ss_am << amount;
									std::string s_am = ss_am.str();
									Message[114 + i].SetText(s_am);
									break;
								}

						}


					}
				}
				break;

				case READY:
				{
					int remote = socket->Data[3];
					std::string ready = "Accepted!";

					//local
					if (remote == 1)
						Message[142].SetText(ready);

					//other player
					if (remote == 2)
						Message[143].SetText(ready);

				}
				break;


				}//end trade switch
			}//end trade
			else if (code == BANK)
			{
				//bank window
				popup_gui_menu = 5;
				popup_menu = 1;

				//set item descriptior when item is selected
				Message[138].pos_x = 819;
				Message[139].pos_x = 964;
				Message[138].pos_y = 452;
				Message[139].pos_y = 452;
				//start at top



			}//end bank
			else if (code == SHOP)
			{
				shopBuyMode = true;
				popup_menu = 1;
				popup_gui_menu = 6;
				currentShop = socket->Data[2];
				//set item descriptior when item is selected
				Message[138].pos_x = 777;
				Message[139].pos_x = 964;
				Message[138].pos_y = 452;
				Message[139].pos_y = 452;

			}//end shop
			else if (code == ENEMY_HP)
			{

				int npc = socket->Data[2];
				int current_map = socket->Data[3];
				int hp = socket->Data[4];

				map[current_map]->Enemy[npc].hp_draw = hp;
				//printf("map[current_map]->Enemy[%i].hp_draw = %i]\n", npc, hp);

			}//end npc_hp
			else if (code == INVENTORY)
			{
				std::string inventory_order = socket->Data.substr(2);
				if (inventory_order.length() == 48)
				{
					for (int i = 0; i < 24; i++)
					{
						Player[MyID].inventory[i][0] = (unsigned char)inventory_order[i];
						Player[MyID].inventory[i][1] = (unsigned char)inventory_order[i+24];
					}
				}
			}
			else if (code == BUDDY_XP)
			{
				int pl = socket->Data[2];

				if (pl != MyID)
				{
					int xp = socket->Data[3];

					//set the xp bar of a buddy
					Player[pl].xp = xp;
				}
			} // end buddy xp
			else if (code == BUDDY_HP)
			{
				int pl = socket->Data[2];

				if (pl != MyID)
				{
					int hp = socket->Data[3];

					//set the xp bar of a buddy
					Player[pl].hp = hp;
				}
			} // end buddy hp
			else if (code == BUDDY_LEVEL)
			{
				int pl = socket->Data[2];

				if (pl != MyID)
				{
					int level = socket->Data[3];

					//set the xp bar of a buddy
					Player[pl].level = level;
				}
			} // end buddy level
			else if (code == WARP)
			{
				int pl = socket->Data[2];
				int map = socket->Data[3];

				//put each byte into memory
				char b1 = (char)socket->Data[4];
				char b2 = (char)socket->Data[5];
				char b3 = (char)socket->Data[6];
				char b4 = (char)socket->Data[7];
				char b5 = (char)socket->Data[8];
				char b6 = (char)socket->Data[9];
				char b7 = (char)socket->Data[10];
				char b8 = (char)socket->Data[11];

				//hold the result...
				float numx;

				//build an int from 4 bytes
				((char*)&numx)[0] = b1;
				((char*)&numx)[1] = b2;
				((char*)&numx)[2] = b3;
				((char*)&numx)[3] = b4;

				//hold the result...
				float numy;


				//build an int from 4 bytes
				((char*)&numy)[0] = b5;
				((char*)&numy)[1] = b6;
				((char*)&numy)[2] = b7;
				((char*)&numy)[3] = b8;

				if (pl == MyID)
					current_map = map;

				Player[pl].current_map = map;
				Player[pl].x = numx;
				Player[pl].y = numy;




				printf("map is: %i\n", map);
			} // end WARP
			else if (code == SPAWN_TARGET)
			{
				int target = socket->Data[2];
				int tmap = socket->Data[3];
				map[tmap]->Target[target].active = true;
			} // end SPAWN_TARGET
			else if (code == DESPAWN_TARGET)
			{
				int target = socket->Data[2];
				int tmap = socket->Data[3];
				map[tmap]->Target[target].active = false;
			} // end DESPAWN_TARGET
			else if (code == CHAT) //display the chat from the server
			{
				std::string inMessage = socket->Data.substr(2, pack_len - 2);
				printf("CHAT [%s]\n", inMessage.c_str());

				if (inMessage.length() > MAX_T_MESSAGE)
					inMessage = inMessage.substr(0, MAX_T_MESSAGE - 1);


				//scroll the stuff up
				chat_line[0] = chat_line[1];
				chat_line[1] = chat_line[2];
				chat_line[2] = chat_line[3];
				chat_line[3] = chat_line[4];
				chat_line[4] = chat_line[5];
				chat_line[5] = chat_line[6];
				chat_line[6] = chat_line[7];
				chat_line[7] = chat_line[8];
				chat_line[8] = chat_line[9];
				chat_line[9] = chat_line[10];
				chat_line[10] = chat_line[11];
				chat_line[11] = new std::string(inMessage);

				//display the new one
				for (int i = 0; i < 12; i++)
					Message[i + 3].SetText(chat_line[i]->c_str());
			}//end chat
			else 
			{
				//Unknown packets should not happen but might if they are parsed wrong or perhaps if the client is outdated
				for (unsigned int i = 0; i < socket->Data.length(); i++)
					printf("[%i]", socket->Data[i]);
			}

			//put the extra packet into just this one
			socket->Data = newPacket;
	}//end there was a full packet
}