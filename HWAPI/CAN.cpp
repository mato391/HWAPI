#include "stdafx.h"
#include "CAN.hpp"


CAN::CAN(int id) : id_(id)
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
	std::cout << "registerMessage::precondition: register is: " << isEmpty << " | " << isEmpty.size() << std::endl;
	if (isEmpty.size() == 0)
	{
		std::bitset<8> id(static_cast<int>(messageTx.id));
		std::string msg = id.to_string();

		for (const auto &element : messageTx.data)
		{
			std::bitset<8> bitset(static_cast<int>(element));
			std::cout << "element: " << static_cast<int>(element) << " | " << bitset.to_string() << std::endl;
			msg += bitset.to_string();
		}
		std::cout << "CAN::registerMessage: " << msg << std::endl;
		std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
		can_recv << msg;
		can_recv.close();
	}	
}