#include <iostream>
#include <string.h>

#include "touch.h"
#include "cmdrunner.h"
#include "logger.h"

Touch::Touch(): Connector("Unknown", "/dev/null")
{
}

Touch::Touch(std::string connector, std::string device): Connector(connector, device)
{
}


Touch::~Touch()
{
}

bool Touch::Test()
{
    std::string cmd;

    Logger::GetLogger()->Log("Running TOUCH Test");

    cmd.append("/usr/bin/ts_calibrate");

    result_.output.clear();
    result_.rv = CmdRunner::Run(cmd,result_.output);

    return result_.rv;
}
