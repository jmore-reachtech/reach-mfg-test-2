#ifndef UART_H
#define UART_H

#include <string>

#include "connector.h"

#define TIOCGRS485 0x542E
#define TIOCSRS485 0x542F

class Uart : public Connector
{

public:
    Uart();
    Uart(std::string connector, std::string device);
    virtual ~Uart();

    virtual bool Test();
    void EnableRs485();

private:
    int fd_{ 0 };
    bool rs485_;
};

#endif
