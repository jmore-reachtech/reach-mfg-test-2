#ifndef CAN_H
#define CAN_H

#include <string>

#include "connector.h"

class Can: public Connector {

public:
    Can();
    Can(std::string connector, std::string device);
    virtual ~Can();

    virtual bool Test();

private:
};

#endif 
