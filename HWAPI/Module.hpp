#pragma once
#include <vector>
#include <iostream>
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
private:
	int id_;
	CAN* can_;
};

