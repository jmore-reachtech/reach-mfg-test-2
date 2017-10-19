#include <iostream>
#include <string.h>

#include "rtc.h"
#include "cmdrunner.h"
#include "logger.h"

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
     
    Logger::GetLogger()->Log("Running RTC Test");

    result_.output.clear();        
    cmd.append("rdate -s ");
    cmd.append(server_addr_);
    
    result_.rv = CmdRunner::Run(cmd,result_.output);
    if (result_.rv) {
        return result_.rv;
    }
    
    cmd.clear();
    cmd.append("hwclock -w");
    result_.rv = CmdRunner::Run(cmd,result_.output);

    return result_.rv;
}

