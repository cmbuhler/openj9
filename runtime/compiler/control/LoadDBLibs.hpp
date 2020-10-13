//
// Created by cmbuhler on 2020-03-11.
//

#ifndef JITSERVER_LOADDBLIBS_HPP
#define JITSERVER_LOADDBLIBS_HPP

#include <stdint.h>
#include <stdlib.h>

#if defined(MONGO_LOGGER)
/*
 * libbson function pointers and typedefs
 */
typedef struct Obson_t Obson_t;

typedef struct
   {
   uint32_t domain;
   uint32_t code;
   char message[504];
   } Obson_error_t;

typedef Obson_t *Obson_new_t(void);

typedef bool
      Obson_append_utf8_t(Obson_t *bson_client,
                          const char *key,
                          int key_length,
                          const char *value,
                          int length);

typedef bool
      Obson_append_date_time_t(Obson_t *bson,
                               const char *key,
                               int key_length,
                               int64_t value);

typedef void
      Obson_destroy_t(Obson_t *bson);

/*
 * libmongoc Function Pointers and typedefs
 */
typedef void Omongoc_init_t(void);

typedef void Omongoc_cleanup_t(void);

typedef struct Omongoc_uri_t Omongoc_uri_t;

typedef struct Omongoc_client_t Omongoc_client_t;

typedef struct Omongoc_database_t Omongoc_database_t;

typedef struct Omongoc_collection_t Omongoc_collection_t;

typedef Omongoc_uri_t *
      Omongoc_uri_new_with_error_t(const char *uri_string,
                                   Obson_error_t *error);

typedef Omongoc_client_t *
      Omongoc_client_new_from_uri_t(const Omongoc_uri_t *uri);

typedef bool
      Omongoc_client_set_appname_t(const Omongoc_client_t *client,
                                   const char *name);

typedef Omongoc_database_t *
      Omongoc_client_get_database_t(const Omongoc_client_t *client,
                                    const char *database_name);

typedef Omongoc_collection_t *
      Omongoc_client_get_collection_t(const Omongoc_client_t *client,
                                      const char *database_name,
                                      const char *collection_name);

typedef bool
      Omongoc_collection_insert_one_t(Omongoc_collection_t *collection,
                                      const Obson_t *document,
                                      const Obson_t *opts,
                                      Obson_t *reply,
                                      Obson_error_t *error);

typedef void Omongoc_collection_destroy_t(Omongoc_collection_t *collection);

typedef void Omongoc_database_destroy_t(Omongoc_database_t *database);

typedef void Omongoc_uri_destroy_t(Omongoc_uri_t *uri);

typedef void Omongoc_client_destroy_t(Omongoc_client_t *client);
#endif // defined(MONGO_LOGGER)

#if defined(CASSANDRA_LOGGER)
/*
 * libcassandra function pointers and typedefs
 */
#define OCASS_OK 0;
typedef int64_t Ocass_int64_t;

typedef uint32_t Ocass_uint32_t;

typedef struct OCassCluster OCassCluster;

typedef struct OCassSession OCassSession;

typedef struct OCassStatement OCassStatement;

typedef struct OCassFuture OCassFuture;

typedef int OCassError;

typedef OCassStatement *
      Ocass_statement_new_t(const char *query, size_t parameter_count);

typedef void Ocass_statement_free_t(OCassStatement *statement);

typedef OCassFuture * Ocass_session_execute_t(OCassSession *session, OCassStatement * statement);

typedef int Ocass_future_error_code_t(OCassFuture *future);

typedef void Ocass_future_error_message_t(OCassFuture * future, const char ** message, size_t * message_length);

typedef void Ocass_future_free_t(OCassFuture *future);

typedef OCassCluster * Ocass_cluster_new_t();

typedef OCassSession * Ocass_session_new_t();

typedef void Ocass_cluster_set_credentials_t(OCassCluster * cluster, const char * username, const char * password);

typedef int Ocass_cluster_set_protocol_version_t(OCassCluster *cluster, int version);

typedef void Ocass_session_free_t(OCassSession *session);

typedef void Ocass_cluster_free_t(OCassCluster *cluster);

typedef int Ocass_cluster_set_contact_points_t(OCassCluster *cluster, const char * contact_points);

typedef int Ocass_cluster_set_port_t(OCassCluster *cluster, int port);

typedef OCassFuture * Ocass_session_connect_t(OCassSession *session, OCassCluster *cluster);

typedef int Ocass_statement_bind_string_t(OCassStatement *statement, size_t index, const char * value);

typedef const char * Ocass_error_desc_t(int error);

typedef Ocass_int64_t Ocass_time_from_epoch_t(Ocass_int64_t epoch_secs);

typedef Ocass_uint32_t Ocass_date_from_epoch_t(Ocass_int64_t epoch_secs);

typedef int Ocass_statement_bind_uint32_t(OCassStatement *statement, size_t index, Ocass_uint32_t value);

typedef int Ocass_statement_bind_int64_t(OCassStatement *statement, size_t index, Ocass_int64_t value);
#endif // defined(CASSANDRA_LOGGER)

/*
 * Function pointer definitions.
 */
#if defined(MONGO_LOGGER)
// mongoc and bson:
extern "C" Obson_new_t *Obson_new;
extern "C" Obson_append_utf8_t *Obson_append_utf8;
extern "C" Obson_append_date_time_t *Obson_append_date_time;
extern "C" Obson_destroy_t *Obson_destroy;

extern "C" Omongoc_init_t *Omongoc_init;
extern "C" Omongoc_cleanup_t *Omongoc_cleanup;
extern "C" Omongoc_uri_new_with_error_t *Omongoc_uri_new_with_error;
extern "C" Omongoc_client_new_from_uri_t *Omongoc_client_new_from_uri;
extern "C" Omongoc_client_set_appname_t *Omongoc_client_set_appname;
extern "C" Omongoc_client_get_database_t *Omongoc_client_get_database;
extern "C" Omongoc_client_get_collection_t *Omongoc_client_get_collection;
extern "C" Omongoc_collection_insert_one_t *Omongoc_collection_insert_one;
extern "C" Omongoc_collection_destroy_t *Omongoc_collection_destroy;
extern "C" Omongoc_database_destroy_t *Omongoc_database_destroy;
extern "C" Omongoc_uri_destroy_t *Omongoc_uri_destroy;
extern "C" Omongoc_client_destroy_t *Omongoc_client_destroy;
#endif // defined(MONGO_LOGGER)
#if defined(CASSANDRA_LOGGER)
// Cassandra:
extern "C" Ocass_statement_new_t *Ocass_statement_new;
extern "C" Ocass_statement_free_t *Ocass_statement_free;
extern "C" Ocass_session_execute_t *Ocass_session_execute;
extern "C" Ocass_future_error_code_t *Ocass_future_error_code;
extern "C" Ocass_future_error_message_t *Ocass_future_error_message;
extern "C" Ocass_future_free_t *Ocass_future_free;
extern "C" Ocass_cluster_new_t *Ocass_cluster_new;
extern "C" Ocass_session_new_t *Ocass_session_new;
extern "C" Ocass_cluster_set_credentials_t *Ocass_cluster_set_credentials;
extern "C" Ocass_cluster_set_protocol_version_t *Ocass_cluster_set_protocol_version;
extern "C" Ocass_session_free_t *Ocass_session_free;
extern "C" Ocass_cluster_free_t *Ocass_cluster_free;
extern "C" Ocass_cluster_set_contact_points_t *Ocass_cluster_set_contact_points;
extern "C" Ocass_cluster_set_port_t *Ocass_cluster_set_port;
extern "C" Ocass_session_connect_t *Ocass_session_connect;
extern "C" Ocass_statement_bind_string_t *Ocass_statement_bind_string;
extern "C" Ocass_error_desc_t *Ocass_error_desc;
extern "C" Ocass_time_from_epoch_t *Ocass_time_from_epoch;
extern "C" Ocass_date_from_epoch_t *Ocass_date_from_epoch;
extern "C" Ocass_statement_bind_int64_t *Ocass_statement_bind_int64;
extern "C" Ocass_statement_bind_uint32_t *Ocass_statement_bind_uint32;
#endif // defined(CASSANDRA_LOGGER)
namespace JITServer
   {
#if defined(MONGO_LOGGER)
      static bool is_mongoc_init = 0;

      inline bool isMongoCInit() { return is_mongoc_init; }

      inline void initMongoC() { Omongoc_init(); is_mongoc_init = 1; }

      inline void cleanupMongoC() { Omongoc_cleanup(); is_mongoc_init = 0; }

      void *loadLibmongoc();

      void *loadLibbson();
#endif // defined(MONGO_LOGGER)
#if defined(CASSANDRA_LOGGER)
      void *loadLibcassandra();
#endif //defined(CASSANDRA_LOGGER)
      void unloadDBLib(void *handle);

      void *findDBLibSymbol(void *handle, const char *sym);
#if defined(MONGO_LOGGER)
      bool loadLibmongocAndSymbols();

      bool loadLibbsonAndSymbols();
#endif // defined(MONGO_LOGGER);
#if defined(CASSANDRA_LOGGER)
      bool loadLibcassandraAndSymbols();
#endif // defined(CASSANDRA_LOGGER)
   }
#endif //JITSERVER_LOADDBLIBS_HPP
