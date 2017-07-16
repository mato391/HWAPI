#pragma once
#include <vector>
#include <iostream>
#include <bitset>
#include "CAN.hpp"
#include <boost\algorithm\string.hpp>
#include <boost\thread.hpp>
class Module
{
public:
	struct Connector
	{
		int id;
		int value;
	};
	std::vector<Connector*> connectors;
	Module(int id);
	~Module();
	void sendWelcomeMessage();
	bool loop();
	bool sendMessage();
	int id_;
	int protocol;
	std::bitset<8> domain;
private:
	CAN* can_;
	std::string buffer_;
	bool messageAvailable();
};

