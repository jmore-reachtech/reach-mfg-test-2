#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <getopt.h>
#include <string.h>

#include "uart.h"
#include "i2c.h"

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

    while (std::getline(ss,item,',')) {
        items.push_back(item);
    }

    return items;
}

static const char *short_opts = "vht:";
static const struct option long_opts[] = {
    { "tests",       required_argument,  0, 't' },
    { "mac-address", required_argument,  0, 'm' },
    { "list-tests",  no_argument,        0, 0 },
    { "verbose",     no_argument,        0, 'v' },
    { "help",        no_argument,        0, 'h' },
    { "splash",      optional_argument,  0, 0 },
    { "version",     no_argument,        0, 0 },
    { 0, 0, 0, 0 },
};

int main(int argc, char **argv)
{
    auto opt_index  = 0;
    auto opt        = 0;
    std::string tests;
    std::string image;
    std::string mac;
    auto verbose    = false;
    std::vector<Connector*> connectors;
    std::vector<std::string> connector_tests;

    if (argc == 1) {
        showUsage(argv[0]);

        return 1;
    }

    while ((opt = getopt_long(argc, argv, short_opts, long_opts, &opt_index)) != -1) {
        switch (opt) {
            case 0:
                switch (opt_index) {
                    case 0: // --tests
                        break;
                    case 1: // --mac-address
                        break;
                    case 2: // --list-tests
                        std::cout << "list tests long" << std::endl;
                        break;
                    case 3: // --verbose
                        break;
                    case 4: // --help
                        break;
                    case 5: // --splash
                        std::cout << "Splash long";
                        if (optarg) {
                            image = strdup(optarg);
                        }
                        std::cout << image << std::endl;
                        return 0;
                    case 6: // --version
                        std::cout << "Version 1.1.1" << std::endl;
                        return 0;
                    default:
                        return 1;
                } break;
            case 't': // --tests
                if (optarg) {
                    tests = strdup(optarg);
                }
                break;
            case 'm': // --mac-address
                if (optarg) {
                    mac = strdup(optarg);
                }
                break;
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

    if (tests.size() != 0) {
        connector_tests = split(tests);
        for (auto t: connector_tests) {
            std::cout << t << std::endl;
            if (t == "AUART1")
                connectors.push_back(new Uart("J2", "/dev/ttymxc1"));
            if (t == "AUART2")
                connectors.push_back(new Uart("J21", "/dev/ttymxc3"));
            if (t == "AUART3")
                connectors.push_back(new Uart("J25", "/dev/ttymxc4"));
            if (t == "I2C")
                connectors.push_back(new I2c("J21", "/dev/i2c-1"));
        }
    }

    if (verbose) {
        for(auto c: connectors) {
            c->Test();
        }

        for(auto c: connectors) {
            delete c;
        }
    }



    return 0;
}
