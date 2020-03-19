#ifndef JITSERVERLOGGER_BASEPERSISTENTLOGGER_H
#define JITSERVERLOGGER_BASEPERSISTENTLOGGER_H
#include <string>

class BasePersistentLogger
    {
    protected:
        const char * _databaseIP;
        std::uint32_t _databasePort;
        const char* _databaseUsername;
        const char* _databasePassword;
        const char* _databaseName;

    public:
        virtual bool connect() = 0;
        virtual void disconnect() = 0;
        
        BasePersistentLogger( const char * databaseIP, std::uint32_t databasePort, const char * databaseName)
            {
            _databaseIP = databaseIP;
            _databasePort = databasePort;
            _databaseName = databaseName;
            _databaseUsername = "";
            _databasePassword = "";
            }
        BasePersistentLogger(const char *databaseIP, std::uint32_t databasePort, const char *databaseName,
                const char *databaseUsername, const char *databasePassword)
            {
            _databaseIP = databaseIP;
            _databasePort = databasePort;
            _databaseName = databaseName;
            _databaseUsername = databaseUsername;
            _databasePassword = databasePassword;
            }
        virtual bool logMethod(const char* method, std::uint64_t clientID, const char *logContent) = 0;
    };

#endif //JITSERVERLOGGER_BASEPERSISTENTLOGGER_H
