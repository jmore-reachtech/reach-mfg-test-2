#include <iostream>
#include <string.h>

#include "usb.h"
#include "cmdrunner.h"

Usb::Usb(): Connector("Unknown", "/dev/null")
{
    if (verbose_)
        std::cout << "Creating USB port" << std::endl; 
}

Usb::Usb(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating USB conector " << connector << " using device " << device << std::endl; 
}

Usb::~Usb()
{
    if (verbose_)
        std::cout << "Destroying USB port" << std::endl; 
}

bool Usb::Test()
{
    std::string cmd;

    if (verbose_) {
        std::cout << "Running USB Test" << std::endl;
    }

    cmd.append("/bin/dd of=/dev/null if=");
    cmd.append(GetDevice());
    cmd.append(" bs=1k count=64");

    result_.output.clear();
    result_.rv = CmdRunner::Run(cmd,result_.output);

    return result_.rv;
}
