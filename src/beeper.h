#ifndef BEEPER_H
#define BEEPER_H

#include <string>

#include "connector.h"

class Beeper: public Connector
{
public:
    Beeper();
    Beeper(std::string connector, std::string device);
    virtual ~Beeper();

    virtual bool Test();

};

#endif // BEEPER_H
