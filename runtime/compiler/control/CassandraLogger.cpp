#include <time.h>

#include "CassandraLogger.hpp"
CassandraLogger::CassandraLogger(const char *databaseIP, 
std::uint32_t databasePort,
const char *databaseName): BasePersistentLogger(databaseIP, databasePort, databaseName)
    {
    _session = NULL;
    _connectFuture = NULL;
    _cluster = NULL;
    }

CassandraLogger::CassandraLogger(const char *databaseIP, std::uint32_t databasePort,
        const char *databaseName, const char *databaseUsername, const char *databasePassword)
        : BasePersistentLogger(databaseIP, databasePort, databaseName, databaseUsername, databasePassword)
    {
    _session = NULL;
    _connectFuture = NULL;
    _cluster = NULL;
    }

bool CassandraLogger::createKeySpace() 
    {
    char queryString[1024]; 
    sprintf(queryString, "CREATE KEYSPACE IF NOT EXISTS %s  WITH REPLICATION = {'class':'SimpleStrategy','replication_factor':1};", _databaseName); 
    CassStatement* statement = cass_statement_new(queryString, 0);

    CassFuture* queryFuture = cass_session_execute(_session, statement);
    cass_statement_free(statement);
    if (cass_future_error_code(queryFuture) != CASS_OK)
        {
        /* Display connection error message */
        const char* message;
        size_t messageLength;
        cass_future_error_message(queryFuture, &message, &messageLength);
        fprintf(stderr, "PersistentLogging: Cassandra Database Keyspace Creation Error: '%.*s'\n", (int)messageLength, message);
        
        cass_future_free(queryFuture);
        return false;
        }

    cass_future_free(queryFuture);
    return true;

    }
bool CassandraLogger::createTable(const char *tableName) 
    {
    char queryString[1024];
    sprintf(queryString, "CREATE TABLE IF NOT EXISTS %s.%s (clientID text, methodName text, logContent text, insertionDate date,insertionTime time, primary key (clientID, methodName, insertionDate, insertionTime));", _databaseName, tableName);
    CassStatement* statement = cass_statement_new(queryString, 0);

    CassFuture* queryFuture = cass_session_execute(_session, statement);
    cass_statement_free(statement);
    if (cass_future_error_code(queryFuture) != CASS_OK)
        {
        /* Display connection error message */
        const char* message;
        size_t messageLength;
        cass_future_error_message(queryFuture, &message, &messageLength);
        fprintf(stderr, "Persistent Logging: Cassandra Database Table Creation Error: '%.*s'\n", (int)messageLength, message);
        
        cass_future_free(queryFuture);
        return false;
        }

    cass_future_free(queryFuture);
    return true;
    }
bool CassandraLogger::connect()
    {
    /* Setup and connect to cluster */
    _cluster = cass_cluster_new();
    _session = cass_session_new();
    /*authenticate using databaseUsername and databasePassword*/
    cass_cluster_set_credentials(_cluster, _databaseUsername, _databasePassword);
    /*Set protocol version */
    CassError rc_set_protocol = cass_cluster_set_protocol_version(_cluster, 4);
    if (rc_set_protocol != CASS_OK)
        {
        printf("Persistent Logging - Cassandra Database Error: %s\n",cass_error_desc(rc_set_protocol));

        }

    /* Add contact points */
    CassError rc_set_ip = cass_cluster_set_contact_points(_cluster, _databaseIP);
    if (rc_set_ip != CASS_OK)
        {
        printf("Persistent Logging - Cassandra Database Error: %s\n",cass_error_desc(rc_set_ip));
        return false;

        }

    /*Set port number*/
    CassError rc_set_port = cass_cluster_set_port(_cluster, _databasePort);
    if (rc_set_port != CASS_OK)
        {
        printf("Persistent Logging - Cassandra Database Error: %s\n",cass_error_desc(rc_set_port));
        return false;

        }

    /* Provide the cluster object as configuration to connect the session */
    _connectFuture = cass_session_connect(_session, _cluster);

    if (cass_future_error_code(_connectFuture) != CASS_OK)
        {
        /* Display connection error message */
        const char* message;
        size_t messageLength;
        cass_future_error_message(_connectFuture, &message, &messageLength);
        fprintf(stderr, "Persistent Logging: Cassandra Database Connection Error: '%.*s'\n", (int)messageLength, message);
        return false;
        }
    return true;

    }


bool CassandraLogger::logMethod(const char *method, std::uint64_t clientID, const char *logContent)
    {
        
    // create table space and table first
    if (!createKeySpace()) return false;
    const char* tableName = "logs";
    if (!createTable(tableName)) return false;
    char queryString[1024];
    
    sprintf(queryString, "INSERT INTO %s.%s (clientID, methodName, logContent, insertionDate, insertionTime) VALUES (?, ?, ?, ?, ?)", _databaseName,tableName);
    CassStatement* statement
    = cass_statement_new(queryString, 5);

    /* Bind the values using the indices of the bind variables */
    char strClientID[64];
    sprintf(strClientID, "%lu", clientID);
    CassError rc_set_bind_pk = cass_statement_bind_string(statement, 0, strClientID);
    if (rc_set_bind_pk != CASS_OK)
        {
        printf("Persistent Logging - Cassandra Database Error: %s\n",cass_error_desc(rc_set_bind_pk));
        cass_statement_free(statement);
        return false;
        }

    CassError rc_set_bind_method = cass_statement_bind_string(statement, 1, method);
    if (rc_set_bind_pk != CASS_OK)
        {
        printf("Persistent Logging - Cassandra Database Error: %s\n",cass_error_desc(rc_set_bind_method));
        cass_statement_free(statement);
        return false;
        }
    CassError rc_set_bind_log_content = cass_statement_bind_string(statement, 2, logContent);

    if (rc_set_bind_log_content != CASS_OK)
        {
        printf("Persistent Logging - Cassandra Database Error: %s\n",cass_error_desc(rc_set_bind_log_content));
        cass_statement_free(statement);
        return false;
        }

    time_t now = time(NULL); /* Time in seconds from Epoch */
    /* Converts the time since the Epoch in seconds to the 'date' type */
    cass_uint32_t year_month_day_of_insertion = cass_date_from_epoch(now);
    /* Converts the time since the Epoch in seconds to the 'time' type */
    cass_int64_t time_of_insertion = cass_time_from_epoch(now);

    /* 'date' uses an unsigned 32-bit integer */
    CassError rc_set_bind_insertion_date = cass_statement_bind_uint32(statement, 3, year_month_day_of_insertion);
    
    if (rc_set_bind_insertion_date != CASS_OK)
        {
        printf("Persistent Logging - Cassandra Database Error: %s\n", cass_error_desc(rc_set_bind_insertion_date));
        cass_statement_free(statement);
        return false;
        }
    /* 'time' uses a signed 64-bit integer */
    CassError rc_set_bind_insertion_time = cass_statement_bind_int64(statement, 4, time_of_insertion);
    if (rc_set_bind_insertion_time != CASS_OK)
        {
        printf("Persistent Logging - Cassandra Database Error: %s\n",cass_error_desc(rc_set_bind_insertion_time));
        cass_statement_free(statement);
        return false;
        }

    CassFuture* queryFuture = cass_session_execute(_session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);
    if (cass_future_error_code(queryFuture) != CASS_OK)
        {
        /* Display connection error message */
        const char* message;
        size_t messageLength;
        cass_future_error_message(queryFuture, &message, &messageLength);
        fprintf(stderr, "query execution error: '%.*s'\n", (int)messageLength, message);
        cass_future_free(queryFuture);
        return false;
        }

    cass_future_free(queryFuture);
    return true;
        

    }


void CassandraLogger::disconnect()
    {
    cass_future_free(_connectFuture);
    cass_session_free(_session);
    cass_cluster_free(_cluster);
    }









