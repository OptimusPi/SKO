#include "SKO_Network.h"
#include "KE_Socket.h"
#include "hasher.h"
#include <iostream>
#include <string>
#include <algorithm>

SKO_Network::SKO_Network()
{
}

void SKO_Network::init(KE_Socket *tempSocket)
{
	// Temporarily store a pointer to the PiSock object in main, whilst everything is migrated over to here
	// This is gonna be a nightmare..
	// When everything is migrated over I can move connect etc here and the socket can be a private variable of this class
	// instead.  Hopefully that makes sense.
	socket = tempSocket;
}

// Create an account
std::string SKO_Network::createAccount(std::string desiredUsername, std::string desiredPassword)
{
	Hasher hasher1;
	// Build the packet
	std::string packet = "0";
	packet += REGISTER; // 3 is REGISTER
	packet += desiredUsername;
	packet += " ";
	std::transform(desiredUsername.begin(), desiredUsername.end(), desiredUsername.begin(), ::tolower);
	packet += hasher1.Hash(desiredUsername + desiredPassword);
	packet[0] = packet.length();

	// Send the packet
	socket->Send(packet);

	// Get messages from the server
	if (socket->Connected)
		if (socket->BReceive() == -1) {
			//Done();
			// Need to look at Done(), seems silly and used all over, think I can get away without it for now
		}
	std::cout << socket->Data[1];
	if (socket->Data[1] == REGISTER_SUCCESS) // 8 is REGISTER_SUCCESS
	{
		// Register successfully, return successful to the client
		socket->Data = "";
		return "success";
	}
	else if (socket->Data[1] == REGISTER_FAIL_DOUBLE) // 9 is REGISTER_DOUBLE_DAIL
	{
		// Registration failed due to duplicate username in the database
		return "username exists";
	}
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