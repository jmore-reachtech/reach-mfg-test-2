#ifndef USB_H
#define USB_H

#include "connector.h"

class Usb: public Connector
{

public:
    Usb();
    Usb(std::string connector, std::string device);
    virtual ~Usb();

    virtual bool Test();

private:

};

#endif
