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
#include "cmdrunner.h"

Gpio2::Gpio2(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating GPIO2 port" << std::endl;
}

Gpio2::Gpio2(std::string connector, std::string device): Connector(connector, device), 
    gpio_42_(), gpio_175_()
{
    if (verbose_)
        std::cout << "Creating GPIO2 conector " << connector << " using device " << device << std::endl;
}

Gpio2::~Gpio2()
{
    std::string log;

    if (verbose_)
        std::cout << "Destroying GPIO2 port" << std::endl; 

    if (fd_ > 0) {
        close(fd_);
    }
    
    if (export_fd_ > 0) {
        fclose(export_fd_);
    }
    
    if (unexport_fd_ > 0) {
        fclose(unexport_fd_);
    }
}

bool Gpio2::Test()
{
    struct termios tcs;
    auto rv = 0;
    char send[2]{0x06, '\0'};
    char recv[2]{'\0', '\0'};
    struct serial_rs485 rs485conf;
    std::string log;

    result_.rv = true;
    result_.output.clear();
    
    if (verbose_)
        std::cout << "Running UART Test" << std::endl;

    if (!setup_gpio()) {
        result_.output.append("Error setting up GPIO SYS interfaces");
        goto out;
    }

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

    receiveData(recv, 2);
    
    result_.rv = ((recv[0] == 0x0) && (recv[1] = 0x1));

out:
    return result_.rv;
}

bool Gpio2::setup_gpio()
{
    int rv = 0;
    
    if ((export_fd_ = fopen(SYS_EXPORT, "w")) == nullptr) {
        return false;
    }
    if ((unexport_fd_ = fopen(SYS_UNEXPORT, "w")) == nullptr) {
        return false;
    }
    
    fprintf(export_fd_, "%d", 42);
    //fwrite("42", sizeof(char), 2, export_fd_);    
    if ((gpio_42_.dir_fd = fopen(SYS_GPIO_42_DIR, "r+")) == nullptr) {
        std::cout << "error open dir: " << errno << std::endl;
        return false;
    }
    if ((gpio_42_.val_fd = fopen(SYS_GPIO_42_VAL, "r+")) == nullptr) {
        std::cout << "error open val: " << errno << std::endl;
        return false;
    }
    fprintf(gpio_42_.dir_fd, "%s", "out");
    //fwrite("out", sizeof(char), 3, gpio_42_.dir_fd);
    fprintf(gpio_42_.val_fd, "%s", "1");
    //fwrite("1", sizeof(char), 1, gpio_42_.val_fd);
    
//    write(export_fd_, "175", 2);
//    if ((gpio_175_.dir_fd = open(SYS_GPIO_175_DIR, O_RDWR)) == -1) {
//        return false;
//    }
//    if ((gpio_175_.val_fd = open(SYS_GPIO_175_VAL, O_RDWR)) == -1) {
//        return false;
//    }
//    write(gpio_175_.dir_fd, "out", 3);
//    write(gpio_175_.val_fd, "1", 1);
    
    return true;
}

int Gpio2::receiveData(char *buf, int size)
{
    struct timeval timeout;
    fd_set rfds;
    auto rv = 0;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    
    FD_ZERO(&rfds);
    FD_SET(fd_, &rfds);
    
    rv = select(fd_+1, &rfds, NULL, NULL, &timeout);
    if (rv == -1) {
        result_.output.append("Error: select() failed: ");
        result_.output.append(device_);
        result_.rv = true;
        return -1;
    }
    else if (rv) {
        rv = read(fd_, buf, size);
        if(rv != 1) {
            result_.output.append("Error: read() failed: ");
            result_.output.append(device_);
            result_.output.append(" (");
            result_.output.append(std::to_string(errno));
            result_.output.append(" )");
            result_.rv = true;
            return -1;
		}
	}
    else {
        result_.output.append("Error: select() timeout: ");
        result_.output.append(device_);
        result_.rv = true;
        return -1;
	}

    return 0;
}