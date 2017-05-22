#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "uart.h"

Uart::Uart(): Connector("Unknown")
{
    std::cout << "Creating UART port" << std::endl; 
}

Uart::Uart(std::string name): Connector(name)
{
    std::cout << "Creating UART for " << name << std::endl; 
}

Uart::~Uart()
{
    std::cout << "Destroying UART port" << std::endl; 
}

bool Uart::Test()
{
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

    fd = open("/dev/ttymxc3", O_RDWR);
    if (fd < 0) {
        std::cout << "Error opening /dev/ttymxc3: " << errno << std::endl;
        return false;
    }

    std::cout << "Opened /dev/ttymxc3" << std::endl;

    close(fd);

    return true;
}
