#ifndef GPIO_H
#define GPIO_H

#include <string>

#include "connector.h"

#define GPIO_INPUT_REG      0x00
#define GPIO_OUT_REG        0x01
#define GPIO_POLARITY_REG   0x02
#define GPIO_CTRL_REG       0x03
#define GPIO_MASK           0x80

class Gpio: public Connector {

public:
    Gpio();
    Gpio(std::string connector, std::string device);
    virtual ~Gpio();

    virtual bool Test();

private:
    int fd_{0};
};

#endif 
