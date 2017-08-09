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
#include <boost/log/trivial.hpp>
#include <boost/move/utility.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/filesystem.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;



BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::logger_mt)

#define OWNID 100
#define DOOR_UNLOCK "011001000000000100000101110011000000101000000001000000000000000000000000"
#define DOOR_LOCK   "011001000000000100000101110011000000101000000000000000000000000000000000"
#define OPEN_FRONT_LEFT  "011001000000000100000101110011000000000000000001000000000000000000000000"
#define CLOSE_FRONT_LEFT "011001000000000100000101110011000000000000000000000000000000000000000000"
#define LIGHT_BEAM_ON "011001000000001000000110110011000000001000000000000000000000000000000000"
#define LIGHT_BEAM_OFF "011001000000001000000110110011000000001000000001000000000000000000000000"
#define EMCY_LIGHT_ON  "011001000000000100000010110011000000000000000001000000000000000000000000"
#define EMCY_LIGHT_OFF "011001000000000100000010110011000000000000000000000000000000000000000000"

std::vector<Module*> modules;
void createModules(src::logger_mt& logger);
void receive(src::logger_mt& logger);
void checkAndExecuteEnvSignal(src::logger_mt& logger);



void createModules(src::logger_mt& logger)
{
	std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\hwf.txt", std::ios::in);
	std::string content;
	if (mmf.good())
	{
		BOOST_LOG(logger) << "INF: " << "createModules: mmf good";
		mmf >> content;
	}
	mmf.close();
	std::vector<std::string> splittedByModules;
	boost::split(splittedByModules, content, boost::is_any_of(";"));
	for (const auto &module : splittedByModules)
	{
		if (module.size() != 0 )
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
					BOOST_LOG(logger) << "INF " << "Module found";
					std::vector<std::string> splittedByValues;
					boost::split(splittedByValues, splittedByParams[1], boost::is_any_of(","));
					modules.push_back(new Module(std::stoul(splittedByValues[1], nullptr, 16), logger));
				}
				else if (splittedByParams[0].find("Connector") != std::string::npos && 
					splittedByParams[0].find("Group") == std::string::npos)
				{
					BOOST_LOG(logger) << "INF " << "Connector found";
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
			BOOST_LOG(logger) << "________________________________________________________________________________";
			BOOST_LOG(logger) << "INF " << "sending welcome Msg from module : " << modules.back()->id_;
			modules.back()->sendWelcomeMessage();
			if (modules.back()->domain != 7)
				receive(logger);
			BOOST_LOG(logger) << "INF " << "Module initialized ";
			
		}
		
	}
}

void receive(src::logger_mt& logger)
{
	bool notLastForModule = true;
	while (notLastForModule)
	{
		for (const auto &module : modules)
		{
			BOOST_LOG(logger) << "INF " << "receiveLoop: module: " << module->domain;
			if (module->loop())
			{
				notLastForModule = module->sendMessage();
				BOOST_LOG(logger) << "INF " << "Last message : " << notLastForModule;
			}
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(200));
	}
	BOOST_LOG(logger) << "____________________END OF RECEIVE_____________________";
}

//CAN myCAN = CAN(OWNID, lg);

// Setting up our devices and I/Os
void setup() {


	// Let's open the bus. Remember the input parameter:
	// 1: 1Mbps
	// 500: 500Kbps  <--- Most frequently used
	// 250: 250Kbp
	// 125: 125Kbps
	//myCAN.begin(1000);
}

void checkAndExecuteEnvSignal(src::logger_mt& logger)
{
	std::fstream sig("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\simulator.txt", std::ios::in);
	std::string content;
	if (sig.good())
	{
		BOOST_LOG(logger) << "INF " << "checkAndExecuteEnvSignal " << "SIGNAL good" << std::endl;
		sig >> content;
	}
	sig.close();
	BOOST_LOG(logger) << "INF " << __FUNCTION__ << "SIGNAL: " << content;
	if (content != "")
	{
		if (content.find("DOOR_UNLOCK") != std::string::npos)
		{
			std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
			std::string content = DOOR_UNLOCK;
			if (mmf.good())
			{
				mmf << content;
			}
			mmf.close();
		}
		else if (content.find("DOOR_LOCK") != std::string::npos)
		{
			std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
			std::string content = DOOR_LOCK;
			if (mmf.good())
			{
				mmf << content;
			}
			mmf.close();
		}
		else if (content.find("OPEN_FRONT_LEFT") != std::string::npos)
		{
			std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
			std::string content = OPEN_FRONT_LEFT;
			if (mmf.good())
			{
				mmf << content;
			}
			mmf.close();
		}
		else if (content.find("CLOSE_FRONT_LEFT") != std::string::npos)
		{
			std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
			std::string content = CLOSE_FRONT_LEFT;
			if (mmf.good())
			{
				mmf << content;
			}
			mmf.close();
		}
		else if (content.find("LIGHT_BEAM_ON") != std::string::npos)
		{
			std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
			std::string content = LIGHT_BEAM_ON;
			if (mmf.good())
			{
				mmf << content;
			}
			mmf.close();
		}
		else if (content.find("LIGHT_BEAM_OFF") != std::string::npos)
		{
			std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
			std::string content = LIGHT_BEAM_OFF;
			if (mmf.good())
			{
				mmf << content;
			}
			mmf.close();
		}
		else if (content.find("EMCY_LIGHT_ON") != std::string::npos)
		{
			std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
			std::string content = EMCY_LIGHT_ON;
			if (mmf.good())
			{
				mmf << content;
			}
			mmf.close();
		}
		else if (content.find("EMCY_LIGHT_OFF") != std::string::npos)
		{
			std::fstream mmf("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
			std::string content = EMCY_LIGHT_OFF;
			if (mmf.good())
			{
				mmf << content;
			}
			mmf.close();
		}
	}
	std::remove("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\simulator.txt");
}

void loop(src::logger_mt& logger) {

	while (1)
	{
		for (const auto &mod : modules)
		{
			if (mod->loop())
			{
				mod->sendMessage();
			}
		}
		checkAndExecuteEnvSignal(logger);
		boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	}
	
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
	src::logger_mt& lg = my_logger::get();
	logging::add_file_log
	(
		keywords::file_name = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Logs\\SIM_SYSLOG_%N.log",
		keywords::rotation_size = 10 * 1024 * 1024,
		keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = "[%TimeStamp%]:[%ThreadID%]:%Message%",
		keywords::auto_flush = true
	);
	lg.add_attribute("ThreadID", boost::log::attributes::current_thread_id());
	logging::add_common_attributes();
	logging::record rec = lg.open_record();

	setup();
	createModules(lg);
	BOOST_LOG(lg) << "INF " << "GOTO RUNTIME";
	while (1) {

		loop(lg);
	}
	return (0);
}

