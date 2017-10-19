#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "gpio.h"
#include "linux/i2c-dev.h"
#include "logger.h"

Gpio::Gpio(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating GPIO port" << std::endl; 
}

Gpio::Gpio(std::string connector, std::string device): Connector(connector, device)
{
}

Gpio::~Gpio()
{
    if (fd_ > 0) {
        close(fd_);
    }
}

bool Gpio::Test()
{
    Logger::GetLogger()->Log("Running GPIO Test");

    auto rv = 0;
    auto x 	= 0;
    
    result_.rv = false;
    result_.output.clear();

    fd_ = open(GetDevice().c_str(), O_RDWR);
    if (fd_ < 0) {
        result_.output.append("Error opening ");
        result_.output.append(name_);
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }

    rv = ioctl(fd_, I2C_SLAVE, 0x3E);
    if (rv < 0) {
        result_.output.append("Error setttng client address: ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }
    
    /* setup CTRL_REG */
    rv = i2c_smbus_write_byte_data(fd_, GPIO_CTRL_REG, 0xF0);
    if(rv < 0) {
        result_.output.append("Error setttng CTRL_REG: ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }
    /* walk 1 over lower 4 bits  */
    for(x = 0; x < 4; x++) {
        /* write to the OUT_REG of the i2c device */	
        rv = i2c_smbus_write_byte_data(fd_, GPIO_OUT_REG, (0x1 << x));
        if(rv < 0) {
            result_.output.append("Error setttng OUT_REG: ");
            result_.output.append(std::to_string(errno));
            result_.rv = true;
            goto out;
        }
        /* read the INPUT_REG register of the i2c device */
        rv = i2c_smbus_read_byte_data(fd_, GPIO_INPUT_REG);
        if(rv < 0) {
            result_.output.append("Error reading INPUT_REG: ");
            result_.output.append(std::to_string(errno));
            result_.rv = true;
            goto out;
    }
        /* check for valid bit */
        if(((GPIO_MASK >> x) != (rv & (GPIO_MASK >> x)))) {
            result_.output.append("Error bit mismatch: ");
            result_.output.append("pin ");
            result_.output.append(std::to_string(x));
            result_.rv = true;
            goto out;
        }
    }
    
    /* setup CTRL_REG */
    rv = i2c_smbus_write_byte_data(fd_, GPIO_CTRL_REG, 0x0F);
    if(rv < 0) {
        result_.output.append("Error setttng CTRL_REG: ");
        result_.output.append(std::to_string(errno));
        result_.rv = true;
        goto out;
    }
    /* walk 1 over upper 4 bits  */
    for(x = 7; x > 3; x--) {
        /* write to the OUT_REG of the i2c device */	
        rv = i2c_smbus_write_byte_data(fd_, GPIO_OUT_REG, (0x1 << x));
        if(rv < 0) {
            result_.output.append("Error setttng OUT_REG: ");
            result_.output.append(std::to_string(errno));
            result_.rv = true;
            goto out;
        }
        /* read the INPUT_REG register of the i2c device */
        rv = i2c_smbus_read_byte_data(fd_, GPIO_INPUT_REG);
        if(rv < 0) {
            result_.output.append("Error reading INPUT_REG: ");
            result_.output.append(std::to_string(errno));
            result_.rv = true;
            goto out;
        }
        /* check for valid bit */
        if(((GPIO_MASK >> x) != (rv & (GPIO_MASK >> x)))) {
            result_.output.append("Error bit mismatch: ");
            result_.output.append("pin ");
            result_.output.append(std::to_string(x));
            result_.rv = true;
            goto out;
        }
    }

out:
    return result_.rv;
}
