#ifndef SPEAKER_H
#define SPEAKER_H

#include <string>

#include "connector.h"

class Speaker: public Connector
{
public:
    Speaker();
    Speaker(std::string connector, std::string device);
    virtual ~Speaker();

    virtual bool Test();

};

#endif // SPEAKER_H
