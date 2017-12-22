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
#include "logger.h"

Gpio2::Gpio2(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating GPIO2 port" << std::endl;
}

Gpio2::Gpio2(std::string connector, std::string device): Connector(connector, device), 
    gpio_42_(), gpio_175_()
{
}

Gpio2::~Gpio2()
{
    std::string log;

    if (fd_ > 0) {
        close(fd_);
    }
    
    if (export_fd_ > 0) {
        close(export_fd_);
    }
    
    if (unexport_fd_ > 0) {
        close(unexport_fd_);
    }
}

bool Gpio2::Test()
{
    struct termios tcs;
    auto rv = 0;
    char send[2]{0x06, '\0'};
    char rst[2]{0x15, '\0'};
    char recv[2]{'\0', '\0'};
    struct serial_rs485 rs485conf;
    std::string log;

    result_.rv = true;
    result_.output.clear();
    
    Logger::GetLogger()->Log("Running GPIO2 Test");

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
        Logger::GetLogger()->Log("TIOCGRS485 ioctl failed " + device_);
    }
    /* enable RS485 mode: */
    rs485conf.flags |= SER_RS485_ENABLED;

    if (ioctl(fd_, TIOCSRS485, &rs485conf) < 0) {
        Logger::GetLogger()->Log("TIOCGRS485 ioctl failed " + device_);
    }

    Logger::GetLogger()->Log("Opened " + device_);
    
    /* send the CTS pin counter reset command byte*/
    rv = write(fd_, rst, 1);
    if (rv != 1) {
        result_.output.append("Error writing to serial port: ");
        result_.output.append(device_);
        result_.output.append(" ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }

    /* we get 2 bytes back; just read 1 byte at a time */
    //TODO: fix receiveData to wait for both bytes
    if (receiveData(recv,1)) {
        goto out;
    }
    if (receiveData(&recv[1],1)) {
        goto out;
    }
    /* both bytes should be 0x0 */
    if ((recv[0] != 0x0) || (recv[1] != 0x0)) {
        result_.output.append("CTS reset invalid");
        result_.rv = true;
        goto out;
    }

    /* set one pin */
    write(gpio_42_.val_fd, GPIO_PIN_HIGH, sizeof(GPIO_PIN_HIGH));
    write(gpio_175_.val_fd, GPIO_PIN_LOW, sizeof(GPIO_PIN_LOW));
    /* ask for data */
    rv = write(fd_, send, 1);
    if (rv != 1) {
        result_.output.append("Error writing to serial port: ");
        result_.output.append(device_);
        result_.output.append(" ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }
    /* we get 2 bytes back; just read 1 byte at a time */
    //TODO: fix receiveData to wait for both bytes
    if (receiveData(recv,1)) {
        goto out;
    }
    if (receiveData(&recv[1],1)) {
        goto out;
    }
    /* check recv */
    if (recv[0] != 0x10 || recv[1] != 0x02) {
        result_.output.append("GPIO mismatch: ");
        result_.rv = true;
        goto out;
    }

    /* flip pin */
    write(gpio_42_.val_fd, GPIO_PIN_LOW, sizeof(GPIO_PIN_LOW));
    write(gpio_175_.val_fd, GPIO_PIN_HIGH, sizeof(GPIO_PIN_HIGH));
    /* ask for data */
    rv = write(fd_, send, 1);
    if (rv != 1) {
        result_.output.append("Error writing to serial port: ");
        result_.output.append(device_);
        result_.output.append(" ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }
    /* we get 2 bytes back; just read 1 byte at a time */
    //TODO: fix receiveData to wait for both bytes
    if (receiveData(recv,1)) {
        goto out;
    }
    if (receiveData(&recv[1],1)) {
        goto out;
    }
    /* check recv */
    if (recv[0] != 0x01 || recv[1] != 0x04) {
        result_.output.append("GPIO mismatch: ");
        result_.rv = true;
        goto out;
    }
    
    result_.rv = false;

out:
    teardown_gpio();
    return result_.rv;
}

bool Gpio2::setup_gpio()
{
    if ((export_fd_ = open(SYS_EXPORT, O_WRONLY)) == -1) {
        return false;
    }
    if ((unexport_fd_ = open(SYS_UNEXPORT, O_WRONLY)) == -1) {
        return false;
    }
    
    write(export_fd_, GPIO_PIN_42, sizeof(GPIO_PIN_42));
    if ((gpio_42_.dir_fd = open(SYS_GPIO_42_DIR, O_RDWR)) == -1) {
        Logger::GetLogger()->Log("Error GPIO open 42");
        return false;
    }
    if ((gpio_42_.val_fd = open(SYS_GPIO_42_VAL, O_RDWR)) == -1) {
        Logger::GetLogger()->Log("Error GPIO val 42");
        return false;
    }
    write(gpio_42_.dir_fd, GPIO_DIR_OUT, sizeof(GPIO_DIR_OUT));
    write(gpio_42_.val_fd, GPIO_PIN_LOW, sizeof(GPIO_PIN_LOW));
    
    write(export_fd_, GPIO_PIN_175, sizeof(GPIO_PIN_175));
    if ((gpio_175_.dir_fd = open(SYS_GPIO_175_DIR, O_RDWR)) == -1) {
        Logger::GetLogger()->Log("Error GPIO open 175");
        return false;
    }
    if ((gpio_175_.val_fd = open(SYS_GPIO_175_VAL, O_RDWR)) == -1) {
        Logger::GetLogger()->Log("Error GPIO val 175");
        return false;
    }
    write(gpio_175_.dir_fd, GPIO_DIR_OUT, sizeof(GPIO_DIR_OUT));
    write(gpio_175_.val_fd, GPIO_PIN_LOW, sizeof(GPIO_PIN_LOW));
    
    return true;
}

bool Gpio2::teardown_gpio()
{
    /* reset gpio to in and low */
    write(gpio_42_.val_fd, GPIO_PIN_LOW, sizeof(GPIO_PIN_LOW));
    write(gpio_42_.dir_fd, GPIO_DIR_IN, sizeof(GPIO_DIR_IN));
    write(unexport_fd_, GPIO_PIN_42, sizeof(GPIO_PIN_42));
    
    write(gpio_175_.val_fd, GPIO_PIN_LOW, sizeof(GPIO_PIN_LOW));
    write(gpio_175_.dir_fd, GPIO_DIR_IN, sizeof(GPIO_DIR_IN));
    write(unexport_fd_, GPIO_PIN_175, sizeof(GPIO_PIN_175));

    return true;
}

bool Gpio2::receiveData(char *buf, int size)
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
        return true;
    }
    else if (rv) {
        rv = read(fd_, buf, size);
        if(rv != size) {
            result_.output.append("Error: read() failed: ");
            result_.output.append(device_);
            result_.output.append(" (");
            result_.output.append(std::to_string(errno));
            result_.output.append(" )");
            result_.rv = true;
            return true;
		}
	}
    else {
        result_.output.append("Error: select() timeout: ");
        result_.output.append(device_);
        result_.rv = true;
        return true;
	}

    return false;
}
