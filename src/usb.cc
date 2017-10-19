#include <iostream>
#include <string.h>

#include "usb.h"
#include "cmdrunner.h"
#include "logger.h"

Usb::Usb(): Connector("Unknown", "/dev/null")
{
}

Usb::Usb(std::string connector, std::string device): Connector(connector, device)
{
}

Usb::~Usb()
{
}

bool Usb::Test()
{
    std::string cmd;

    Logger::GetLogger()->Log("Running USB Test");

    cmd.append("/bin/dd of=/dev/null if=");
    cmd.append(GetDevice());
    cmd.append(" bs=1k count=64");

    result_.output.clear();
    result_.rv = CmdRunner::Run(cmd,result_.output);

    return result_.rv;
}
