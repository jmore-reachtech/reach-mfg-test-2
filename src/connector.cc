#include <iostream>
#include <string>

#include "connector.h"

Connector::Connector(std::string name, std::string device): device_(device),  
    name_(name), result_(), verbose_(false)
{
    if (verbose_)
        std::cout << "Creating Connector: " << name_ << std::endl; 
}

Connector::~Connector()
{
    if (verbose_)
        std::cout << "Destroying Connector: " << name_ << std::endl; 
}

std::string Connector::GetDevice() const
{
    return device_;
}

std::string Connector::GetName() const
{
    return name_;
}

Connector::connector_result Connector::get_connector_result() const {
    return result_;
}

void Connector::set_verbose(bool verbose)
{
    verbose_ = verbose;
}
