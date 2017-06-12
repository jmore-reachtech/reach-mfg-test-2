#include <iostream>
#include <string.h>
#include <unistd.h>

#include "speaker.h"
#include "cmdrunner.h"

Speaker::Speaker(): Connector("Unknown", "/dev/null")
{
    if (verbose_)
        std::cout << "Creating Speaker port" << std::endl; 
}

Speaker::Speaker(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating Speaker conector " << connector << " using device " << device << std::endl; 
}

Speaker::~Speaker()
{
    if (verbose_)
        std::cout << "Destroying Speaker port" << std::endl; 
}

bool Speaker::Test()
{
    std::string cmd;
    result_.output.clear();
    
    if (verbose_) {
        std::cout << "Running Speaker Test" << std::endl;
    }

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
