#ifndef __SKO_PACKETFACTORY_H_
#define __SKO_PACKETFACTORY_H_

#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>

class SKO_PacketFactory
{
public: 
	static std::string getPacketFloat(float value)
	{
		//convert 4-byte floating point value into a string of bytes 
		char * bytes = (char*)&value;

		//construct packet format string of bytes
		std::string packetBytes = "";
		packetBytes += bytes[0];
		packetBytes += bytes[1];
		packetBytes += bytes[2];
		packetBytes += bytes[3];

		//return a 4-byte string
		return packetBytes;
	}

	static std::string getPacketInt(unsigned int value)
	{
		//convert 4-byte integer value into a string of bytes 
		char * bytes = (char*)&value;

		//construct packet format string of bytes
		std::string packetBytes = "";
		packetBytes += bytes[0];
		packetBytes += bytes[1];
		packetBytes += bytes[2];
		packetBytes += bytes[3];

		//return a 4-byte string
		return packetBytes;
	}

	static std::string getPacketShort(unsigned short value)
	{
		//convert 2-byte short integer value into a string of bytes 
		char * bytes = (char*)&value;

		//construct packet format string of bytes
		std::string packetBytes = "";
		packetBytes += bytes[0];
		packetBytes += bytes[1];

		//return a 4-byte string
		return packetBytes;
	}

	static std::string getAsString(int value)
	{
		return getPacketInt(value);
	}

	static std::string getAsString(unsigned int value)
	{
		return getPacketInt(value);
	}

	static std::string getAsString(short value)
	{
		return getPacketShort(value);
	}

	static std::string getAsString(unsigned short value)
	{
		return getPacketShort(value);
	}

	static std::string getAsString(float value)
	{
		return getPacketFloat(value);
	}

	template<typename T>
	static std::string getAsString(T const& t)
	{
		//declare return value
		std::string str = "";

		//default to std::string concatenation
		str += t;

		//return converted string
		return str;
	}

	template<typename T>
	static std::string getAsPacket(T const& t)
	{
		return getAsString(t);
	}

	template<typename First, typename ... Rest>
	static std::string getAsPacket(First const& first, Rest const& ... rest)
	{
		return getAsString(first) + getAsPacket(rest ...);
	}

	template<typename First, typename ... Rest>
	static std::string getPacket(First const& first, Rest const& ... rest)
	{
		//declare packet
		std::string packet = "0";

		//fill with formatted packet data
		packet += getAsPacket(first, rest ...);
		packet[0] = packet.length();

		//return resulting packet std::string
		return packet;
	}
};



#endif // !__SKO_PACKETFACTORY_H_
