#include "stdafx.h"
#include "CAN.hpp"


CAN::CAN(int id, src::logger_mt& lg) : id_(id), lg_(lg)
{

}


CAN::~CAN()
{
}

void CAN::registerMessage()
{
	std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::in);
	std::string isEmpty;
	can_recv >> isEmpty;
	can_recv.close();
	BOOST_LOG(lg_) << "INF " << "CAN::registerMessage " << "registerMessage::precondition: register is: " << isEmpty << " | " << isEmpty.size();
	if (isEmpty.size() == 0)
	{
		std::bitset<8> id(static_cast<int>(messageTx.id));
		std::string msg = id.to_string();

		for (const auto &element : messageTx.data)
		{
			std::bitset<8> bitset(static_cast<int>(element));
			BOOST_LOG(lg_) << "INF " << "CAN::registerMessage " << "element: " << static_cast<int>(element) << " | " << bitset.to_string();
			msg += bitset.to_string();
		}
		BOOST_LOG(lg_) << "INF " << "CAN::registerMessage " << "CAN::registerMessage: SENDING: " << msg;
		BOOST_LOG(lg_) << "____________________________________________________";
		std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
		can_recv << msg;
		can_recv.close();
	}
	else
	{	
		std::cout << "BUFFER IS NOT EMPTY" << std::endl;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
		registerMessage();
	}
}