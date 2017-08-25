#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/serial.h>
#include <sys/ioctl.h>


#include "gpio2.h"

Gpio2::Gpio2(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating GPIO2 port" << std::endl;
}

Gpio2::Gpio2(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating GPIO2 conector " << connector << " using device " << device << std::endl;
}

Gpio2::~Gpio2()
{
    if (verbose_)
        std::cout << "Destroying GPIO2 port" << std::endl; 

    if (fd_ > 0) {
        close(fd_);
    }
}

bool Gpio2::Test()
{
    struct termios tcs;
    struct timeval timeout;
    auto rv = 0;
    char send[2]{'a', '\0'};
    char recv[2]{'\0', '\0'};
    fd_set rfds;
    struct serial_rs485 rs485conf;

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

    if (ioctl(fd_, TIOCGRS485, &rs485conf) < 0) {
        std::cout << "TIOCGRS485 ioctl failed " << device_ << std::endl;
    }
    /* enable RS485 mode: */
    rs485conf.flags |= SER_RS485_ENABLED;

    if (ioctl(fd_, TIOCSRS485, &rs485conf) < 0) {
        std::cout << "TIOCSRS485 ioctl failed " << device_ << std::endl;
    }

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
        
//        /* we send lowercase 'a' 0x61 and should get back an uppercase 'A' 0x41 */
//        send[0] -= 0x20;
//        if(strcmp(send,recv)) {
//            result_.output.append("Error: read() mismtach: ");
//            result_.output.append(device_);
//            result_.output.append(" Expected ");
//            result_.output.append(std::to_string(send[0]));
//            result_.output.append(", Got ");
//            result_.output.append(std::to_string(recv[0]));
//            result_.rv = true;
//            goto out;
//		}
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
