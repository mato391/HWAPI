#include "stdafx.h"
#include "Module.hpp"


Module::Module(int id) : 
	id_(id), 
	can_(new CAN(id)), 
	domain(id)
{
	std::cout << "MODULE " << id << " | " << domain.to_string() << " was created" << std::endl; 
	
}


Module::~Module()
{
}

void Module::sendWelcomeMessage()
{
	std::cout << "Module::sendWelcomeMessage" << std::endl;
	can_->messageTx.id = 100;
	can_->messageTx.data[0] = id_;
	can_->messageTx.data[1] = 170;
	can_->messageTx.data[2] = 0;
	can_->messageTx.data[3] = 0;
	can_->messageTx.data[4] = 0;
	can_->messageTx.data[5] = 0;
	can_->messageTx.data[6] = 0;
	can_->messageTx.data[7] = 0;
	///std::cout << "sendWelcomeMessage data size: " << static_cast<int>(can_->messageTx.data[0])<< std::endl;
	can_->registerMessage();
}

void Module::sendMessage()
{
	can_->registerMessage();
}

bool Module::loop()
{
	if (messageAvailable())
	{
		std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
		can_recv << "";
		can_recv.close();
		std::cout << "MESSAGE AVAILABLE" << std::endl;
		int id = 0;
		for (int i = 0; i < 8; i++)
		{
			id += std::stoi(buffer_.substr(i, 1)) * std::pow(2, (7 - i));
			std::cout << "TMP equaling id: " << std::stoi(buffer_.substr(i, 1)) << " * " << std::pow(2, (7 - i)) << " = " << id <<  std::endl;
		}
		can_->messageRx.id = id;
		std::cout << "Message from: " << id << std::endl;
		for (int i = 1; i <= 8; i++)
		{
			int tmp = 0;
			int powTmp = 7;
			for (int j = i * 8; j < (i + 1)*8; j++)
			{
				tmp += std::stoi(buffer_.substr(j, 1)) * std::pow(2, powTmp);
				powTmp--;
			}
			can_->messageRx.data[i] = tmp;
			std::cout << "Data[" << i << "] = " << tmp << std::endl;
		}
		std::cout << "DBGOWE COS CO: " << can_->messageRx.data[1] << std::endl;
		if (can_->messageRx.data[2] == 170)
		{
			std::cout << "AA message responded" << std::endl;
			can_->messageTx.id = can_->messageRx.data[1];
			can_->messageTx.data[0] = id_;
			can_->messageTx.data[1] = 187;
			can_->messageTx.data[2] = 15;
			can_->messageTx.data[3] = 15;
			can_->messageTx.data[4] = 15;
			can_->messageTx.data[5] = 15;
			can_->messageTx.data[7] = 0;
			can_->messageTx.data[6] = 0;
		}
		else if (can_->messageRx.data[1] == 187)
		{
			return false;
		}
		else
		{
			can_->messageTx = can_->messageRx;
			can_->messageTx.id = can_->messageRx.data[0];
			can_->messageTx.data[0] = id_;
		}
		
		return true;
	}
	return false;
}

bool Module::messageAvailable()
{
	std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::in);
	can_recv >> buffer_;
	can_recv.close();
	std::cout << "Module::messageAvailable: " << buffer_ << " | " << buffer_.size() << std::endl;
	if (buffer_ != "" && buffer_.size() == 72)
	{
		auto domainBinary = buffer_.substr(0, 8);
		//std::cout << domainBinary << " | " << domain.to_string()  << std::endl;
		if (domainBinary == domain.to_string())
			return true;
		else
			return false;
	}
	return false;
}
