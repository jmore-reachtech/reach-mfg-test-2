#ifndef FLASH_H_
#define FLASH_H_

#include "connector.h"

class Flash: public Connector
{

public:
    Flash();
    Flash(std::string connector, std::string device);
    virtual ~Flash();

    virtual bool Test();

private:

};

#endif // FLASH_H_