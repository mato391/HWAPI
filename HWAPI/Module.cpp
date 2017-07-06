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
	can_->messageTx.data[0] = 0;
	can_->messageTx.data[1] = id_;
	can_->messageTx.data[2] = 170;
	can_->messageTx.data[3] = 0;
	can_->messageTx.data[4] = 0;
	can_->messageTx.data[5] = 0;
	can_->messageTx.data[6] = 0;
	can_->messageTx.data[7] = 0;
	///std::cout << "sendWelcomeMessage data size: " << static_cast<int>(can_->messageTx.data[0])<< std::endl;
	can_->registerMessage();
}

bool Module::sendMessage()
{
	can_->registerMessage();
	if (can_->messageTx.data[2] == 188)
		return false;
	else
		return true;
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
		if (id == this->id_)
		{

			std::cout << "Message to: " << id << std::endl;
			for (int i = 1; i <= 8; i++)
			{
				int tmp = 0;
				int powTmp = 7;
				for (int j = i * 8; j < (i + 1) * 8; j++)
				{
					tmp += std::stoi(buffer_.substr(j, 1)) * std::pow(2, powTmp);
					powTmp--;
				}
				can_->messageRx.data[i - 1] = tmp;
				std::cout << "Data[" << i << "] = " << tmp << std::endl;
			}
			buffer_ = "";
			std::cout << "DBGOWE COS CO: " << can_->messageRx.data[1] << std::endl;
			int protocol = can_->messageRx.data[0];
			if (protocol == 0)
			{
				if (can_->messageRx.data[2] == 170)
				{
					std::cout << "AA message responded" << std::endl;
					can_->messageTx.id = can_->messageRx.data[1];
					can_->messageTx.data[0] = 0;
					can_->messageTx.data[1] = id_;
					can_->messageTx.data[2] = 187;

					int i = connectors.size();
					int bytes = i / 8;
					std::cout << "Connectors counter: " << i << " bytes to use: " << bytes << std::endl;
					int rest = i % 8;
					if (bytes > 0)
					{
						for (int i = 0; i < bytes; i++)
						{
							std::cout << "I: " << i << " messageTx.data[" << 3 + i << "]" << std::endl;
							can_->messageTx.data[3 + i] = 255;
						}

						can_->messageTx.data[3 + bytes] = std::pow(2, rest);
						for (int i = bytes + 3; i < 7 - bytes; i++)
						{
							std::cout << "messageTx.data[" << i << "] = " << 0 << std::endl;
							can_->messageTx.data[i] = 0;
						}
					}
					else
					{
						can_->messageTx.data[3] = std::pow(2, i - 1);
					}
					for (const auto &byte : can_->messageTx.data)
					{
						std::cout << static_cast<int>(byte) << std::endl;
					}
				}
				else if (can_->messageRx.data[2] == 188)
				{
					protocol = can_->messageRx.data[3] + can_->messageRx.data[4];
					std::cout << "PROTOCOL SETUP MESSAGE DETECTED" << can_->messageRx.data[3] << can_->messageRx.data[4] << std::endl;
					can_->messageTx = can_->messageRx;
					can_->messageTx.id = can_->messageRx.data[1];
					can_->messageTx.data[2] = can_->messageRx.data[2];
					can_->messageTx.data[1] = id_;
					can_->messageRx.data[0] = protocol;
				}
				else if (can_->messageRx.data[2] == 204)
				{
					for (auto &conn : connectors)
					{
						if (conn->id == can_->messageRx.data[3])
						{
							std::cout << "MODULE PORT " << can_->messageRx.data[3] << " CHANGING VALUE TO " << can_->messageRx.data[4] << std::endl;
							conn->value = can_->messageRx.data[4];
						}
					}
					can_->messageTx = can_->messageRx;
					can_->messageTx.id = can_->messageRx.data[1];
					can_->messageTx.data[2] = can_->messageRx.data[2];
					can_->messageTx.data[1] = id_;
					can_->messageTx.data[0] = can_->messageRx.data[0];
				}
				else
				{
					can_->messageTx = can_->messageRx;
					can_->messageTx.id = can_->messageRx.data[1];
					can_->messageTx.data[1] = id_;
					can_->messageTx.data[0] = can_->messageRx.data[0];
				}
			}
			else if (protocol == 1)
			{
				if (can_->messageRx.data[2] == 204)
				{
					for (auto &conn : connectors)
					{
						if (conn->id == can_->messageRx.data[3])
						{
							std::cout << "MODULE PORT " << can_->messageRx.data[3] << " CHANGING VALUE TO " << can_->messageRx.data[4] << std::endl;
							conn->value = can_->messageRx.data[4];
							break;
						}
					}
					can_->messageTx = can_->messageRx;
					can_->messageTx.id = can_->messageRx.data[1];
					can_->messageTx.data[2] = 205;	//CD
					can_->messageTx.data[1] = id_;
					can_->messageTx.data[0] = protocol;
					can_->messageTx.data[3] = can_->messageRx.data[3];
					can_->messageTx.data[4] = can_->messageRx.data[4];
					can_->messageTx.data[5] = 0;
					can_->messageTx.data[6] = 0;
					can_->messageTx.data[7] = 0;
					return true;
				}
			}
		}
		return true;
	}
	else
	{
		buffer_ = "";
		return false;
	}
	
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
		{
			buffer_ = "";
			return false;
		}
			
	}
	buffer_ = "";
	return false;
}
