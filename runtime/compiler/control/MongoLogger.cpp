#include "MongoLogger.hpp"
#include <ctime>
#include <iostream>
#include "LoadDBLibs.hpp"

MongoLogger::MongoLogger(std::string const &databaseIP, std::uint32_t databasePort, std::string const &databaseName)
        : BasePersistentLogger(databaseIP, databasePort, databaseName){

}

MongoLogger::MongoLogger(std::string const &databaseIP, std::uint32_t databasePort, std::string const &databaseName,
        std::string const &databaseUsername, std::string const &databasePassword)
        : BasePersistentLogger(databaseIP, databasePort, databaseName, databaseUsername, databasePassword){

}

MongoLogger::~MongoLogger() {
    //Clean up this mongoc logger.
    Omongoc_collection_destroy(_collection);
    Omongoc_database_destroy(_db);
    Omongoc_uri_destroy(_uri);
    Omongoc_client_destroy(_client);
}

std::string MongoLogger::constructURI() {
    // Check if we have the database name
    if(_databaseName.empty()){
        return "jitserver_logs";
    }

    // Check if we have db IP and Port
    if(_databaseIP.empty()) {
        // No IP try localhost
        _databaseIP = "127.0.0.1";
    }
    if(!_databasePort) {
        // No Port try default MongoDB port
        _databasePort = 27017;
    }

    std::string host = _databaseIP + ":" + std::to_string(_databasePort);

    // Check if we have credentials
    std::string credentials = "";
    if(!_databaseUsername.empty()){
        if(_databasePassword.empty()){
            credentials = _databaseUsername;
        } else {
            credentials = _databaseUsername + ":" + _databasePassword;
        }
    }

    if(credentials.empty()){
        return "mongodb://" + host + "";
    }
    return "mongodb://" + credentials + "@" + host + "/?authSource=" + _databaseName;
}

bool MongoLogger::connect() {
    Obson_error_t error;

    //Validate URI.
    _uri = Omongoc_uri_new_with_error(constructURI().c_str(), &error);
    if (!_uri) {
        fprintf (stderr,
                 "JITServer: Persistent Logger failed to parse URI: %s\n"
                 "error message:       %s\n",
                 constructURI().c_str(),
                 error.message);
        return false;
    }

    //Create a client
    _client = Omongoc_client_new_from_uri(_uri);
    if(!_client){
        return false;
    }

    //Register the application name so we can track it in the profile logs
    //on the server if we want.
    Omongoc_client_set_appname(_client, "jitserver");

    //Get a handle on the database and collection.
    _db = Omongoc_client_get_database(_client, _databaseName.c_str());
    _collection = Omongoc_client_get_collection (_client, _databaseName.c_str(), "logs");

    //Mongo is designed to be always available. Thus there is no "Connection" object
    //and you will find that the "Connection" is tested on every read/write.
    return true;
}

void MongoLogger::disconnect() {
    //Does not actually do anything for Mongo as noted in connect()
    return;
}

bool MongoLogger::logMethod(std::string const &method, std::uint64_t clientID, std::string const &logContent) {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    int64_t timestamp = t.tv_sec * INT64_C(1000) + t.tv_nsec / 1000000;
    /*
     * The following constructs and inserts the following JSON structure:
     * {
     *     "method" : "method/package/methodName()",
     *     "client_id" : "clientid",
     *     "log" : "big_log",
     *     "timestamp" : ISODate
     * }
     */
    Obson_t *insert = Obson_new();
    Obson_error_t error;
    Obson_append_utf8(insert, "method", -1, method.c_str(), -1);
    Obson_append_utf8(insert, "client_id", -1, std::to_string(clientID).c_str(), -1);
//TODO: CONVER CLIENTID to CHAR * without using STRING.
    Obson_append_utf8(insert, "log", -1, logContent.c_str(), -1);
    Obson_append_date_time(insert, "timestamp", -1, timestamp);

    if (!Omongoc_collection_insert_one(_collection, insert, NULL, NULL, &error)) {
        fprintf(stderr, "JITServer: Mongo Logger failed to insert log.\n"
                        "error message: %s\n", error.message);
    }

    Obson_destroy (insert);

    return true;
}

