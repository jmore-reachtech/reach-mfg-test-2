#include <iostream>
#include <string>
#include <sstream>

#include "connector.h"

Connector::Connector(std::string name, std::string device): device_(device),  
    name_(name), result_()
{

}

Connector::~Connector()
{

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

std::string Connector::ToString() const
{
    std::stringstream ss;
    ss << "Connector: " << this->name_;
    if (this->result_.rv) {
        ss << "\tFailed";
    } else {
        ss << "\tPassed";
    }

    return ss.str();
}

std::ostream &operator<<(std::ostream &out, const Connector &c)
{
    out << "Connector: " << c.name_ << " ";
    if (c.result_.rv) {
        out << "\033[0;31m";
        if (c.name_.length() <= 3) {
            out << "\t\t\t";
        } else if (c.name_.length() > 11) {
            out << "\t";
        } else {
            out << "\t\t";
        }
        out << "Failed";
    } else {
        out << "\033[0;32m";
        if (c.name_.length() <= 3) {
            out << "\t\t\t";
        } else if (c.name_.length() > 11) {
            out << "\t";
        } else {
            out << "\t\t";
        }
        out << "Passed";
    }
    out << "\033[0m";
    
    return out;
}
