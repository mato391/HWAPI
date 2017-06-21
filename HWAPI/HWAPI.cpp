// HWAPI.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "CAN.hpp"
#include "Module.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost\algorithm\string.hpp>
#include <boost\thread.hpp>

#define OWNID 100

std::vector<Module*> modules;
void createModules();
void receive();

void createModules()
{
	std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\hwf.txt", std::ios::in);
	std::string content;
	if (mmf.good())
	{
		std::cout << "createModules: mmf good" << std::endl;
		mmf >> content;
	}
	mmf.close();
	std::vector<std::string> splittedByModules;
	boost::split(splittedByModules, content, boost::is_any_of(";"));
	for (const auto &module : splittedByModules)
	{
		//std::cout << "Getting elements from mmf " << module  << std::endl;
		std::vector<std::string> splittedByElements;
		boost::split(splittedByElements, module, boost::is_any_of("|"));
		for (const auto &element : splittedByElements)
		{
			std::vector<std::string> splittedByParams;
			boost::split(splittedByParams, element, boost::is_any_of(":"));
			if (splittedByParams[0] == "Module")
			{
				std::cout << "Module found" << std::endl;
				std::vector<std::string> splittedByValues;
				boost::split(splittedByValues, splittedByParams[1], boost::is_any_of(","));
				modules.push_back(new Module(std::stoul(splittedByValues[1], nullptr, 16)));
			}
			else if (splittedByParams[0].find("Connector") != std::string::npos)
			{
				std::cout << "Connector found" << std::endl;
				std::vector<std::string> splittedByValues;
				boost::split(splittedByValues, splittedByParams[1], boost::is_any_of(","));
				for (auto &module : modules)
				{
					if (module->connectors.size() <= std::stoi(splittedByValues[0]))
					{
						module->connectors.push_back(new Module::Connector());
						module->connectors.back()->id = std::stoi(splittedByValues[0]);
						module->connectors.back()->value = !std::stoi(splittedByValues[0]);
					}

				}
			}
		}
		std::cout << "sending welcome Msg from module: " << modules.back()->id_ << std::endl;
		modules.back()->sendWelcomeMessage();
		receive();
	}
}

void receive()
{
	for (;;)
	{
		for (const auto &module : modules)
		{
			if (module->loop())
			{
				module->sendMessage();
			}
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(2000));
	}
	
}

CAN myCAN = CAN(OWNID);

// Setting up our devices and I/Os
void setup() {


	// Let's open the bus. Remember the input parameter:
	// 1: 1Mbps
	// 500: 500Kbps  <--- Most frequently used
	// 250: 250Kbp
	// 125: 125Kbps
	//myCAN.begin(1000);
}

void loop() {

	//****************************************
	// 1. Receive data
	//****************************************

	/*if (myCAN.messageAvailable() == 1) {
		// Read the last message received.
		myCAN.getMessage(&myCAN.messageRx);
		// Print in the serial monitor the received message
		myCAN.printMessage(&myCAN.messageRx);
	}*/


	//****************************************
	// 2. Send data
	//****************************************

	// Insert the ID in the data structure
	/*myCAN.messageTx.id = OWNID;
	// These fields include the data to send
	myCAN.messageTx.data[0] = 0;
	myCAN.messageTx.data[1] += 1;
	myCAN.messageTx.data[2] = 2;
	myCAN.messageTx.data[3] = 3;
	myCAN.messageTx.data[4] = 4;
	myCAN.messageTx.data[5] = 5;
	myCAN.messageTx.data[6] = 6;
	myCAN.messageTx.data[7] = 7;

	// The length of the data structure
	myCAN.messageTx.header.length = 8;
	// Send data
	myCAN.sendMessage(&myCAN.messageTx);
	// A time delay
	delay(1000);*/
}

int main() {
	setup();
	createModules();
	while (1) {
		loop();
	}
	return (0);
}

