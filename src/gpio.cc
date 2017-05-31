#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "gpio.h"
#include "linux/i2c-dev.h"

Gpio::Gpio(): Connector("Unknown", "/dev/null")
{
    std::cout << "Creating GPIO port" << std::endl; 
}

Gpio::Gpio(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating GPIO conector " << connector << " using device " << device << std::endl; 
}

Gpio::~Gpio()
{
    if (verbose_)
        std::cout << "Destroying GPIO port" << std::endl; 

    if (fd_ > 0) {
        close(fd_);
    }
}

bool Gpio::Test()
{
    if (verbose_)
        std::cout << "Running GPIO Test" << std::endl;

    auto rv = 0;
    auto x 	= 0;
    
    result_.output.clear();

    if (verbose_)
        std::cout << "Running I2C" << std::endl; 

    fd_ = open(GetDevice().c_str(), O_RDWR);
    if (fd_ < 0) {
        std::cout << "Error opening " << GetDevice() << errno << std::endl;
        return true;
    }

    rv = ioctl(fd_, I2C_SLAVE, 0x3E);
    if (rv < 0) {
        std::cout << "Error setttng client address: " << errno << std::endl;
        close (fd_);
        return true;
    }
    
    /* setup CTRL_REG */
    rv = i2c_smbus_write_byte_data(fd_, GPIO_CTRL_REG, 0xF0);
    if(rv < 0) {
        std::cout << "Error setttng CTRL_REG: " << errno << std::endl;
        return true;
    }
    /* walk 1 over lower 4 bits  */
    for(x = 0; x < 4; x++) {
        /* write to the OUT_REG of the i2c device */	
        rv = i2c_smbus_write_byte_data(fd_, GPIO_OUT_REG, (0x1 << x));
        if(rv < 0) {
            std::cout << "Error setttng OUT_REG: " << errno << std::endl;
            return true;
        }
        /* read the INPUT_REG register of the i2c device */
        rv = i2c_smbus_read_byte_data(fd_, GPIO_INPUT_REG);
        if(rv < 0) {
            std::cout << "Error setttng OUT_REG: " << errno << std::endl;
            return true;
    }
        /* check for valid bit */
        if(((GPIO_MASK >> x) != (rv & (GPIO_MASK >> x)))) {
            std::cout << "Error reading INPUT_REG: " << errno << std::endl;
            return true;
        }
    }
    
    /* setup CTRL_REG */
    rv = i2c_smbus_write_byte_data(fd_, GPIO_CTRL_REG, 0x0F);
    if(rv < 0) {
        std::cout << "Error setttng CTRL_REG: " << errno << std::endl;
        return true;
    }
    /* walk 1 over upper 4 bits  */
    for(x = 7; x > 3; x--) {
        /* write to the OUT_REG of the i2c device */	
        rv = i2c_smbus_write_byte_data(fd_, GPIO_OUT_REG, (0x1 << x));
        if(rv < 0) {
            std::cout << "Error setttng OUT_REG: " << errno << std::endl;
            return true;
        }
        /* read the INPUT_REG register of the i2c device */
        rv = i2c_smbus_read_byte_data(fd_, GPIO_INPUT_REG);
        if(rv < 0) {
            std::cout << "Error setttng OUT_REG: " << errno << std::endl;
            return true;
        }
        /* check for valid bit */
        if(((GPIO_MASK >> x) != (rv & (GPIO_MASK >> x)))) {
            std::cout << "Error reading INPUT_REG: " << errno << std::endl;
            return true;
        }
    }

    return false;
}
