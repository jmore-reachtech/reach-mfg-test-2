#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <stdio.h>
#include "connector.h"

#define MAC0 "/sys/fsl_otp/HW_OCOTP_MAC0"
#define MAC1 "/sys/fsl_otp/HW_OCOTP_MAC1"
#define DEFAULT_LOCAL_ADDR "10.10.10.3"
#define DEFAULT_LOCAL_GATEWAY ""
#define DEFAULT_ETH_INTERFACE "eth0"
#define IF_UP true
#define IF_DOWN false

class Board
{
public:
    Board();
    ~Board();
    
    bool AssignMac(const std::string addr);
    bool AddConnector(Connector *c);
    bool Test(void);
    bool Reset(void);

    void SetVerbose(bool v);
    void SetIpAddr(std::string ip);
    void SetIpGateway(std::string ip);

private:
    std::vector<Connector*> connectors_;
    FILE *fp_mac_0_{nullptr};
    FILE *fp_mac_1_{nullptr};
    bool verbose_;
    std::string ip_addr_;
    std::string ip_gw_;

    bool testAndSetMac(uint32_t new_mac, FILE *fp);
    bool ifUpDown(bool up);
};

#endif // BOARD_H
