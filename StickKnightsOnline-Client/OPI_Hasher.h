#ifndef _OPI_Hasher_CPP__
#define _OPI_Hasher_CPP__

#include <string>

class OPI_Hasher
{
public:
	OPI_Hasher();
	bool verifyHash(std::string secret, std::string expectedHash);
	void setHashLength(unsigned char hashLength);
	void setDefaultSalt(std::string defaultSalt);
	std::string getUsername();
	std::string getPasswordHash();
	void storePassword(std::string username, std::string passwordCleartext);
private:
	//Hashing properties
	unsigned char hashLength;
	std::string defaultSalt;

	//Safe password storage
	std::string passwordHash;
	std::string username;

	//Hashing functions
	std::string generateHash(std::string salt, std::string secret);
	std::string generateHash(std::string secret);
};

#endif