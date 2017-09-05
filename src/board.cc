#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "board.h"
#include "cmdrunner.h"

Board::Board(): verbose_(false), ip_addr_(DEFAULT_LOCAL_ADDR), ip_gw_(DEFAULT_LOCAL_GATEWAY)
{
    if(std::getenv("TEST_NO_NETWORK_CLOBBER") != nullptr) {
        no_network_clobber_ = true;
    }
}

Board::~Board()
{
    if (fp_mac_0_ != nullptr) {
        fclose(fp_mac_0_);
    }
    
    if (fp_mac_1_ != nullptr) {
        fclose(fp_mac_1_);
    }
}

bool Board::AddConnector(Connector *c)
{
    connectors_.push_back(c);

    return false;
}

void Board::SetVerbose(bool v)
{
    verbose_ = v;
}

void Board::SetIpAddr(std::string ip)
{
    ip_addr_ = ip;
}

void Board::SetIpGateway(std::string ip)
{
    ip_gw_ = ip;
}

bool Board::Test(void)
{
    std::fstream fs;

    if (connectors_.size() > 0) {
        /* bring up the eth interface */
        if (!no_network_clobber_) {
            this->ifUpDown(IF_UP);
        }
        fs.open("test_log.txt", std::fstream::out | std::fstream::trunc);
        for(auto c : connectors_) {
            c->set_verbose(verbose_);
            c->Test();
            std::cout << *c << std::endl;

            /* save to test log */
            fs << "Connector: " << c->GetName() << std::endl;
            fs << "Status: " << c->get_connector_result().rv << std::endl;
            fs << "**********************" << std::endl;
            fs << "Output: " << c->get_connector_result().output << std::endl << std::endl;
        }
        std::cout << std::endl << "Test Complete" << std::endl;

        for(auto c : connectors_) {
            delete c;
        }

        /* close test log */
        fs.close();

        /* bring down the eth interface */
        if (!no_network_clobber_) {
            this->ifUpDown(IF_DOWN);
        }
    }

    return false;
}

bool Board::Reset(void)
{
    for(auto c :  connectors_) {
        delete c;
    }

    return false;
}

bool Board::AssignMac(const std::string addr)
{
    std::vector<char>   buf;
    uint32_t mac_0_n    = ~0x0;
    uint32_t mac_1_n    = ~0x0;
    auto mac_0_rv       = true;
    auto mac_1_rv       = true;
    uint8_t mac_addr[6] = {0};
    std::string out;

    /* remove any : */
    for (auto c: addr) {
        if (c == ':')
            continue;
        buf.push_back(c);
    }

    if (buf.size() != 12) {
        std::cout << "Invalid MAC length!" << std::endl;
        return true;
    }
    
    /* convert string to 6 octets */
    uint64_t t = strtoll(std::string(buf.begin(), buf.end()).c_str(),nullptr,16);
    for (unsigned int i = 0; i < sizeof(mac_addr); i++) {
        mac_addr[sizeof(mac_addr) - i - 1] = static_cast<uint8_t>(t >> (8 * i));
    }
    
    /* new Mac split by sys interface */
    mac_0_n = (mac_addr[2] << 24) | (mac_addr[3] << 16) | (mac_addr[4] << 8) | mac_addr[5];
    mac_1_n = (mac_addr[0] << 8) | (mac_addr[1]);
    
    //std::cout << "0x" << std::hex << mac_0_n << std::endl;
    //std::cout << "0x" << std::hex << mac_1_n << std::endl;

    /* load the fsl_otp kernel module so the sys interface is available */
    CmdRunner::Run("modprobe fsl_otp", out);

    /* /sys/fsl_otp/HW_OCOTP_MAC0 holds the OUI, lower 4 bytes (0x8C:XX:XX:XX) */
    fp_mac_0_ = fopen(MAC0, "a+b");
    if (fp_mac_0_ == nullptr) {
        std::cout << "Failed to open HW_OCOTP_MAC0" << std::endl;
        return true;
    }
    
    /* /sys/fsl_otp/HW_OCOTP_MAC1 holds the upper 2 bytes (0x30:68) */
    fp_mac_1_ = fopen(MAC1, "a+b");
    if (fp_mac_1_ == nullptr) {
        std::cout << "Failed to open HW_OCOTP_MAC1" << std::endl;
        return true;
    }
    
    /* check new and current and save */
    mac_0_rv = testAndSetMac(mac_0_n, fp_mac_0_);
    mac_1_rv = testAndSetMac(mac_1_n, fp_mac_1_);

    /* unload the fsl_otp kernel module */
    CmdRunner::Run("modprobe -r fsl_otp", out);
    
    return mac_0_rv || mac_1_rv;
}

bool Board::testAndSetMac(uint32_t mac_new, FILE* fp)
{
    uint32_t mac_cur    = ~0x0;
    auto rv             = 0;
    
    /* read current MAC */
    rv = fscanf(fp, "%X", &mac_cur);
    if (rv != 1) {
        std::cout << "Failed to read HW_OCOTP_MACX: " << strerror(errno) << std::endl;
        return true;
    }
    
    /* Mac already set */
    if (mac_new == mac_cur) {
        return false;
    }
    
    /* Mac set but does not match */
    if (mac_cur != 0x0) {
        std::cout << "Mac set but mismatch! New: 0x" << std::hex << mac_new << ", Cur: 0x" << mac_cur << std::endl;
        return true;
    }
    
    std::cout << "saving new mac: 0x" << mac_new << std::endl;
    rv = fprintf(fp, "0x%08X", mac_new); 
    fflush(fp);
    if (rv < 0)
    {
        std::cout << "Error writng MAC: " << mac_new << " " << strerror(errno) << std::endl;
        return true;
    }
    
    return false;
}

bool Board::ifUpDown(bool up)
{
    std::string ip_cmd;
    std::string route_cmd;
    std::string ping_cmd;
    std::string out;

    if (no_network_clobber_) {
        return false;
    }

    ip_cmd.append("ifconfig ");
    ip_cmd.append(DEFAULT_ETH_INTERFACE);
    if (up) {
        ip_cmd.append(" netmask 255.255.255.0 ");
        ip_cmd.append(ip_addr_);
        ip_cmd.append(" up");
        CmdRunner::Run(ip_cmd, out);

        /* if there is a gateway set, add the route */
        if (ip_gw_.length() > 0) {
            route_cmd.append("route add default gw ");
            route_cmd.append(ip_gw_);
            CmdRunner::Run(route_cmd, out);
        }
        /* give the link some time to come up */
        sleep(3);
        //TODO: add name serves to /etc/resolv.conf
        //nameserver 8.8.8.8
        //nameserver 8.8.4.4
    } else {
        ip_cmd.append(" down");
        CmdRunner::Run(ip_cmd, out);
    }

    return false;
}
