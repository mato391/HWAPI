#pragma once
#include <vector>
#include <iostream>
#include <bitset>
#include "CAN.hpp"
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

class Interuption
{
public:
	Interuption(int connId) { this->connId = connId; value = false; };
	int connId;
	bool value;
};

class Module
{
public:
	struct Connector
	{
		int id;
		int value;
	};
	std::vector<Connector*> connectors;
	Module(int id, src::logger_mt& lg);
	~Module();
	void sendWelcomeMessage();
	bool loop();
	bool sendMessage();
	int id_;
	int protocol;
	std::bitset<8> domain;
private:
	src::logger_mt& lg_;
	CAN* can_;
	std::string buffer_;
	Interuption* interuption_;
	bool protocol7Flag_;
	bool messageAvailable();
	void protocol2();
	void protocol6();
	void protocol7();
	boost::mutex mtx;
};

