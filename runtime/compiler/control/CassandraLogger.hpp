#ifndef CASSANDRALOGGER_H
#define CASSANDRALOGGER_H

#include <cassandra.h>
#include "BasePersistentLogger.hpp"
class CassandraLogger : public BasePersistentLogger {

    private:
        CassCluster* _cluster;
        CassSession* _session;
        CassFuture* _connectFuture;
        bool createKeySpace();
        bool createTable(std::string const &tableName);
        
        
    public: 
        bool connect();
        void disconnect();
        CassandraLogger(std::string const &databaseIP, std::uint32_t databasePort, std::string const &databaseName,
                        std::string const &databaseUsername, std::string const &databasePassword);
        CassandraLogger(std::string const &databaseIP, std::uint32_t databasePort, std::string const &databaseName);
        bool logMethod(std::string const &method, std::uint64_t clientID, std::string const &logContent);
       
};

#endif // CASSANDRALOGGER_H