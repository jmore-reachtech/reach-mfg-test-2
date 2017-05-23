#include <iostream>
#include <string.h>

#include "gpio.h"

Gpio::Gpio(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating GPIO port" << std::endl; 
}

Gpio::Gpio(std::string connector, std::string device): Connector(connector, device)
{
    std::cout << "Creating GPIO conector " << connector 
        << " using device " << device << std::endl; 
}

Gpio::~Gpio()
{
    std::cout << "Destroying GPIO port" << std::endl; 
}

bool Gpio::Test()
{
    std::cout << "Running GPIO Test" << std::endl;

    return true;
}
