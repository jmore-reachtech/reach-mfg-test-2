#ifndef TOUCH_H_
#define TOUCH_H_

#include "connector.h"

class Touch: public Connector
{

public:
    Touch();
    Touch(std::string connector, std::string device);
    virtual ~Touch();

    virtual bool Test();

private:

};

#endif // TOUCH_H_