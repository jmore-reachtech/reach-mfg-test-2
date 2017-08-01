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
    
    if (fd_ > 0) {
        close(fd_);
    }
}

bool Uart::Test()
{
    struct termios tcs;
    struct timeval timeout;
    auto rv = 0;
    char send[2]{'a', '\0'};
    char recv[2]{'\0', '\0'};
    fd_set rfds;

    result_.rv = false;
    result_.output.clear();
    
    if (verbose_)
        std::cout << "Running UART Test" << std::endl;

    memset(&tcs, 0, sizeof(tcs));
    tcs.c_iflag = 0;
    tcs.c_oflag = 0;
    tcs.c_cflag = CS8 | CREAD | CLOCAL;
    tcs.c_lflag = 0;
    tcs.c_cc[VMIN] = 1;
    tcs.c_cc[VTIME] = 5;

    fd_ = open(device_.c_str(), O_RDWR);
    if (fd_ < 0) {
        result_.output.append("Error opening: ");
        result_.output.append(device_);
        result_.output.append(" ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }
    cfsetospeed(&tcs, B115200);
    cfsetispeed(&tcs, B115200);
    tcsetattr(fd_, TCSANOW, &tcs);

    if (verbose_)
        std::cout << "Opened " << device_ << std::endl;
    
    rv = write(fd_, send, 1);
    if (rv != 1) {
        result_.output.append("Error writing to serial port: ");
        result_.output.append(device_);
        result_.output.append(" ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }
    
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    
    FD_ZERO(&rfds);
    FD_SET(fd_, &rfds);
    
    rv = select(fd_+1, &rfds, NULL, NULL, &timeout);
    if (rv == -1) {
        result_.output.append("Error: select() failed: ");
        result_.output.append(device_);
        result_.rv = true;
        goto out;
    }
    else if (rv) {
        rv = read(fd_,recv,1);
        if(rv != 1) {
            result_.output.append("Error: read() failed: ");
            result_.output.append(device_);
            result_.rv = true;
            goto out;
		}
        
        if(strcmp(send,recv)) {
            result_.output.append("Error: read() mismtach: ");
            result_.output.append(device_);
            result_.rv = true;
            goto out;
		}
	}
    else {
        result_.output.append("Error: select() timeout: ");
        result_.output.append(device_);
        result_.rv = true;
        goto out;
	}

out:
    return result_.rv;
}
