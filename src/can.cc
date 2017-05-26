#include <iostream>
#include <string.h>

#include "can.h"

Can::Can(): Connector("Unknown", "/dev/null")
{
    if (verbose_)
        std::cout << "Creating CAN port" << std::endl; 
}

Can::Can(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating CAN conector " << connector << " using device " << device << std::endl; 
}

Can::~Can()
{
    if (verbose_)
        std::cout << "Destroying CAN port" << std::endl; 
}

bool Can::Test()
{
    if (verbose_)
        std::cout << "Running CAN Test" << std::endl;

    return false;
}
