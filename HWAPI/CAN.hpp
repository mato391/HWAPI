#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
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

	CAN(int id, src::logger_mt& lg);
	~CAN();
	void registerMessage();
private:
	int id_;
	src::logger_mt& lg_;
};

