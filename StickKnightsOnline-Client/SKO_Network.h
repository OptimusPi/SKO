#pragma once
#include "KE_Socket.h"
#include <string>

class SKO_Network
{
	public:
		SKO_Network();
		std::string init(std::string server, unsigned short port);
		std::string connect();
		std::string sendVersion(unsigned char major, unsigned char minor, unsigned char patch);
		bool isConnected();
		bool SKO_Network::TryReconnect(unsigned int retries, unsigned int sleep);
		
		void receivePacket(bool);
		void saveInventory(unsigned int [24][2]);
		void allocateStatPoint(std::string desiredStat);

		//using items
		void unequipItem(unsigned char equipmentSlot);
		void useItem(unsigned int itemId);
		void dropItem(unsigned char itemId, unsigned int amount);
		void castSpell();

		//Movement and actions
		void playerAction(std::string actionType, float playerX, float playerY);
		void sendChat(std::string message);

		//Clan with other players
		void createClan(std::string clanName);
		void sendClanInvite(unsigned char requestedPlayer);
		void acceptClanInvite();
		void cancelClanInvite();

		//Party with other players
		void cancelParty();
		void sendPartyInvite(unsigned char playerId);
		void acceptPartyInvite();

		//Trading with other players
		void cancelTrade();
		void confirmTrade();
		void acceptTradeInvite();
		void sendTradeInvite(unsigned char playerId);
		void setTradeItemOffer(unsigned char itemId, unsigned int amount);

		//Using a shop
		void openShop(unsigned char shopId);
		void closeShop();
		void buyItem(unsigned char itemSelectionId, unsigned int amount);
		void sellItem(unsigned char itemId, unsigned int amount);

		//Using a bank
		void depositBankItem(unsigned char itemId, unsigned int amount);
		void withdrawalBankItem(unsigned char itemId, unsigned int amount);
		void closeBank();

		//Client health check
		void checkPing();

		//SKO account functions.
		std::string createAccount(std::string, std::string);
		std::string sendLoginRequest(std::string username, std::string password);
		std::string getSaltedHash(std::string username, std::string password);
	private:
		//TCP Socket to SKO Server
		KE_Socket *socket;
		//Server information
		std::string server;
		unsigned short port;

		//ping stuff
		unsigned long int pingRequestTime = 0;
		unsigned long int pingRequestTicker = 0;
		bool pingWaiting = false;

		//Packet numeric formatting helpers
		std::string getPacketFloat(float);
		std::string getPacketInt(unsigned int);
		std::string getPacketShort(unsigned short);
		std::string pingPacket;

		

		std::string getAsString(int value);
		std::string getAsString(unsigned int value);
		std::string getAsString(short value);
		std::string getAsString(unsigned short value);
		std::string getAsString(float value);

		template<typename T>
		std::string getAsString(T const& t);

		template<typename T>
		std::string getAsPacket(T const& t);

		template<typename First, typename ... Rest>
		std::string getAsPacket(First const& first, Rest const& ... rest);

		template<typename First, typename ... Rest>
		void send(First const& first, Rest const& ... rest);



};