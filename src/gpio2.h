#ifndef GPIO2_H
#define GPIO2_H

#include "connector.h"

#define SYS_EXPORT          "/sys/class/gpio/export"
#define SYS_UNEXPORT        "/sys/class/gpio/unexport"
#define SYS_GPIO_42_DIR     "/sys/class/gpio/gpio42/direction"
#define SYS_GPIO_42_VAL     "/sys/class/gpio/gpio42/value"
#define SYS_GPIO_175_DIR    "/sys/class/gpio/gpio175/direction"
#define SYS_GPIO_175_VAL    "/sys/class/gpio/gpio175/value"

class Gpio2: public Connector {

public:
    Gpio2();
    Gpio2(std::string connector, std::string device);
    virtual ~Gpio2();
    
    struct gpio_sys_handle {
        int dir_fd;
        int val_fd;
    };

    virtual bool Test();

private:
    int fd_{0};
    int export_fd_{0};
    int unexport_fd_{0};
    gpio_sys_handle gpio_42_;
    gpio_sys_handle gpio_175_;
    
    bool receiveData(char *buf, int size);
    bool setup_gpio(void);
    bool teardown_gpio(void);
};

#endif // GPIO2_H
