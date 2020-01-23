#include <time.h>
#include <stdio.h>

#include "CassandraLogger.hpp"
#include "LoadDBLibs.hpp"
CassandraLogger::CassandraLogger(const char *databaseIP,
uint32_t databasePort,
const char *databaseName): BasePersistentLogger(databaseIP, databasePort, databaseName)
   {
   _session = NULL;
   _connectFuture = NULL;
   _cluster = NULL;
   }

CassandraLogger::CassandraLogger(const char *databaseIP, uint32_t databasePort,
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
   OCassStatement* statement = Ocass_statement_new(queryString, 0);

   OCassFuture* queryFuture = Ocass_session_execute(_session, statement);
   Ocass_statement_free(statement);
   if (Ocass_future_error_code(queryFuture) != 0)
      {
      /* Display connection error message */
      const char* message;
      size_t messageLength;
      Ocass_future_error_message(queryFuture, &message, &messageLength);
      fprintf(stderr, "PersistentLogging: Cassandra Database Keyspace Creation Error: '%.*s'\n", (int)messageLength, message);
      
      Ocass_future_free(queryFuture);
      return false;
      }

   Ocass_future_free(queryFuture);
   return true;

}
bool CassandraLogger::createTable(const char *tableName) 
   {
   char queryString[1024];
   sprintf(queryString, "CREATE TABLE IF NOT EXISTS %s.%s (clientID text, methodName text, logContent text, insertionDate date,insertionTime time, primary key (clientID, methodName, insertionDate, insertionTime));", _databaseName, tableName);
   OCassStatement* statement = Ocass_statement_new(queryString, 0);
   OCassFuture* queryFuture = Ocass_session_execute(_session, statement);
   Ocass_statement_free(statement);
   if (Ocass_future_error_code(queryFuture) != 0)
      {
      /* Display connection error message */
      const char* message;
      size_t messageLength;
      Ocass_future_error_message(queryFuture, &message, &messageLength);
      fprintf(stderr, "Persistent Logging: Cassandra Database Table Creation Error: '%.*s'\n", (int)messageLength, message);
      
      Ocass_future_free(queryFuture);
      return false;
      }

   Ocass_future_free(queryFuture);
   return true;
   }
bool CassandraLogger::connect()
   {
   /* Setup and connect to cluster */
   _cluster = Ocass_cluster_new();
   _session = Ocass_session_new();
   /*authenticate using databaseUsername and databasePassword*/
   Ocass_cluster_set_credentials(_cluster, _databaseUsername, _databasePassword);
   /*Set protocol version */
   int rc_set_protocol = Ocass_cluster_set_protocol_version(_cluster, 4);
   if (rc_set_protocol != 0)
      {
      printf("Persistent Logging - Cassandra Database Error: %s\n",Ocass_error_desc(rc_set_protocol));
      Ocass_session_free(_session);
      Ocass_cluster_free(_cluster);
      return false;

      }

   /* Add contact points */
   int rc_set_ip = Ocass_cluster_set_contact_points(_cluster, _databaseIP);
   if (rc_set_ip != 0)
      {
      printf("Persistent Logging - Cassandra Database Error: %s\n",Ocass_error_desc(rc_set_ip));
      Ocass_session_free(_session);
      Ocass_cluster_free(_cluster);
      return false;

      }

   /*Set port number*/
   int rc_set_port = Ocass_cluster_set_port(_cluster, _databasePort);
   if (rc_set_port != 0)
      {
      printf("Persistent Logging - Cassandra Database Error: %s\n",Ocass_error_desc(rc_set_port));
      Ocass_session_free(_session);
      Ocass_cluster_free(_cluster);
      
      return false;

      }

   /* Provide the cluster object as configuration to connect the session */
   _connectFuture = Ocass_session_connect(_session, _cluster);

   if (Ocass_future_error_code(_connectFuture) != 0)
      {
      /* Display connection error message */
      const char* message;
      size_t messageLength;
      Ocass_future_error_message(_connectFuture, &message, &messageLength);
      fprintf(stderr, "Persistent Logging: Cassandra Database Connection Error: '%.*s'\n", (int)messageLength, message);
      Ocass_session_free(_session);
      Ocass_cluster_free(_cluster);
      Ocass_future_free(_connectFuture);
      return false;
      }
   return true;

   }


bool CassandraLogger::logMethod(const char *method, uint64_t clientID, const char *logContent)
   {
      
   // create table space and table first
   if (!createKeySpace()) return false;
   const char* tableName = "logs";
   if (!createTable(tableName)) return false;
   char queryString[1024];
   
   sprintf(queryString, "INSERT INTO %s.%s (clientID, methodName, logContent, insertionDate, insertionTime) VALUES (?, ?, ?, ?, ?)", _databaseName,tableName);
   OCassStatement* statement
   = Ocass_statement_new(queryString, 5);

   /* Bind the values using the indices of the bind variables */
   char strClientID[64];
   sprintf(strClientID, "%lu", clientID);
   int rc_set_bind_pk = Ocass_statement_bind_string(statement, 0, strClientID);
   if (rc_set_bind_pk != 0)
      {
      printf("Persistent Logging - Cassandra Database Error: %s\n",Ocass_error_desc(rc_set_bind_pk));
      Ocass_statement_free(statement);
      return false;
      }

   int rc_set_bind_method = Ocass_statement_bind_string(statement, 1, method);
   if (rc_set_bind_pk != 0)
      {
      printf("Persistent Logging - Cassandra Database Error: %s\n",Ocass_error_desc(rc_set_bind_method));
      Ocass_statement_free(statement);
      return false;
      }
   int rc_set_bind_log_content = Ocass_statement_bind_string(statement, 2, logContent);

   if (rc_set_bind_log_content != 0)
      {
      printf("Persistent Logging - Cassandra Database Error: %s\n",Ocass_error_desc(rc_set_bind_log_content));
      Ocass_statement_free(statement);
      return false;
      }

   time_t now = time(NULL); /* Time in seconds from Epoch */
   /* Converts the time since the Epoch in seconds to the 'date' type */
   Ocass_uint32_t year_month_day_of_insertion = Ocass_date_from_epoch(now);
   /* Converts the time since the Epoch in seconds to the 'time' type */
   Ocass_int64_t time_of_insertion = Ocass_time_from_epoch(now);

   /* 'date' uses an unsigned 32-bit integer */
   int rc_set_bind_insertion_date = Ocass_statement_bind_uint32(statement, 3, year_month_day_of_insertion);
   
   if (rc_set_bind_insertion_date != 0)
      {
      printf("Persistent Logging - Cassandra Database Error: %s\n", Ocass_error_desc(rc_set_bind_insertion_date));
      Ocass_statement_free(statement);
      return false;
      }
   /* 'time' uses a signed 64-bit integer */
   int rc_set_bind_insertion_time = Ocass_statement_bind_int64(statement, 4, time_of_insertion);
   if (rc_set_bind_insertion_time != 0)
      {
      printf("Persistent Logging - Cassandra Database Error: %s\n",Ocass_error_desc(rc_set_bind_insertion_time));
      Ocass_statement_free(statement);
      return false;
      }

   OCassFuture* queryFuture = Ocass_session_execute(_session, statement);

   /* Statement objects can be freed immediately after being executed */
   Ocass_statement_free(statement);
   if (Ocass_future_error_code(queryFuture) != 0)
      {
      /* Display connection error message */
      const char* message;
      size_t messageLength;
      Ocass_future_error_message(queryFuture, &message, &messageLength);
      fprintf(stderr, "query execution error: '%.*s'\n", (int)messageLength, message);
      Ocass_future_free(queryFuture);
      return false;
      }

   Ocass_future_free(queryFuture);
   return true; 
   }


void CassandraLogger::disconnect()
   {
   Ocass_future_free(_connectFuture);
   Ocass_session_free(_session);
   Ocass_cluster_free(_cluster);
   }









