#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "i2c.h"
#include "linux/i2c-dev.h"
#include "logger.h"

I2c::I2c(): Connector("Unknown", "/dev/null")
{
}

I2c::I2c(std::string connector, std::string device): Connector(connector, device)
{
}

I2c::~I2c()
{
    if (fd_ > 0) {
        close(fd_);
    }
}

bool I2c::Test()
{
    auto rv = 0;
    
    result_.rv = true;
    result_.output.clear();

    Logger::GetLogger()->Log("Running I2C Test");

    fd_ = open(GetDevice().c_str(), O_RDWR);
    if (fd_ < 0) {
        result_.output.append("Error opening: ");
        result_.output.append(device_);
        result_.output.append(" ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }

    rv = ioctl(fd_, I2C_SLAVE, I2C_SLAVE_ADDR);
    if (rv < 0) {
        result_.output.append("Error setttng client address: ");
        result_.output.append(device_);
        result_.rv = true;
        goto out;
    }

    rv = i2c_smbus_read_byte_data(fd_,I2C_CTRL_ADDR);
    if (rv < 1) {
        result_.output.append("Error reading client data: ");
        result_.output.append(device_);
        result_.output.append(", ");
        result_.output.append(std::to_string(I2C_CTRL_ADDR));
        result_.output.append(", errno ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }

    if (rv != I2C_CTRL_DATA) {
        result_.output.append("Slave address mismatch. expected ");
        result_.output.append(std::to_string(I2C_CTRL_DATA));
        result_.output.append(", got ");
        result_.output.append(std::to_string(rv));
        result_.rv = true;
    } else {
        result_.rv = false;
    }

out:
    return result_.rv;
}
