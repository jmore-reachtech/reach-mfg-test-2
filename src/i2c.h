#ifndef I2C_H
#define I2C_H

#define I2C_SLAVE_ADDR  0x05
#define I2C_CTRL_ADDR   0x0C
#define I2C_CTRL_DATA   0x03

#include "connector.h"

class I2c: public Connector
{

public:
    I2c();
    I2c(std::string connector, std::string device);
    virtual ~I2c();

    virtual bool Test();

private:
    int fd_{0};
};

#endif
