#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "board.h"

Board::Board()
{
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

bool Board::AssignMac(const std::string addr)
{
    std::vector<char>   buf;
    uint32_t mac_0_n    = ~0x0;
    uint32_t mac_1_n    = ~0x0;
    auto mac_0_rv       = true;
    auto mac_1_rv       = true;
    uint8_t mac_addr[6] = {0};

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
        std::cout << "Mac set but mismatch! New: 0x" << mac_new << ", Cur: 0x" << mac_cur << std::endl;
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