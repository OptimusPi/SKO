#include "argon2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "base64.h"
#include "OPI_Hasher.h"
#include <algorithm>

// Constructor
OPI_Hasher::OPI_Hasher()
{
	this->defaultSalt = "2616e26e9c5a4decb08353c1bcb2cf7e";
	this->hashLength = 32;
}

// Setters
void OPI_Hasher::setDefaultSalt(std::string defaultSalt)
{
	this->defaultSalt = defaultSalt;
}
void OPI_Hasher::setHashLength(unsigned char hashLength)
{
	this->hashLength = hashLength;
}

// Getters
std::string OPI_Hasher::getUsername()
{
	return this->username;
}
std::string OPI_Hasher::getPasswordHash()
{
	return this->passwordHash;
}
bool OPI_Hasher::verifyHash(std::string secret, std::string expectedHash)
{
	if (generateHash(secret) != expectedHash)
	{
		printf("The OPI_Hasher does not seem to be working properly. Check argon2 implementation.\r\n");
		return false;
	}
	// If there are no errors, this hasher is verified to work 
	return true;
}
// Store username and password safely
void OPI_Hasher::storePassword(std::string username, std::string passwordCleartext)
{
	// Store username in cleartext,
	// But hash the password 
	this->username = username;

	//We must use a lowercase username because a user may want to login sometimes as "pifreak" and sometimes as "PiFreak"
	std::transform(username.begin(), username.end(), username.begin(), ::tolower);
	this->passwordHash = generateHash(username + passwordCleartext);
}



// Hashing Functions
std::string OPI_Hasher::generateHash(std::string secret)
{
	return generateHash(this->defaultSalt, secret);
}
std::string OPI_Hasher::generateHash(std::string _salt, std::string _secret)
{
	size_t hashLength = this->hashLength;
	size_t saltLength = _salt.length();
	std::string hashResult = "";
	uint8_t hashArray[hashLength];
	uint8_t saltArray[_salt.length()];

	// Copy salt to saltArray
	for (size_t i = 0; i < saltLength; i++)
		saltArray[i] = (uint8_t)_salt[i];

	uint8_t *pwd = (uint8_t *)strdup(_secret.c_str());
	uint32_t pwdlen = strlen((char *)pwd);
	uint32_t t_cost = 2;            // 1-pass computation
	uint32_t m_cost = (1 << 16);    // 64 mebibytes memory usage
	uint32_t parallelism = 1;       // number of threads and lanes

									// high-level API
	argon2d_hash_raw(t_cost, m_cost, parallelism, pwd, pwdlen, saltArray, saltLength, hashArray, hashLength);

	// Copy hashResult from hashArray
	for (size_t i = 0; i<hashLength; ++i)
		hashResult += hashArray[i];

	// Return
	return base64_encode(hashResult);
}