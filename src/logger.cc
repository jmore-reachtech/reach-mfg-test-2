#include "logger.h"
#include "datetime.h"
    
bool Logger::verbose                    = false;
const std::string    Logger::filename_  = "test-run.log";
Logger*              Logger::plog_      = nullptr;
std::ofstream        Logger::log_file_;

Logger::Logger(){}

Logger* Logger::GetLogger()
{
    if (plog_ == nullptr) {
        plog_ = new Logger();
        log_file_.open(filename_.c_str(), std::ios::out | std::ios::trunc);
    }

    return plog_;
}

void Logger::Log(const std::string& msg)
{
    if (Logger::verbose) {
        std::cout << msg << std::endl;
    }
    log_file_ << datetime::CurrentDateTime() << msg << std::endl;
}
