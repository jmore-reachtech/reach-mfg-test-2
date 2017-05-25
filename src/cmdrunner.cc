#include <iostream>
#include <string>
#include <stdio.h>

#include "cmdrunner.h"

bool CmdRunner::Run(std::string cmd, std::string &out)
{
    FILE *in;
    char buf[512];

    /* make sure we get stdout and stderr */
    cmd.append(" 2>&1");

    if (!(in = popen(cmd.c_str(),"r"))) {
        return false;
    }

    while (fgets(buf, sizeof(buf), in) != NULL) {
        out.append(buf);
    }

    return pclose(in);
}
