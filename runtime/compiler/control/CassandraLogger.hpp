#ifndef CASSANDRALOGGER_H
#define CASSANDRALOGGER_H

#include "LoadDBLibs.hpp"
#include "BasePersistentLogger.hpp"
class CassandraLogger : public BasePersistentLogger
   {
   private:
      OCassCluster* _cluster;
      OCassSession* _session;
      OCassFuture* _connectFuture;
      bool createKeySpace();
      bool createTable(const char *tableName);
   public: 
      bool connect() override;
      void disconnect() override;
      CassandraLogger(const char *databaseIP, uint32_t databasePort, const char *databaseName,
                     const char *databaseUsername, const char *databasePassword);
      CassandraLogger(const char *databaseIP, uint32_t databasePort, const char *databaseName);
      bool logMethod(const char *method, uint64_t clientID, const char *logContent, J9JITConfig* j9JitConfig) override;
   };

#endif // CASSANDRALOGGER_H