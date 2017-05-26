#include <iostream>
#include <string.h>

#include "ethernet.h"

Ethernet::Ethernet(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating ETHERNET port" << std::endl; 
}

Ethernet::Ethernet(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating ETHERNET conector " << connector << " using device " << device << std::endl; 
}

Ethernet::~Ethernet()
{
    if (verbose_)
        std::cout << "Destroying ETHERNET port" << std::endl; 
}

bool Ethernet::Test()
{
    if (verbose_)
        std::cout << "Running ETHERNET Test" << std::endl;

    return false;
}
