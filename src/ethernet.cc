#include <iostream>
#include <string.h>

#include "ethernet.h"
#include "cmdrunner.h"

Ethernet::Ethernet(): Connector("Unknown", "/dev/null")
{
}

Ethernet::Ethernet(std::string connector, std::string device, std::string server): 
    Connector(connector, device)
{
    if (server.empty())
        server_addr_ = DEFAULT_SERVER_ADDR;
    else
        server_addr_ = server;
}

Ethernet::~Ethernet()
{
}

bool Ethernet::Test()
{
    std::string cmd;
     
    if (verbose_)
        std::cout << "Running ETHERNET Test" << std::endl;
        
    cmd.append("wget -O /tmp/file-32M http://");
    cmd.append(server_addr_);
    cmd.append("/file-32M");
    
    if (verbose_)
        std::cout << "Running ETHERNET Test - server: " << server_addr_ << std::endl;

    result_.output.clear();
    result_.rv = CmdRunner::Run(cmd,result_.output);

    return result_.rv;

    return false;
}
