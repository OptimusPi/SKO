#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include "SKO_PacketFactory.h"

SKO_PacketFactory::SKO_PacketFactory()
{
	
}

std::string SKO_PacketFactory::getPacketFloat(float value)
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
std::string SKO_PacketFactory::getPacketInt(unsigned int value)
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
std::string SKO_PacketFactory::getPacketShort(unsigned short value)
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

std::string SKO_PacketFactory::getAsString(int value)
{
	return getPacketInt(value);
}

std::string SKO_PacketFactory::getAsString(unsigned int value)
{
	return getPacketInt(value);
}

std::string SKO_PacketFactory::getAsString(short value)
{
	return getPacketShort(value);
}

std::string SKO_PacketFactory::getAsString(unsigned short value)
{
	return getPacketShort(value);
}

std::string SKO_PacketFactory::getAsString(float value)
{
	return getPacketFloat(value);
}

template<typename T>
std::string SKO_PacketFactory::getAsString(T const& t)
{
	//declare return value
	std::string str = "";

	//default to std::string concatenation
	str += t;

	//return converted string
	return str;
}

template<typename T>
std::string SKO_PacketFactory::getAsPacket(T const& t)
{
	return getAsString(t);
}

template<typename First, typename ... Rest>
std::string SKO_PacketFactory::getAsPacket(First const& first, Rest const& ... rest)
{
	return getAsString(first) + getAsPacket(rest ...);
}

template<typename First, typename ... Rest>
std::string SKO_PacketFactory::getPacket(First const& first, Rest const& ... rest)
{
	//declare packet
	std::string packet = "0";

	//fill with formatted packet data
	packet += getAsPacket(first, rest ...);
	packet[0] = packet.length();

	//return resulting packet std::string
	return packet;
}