#ifndef GPIO2_H
#define GPIO2_H

#include "connector.h"

class Gpio2: public Connector {

public:
    Gpio2();
    Gpio2(std::string connector, std::string device);
    virtual ~Gpio2();

    virtual bool Test();

private:
    int fd_{0};
};

#endif // GPIO2_H
