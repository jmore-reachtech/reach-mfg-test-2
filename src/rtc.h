#ifndef RTC_H_
#define RTC_H_

#include <string>

#include "connector.h"

#define DEFAULT_RTC_ADDR "10.10.10.2"

class Rtc : public Connector
{
public:
    Rtc();
    Rtc(std::string connector, std::string device, std::string server);
    virtual ~Rtc();

    virtual bool Test();

private:
    std::string server_addr_;

};

#endif // RTC_H_
