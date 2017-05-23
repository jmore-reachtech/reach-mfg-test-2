#include <iostream>
#include <string>

#include "connector.h"

Connector::Connector(std::string name, std::string device): m_device(device),  m_name(name)
{
    std::cout << "Creating Connector: " << name << std::endl; 
}

Connector::~Connector()
{
    std::cout << "Destroying Connector: " << this->m_name << std::endl; 
}

std::string Connector::GetDevice() const
{
    return m_device;
}

std::string Connector::GetName() const
{
    return m_name;
}
