#pragma once
#include "KE_Socket.h"
#include <string>

class SKO_Network
{
	public:
		SKO_Network(KE_Socket);
		inline std::string createAccount(std::string, std::string);
		//void init(KE_Socket);

	private:
		KE_Socket socket;
		std::string HashTemp(std::string);
};

