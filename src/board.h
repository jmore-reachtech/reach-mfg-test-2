#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <stdio.h>

#define MAC0 "/sys/fsl_otp/HW_OCOTP_MAC0"
#define MAC1 "/sys/fsl_otp/HW_OCOTP_MAC1"

class Board
{
public:
    Board();
    ~Board();
    
    bool AssignMac(const std::string addr);

private:
    FILE *fp_mac_0_{nullptr};
    FILE *fp_mac_1_{nullptr};
    
    bool testAndSetMac(uint32_t new_mac, FILE *fp);
};

#endif // BOARD_H
