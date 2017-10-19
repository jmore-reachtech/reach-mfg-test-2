#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <getopt.h>
#include <string.h>
#include <cstdlib>

#include "board.h"
#include "uart.h"
#include "i2c.h"
#include "can.h"
#include "ethernet.h"
#include "usb.h"
#include "gpio.h"
#include "lcd.h"
#include "touch.h"
#include "flash.h"
#include "rtc.h"
#include "beeper.h"
#include "speaker.h"
#include "gpio2.h"
#include "logger.h"

static void showUsage(std::string name)
{
    std::cout << std::endl << "Perform Manufacturing Test" << std::endl << std::endl;
    std::cout << "Usage: " << name << " [options]" << std::endl << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "  -h, --help                          Display this help and exit" << std::endl;
    std::cout << "  -v, --verbose                       Enable verbose output" << std::endl;
    std::cout << "  -t, --tests                         Tests to run" << std::endl;
    std::cout << "      --list-tests                    List available tests and exit" << std::endl;
    std::cout << "      --half-duplex                   Set AUART3 (J25) to half duplex" << std::endl;
    std::cout << "      --mac-address                   Burn in MAC address" << std::endl;
    std::cout << "      --splash                        Splash image to panel" << std::endl;
    std::cout << "      --version                       Display Program version" << std::endl << std::endl;
    std::cout << "Environment:" << std::endl;
    std::cout << "  TEST_WEB_SERVER_ADDR                Web server address to test ETHERNET" << std::endl;
    std::cout << "                                          Defaults to: 10.10.10.2" << std::endl;
    std::cout << "  TEST_RTC_SERVER_ADDR                Time server address to test RTC" << std::endl;
    std::cout << "                                          Defaults to: 10.10.10.2" << std::endl;
    std::cout << "  TEST_MODULE_ADDR                    Module local IP address" << std::endl;
    std::cout << "                                          Defaults to: 10.10.10.3" << std::endl;
    std::cout << "  TEST_MODULE_GATEWAY                 Module default gateway address" << std::endl;
    std::cout << "                                          Defaults to: not set" << std::endl;
    std::cout << "  TEST_NO_NETWORK_CLOBBER             Do not reset the module IP address" << std::endl;
    std::cout << "                                          Defaults to: not set" << std::endl;
    std::cout << "  TEST_SHOW_RESULT_OUTPUT             Show the tests results on stdout" << std::endl;
    std::cout << std::endl;
}

static void listTests(void)
{
    std::cout << std::endl << "Available Manufacturing Tests" << std::endl << std::endl;
    std::cout << "TOUCH     (J0)" << std::endl;
    std::cout << "AUART1    (J2)" << std::endl;
    std::cout << "AUART3    (J25)" << std::endl;
    std::cout << "AUART4    (J21)" << std::endl;
    std::cout << "I2C       (J21)" << std::endl;
    std::cout << "CAN       (J20)" << std::endl;
    std::cout << "ETHERNET  (J3)" << std::endl;
    std::cout << "USB1      (J4)" << std::endl;
    std::cout << "USB2      (J5)" << std::endl;
    std::cout << "GPIO      (J22)" << std::endl;
    std::cout << "FLASH     (J0)" << std::endl;
    std::cout << "LCD       (J0)" << std::endl;
    std::cout << "BEEPER    (L52)" << std::endl;
    std::cout << "SPEAKER   (L52)" << std::endl;
    std::cout << "RTC       (J0)" << std::endl;
    std::cout << "GPIO2     (J21)" << std::endl << std::endl;
}

std::vector<std::string> split(std::string s)
{
    std::vector<std::string> items;
    std::stringstream ss;
    ss.str(s);
    std::string item;

    while(std::getline(ss, item, ',')) {
        items.push_back(item);
    }

    return items;
}

static const char* short_opts = "vht:";
static const struct option long_opts[] = {
    { "tests",          required_argument, 0, 't' },
    { "mac-address",    required_argument, 0, 0 },
    { "list-tests",     no_argument,       0, 0 },
    { "half-duplex",    no_argument,       0, 0 },
    { "verbose",        no_argument,       0, 'v' },
    { "help",           no_argument,       0, 'h' },
    { "splash",         optional_argument, 0, 0 },
    { "version",        no_argument,       0, 0 },
    { 0, 0, 0, 0 },
};

int main(int argc, char** argv)
{
    auto ret            = false;
    auto opt_index      = 0;
    auto opt            = 0;
    auto half_duplex    = false;
    std::string tests;
    std::string image;
    std::string mac;
    Board b;
    std::vector<std::string> connector_tests;

    /* pull server vars from env */
    auto server_addr    = "";
    auto rtc_addr       = "";

    if(const char* env_p = std::getenv("TEST_WEB_SERVER_ADDR")) {
        server_addr = env_p;
    }
    if(const char* env_p = std::getenv("TEST_RTC_SERVER_ADDR")) {
        rtc_addr = env_p;
    }
    if(const char* env_p = std::getenv("TEST_MODULE_ADDR")) {
        b.SetIpAddr(env_p);
    }
    if(const char* env_p = std::getenv("TEST_MODULE_GATEWAY")) {
        b.SetIpGateway(env_p);
    }

    if(argc == 1) {
        showUsage(argv[0]);

        return 1;
    }

    while((opt = getopt_long(argc, argv, short_opts, long_opts, &opt_index)) != -1) {
        switch(opt) {
        case 0:
            switch(opt_index) {
            case 0: // --tests
                break;
            case 1: // --mac-address
                if(optarg) {
                    mac = strdup(optarg);
                }
                break;
            case 2: // --list-tests
                listTests();
                return 0;
            case 3: // --half-duplex
                half_duplex = true;
                break;
            case 4: // --verbose
                break;
            case 5: // --help
                break;
            case 6: // --splash
                std::cout << "Splash long";
                if(optarg) {
                    image = strdup(optarg);
                }
                std::cout << image << std::endl;
                return 0;
            case 7: // --version
                std::cout << "Version 1.0.2" << std::endl;
                return 0;
            default:
                return 1;
            }
            break;
        case 't': // --tests
            if(optarg) {
                tests = strdup(optarg);
            }
            break;
        case 'v': // --verbose
            b.SetVerbose(true);
            Logger::verbose = true;
            break;
        case 'h': // --help
            showUsage(argv[0]);
            return 0;
        case '?': // error on long opt
            break;
        default:
            return 1;
        }
    }

    if(tests.size() != 0) {
        connector_tests = split(tests);
        for(auto t : connector_tests) {
            if(t == "TOUCH") {
                b.AddConnector(new Touch("J0", "/dev/input/touchscreen0"));
                continue;
            }
            if(t == "AUART1") {
                b.AddConnector(new Uart("J2", "/dev/ttymxc1"));
                continue;
            }
            if(t == "AUART3") {
                Uart *u = new Uart("J25", "/dev/ttymxc3");
                if (half_duplex) {
                    u->EnableHalfDuplex();
                }
                b.AddConnector(u);
                continue;
            }
            if(t == "AUART4") {
                b.AddConnector(new Uart("J21", "/dev/ttymxc4"));
                continue;
            }
            if(t == "I2C") {
                b.AddConnector(new I2c("J21", "/dev/i2c-1"));
                continue;
            }
            if(t == "CAN") {
                b.AddConnector(new Can("J20", "can0"));
                continue;
            }
            if(t == "ETHERNET") {
                b.AddConnector(new Ethernet("J3", "eth0", server_addr));
                continue;
            }
            if(t == "USB1") {
                b.AddConnector(new Usb("J4", "/dev/sda1"));
                continue;
            }
            if(t == "USB2") {
                b.AddConnector(new Usb("J5", "/dev/sdb1"));
                continue;
            }
            if(t == "GPIO") {
                b.AddConnector(new Gpio("J22", "/dev/i2c-0"));
                continue;
            }
            if(t == "FLASH") {
                b.AddConnector(new Flash("J0", "/dev/mtd0"));
                continue;
            }
            if(t == "LCD") {
                b.AddConnector(new Lcd("J0", "/dev/fb0"));
                continue;
            }
            if(t == "RTC") {
                b.AddConnector(new Rtc("J3", "eth0", rtc_addr));
                continue;
            }
            if(t == "BEEPER") {
                b.AddConnector(new Beeper("L52", "/dev/null"));
                continue;
            }
            if(t == "SPEAKER") {
                b.AddConnector(new Speaker("L52", "/dev/null"));
                continue;
            }
            if(t == "GPIO2") {
                b.AddConnector(new Gpio2("J21", "/dev/ttymxc4"));
                continue;
            }

            /* Invalid test */
            Logger::GetLogger()->Log("Invalid Test: " + t);

            b.Reset();

            return 1;
        }
    }


    ret = b.Test();

    /* assign mac if set */
    if (mac.length() > 0) {
        Board b;
        ret |= b.AssignMac(mac);
        
        if (ret) {
            Logger::GetLogger()->Log("MAC address: " + mac + " Failed!");
        } else {
            Logger::GetLogger()->Log("MAC address: " + mac + " Success!");
        }
    }

    return ret;
}
