#include <iostream>
#include <string.h>

#include "can.h"

Can::Can(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating CAN port" << std::endl; 
}

Can::Can(std::string connector, std::string device): Connector(connector, device)
{
    std::cout << "Creating CAN conector " << connector 
        << " using device" << device << std::endl; 
}

Can::~Can()
{
    std::cout << "Destroying CAN port" << std::endl; 
}

bool Can::Test()
{
    std::cout << "Running CAN Test" << std::endl;

    return true;
}
