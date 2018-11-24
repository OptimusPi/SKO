#include "argon2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "base64.h"
#include "hasher.h"

const int HASHLEN = 32;
const int SALTLEN = 32;
const std::string _salt = "2616e26e9c5a4decb08353c1bcb2cf7e";


Hasher::Hasher()
{

}

std::string Hasher::Hash(std::string _password)
{
	std::string hashResult = "";
	//const uint8_t SALTLEN = _salt.length();
	uint8_t hash[HASHLEN];
	uint8_t salt[SALTLEN];

	for (int i = 0; i < SALTLEN; i++)
		salt[i] = (uint8_t)_salt[i];

	uint8_t *pwd = (uint8_t *)strdup(_password.c_str());
	uint32_t pwdlen = strlen((char *)pwd);
	uint32_t t_cost = 2;            // 1-pass computation
	uint32_t m_cost = (1 << 16);    // 64 mebibytes memory usage
	uint32_t parallelism = 1;       // number of threads and lanes

									// high-level API
	argon2d_hash_raw(t_cost, m_cost, parallelism, pwd, pwdlen, salt, SALTLEN, hash, HASHLEN);


	for (int i = 0; i<HASHLEN; ++i)
		hashResult += hash[i];

	return base64_encode(hashResult);
}