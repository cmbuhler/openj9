#ifndef JITSERVERLOGGER_BASEPERSISTENTLOGGER_H
#define JITSERVERLOGGER_BASEPERSISTENTLOGGER_H
#include <string>

class BasePersistentLogger {
protected:
    std::string _databaseIP;
    std::uint32_t _databasePort;
    std::string _databaseUsername;
    std::string _databasePassword;
    std::string _databaseName;

public:
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    
    BasePersistentLogger(std::string const &databaseIP, std::uint32_t databasePort, std::string const &databaseName){
        _databaseIP = databaseIP;
        _databasePort = databasePort;
        _databaseName = databaseName;
        _databaseUsername = "";
        _databasePassword = "";
    }
    BasePersistentLogger(std::string const &databaseIP, std::uint32_t databasePort, std::string const &databaseName,
            std::string const &databaseUsername, std::string const &databasePassword){
        _databaseIP = databaseIP;
        _databasePort = databasePort;
        _databaseName = databaseName;
        _databaseUsername = databaseUsername;
        _databasePassword = databasePassword;
    }
    virtual bool logMethod(std::string const &method, std::uint64_t clientID, std::string const &logContent) = 0;
};

#endif //JITSERVERLOGGER_BASEPERSISTENTLOGGER_H
