#include <iostream>

#include "connector.h"

Connector::Connector(std::string name):_name(name)
{
    std::cout << "Creating Connector: " << name << std::endl; 
}

Connector::~Connector()
{
    std::cout << "Destroying Connector: " << this->_name << std::endl; 
}
