#ifndef CANLOCAL_H
#define CANLOCAL_H

#include <string>

#include "connector.h"

#define CANLOCAL_IF "can0"
#define CANLOCAL_BAUD_RATE 500000
#define CANLOCAL_MSG_EXPECTED 0xBEEF

class Can: public Connector {

public:
    Can();
    Can(std::string connector, std::string device);
    virtual ~Can();

    virtual bool Test();

private:
    int fd_{0};
};

#endif 
