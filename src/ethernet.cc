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
    std::string ping_cmd;
    std::string out;
    auto rv = false;
    auto i = 0;

    if (verbose_)
        std::cout << "Running ETHERNET Test" << std::endl;

    /* test to see if the server is responding */
    ping_cmd.append("ping -c 1 ");
    ping_cmd.append(server_addr_);
    for (i = 0; i < PING_MAX; i++) {
        if (verbose_) {
            std::cout << "pinging test server " << server_addr_ << std::endl;
        }
        rv = CmdRunner::Run(ping_cmd, out);
        if (rv == 0) {
            break;
        }
        if (verbose_) {
            std::cout << "server ping failed, retry" << std::endl;
        }
    }

    if (i == PING_MAX) {
        /* ping failed, server down */
        if (verbose_) {
            std::cout << out << std::endl;
        }
        result_.rv = true;
        result_.output.append(out);

        return result_.rv;
    }

    cmd.append("wget -O /tmp/file-32M http://");
    cmd.append(server_addr_);
    cmd.append("/file-32M");

    if (verbose_)
        std::cout << "Running ETHERNET Test - server: " << server_addr_ << std::endl;

    result_.output.clear();
    result_.rv = CmdRunner::Run(cmd,result_.output);

    return result_.rv;
}
