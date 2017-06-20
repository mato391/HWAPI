#include "stdafx.h"
#include "Module.hpp"


Module::Module(int id) : id_(id), can_(new CAN(id))
{
	std::cout << "MODULE " << id << " was created" << std::endl; 
}


Module::~Module()
{
}
