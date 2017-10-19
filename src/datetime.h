#ifndef DATETIME_H
#define DATETIME_H

#include <iostream>
#include <string>
#include <ctime>
#include <array>

namespace datetime {
    const std::string CurrentDateTime(void)
    {
        /* std::put_time not available in GCC 4.9 */
        std::array<char, 64> buffer;
        buffer.fill(0);
        time_t rawtime;
        time(&rawtime);
        const auto timeinfo = localtime(&rawtime);
        strftime(buffer.data(), sizeof(buffer), "[%Y-%m-%d %H:%M:%S] ", timeinfo);
        
        return std::string(buffer.data());
    }
}

#endif // DATETIME_H
