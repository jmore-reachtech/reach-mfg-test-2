#ifndef ETHERNET_H
#define ETHERNET_H

#include <string>

#include "connector.h"

class Ethernet: public Connector {

public:
    Ethernet();
    Ethernet(std::string connector, std::string device);
    virtual ~Ethernet();

    virtual bool Test();

private:
};

#endif 
