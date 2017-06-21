#pragma once
#include <vector>
#include <iostream>
#include <bitset>
#include "CAN.hpp"
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
	void sendMessage();
	int id_;
	std::bitset<8> domain;
private:
	CAN* can_;
	std::string buffer_;
	bool messageAvailable();
};

