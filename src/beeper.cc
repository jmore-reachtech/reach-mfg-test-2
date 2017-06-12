#include <iostream>
#include <string.h>
#include <unistd.h>

#include "beeper.h"
#include "cmdrunner.h"

Beeper::Beeper(): Connector("Unknown", "/dev/null")
{
    if (verbose_)
        std::cout << "Creating Beeper port" << std::endl; 
}

Beeper::Beeper(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating Beeper conector " << connector << " using device " << device << std::endl; 
}

Beeper::~Beeper()
{
    if (verbose_)
        std::cout << "Destroying Beeper port" << std::endl; 
}

bool Beeper::Test()
{
    std::string cmd;
    
    if (verbose_) {
        std::cout << "Running Beeper Test" << std::endl;
    }

    result_.output.clear();
    cmd.append("beep -f 3500 -l 150");
    for(int i = 0; i < 4; i++) {
        result_.rv = CmdRunner::Run(cmd,result_.output);
        sleep(1);
    }

    return result_.rv;
}
