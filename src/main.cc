#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <getopt.h>
#include <string.h>
#include <cstdlib>

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

static void showUsage(std::string name)
{
    std::cout << std::endl << "Perform Manufacturing Test" << std::endl << std::endl;
    std::cout << "Usage: " << name << " [options]" << std::endl << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "  -h, --help                          Display this help and exit" << std::endl;
    std::cout << "  -v, --verbose                       Enable verbose output" << std::endl;
    std::cout << "  -t, --tests                         Tests to run" << std::endl;
    std::cout << "      --list-tests                    Display this help and exit" << std::endl;
    std::cout << "      --mac-address                   Burn in MAC address" << std::endl;
    std::cout << "      --splash                        Splash image to panel" << std::endl;
    std::cout << "      --version                       Display Program version" << std::endl << std::endl;
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
    { "mac-address",    required_argument, 0, 'm' },
    { "list-tests",     no_argument,       0, 0 },
    { "verbose",        no_argument,       0, 'v' },
    { "help",           no_argument,       0, 'h' },
    { "splash",         optional_argument, 0, 0 },
    { "version",        no_argument,       0, 0 },
    { 0, 0, 0, 0 },
};

int main(int argc, char** argv)
{
    auto opt_index      = 0;
    auto opt            = 0;
    auto verbose        = false;
    auto rv             = false;
    std::string tests;
    std::string image;
    std::string mac;
    std::vector<Connector*> connectors;
    std::vector<std::string> connector_tests;
    std::fstream fs;
    
    /* pull server vars from env */
    auto server_addr    = "";
    auto rtc_addr       = "";
    
    if(const char* env_p = std::getenv("TEST_WEB_SERVER_ADDR")) {
        server_addr = env_p;
    }
    if(const char* env_p = std::getenv("TEST_RTC_SERVER_ADDR")) {
        rtc_addr = env_p;
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
                break;
            case 2: // --list-tests
                std::cout << "list tests long" << std::endl;
                return 0;
            case 3: // --verbose
                break;
            case 4: // --help
                break;
            case 5: // --splash
                std::cout << "Splash long";
                if(optarg) {
                    image = strdup(optarg);
                }
                std::cout << image << std::endl;
                return 0;
            case 6: // --version
                std::cout << "Version 1.1.1" << std::endl;
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
        case 'm': // --mac-address
            if(optarg) {
                mac = strdup(optarg);
            }
            return 0;
        case 'v': // --verbose
            verbose = true;
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
                connectors.push_back(new Touch("J0", "/dev/input/touchscreen0"));
                continue;
            }
            if(t == "AUART1") {
                connectors.push_back(new Uart("J2", "/dev/ttymxc1"));
                continue;
            }
            if(t == "AUART2") {
                connectors.push_back(new Uart("J21", "/dev/ttymxc3"));
                continue;
            }
            if(t == "AUART3") {
                connectors.push_back(new Uart("J25", "/dev/ttymxc4"));
                continue;
            }
            if(t == "I2C") {
                connectors.push_back(new I2c("J21", "/dev/i2c-1"));
                continue;
            }
            if(t == "CAN") {
                connectors.push_back(new Can("J20", "can0"));
                continue;
            }
            if(t == "ETHERNET") {
                connectors.push_back(new Ethernet("J3", "eth0", server_addr));
                continue;
            }
            if(t == "USB1") {
                connectors.push_back(new Usb("J4", "/dev/sda1"));
                continue;
            }
            if(t == "USB2") {
                connectors.push_back(new Usb("J5", "/dev/sdb1"));
                continue;
            }
            if(t == "GPIO") {
                connectors.push_back(new Gpio("J22", "gpio"));
                continue;
            }
            if(t == "FLASH") {
                connectors.push_back(new Flash("J0", "/dev/mtd0"));
                continue;
            }
            if(t == "LCD") {
                connectors.push_back(new Lcd("J0", "/dev/fb0"));
                continue;
            }
            if(t == "RTC") {
                connectors.push_back(new Rtc("J3", "eth0", rtc_addr));
                continue;
            }

            /* Invalid test */
            std::cout << "Invalid test: " << t << std::endl;

            for(auto c : connectors) {
                delete c;
            }

            return 1;
        }
    }

    fs.open("test_log.txt", std::fstream::out | std::fstream::trunc);
    for(auto c : connectors) {
        c->set_verbose(verbose);
        std::cout << "Testing: " << c->GetName() << " ";
        rv = c->Test();
        std::cout << rv << std::endl;
        
        /* save to test log */
        fs << "Connector: " << c->GetName() << std::endl;
        fs << "Status: " << c->get_connector_result().rv << std::endl;
        fs << "**********************" << std::endl;
        fs << "Output: " << c->get_connector_result().output << std::endl << std::endl;
    }
    std::cout << std::endl << "Test Complete" << std::endl;

    for(auto c : connectors) {
        delete c;
    }
    
    /* close test log */
    fs.close();

    return 0;
}
