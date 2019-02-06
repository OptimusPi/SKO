#pragma once
#include "KE_Socket.h"
#include "SKO_PacketTypes.h"
#include "SKO_PacketFactory.h"
#include "OPI_Text.h"
#include <string> 

class SKO_Network
{
	public:
		SKO_Network();
		std::string init(std::string server, unsigned short port);
		std::string connect();
		void disconnect();
		bool done = false;

		void sendVersion(unsigned char major, unsigned char minor, unsigned char patch);
		bool isConnected();
		bool TryReconnect(unsigned long long int timeout);
		
		void receivePacket(bool);
		void saveInventory(unsigned int [24][2]);
		void allocateStatPoint(std::string desiredStat);

		//using items
		void unequipItem(unsigned char equipmentSlot);
		void useItem(unsigned char itemId);
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
		void openStall(unsigned char shopId);
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
		void createAccount(std::string, std::string);
		void sendLoginRequest(std::string username, std::string passwordHash);
	private:
		//TCP Socket to SKO Server
		KE_Socket *socket;

		//Server information
		std::string server;
		unsigned short port;

		//ping stuff
		unsigned long long int pingRequestTime = 0;
		unsigned long long int pingRequestTicker = 0;
		bool pingWaiting = false;

		template<typename First, typename ... Rest>
		void send(First const& first, Rest const& ... rest);

		//debug flag for logging
		bool log;
};