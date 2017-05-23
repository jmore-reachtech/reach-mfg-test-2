#ifndef GPIO_H
#define GPIO_H

#include <string>

#include "connector.h"

class Gpio: public Connector {

public:
    Gpio();
    Gpio(std::string connector, std::string device);
    virtual ~Gpio();

    virtual bool Test();

private:
};

#endif 
