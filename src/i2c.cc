#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "i2c.h"
#include "linux/i2c-dev.h"

I2c::I2c(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating I2C device" << std::endl;
}

I2c::I2c(std::string connector, std::string device): Connector(connector, device)
{
    std::cout << "Creating I2C device" << std::endl;
}

I2c::~I2c()
{
    std::cout << "Destroying I2C device" << std::endl;
}

bool I2c::Test()
{
    std::cout << "Running I2C" << std::endl; 
    int fd = 0;
    int rv = 0;

    fd = open(GetDevice().c_str(), O_RDWR);
    if (fd < 0) {
        std::cout << "Error opening " << GetDevice() << errno << std::endl;
        return false;
    }

    rv = ioctl(fd, I2C_SLAVE, 0x3E);
    if (rv < 0) {
        std::cout << "Error setttng client address: " << errno << std::endl;
        close (fd);
        return false;
    }

    rv = i2c_smbus_read_byte_data(fd,0x1);

    std::cout << "read 0x" << std::hex << rv << std::endl;

    close (fd);

    return true;
}
