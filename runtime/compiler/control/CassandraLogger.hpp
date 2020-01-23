#ifndef CASSANDRALOGGER_H
#define CASSANDRALOGGER_H

#include <cassandra.h>
#include "BasePersistentLogger.hpp"
class CassandraLogger : public BasePersistentLogger
   {

   private:
      CassCluster* _cluster;
      CassSession* _session;
      CassFuture* _connectFuture;
      bool createKeySpace();
      bool createTable(const char *tableName);
      
      
   public: 

      bool connect() override;
      void disconnect() override;
      CassandraLogger(const char *databaseIP, std::uint32_t databasePort, const char *databaseName,
                     const char *databaseUsername, const char *databasePassword);
      CassandraLogger(const char *databaseIP, std::uint32_t databasePort, const char *databaseName);
      bool logMethod(const char *method, std::uint64_t clientID, const char *logContent) override;
      
   };

#endif // CASSANDRALOGGER_H