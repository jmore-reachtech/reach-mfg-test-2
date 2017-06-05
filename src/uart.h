#ifndef UART_H
#define UART_H

#include <string>

#include "connector.h"

class Uart: public Connector {

public:
    Uart();
    Uart(std::string connector, std::string device);
    virtual ~Uart();

    virtual bool Test();

private:
    int fd_{0};
};

#endif 
