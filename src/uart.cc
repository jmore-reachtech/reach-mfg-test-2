#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "uart.h"

Uart::Uart(): Connector("Unknown", "/dev/null")
{
    if (verbose_)
        std::cout << "Creating UART port" << std::endl; 
}

Uart::Uart(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating UART conector " << connector << " using device" << device << std::endl; 
}

Uart::~Uart()
{
    if (verbose_)
        std::cout << "Destroying UART port" << std::endl; 
}

bool Uart::Test()
{
    if (verbose_)
        std::cout << "Running UART Test" << std::endl;

    int fd = 0;
    struct termios tcs;
    struct timeval timout;

    memset(&tcs, 0, sizeof(tcs));
    tcs.c_iflag = 0;
    tcs.c_oflag = 0;
    tcs.c_cflag = CS8 | CREAD | CLOCAL;
    tcs.c_lflag = 0;
    tcs.c_cc[VMIN] = 1;
    tcs.c_cc[VTIME] = 5;

    fd = open(GetDevice().c_str(), O_RDWR);
    if (fd < 0) {
        if (verbose_)
            std::cout << "Error opening: " << GetDevice()  << errno << std::endl;
        return false;
        
    }

    if (verbose_)
        std::cout << "Opened " << GetDevice() << std::endl;

    close(fd);

    return true;
}
