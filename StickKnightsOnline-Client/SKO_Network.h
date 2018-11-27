#pragma once
#include "KE_Socket.h"
#include <string>

class SKO_Network
{
	public:
		SKO_Network();
		void init(KE_Socket*);
		void receivePacket(bool);
		void saveInventory(unsigned int [24][2]);
		void allocateStatPoint(std::string);
		void useItem(unsigned int);
		void createClan(std::string);
		
		void castSpell();

		//Trading with other players
		void sendTradeItemOffer(unsigned char item, unsigned int amount);

		//Client health check
		void checkPing();

		//SKO account functions.
		std::string createAccount(std::string, std::string);
		std::string sendLoginRequest(std::string username, std::string password);
	private:
		//TCP Socket to SKO Server
		KE_Socket *socket;

		//ping stuff
		unsigned long int pingRequestTime = 0;
		unsigned long int pingRequestTicker = 0;
		bool pingWaiting = false;
		std::string pingPacket;
		std::string getPacketFloat(float);
		std::string getPacketInt(unsigned int);
		std::string getPacketShort(unsigned short);
};