#ifndef ETHERNET_H
#define ETHERNET_H

#include <string>

#include "connector.h"

#define DEFAULT_SERVER_ADDR "10.10.10.2"

class Ethernet: public Connector {

public:
    Ethernet();
    Ethernet(std::string connector, std::string device, std::string server);
    virtual ~Ethernet();

    virtual bool Test();

private:
    std::string server_addr_;
};

#endif