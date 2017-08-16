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

std::vector<int> Module::prepareIntialStates()
{
	int mask1 = 0;
	int mask2 = 0;
	int mask3 = 0;
	int mask4 = 0;
	BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " conn.size is " << connectors.size();
	if (connectors.size() <= 8)
	{
		for (int i = 0; i < connectors.size(); i++)
		{
			if (connectors[i]->type == 0)
			{
				mask1 += std::pow(2, i);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask1 " << mask1;
	}
	else if (connectors.size() > 8 && connectors.size() < 16)
	{
		for (int i = 0; i < 8; i++)
		{
			if (connectors[i]->type == 0)
			{
				mask1 += std::pow(2, i);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask1 " << mask1;
		for (int i = 8; i < connectors.size(); i++)
		{
			if (connectors[i]->type == 0)
			{
				mask2 += std::pow(2, i - 8);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask2 " << mask2;
	}
	else if (connectors.size() > 16 && connectors.size() < 24)
	{
		for (int i = 0; i < 8; i++)
		{
			if (connectors[i]->type == 0)
			{
				mask1 += std::pow(2, i);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask1 " << mask1;
		for (int i = 8; i < 16; i++)
		{
			if (connectors[i]->type == 0)
			{
				mask2 += std::pow(2, i - 8);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask2 " << mask2;
		for (int i = 16; i < connectors.size(); i++)
		{
			if (connectors[i]->type == 0)
			{
				mask3 += std::pow(2, i - 16);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask3 " << mask3;
	}
	else if (connectors.size() > 24)
	{
		for (int i = 0; i < 8; i++)
		{
			if (connectors[i]->type == 0)
			{
				mask1 += std::pow(2, i);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask1 " << mask1;
		for (int i = 8; i < 16; i++)
		{
			if (connectors[i]->type == 0)
			{
				mask2 += std::pow(2, i - 8);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask2 " << mask2;
		for (int i = 16; i < 24; i++)
		{
			if (connectors[i]->type == 0)
			{
				mask3 += std::pow(2, i - 16);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask3 " << mask3;
		for (int i = 24; i < connectors.size(); i++)
		{
			if (connectors[i]->type == 0)
			{
				mask4 += std::pow(2, i - 24);
			}
		}
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask4 " << mask4;
	}
	return {mask1, mask2, mask3, mask4};
	
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
					//auto masks = prepareIntialStates();
					can_->messageTx.id = can_->messageRx.data[1];
					can_->messageTx.data[0] = 0;
					can_->messageTx.data[1] = id_;
					can_->messageTx.data[2] = 187;
					can_->messageTx.data[3] = 255;//masks[0];
					can_->messageTx.data[4] = 255;//masks[1];
					can_->messageTx.data[5] = 255;//masks[2];
					can_->messageTx.data[6] = 255;//masks[3];
					can_->messageTx.data[7] = 0;

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
			else if (protocol == 7)
			{
				boost::thread t(std::bind(&Module::protocol7, this));
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

std::bitset<8> Module::reverse(std::bitset<8> &set)
{
	std::bitset<8> result;

	for (size_t i = 0; i < 8; i++)
		result[i] = set[8 - i - 1];

	return result;
}

void Module::protocol7()
{
	BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " Protocol7 detected";
	int mask1 = can_->messageRx.data[3];
	int mask2 = can_->messageRx.data[4];
	int mask3 = can_->messageRx.data[5];
	int mask4 = can_->messageRx.data[6];
	BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " mask1: " << mask1 << " mask2: " << mask2;
	
	std::vector<int> connIds;
	std::bitset<8> bMask1(mask1);
	bMask1 = reverse(bMask1);
	std::bitset<8> bMask2(mask2);
	bMask2 = reverse(bMask2);
	std::bitset<8> bMask3(mask3);
	bMask3 = reverse(bMask3);
	std::bitset<8> bMask4(mask4);
	bMask4 = reverse(bMask4);
	auto data7s = std::to_string(static_cast<int>(can_->messageRx.data[7]));
	int counter = 0;
	int interval = 0;
	if (data7s.size() == 2)
	{
		interval = std::stoi(std::to_string(static_cast<int>(can_->messageRx.data[7])).substr(0, 1)) * 100;
		counter = std::stoi(std::to_string(static_cast<int>(can_->messageRx.data[7])).substr(1, 1));
	}
	else if (data7s.size() == 3)
	{
		if (data7s[0] == '0')
		{
			interval= std::stoi(std::to_string(static_cast<int>(can_->messageRx.data[7])).substr(0, 2)) * 100;
			counter = std::stoi(std::to_string(static_cast<int>(can_->messageRx.data[7])).substr(2, 1));
		}
		else
		{
			interval = std::stoi(std::to_string(static_cast<int>(can_->messageRx.data[7])).substr(0, 1)) * 100;
			counter = std::stoi(std::to_string(static_cast<int>(can_->messageRx.data[7])).substr(1, 2));
		}

	}
	BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " counter: " << counter << " interval: " << interval;
	for (const auto conn : connectors)
	{
		if (conn->id < 8)
		{
			if (bMask1[conn->id] != conn->value)
				connIds.push_back(conn->id);
		}
		else if (conn->id > 8 && conn->id < 16)
		{
			if (bMask2[conn->id - 8] != conn->value)
				connIds.push_back(conn->id);
		}
		else if (conn->id > 16 && conn->id < 24)
		{
			if (bMask3[conn->id - 16] != conn->value)
				connIds.push_back(conn->id);
		}
		else if (conn->id >24 && conn->id < 32)
		{
			if (bMask4[conn->id - 24] != conn->value)
				connIds.push_back(conn->id);
		}
	}
	BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connIds.size() " << connIds.size();
	if (counter == 0)
	{
		mtx.lock();
		if (protocol7Flag_ == true)
			protocol7Flag_ = false;
		mtx.unlock();
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " counter 0";
		if (mask1 != 0)
		{
					for (int i = 0; i < 8; i++)
			{
				if (bMask1[i] == 1 && i < connectors.size())
				{
					connectors[i]->value = ((connectors[i]->value == 1) ? 0 : 1);
					BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
				}

			}
		}
		if (mask2 != 0)
		{
			for (int i = 8; i < 16; i++)
			{
				if (bMask2[i - 8] == 1 && i < connectors.size())
				{
					connectors[i]->value = ((connectors[i]->value == 1) ? 0 : 1);
					BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
				}

			}
		}
		if (mask3 != 0)
		{
			for (int i = 16; i < 24; i++)
			{
				if (bMask3[i - 16] == 1 && i < connectors.size())
				{
					connectors[i]->value = ((connectors[i]->value == 1) ? 0 : 1);
					BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
				}

			}
		}
		if (mask4 != 0)
		{
			for (int i = 24; i < connectors.size(); i++)
			{
				if (bMask4[i - 24] == 1 && i < connectors.size())
				{
					connectors[i]->value = ((connectors[i]->value == 1) ? 0 : 1);
					BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
				}

			}
		}
		can_->messageTx = can_->messageRx;
		can_->messageTx.id = can_->messageRx.data[1];
		can_->messageTx.data[2] = 205;	//CD
		can_->messageTx.data[1] = id_;
		can_->messageTx.data[0] = 6;
		can_->messageTx.data[3] = bMask1.to_ulong();
		can_->messageTx.data[4] = bMask2.to_ulong();
		can_->messageTx.data[5] = bMask3.to_ulong();
		can_->messageTx.data[6] = bMask4.to_ulong();
		can_->messageTx.data[7] = 0;
		sendMessage();
	}
	else if (counter >= 9)
	{
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " counter 9";
		mtx.lock();
		if (protocol7Flag_ == true)
			protocol7Flag_ = false;
		else
			protocol7Flag_ = true;
		mtx.unlock();
		while (protocol7Flag_)
		{
			if (mask1 != 0)
			{
				for (int i = 0; i < 8; i++)
				{
					if (bMask1[i] == 1)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}
					
				}
			}
			if (mask2 != 0)
			{
				for (int i = 8; i < 16; i++)
				{
					if (bMask2[i - 8] == 1)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}
					
				}
			}
			if (mask3 != 0)
			{
				for (int i = 16; i < 24; i++)
				{
					if (bMask3[i - 16] == 1)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask4 != 0)
			{
				for (int i = 24; i < connectors.size(); i++)
				{
					if (bMask2[i - 24] == 1)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			can_->messageTx = can_->messageRx;
			can_->messageTx.id = can_->messageRx.data[1];
			can_->messageTx.data[2] = 205;	//CD
			can_->messageTx.data[1] = id_;
			can_->messageTx.data[0] = 6;
			can_->messageTx.data[3] = bMask1.to_ulong();
			can_->messageTx.data[4] = bMask2.to_ulong();
			can_->messageTx.data[5] = bMask3.to_ulong();
			can_->messageTx.data[6] = bMask4.to_ulong();
			can_->messageTx.data[7] = 0;
			sendMessage();
			boost::this_thread::sleep(boost::posix_time::milliseconds(interval));
			if (mask1 != 0)
			{
				for (int i = 0; i < 8; i++)
				{
					if (bMask1[i] == 1)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask2 != 0)
			{
				for (int i = 8; i < 16; i++)
				{
					if (bMask2[i - 8] == 1)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask3 != 0)
			{
				for (int i = 16; i < 24; i++)
				{
					if (bMask3[i - 16] == 1)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask4 != 0)
			{
				for (int i = 24; i < connectors.size(); i++)
				{
					if (bMask2[i - 24] == 1)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			can_->messageTx = can_->messageRx;
			can_->messageTx.id = can_->messageRx.data[1];
			can_->messageTx.data[2] = 205;	//CD
			can_->messageTx.data[1] = id_;
			can_->messageTx.data[0] = 6;
			can_->messageTx.data[3] = bMask1.to_ulong();
			can_->messageTx.data[4] = bMask2.to_ulong();
			can_->messageTx.data[5] = bMask3.to_ulong();
			can_->messageTx.data[6] = bMask4.to_ulong();
			can_->messageTx.data[7] = 0;
			sendMessage();
		}
	}
	else
	{
		BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " counter 0<x<9 " << counter;
		for (int i = 0; i < counter; i++)
		{
			if (mask1 != 0)
			{
				for (int i = 0; i < 8; i++)
				{
					if (bMask1[i] == 1 && connectors.size() > i)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask2 != 0)
			{
				for (int i = 8; i < 16; i++)
				{
					if (bMask2[i - 8] == 1 && connectors.size() > i)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask3 != 0)
			{
				for (int i = 16; i < 24; i++)
				{
					if (bMask3[i - 16] == 1 && connectors.size() > i)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask4 != 0 && connectors.size() > i)
			{
				for (int i = 24; i < connectors.size(); i++)
				{
					if (bMask2[i - 24] == 1)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			can_->messageTx = can_->messageRx;
			can_->messageTx.id = can_->messageRx.data[1];
			can_->messageTx.data[2] = 205;	//CD
			can_->messageTx.data[1] = id_;
			can_->messageTx.data[0] = 6;
			can_->messageTx.data[3] = bMask1.to_ulong();
			can_->messageTx.data[4] = bMask2.to_ulong();
			can_->messageTx.data[5] = bMask3.to_ulong();
			can_->messageTx.data[6] = bMask4.to_ulong();
			can_->messageTx.data[7] = 0;
			sendMessage();
			boost::this_thread::sleep(boost::posix_time::milliseconds(interval));
			if (mask1 != 0)
			{
				for (int i = 0; i < 8; i++)
				{
					if (bMask1[i] == 1 && connectors.size() > i)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask2 != 0)
			{
				for (int i = 8; i < 16; i++)
				{
					if (bMask2[i - 8] == 1 && connectors.size() > i)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask3 != 0)
			{
				for (int i = 16; i < 24; i++)
				{
					if (bMask3[i - 16] == 1 && connectors.size() > i)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			if (mask4 != 0)
			{
				for (int i = 24; i < connectors.size(); i++)
				{
					if (bMask2[i - 24] == 1 && connectors.size() > i)
					{
						connectors[i]->value = !connectors[i]->value;
						BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << connectors[i]->value;
					}

				}
			}
			can_->messageTx = can_->messageRx;
			can_->messageTx.id = can_->messageRx.data[1];
			can_->messageTx.data[2] = 205;	//CD
			can_->messageTx.data[1] = id_;
			can_->messageTx.data[0] = 6;
			can_->messageTx.data[3] = bMask1.to_ulong();
			can_->messageTx.data[4] = bMask2.to_ulong();
			can_->messageTx.data[5] = bMask3.to_ulong();
			can_->messageTx.data[6] = bMask4.to_ulong();
			can_->messageTx.data[7] = 0;
			sendMessage();
		}
	}
}

void Module::protocol6()
{
	BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " Protocol6 detected";
	int mask1 = can_->messageRx.data[3];
	int mask2 = can_->messageRx.data[4];
	int mask3 = can_->messageRx.data[5];
	int mask4 = can_->messageRx.data[6];
	if (mask1 != 0)
	{
		std::bitset<8> bMask1(mask1);

		for (int i = 0; i < 8; i++)
		{
			BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << bMask1[i];
			connectors[i]->value = bMask1[i];
		}
	}
	if (mask2 != 0)
	{
		std::bitset<8> bMask2(mask2);

		for (int i = 8; i < 16; i++)
		{
			BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << bMask2[i - 8];
			connectors[i]->value = bMask2[i - 8];
		}
	}
	if (mask3 != 0)
	{
		std::bitset<8> bMask3(mask3);

		for (int i = 16; i < 24; i++)
		{
			BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << bMask3[i - 16];
			connectors[i]->value = bMask3[i - 16];
		}
	}
	if (mask4 != 0)
	{
		std::bitset<8> bMask4(mask4);

		for (int i = 24; i < 32; i++)
		{
			BOOST_LOG(lg_) << "INF " << __FUNCTION__ << " connector id: " << i << " changing value to " << bMask4[i - 24];
			connectors[i]->value = bMask4[i - 24];
		}
	}
	can_->messageTx = can_->messageRx;
	can_->messageTx.id = can_->messageRx.data[1];
	can_->messageTx.data[2] = 205;	//CD
	can_->messageTx.data[1] = id_;
	can_->messageTx.data[0] = protocol;
	can_->messageTx.data[3] = can_->messageRx.data[3];
	can_->messageTx.data[4] = can_->messageRx.data[4];
	can_->messageTx.data[5] = can_->messageRx.data[5];
	can_->messageTx.data[6] = can_->messageRx.data[6];
	can_->messageTx.data[7] = 0;
	sendMessage();
	return;
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
	if (counter < 9)
	{
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
	else
	{
		BOOST_LOG(lg_) << "COUNTER IS BIG";
		if (interuption_ == nullptr)
			interuption_ = new Interuption(can_->messageRx.data[3]);
		else if (interuption_ != nullptr && can_->messageRx.data[3] == interuption_->connId)
		{
			interuption_->value = true;
		}
		while (!interuption_->value)
		{
			BOOST_LOG(lg_) << "SENDING";
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
		BOOST_LOG(lg_) << "DELETING INTERUPTION";
		interuption_ = nullptr;
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
