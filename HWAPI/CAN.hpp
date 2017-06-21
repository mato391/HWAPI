#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
class CAN
{
public:
	typedef struct 
	{
		unsigned int id;
		struct
		{
			char rtr : 1;
			char length : 4;
		}header;

		uint8_t data[8];
	} messageCAN;

	messageCAN messageTx;
	messageCAN messageRx;

	CAN(int id);
	~CAN();
	void registerMessage();
private:
	int id_;
};

