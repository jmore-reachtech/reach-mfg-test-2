#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

class Logger {

public:

    static bool verbose;
    static void Log(const std::string& msg);

    static Logger* GetLogger(void);

private:
    Logger();
    Logger(const Logger&){};

    static const std::string    filename_;
    static Logger*              plog_;
    static std::ofstream        log_file_;
};

#endif // LOGGER_H
