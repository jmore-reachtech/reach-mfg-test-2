#include <iostream>
#include <string.h>

#include "usb.h"

Usb::Usb(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating USB port" << std::endl; 
}

Usb::Usb(std::string connector, std::string device): Connector(connector, device)
{
    std::cout << "Creating USB conector " << connector 
        << " using device " << device << std::endl; 
}

Usb::~Usb()
{
    std::cout << "Destroying USB port" << std::endl; 
}

bool Usb::Test()
{
    std::cout << "Running USB Test" << std::endl;

    return true;
}
