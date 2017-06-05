#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <string>

class Connector
{
public:
    Connector(std::string name, std::string device);
    virtual ~Connector();
    friend std::ostream &operator<<(std::ostream &out, const Connector &c);

    struct connector_result {
        int rv;
        std::string output;
    };

    virtual bool Test() = 0;
    std::string GetDevice() const;
    std::string GetName() const;
    connector_result get_connector_result() const;
    void set_verbose(bool verbose);

private:
    
protected:
    std::string device_;
    std::string name_;
    connector_result result_;
    bool verbose_;
};

#endif
