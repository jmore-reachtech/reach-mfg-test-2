#include <iostream>
#include <string.h>
#include <unistd.h>

#include "beeper.h"
#include "cmdrunner.h"
#include "logger.h"

Beeper::Beeper(): Connector("Unknown", "/dev/null")
{
}

Beeper::Beeper(std::string connector, std::string device): Connector(connector, device)
{
}

Beeper::~Beeper()
{
}

bool Beeper::Test()
{
    std::string cmd;
    
    Logger::GetLogger()->Log("Running Beeper Test");

    result_.output.clear();
    cmd.append("beep -f 3500 -l 150");
    for(int i = 0; i < 4; i++) {
        result_.rv = CmdRunner::Run(cmd,result_.output);
        sleep(1);
    }

    return result_.rv;
}
