

#ifndef _HASHER_H_
#define _HASHER_H_

#include <stdint.h>
#include <cstddef>
#include <vector>

class Hasher {
	public:
		static std::string Hash(std::string password);

	private:
		Hasher();
		uint32_t pary_[18];
		uint32_t sbox_[4][256];
		std::vector<char> Encrypt(const std::vector<char> &src);
		void SetKey(const char *key, size_t byte_length);
		void EncryptBlock(uint32_t *left, uint32_t *right);
		uint32_t Feistel(uint32_t value);

};

#endif /* defined(__blowfish__h) */