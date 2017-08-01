#include "stdafx.h"
#include "Module.hpp"


Module::Module(int id, src::logger_mt& lg) :
	id_(id), 
	can_(new CAN(id, lg)), 
	domain(id),
	lg_(lg)
{
	BOOST_LOG(lg_) << "INF " << "MODULE " << id << " | " << domain.to_string() << " was created";
}


Module::~Module()
{
}

void Module::sendWelcomeMessage()
{
	BOOST_LOG(lg_) << "INF " << "Module::sendWelcomeMessage";
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
	if (can_->messageTx.data[2] == 188 || (can_->messageTx.data[2] == 187 && can_->messageTx.data[1] == 7))
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
		BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "MESSAGE AVAILABLE";
		int id = 0;
		for (int i = 0; i < 8; i++)
		{
			id += std::stoi(buffer_.substr(i, 1)) * std::pow(2, (7 - i));
			BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "TMP equaling id: " << std::stoi(buffer_.substr(i, 1)) << " * " << std::pow(2, (7 - i)) << " = " << id;
		}
		can_->messageRx.id = id;
		if (id == this->id_)
		{

			BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "Message to: " << id;
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
				BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "Data[" << i << "] = " << tmp;
			}
			buffer_ = "";
			//std::cout << "DBGOWE COS CO: " << can_->messageRx.data[1] << std::endl;
			int protocol = can_->messageRx.data[0];
			if (protocol == 0)
			{
				if (can_->messageRx.data[2] == 170)
				{
					BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "AA message responded";
					can_->messageTx.id = can_->messageRx.data[1];
					can_->messageTx.data[0] = 0;
					can_->messageTx.data[1] = id_;
					can_->messageTx.data[2] = 187;

					int i = connectors.size();
					int bytes = i / 8;
					BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "Connectors counter: " << i << " bytes to use: " << bytes;
					int rest = i % 8;
					if (bytes > 0)
					{
						BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "I: " << i << " messageTx.data[" << 3 << "]";
						can_->messageTx.data[3] = 255;
						can_->messageTx.data[4] = std::pow(2, rest);

					}
					else
					{
						can_->messageTx.data[3] = std::pow(2, rest);
					}
					for (const auto &byte : can_->messageTx.data)
					{
						std::cout << static_cast<int>(byte) << std::endl;
					}
				}
				else if (can_->messageRx.data[2] == 188)
				{
					this->protocol = static_cast<int>(can_->messageRx.data[3]) + static_cast<int>(can_->messageRx.data[4]);
					BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "PROTOCOL SETUP MESSAGE DETECTED" << static_cast<int>(can_->messageRx.data[3]) << static_cast<int>(can_->messageRx.data[4]) << std::endl;
					can_->messageTx = can_->messageRx;
					can_->messageTx.id = can_->messageRx.data[1];
					can_->messageTx.data[2] = can_->messageRx.data[2];
					can_->messageTx.data[1] = id_;
					can_->messageRx.data[0] = this->protocol;
				}
				else if (can_->messageRx.data[2] == 204)
				{
					for (auto &conn : connectors)
					{
						if (conn->id == can_->messageRx.data[3])
						{
							BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "MODULE PORT " << static_cast<int>(can_->messageRx.data[3]) << " CHANGING VALUE TO " << static_cast<int>(can_->messageRx.data[4]);
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
							BOOST_LOG(lg_) << "INF " << "Module::loop: MOD[" << this->domain << "] " << "MODULE PORT " << can_->messageRx.data[3] << " CHANGING VALUE TO " << can_->messageRx.data[4];
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
			else if (protocol == 2)
			{
				boost::thread t(std::bind(&Module::protocol2, this));
				t.detach();
				return false;
			}
			else if (protocol == 6)
			{
				boost::thread t(std::bind(&Module::protocol6, this));
				t.detach();
				return false;
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

void Module::protocol6()
{
	BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " Protocol6 detected";
	int mask1 = can_->messageRx.data[3];
	int mask2 = can_->messageRx.data[4];
	if (mask1 != 0)
	{
		std::bitset<8> bMask1(mask1);

		for (int i = 0; i < 8; i++)
		{
			BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << bMask1[i];
			connectors[i]->value = bMask1[i];
		}
	}
	std::bitset<8> bMask2(mask2);
	if (connectors.size() >= 8)
	{
		for (int i = 8; i < 16; i++)
		{
			if (i >= connectors.size())
			{
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
				sendMessage();
				return;
			}
			connectors[i]->value = bMask2[i - 8];
			BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << bMask2[i - 8];

		}
	}
}

void Module::protocol2()
{
	std::string additional = std::to_string(can_->messageRx.data[5]);
	int counter = std::stoi(additional.substr(0, 1));
	BOOST_LOG(lg_) << "INF " << "Module::protocol2: MOD[" << this->domain << "] " << "COUNTER: " << counter;
	int interval = 0;
	if (additional.size() == 2)
		interval = std::stoi(additional.substr(1, 1)) * 100;
	else
		interval = std::stoi(additional.substr(1, 2)) * 100;
	BOOST_LOG(lg_) << "INF " << "Module::protocol2: MOD[" << this->domain << "] " << "INTERVAL: " << interval;
	Connector* GlobalConn = new Connector();
	for (int i = 0; i < counter; i++)
	{
		if (can_->messageRx.data[2] == 204)
		{
			for (auto &conn : connectors)
			{
				if (conn->id == can_->messageRx.data[3])
				{
					BOOST_LOG(lg_) << "INF " << "Module::protocol2: MOD[" << this->domain << "] " << "MODULE PORT " << can_->messageRx.data[3] << " CHANGING VALUE TO " << can_->messageRx.data[4];
					conn->value = can_->messageRx.data[4];
					GlobalConn = conn;
					break;
				}
			}
			can_->messageTx = can_->messageRx;
			can_->messageTx.id = can_->messageRx.data[1];
			can_->messageTx.data[2] = 205;	//CD
			can_->messageTx.data[1] = id_;
			can_->messageTx.data[0] = protocol;
			can_->messageTx.data[3] = can_->messageRx.data[3];
			can_->messageTx.data[4] = GlobalConn->value;
			can_->messageTx.data[5] = can_->messageRx.data[5];
			can_->messageTx.data[6] = 0;
			can_->messageTx.data[7] = 0;
			sendMessage();

			boost::this_thread::sleep(boost::posix_time::milliseconds(interval));

			can_->messageTx = can_->messageRx;
			can_->messageTx.id = can_->messageRx.data[1];
			can_->messageTx.data[2] = 205;	//CD
			can_->messageTx.data[1] = id_;
			can_->messageTx.data[0] = protocol;
			can_->messageTx.data[3] = can_->messageRx.data[3];
			can_->messageTx.data[4] = !GlobalConn->value;
			can_->messageTx.data[5] = can_->messageRx.data[5];
			can_->messageTx.data[6] = 0;
			can_->messageTx.data[7] = 0;
			sendMessage();
			boost::this_thread::sleep(boost::posix_time::milliseconds(interval));
		}
	}
}

bool Module::messageAvailable()
{
	std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::in);
	can_recv >> buffer_;
	can_recv.close();
	BOOST_LOG(lg_) << "INF " << "Module::messageAvailable: MOD[" << this->domain << "] " << "Module::messageAvailable: " << buffer_ << " | " << buffer_.size();
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
