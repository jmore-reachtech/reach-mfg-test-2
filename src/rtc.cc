#include <iostream>
#include <string.h>

#include "rtc.h"
#include "cmdrunner.h"

Rtc::Rtc(): Connector("Unknown", "/dev/null")
{
}

Rtc::Rtc(std::string connector, std::string device, std::string server): 
    Connector(connector, device)
{
    if (server.empty())
        server_addr_ = DEFAULT_RTC_ADDR;
    else
        server_addr_ = server;
}

Rtc::~Rtc()
{
}

bool Rtc::Test()
{
    std::string cmd;
     
    if (verbose_)
        std::cout << "Running RTC Test" << std::endl;

    result_.output.clear();        
    cmd.append("rdate -p -s ");
    cmd.append(server_addr_);
    
    if (verbose_)
        std::cout << "Running RTC Test - server: " << server_addr_ << std::endl;

    result_.rv = CmdRunner::Run(cmd,result_.output);
    if (result_.rv) {
        return result_.rv;
    }
    
    cmd.clear();
    cmd.append("hwclock -w");
    result_.rv = CmdRunner::Run(cmd,result_.output);

    return result_.rv;
}

