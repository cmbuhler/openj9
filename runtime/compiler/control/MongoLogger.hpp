#ifndef MONGOLOGGER_HPP
#define MONGOLOGGER_HPP

#include "BasePersistentLogger.hpp"
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

class MongoLogger : public BasePersistentLogger {
private:
    static mongocxx::instance _inst;
    mongocxx::uri _uri;
    mongocxx::client _client;
    mongocxx::database _db;

    std::string constructURI();
public:
    bool connect();
    void disconnect();
    MongoLogger(std::string const &databaseIP, std::string const &databasePort, std::string const &databaseName);
    MongoLogger(std::string const &databaseIP, std::string const &databasePort, std::string const &databaseName,
                std::string const &databaseUsername, std::string const &databasePassword);
    MongoLogger();
    ~MongoLogger();
    bool logMethod(std::string const &method, std::string const &clientID, std::string const &logContent);
};

#endif //MONGOLOGGER_HPP
