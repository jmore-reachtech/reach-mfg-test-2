#ifndef CAN_H
#define CAN_H

#include <string>

#include "connector.h"

#define CAN_IF "can0"
#define CAN_BAUD_RATE 500000
#define CAN_MSG_EXPECTED 0xBEEF

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
