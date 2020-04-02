#ifndef MONGOLOGGER_HPP
#define MONGOLOGGER_HPP

#include "BasePersistentLogger.hpp"
#include "LoadDBLibs.hpp"

class MongoLogger : public BasePersistentLogger
   {
private:
   //TODO: Allocate this with OpenJ9 Allocators.
   char _uri_string[256];
   Omongoc_uri_t *_uri;
   Omongoc_client_t *_client;
   Omongoc_database_t *_db;
   Omongoc_collection_t *_collection;

   void init();
   char* constructURI();

public:
   bool connect() override;

   void disconnect() override;

   MongoLogger(const char* databaseIP, uint32_t databasePort, const char* databaseName);

   MongoLogger(const char* databaseIP, uint32_t databasePort, const char* databaseName,
               const char* databaseUsername, const char* databasePassword);

   MongoLogger();

   ~MongoLogger();

   bool logMethod(const char* method, uint64_t clientID, const char* logContent) override;
   };

#endif //MONGOLOGGER_HPP
