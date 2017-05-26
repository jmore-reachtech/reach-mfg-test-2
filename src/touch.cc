#include <iostream>
#include <string.h>

#include "touch.h"
#include "cmdrunner.h"

Touch::Touch(): Connector("Unknown", "/dev/null")
{
    if (verbose_)
        std::cout << "Creating Touch port" << std::endl; 
}

Touch::Touch(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating Touch conector " << connector << " using device " << device << std::endl; 
}


Touch::~Touch()
{
    if (verbose_)
        std::cout << "Destroying Touch port" << std::endl; 
}

bool Touch::Test()
{
    std::string cmd;

    if (verbose_) {
        std::cout << "Running Touch Test" << std::endl;
    }

    cmd.append("/usr/bin/ts_calibrate");

    result_.output.clear();
    result_.rv = CmdRunner::Run(cmd,result_.output);

    return result_.rv;
}
