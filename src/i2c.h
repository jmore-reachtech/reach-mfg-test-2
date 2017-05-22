#ifndef I2C_H
#define I2C_H

#include "connector.h"

class I2c: public Connector
{

public:
    I2c();
    I2c(std::string name);
    virtual ~I2c();

    virtual bool Test();

private:

};

#endif
