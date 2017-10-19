#include <iostream>
#include <string.h>

#include "flash.h"
#include "cmdrunner.h"
#include "logger.h"

Flash::Flash(): Connector("Unknown", "/dev/null")
{
}

Flash::Flash(std::string connector, std::string device): Connector(connector, device)
{
}


Flash::~Flash()
{
}

bool Flash::Test()
{
    std::string cmd;
    auto rv = false;

    Logger::GetLogger()->Log("Running FLASH Test ");

    result_.output.clear();
    
    Logger::GetLogger()->Log("Testing /dev/mtd0");
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd0";
    if (CmdRunner::Run(cmd,result_.output))
        return true;

    Logger::GetLogger()->Log("Testing /dev/mtd1");
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd1";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    Logger::GetLogger()->Log("Testing /dev/mtd2");
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd2";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    Logger::GetLogger()->Log("Testing /dev/mtd3");
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd3";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    Logger::GetLogger()->Log("Testing /dev/mtd4");
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd4";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    Logger::GetLogger()->Log("Testing /dev/mtd5");
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd5";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    Logger::GetLogger()->Log("Testing /dev/mtd6");
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd6";
    if (CmdRunner::Run(cmd,result_.output))
        return true;

    result_.rv = rv;
    
    return result_.rv;
}
