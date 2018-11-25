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

		std::string createAccount(std::string, std::string);

	private:
		KE_Socket *socket;
};