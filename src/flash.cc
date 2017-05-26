#include <iostream>
#include <string.h>

#include "flash.h"
#include "cmdrunner.h"

Flash::Flash(): Connector("Unknown", "/dev/null")
{
    if (verbose_)
        std::cout << "Creating Flash port" << std::endl; 
}

Flash::Flash(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating Flash conector " << connector << " using device " << device << std::endl; 
}


Flash::~Flash()
{
    if (verbose_)
        std::cout << "Destroying Flash port" << std::endl; 
}

bool Flash::Test()
{
    std::string cmd;
    auto rv = true;

    if (verbose_) {
        std::cout << "Running Flash Test" << std::endl;
    }

    result_.output.clear();
    
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd0";
    if (!CmdRunner::Run(cmd,result_.output))
        return true;
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd1";
    if (!CmdRunner::Run(cmd,result_.output))
        return true;
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd2";
    if (!CmdRunner::Run(cmd,result_.output))
        return true;
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd3";
    if (!CmdRunner::Run(cmd,result_.output))
        return true;
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd4";
    if (!CmdRunner::Run(cmd,result_.output))
        return true;
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd5";
    if (!CmdRunner::Run(cmd,result_.output))
        return true;
    cmd = "/usr/sbin/nandtest -m -o 0x0 /dev/mtd6";
    if (!CmdRunner::Run(cmd,result_.output))
        return true;

    result_.rv = rv;
    
    return result_.rv;
}
