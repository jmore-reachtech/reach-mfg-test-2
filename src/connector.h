#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <string>

class Connector
{

public:
    Connector(std::string name, std::string device);
    virtual ~Connector();

    virtual bool Test() = 0;
    std::string GetDevice() const;
    std::string GetName() const;

private:
    std::string m_device;
    std::string m_name;
};

#endif
