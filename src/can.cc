#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "can.h"
#include "libsocketcan.h"
#include "cmdrunner.h"

Can::Can(): Connector("Unknown", "/dev/null")
{
    if (verbose_)
        std::cout << "Creating CAN port" << std::endl; 
}

Can::Can(std::string connector, std::string device): Connector(connector, device)
{
    if (verbose_)
        std::cout << "Creating CAN conector " << connector << " using device " << device << std::endl; 
}

Can::~Can()
{
    std::string out;
    
    if (verbose_)
        std::cout << "Destroying CAN port" << std::endl;
    
    if (fd_ > 0) {
        close(fd_);
        /* we don't care if this succeeds or not, just execute it */
        CmdRunner::Run("modprobe -r flexcan",out);
        CmdRunner::Run("modprobe -r can_raw",out);
    }
}

bool Can::Test()
{
    struct sockaddr_can sock_addr;
    struct ifreq ifr;
    struct can_frame txframe;
    struct can_frame rxframe;
    std::string cmd;
    char msg[2] = {0xDE, 0xAD};
    struct timeval timeout;
    fd_set rfds;
    auto rv = 0;
    std::stringstream ss;
    int msg_rec = 0;
    
    result_.rv = false;
    result_.output.clear();
    
    /* load the can kernel module */
    cmd = "modprobe flexcan";
    if ((CmdRunner::Run(cmd,result_.output))) {
        std::cout << "Failed to load flexcan module" << std::endl;
        result_.rv = true;
        goto out;
    }
    
    /* set the interface name */
    memset(&ifr, 0, sizeof(ifr));
    strcpy(reinterpret_cast<char *>(&(ifr.ifr_ifrn)), "can0");
    
    /* stop the can bus */
    if (can_do_stop(ifr.ifr_name) < 0) {
        std::cout << "Failed to stop the CAN bus" << std::endl;
        result_.rv = true;
        goto out;
    }
    
    /* open socket */
    if ((fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        std::cout << "Failed to open the CAN bus socket" << std::endl;
        result_.rv = true;
        goto out;
    }
    
    /* set the ifreq */
    if (ioctl(fd_, SIOGIFINDEX, &ifr) < 0) {
        std::cout << "Failed to set the CAN bus ioctl" << std::endl;
        result_.rv = true;
        goto out;
    }
    
    /* set the bitrate */
    if (can_set_bitrate(ifr.ifr_name, CAN_BAUD_RATE) < 0) {
        std::cout << "Failed to set the CAN bus bitrate" << std::endl;
        result_.rv = true;
        goto out;
    }
    
    /* start the can bus */
    if (can_do_start(ifr.ifr_name) < 0) {
        std::cout << "Failed to start the CAN bus" << std::endl;
        result_.rv = true;
        goto out;
    }
    
    /* bind to the can bus socket */
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.can_family = AF_CAN;
    sock_addr.can_ifindex = ifr.ifr_ifindex;
    if ((bind(fd_,(struct sockaddr *)&sock_addr, sizeof(sock_addr))) < 0) {
        std::cout << "Failed to bind the CAN bus" << std::endl;
        result_.rv = true;
        goto out;
    }
    
    if (verbose_)
        std::cout << "Running CAN Test" << std::endl;
    
    /* send the message */
    memset(&txframe, 0, sizeof(txframe));
    txframe.can_id = 0x3E;
    txframe.can_dlc = 2;
    strncpy(reinterpret_cast<char *>(txframe.data), msg, sizeof(msg));
    rv = write(fd_, &txframe, sizeof(txframe));
    if (rv < 0) {
        std::cout << "Failed to write to the CAN bus" << std::endl;
        result_.rv = true;
        goto out;
    }
    
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    
    FD_ZERO(&rfds);
    FD_SET(fd_, &rfds);

    memset(&rxframe, 0, sizeof(rxframe));
    rxframe.can_dlc = 2;
    rv = select(fd_+1, &rfds, NULL, NULL, &timeout);
    if (rv == -1) {
        result_.output.append("Error: select() failed: ");
        result_.output.append(device_);
        result_.rv = true;
        goto out;
    }
    else if (rv) {
        rv = read(fd_, &rxframe, sizeof(rxframe));
        if(rv == -1) {
            result_.output.append("Error: read() failed: ");
            result_.output.append(device_);
            result_.output.append(" ");
            result_.output.append(std::to_string(errno));
            result_.rv = true;
            goto out;
		}
        
        msg_rec |= (rxframe.data[0] << 8);
        msg_rec |= (rxframe.data[1] << 0);
        
        if (verbose_)
            std::cout << "0x" << std::uppercase << std::hex << msg_rec << std::endl;
        
        /* tesst for match */
        if (msg_rec == CAN_MSG_EXPECTED) {
            result_.rv = false;
        } else {
            result_.output.append("Frame mismatch: ");
            /* TODO: fix me, hard coded for now */
            result_.output.append("expected 0xBEEF");
            result_.output.append(", got 0x");
            ss << std::hex;
            ss << std::uppercase << std::setw(4) << std::setfill('0') << msg_rec;
            result_.output.append(ss.str());
            result_.rv = true;
        }
	}
    else {
        result_.output.append("Error: select() timeout: ");
        result_.output.append(device_);
        result_.rv = true;
        goto out;
	}

out:
    return result_.rv;
}
