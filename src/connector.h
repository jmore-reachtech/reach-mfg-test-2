#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <string>

class Connector
{

public:
    Connector(std::string name);
    virtual ~Connector();

    virtual bool Test() = 0;

private:
    std::string _name;
};

#endif
