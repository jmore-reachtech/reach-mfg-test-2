#include <iostream>
#include <string.h>
#include <unistd.h>

#include "speaker.h"
#include "cmdrunner.h"
#include "logger.h"

Speaker::Speaker(): Connector("Unknown", "/dev/null")
{
}

Speaker::Speaker(std::string connector, std::string device): Connector(connector, device)
{
}

Speaker::~Speaker()
{
}

bool Speaker::Test()
{
    std::string cmd;
    result_.output.clear();
    
    Logger::GetLogger()->Log("Running SPEAKER Test");

    /* set the speaker volume */
    cmd.append("amixer set Headphone 90%");
    result_.rv = CmdRunner::Run(cmd,result_.output);
    cmd.clear();
    
    /* set the headphone volume */
    cmd.append("amixer set PCM 90%");
    result_.rv = CmdRunner::Run(cmd,result_.output);
    cmd.clear();

    cmd.append("aplay /usr/share/mfg-test/beep.wav");
    for(int i = 0; i < 4; i++) {
        result_.rv = CmdRunner::Run(cmd,result_.output);
        sleep(1);
    }

    return result_.rv;
}
