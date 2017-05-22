#ifndef UART_H
#define UART_H

#include <string>

#include "connector.h"

class Uart: public Connector {

public:
    Uart();
    Uart(std::string name);
    virtual ~Uart();

    virtual bool Test();

private:

};

#endif 
