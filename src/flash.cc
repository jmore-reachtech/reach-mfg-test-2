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
    
    // dtb
    Logger::GetLogger()->Log("Testing /dev/mtd0");
    //cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd0";
    cmd = "/usr/sbin/flash_erase /dev/mtd0 0 0";
    if (CmdRunner::Run(cmd,result_.output))
        return true;

    // kernel
    Logger::GetLogger()->Log("Testing /dev/mtd1");
    //cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd1";
    cmd = "/usr/sbin/flash_erase /dev/mtd1 0 0";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    // rootfs
    Logger::GetLogger()->Log("Testing /dev/mtd2");
    //cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd2";
    cmd = "/usr/sbin/flash_erase /dev/mtd2 0 0";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    // boot
    Logger::GetLogger()->Log("Testing /dev/mtd3");
    //cmd = "/usr/sbin/nandtest -o 0x0 -l 0x10000 /dev/mtd3";
    cmd = "/usr/sbin/flash_erase /dev/mtd3 0 2";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    // boot env
    Logger::GetLogger()->Log("Testing /dev/mtd4");
    //cmd = "/usr/sbin/nandtest -o 0x0 -l 0x10000 /dev/mtd4";
    cmd = "/usr/sbin/flash_erase /dev/mtd4 0 2";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    // general env
    Logger::GetLogger()->Log("Testing /dev/mtd5");
    //cmd = "/usr/sbin/nandtest -o 0x0 -l 0x10000 /dev/mtd5";
    cmd = "/usr/sbin/flash_erase /dev/mtd5 0 2";
    if (CmdRunner::Run(cmd,result_.output))
        return true;
    
    // custom
    Logger::GetLogger()->Log("Testing /dev/mtd6");
    //cmd = "/usr/sbin/nandtest -o 0x0 -l 0x10000 /dev/mtd6";
    cmd = "/usr/sbin/flash_erase /dev/mtd6 0 2";
    if (CmdRunner::Run(cmd,result_.output))
        return true;

    result_.rv = rv;
    
    return result_.rv;
}
