#ifndef __SKO_PACKETFACTORY_H_
#define __SKO_PACKETFACTORY_H_


#include <string>

class SKO_PacketFactory
{

public:
	SKO_PacketFactory();

	template<typename First, typename ... Rest>
	std::string getPacket(First const& first, Rest const& ... rest);
	//Packet numeric formatting helpers
	std::string getPacketFloat(float);
	std::string getPacketInt(unsigned int);
	std::string getPacketShort(unsigned short);
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
};

#endif // !__SKO_PACKETFACTORY_H_
