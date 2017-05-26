#ifndef USB_H_
#define USB_H_

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

#endif // USB_H_
