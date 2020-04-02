//
// Created by cmbuhler on 2020-03-11.
//
#include "LoadDBLibs.hpp"

#include <dlfcn.h>
#include <cassandra.h>
#include <stdio.h>

#if defined(MONGO_LOGGER)
/*
 * MONGOC and BSON Library functions and strcuts
 */
Obson_new_t *Obson_new = NULL;
Obson_append_utf8_t *Obson_append_utf8 = NULL;
Obson_append_date_time_t *Obson_append_date_time = NULL;
Obson_destroy_t *Obson_destroy = NULL;

Omongoc_init_t *Omongoc_init = NULL;
Omongoc_cleanup_t *Omongoc_cleanup = NULL;
Omongoc_uri_new_with_error_t *Omongoc_uri_new_with_error = NULL;
Omongoc_client_new_from_uri_t *Omongoc_client_new_from_uri = NULL;
Omongoc_client_set_appname_t *Omongoc_client_set_appname = NULL;
Omongoc_client_get_database_t *Omongoc_client_get_database = NULL;
Omongoc_client_get_collection_t *Omongoc_client_get_collection = NULL;
Omongoc_collection_insert_one_t *Omongoc_collection_insert_one = NULL;
Omongoc_collection_destroy_t *Omongoc_collection_destroy = NULL;
Omongoc_database_destroy_t *Omongoc_database_destroy = NULL;
Omongoc_uri_destroy_t *Omongoc_uri_destroy = NULL;
Omongoc_client_destroy_t *Omongoc_client_destroy = NULL;
#endif // defined(MONGO_LOGGER)

#if defined(CASSANDRA_LOGGER)
/*
 * CASSANDRA functions and structs
 */
Ocass_statement_new_t *Ocass_statement_new = NULL;
Ocass_statement_free_t *Ocass_statement_free = NULL;
Ocass_session_execute_t *Ocass_session_execute = NULL;
Ocass_future_error_code_t *Ocass_future_error_code = NULL;
Ocass_future_error_message_t *Ocass_future_error_message = NULL;
Ocass_future_free_t *Ocass_future_free = NULL;
Ocass_cluster_new_t *Ocass_cluster_new = NULL;
Ocass_session_new_t *Ocass_session_new = NULL;
Ocass_cluster_set_credentials_t *Ocass_cluster_set_credentials = NULL;
Ocass_cluster_set_protocol_version_t *Ocass_cluster_set_protocol_version = NULL;
Ocass_session_free_t *Ocass_session_free = NULL;
Ocass_cluster_free_t *Ocass_cluster_free = NULL;
Ocass_cluster_set_contact_points_t *Ocass_cluster_set_contact_points = NULL;
Ocass_cluster_set_port_t *Ocass_cluster_set_port = NULL;
Ocass_session_connect_t *Ocass_session_connect = NULL;
Ocass_statement_bind_string_t *Ocass_statement_bind_string = NULL;
Ocass_error_desc_t *Ocass_error_desc = NULL;
Ocass_time_from_epoch_t *Ocass_time_from_epoch = NULL;
Ocass_date_from_epoch_t *Ocass_date_from_epoch = NULL;
Ocass_statement_bind_int64_t *Ocass_statement_bind_int64 = NULL;
Ocass_statement_bind_uint32_t *Ocass_statement_bind_uint32 = NULL;
#endif // defined(CASSANDRA_LOGGER)

namespace JITServer
   {
#if defined(MONGO_LOGGER)
      void *loadLibmongoc()
         {
         void *result = NULL;
         result = dlopen("libmongoc-1.0.so", RTLD_NOW);
         return result;
         }

      void *loadLibbson()
         {
         void *result = NULL;
         result = dlopen("libbson-1.0.so", RTLD_NOW);
         return result;
         }
#endif // defined(MONGO_LOGGER)
#if defined(CASSANDRA_LOGGER)
      void *loadLibcassandra()
         {
         void *result = NULL;
         result = dlopen("libcassandra.so", RTLD_NOW);
         return result;
         }
#endif //defined(CASSANDRA_LOGGER)

      void unloadDBLib(void *handle)
         {
         (void) dlclose(handle);
         }

      void *findDBLibSymbol(void *handle, const char *sym)
         {
         return dlsym(handle, sym);
         }

#if defined(MONGO_LOGGER)
      bool loadLibmongocAndSymbols()
         {
         void *handle = NULL;

         handle = loadLibmongoc();
         if (!handle)
            {
            printf("#JITServer: Failed to load libmongoc\n");
            return false;
            }

         Omongoc_init = (Omongoc_init_t *) findDBLibSymbol(handle, "mongoc_init");
         Omongoc_cleanup = (Omongoc_cleanup_t *) findDBLibSymbol(handle, "mongoc_cleanup");
         Omongoc_uri_new_with_error = (Omongoc_uri_new_with_error_t *) findDBLibSymbol(handle,
                                                                                       "mongoc_uri_new_with_error");
         Omongoc_client_new_from_uri = (Omongoc_client_new_from_uri_t *) findDBLibSymbol(handle,
                                                                                         "mongoc_client_new_from_uri");
         Omongoc_client_set_appname = (Omongoc_client_set_appname_t *) findDBLibSymbol(handle,
                                                                                       "mongoc_client_set_appname");
         Omongoc_client_get_database = (Omongoc_client_get_database_t *) findDBLibSymbol(handle,
                                                                                         "mongoc_client_get_database");
         Omongoc_client_get_collection = (Omongoc_client_get_collection_t *) findDBLibSymbol(handle,
                                                                                             "mongoc_client_get_collection");
         Omongoc_collection_insert_one = (Omongoc_collection_insert_one_t *) findDBLibSymbol(handle,
                                                                                             "mongoc_collection_insert_one");
         Omongoc_collection_destroy = (Omongoc_collection_destroy_t *) findDBLibSymbol(handle,
                                                                                       "mongoc_collection_destroy");
         Omongoc_database_destroy = (Omongoc_database_destroy_t *) findDBLibSymbol(handle, "mongoc_database_destroy");
         Omongoc_uri_destroy = (Omongoc_uri_destroy_t *) findDBLibSymbol(handle, "mongoc_uri_destroy");
         Omongoc_client_destroy = (Omongoc_client_destroy_t *) findDBLibSymbol(handle, "mongoc_client_destroy");

         if (
               (Omongoc_init == NULL) ||
               (Omongoc_cleanup == NULL) ||
               (Omongoc_uri_new_with_error == NULL) ||
               (Omongoc_client_new_from_uri == NULL) ||
               (Omongoc_client_set_appname == NULL) ||
               (Omongoc_client_get_database == NULL) ||
               (Omongoc_client_get_collection == NULL) ||
               (Omongoc_collection_insert_one == NULL) ||
               (Omongoc_collection_destroy == NULL) ||
               (Omongoc_database_destroy == NULL) ||
               (Omongoc_uri_destroy == NULL) ||
               (Omongoc_client_destroy == NULL)
               )
            {
            printf("#JITServer: Failed to load all the required Mongoc symbols\n");
            unloadDBLib(handle);
            return false;
            }

         return true;
         }

      bool loadLibbsonAndSymbols()
         {
         void *handle = NULL;

         handle = loadLibbson();
         if (!handle)
            {
            printf("#JITServer: Failed to load libbson\n");
            return false;
            }

         Obson_new = (Obson_new_t *) findDBLibSymbol(handle, "bson_new");
         Obson_append_utf8 = (Obson_append_utf8_t *) findDBLibSymbol(handle, "bson_append_utf8");
         Obson_append_date_time = (Obson_append_date_time_t *) findDBLibSymbol(handle, "bson_append_date_time");
         Obson_destroy = (Obson_destroy_t *) findDBLibSymbol(handle, "bson_destroy");

         if (
               (Obson_new == NULL) ||
               (Obson_append_utf8 == NULL) ||
               (Obson_append_date_time == NULL) ||
               (Obson_destroy == NULL)
               )
            {
            printf("#JITServer: Failed to load all the required bson symbols\n");
            unloadDBLib(handle);
            return false;
            }

         return true;
         }
#endif // defined(MONGO_LOGGER)

#if defined(CASSANDRA_LOGGER)
      bool loadLibcassandraAndSymbols()
         {
         void *handle = NULL;

         handle = loadLibcassandra();
         if (!handle)
            {
            printf("#JITServer: Failed to load libcassandra.\n");
            return false;
            }

         Ocass_statement_new = (Ocass_statement_new_t *) findDBLibSymbol(handle, "cass_statement_new");
         Ocass_statement_free = (Ocass_statement_free_t *) findDBLibSymbol(handle, "cass_statement_free");
         Ocass_session_execute = (Ocass_session_execute_t *) findDBLibSymbol(handle, "cass_session_execute");
         Ocass_future_error_code = (Ocass_future_error_code_t *) findDBLibSymbol(handle, "cass_future_error_code");
         Ocass_future_error_message = (Ocass_future_error_message_t *) findDBLibSymbol(handle, "cass_future_error_message");
         Ocass_future_free = (Ocass_future_free_t *) findDBLibSymbol(handle, "cass_future_free");
         Ocass_cluster_new = (Ocass_cluster_new_t *) findDBLibSymbol(handle, "cass_cluster_new");
         Ocass_session_new = (Ocass_session_new_t *) findDBLibSymbol(handle, "cass_session_new");
         Ocass_cluster_set_credentials = (Ocass_cluster_set_credentials_t *) findDBLibSymbol(handle, "cass_cluster_set_credentials");
         Ocass_cluster_set_protocol_version = (Ocass_cluster_set_protocol_version_t *) findDBLibSymbol(handle, "cass_cluster_set_protocol_version");
         Ocass_session_free = (Ocass_session_free_t *) findDBLibSymbol(handle, "cass_session_free");
         Ocass_cluster_free = (Ocass_cluster_free_t *) findDBLibSymbol(handle, "cass_cluster_free");
         Ocass_cluster_set_contact_points = (Ocass_cluster_set_contact_points_t *) findDBLibSymbol(handle, "cass_cluster_set_contact_points");
         Ocass_cluster_set_port = (Ocass_cluster_set_port_t *) findDBLibSymbol(handle, "cass_cluster_set_port");
         Ocass_session_connect = (Ocass_session_connect_t *) findDBLibSymbol(handle, "cass_session_connect");
         Ocass_statement_bind_string = (Ocass_statement_bind_string_t *) findDBLibSymbol(handle, "cass_statement_bind_string");
         Ocass_error_desc = (Ocass_error_desc_t *) findDBLibSymbol(handle, "cass_error_desc");
         Ocass_time_from_epoch = (Ocass_time_from_epoch_t *) findDBLibSymbol(handle, "cass_time_from_epoch");
         Ocass_date_from_epoch = (Ocass_date_from_epoch_t *) findDBLibSymbol(handle, "cass_date_from_epoch");
         Ocass_statement_bind_int64 = (Ocass_statement_bind_int64_t *) findDBLibSymbol(handle, "cass_statement_bind_int64");
         Ocass_statement_bind_uint32 = (Ocass_statement_bind_uint32_t *)findDBLibSymbol(handle, "cass_statement_bind_uint32");


         if ((Ocass_statement_free == NULL ) ||
               (Ocass_statement_new == NULL) ||
               (Ocass_session_execute == NULL) ||
               (Ocass_future_error_code == NULL) ||
               (Ocass_future_error_message == NULL) ||
               (Ocass_future_free == NULL) ||
               (Ocass_cluster_new == NULL) ||
               (Ocass_session_new == NULL) ||
               (Ocass_cluster_set_credentials == NULL) ||
               (Ocass_cluster_set_protocol_version == NULL) ||
               (Ocass_session_free == NULL) ||
               (Ocass_cluster_free == NULL) ||
               (Ocass_cluster_set_contact_points == NULL) ||
               (Ocass_cluster_set_port == NULL) ||
               (Ocass_session_connect == NULL) ||
               (Ocass_statement_bind_string == NULL) ||
               (Ocass_error_desc == NULL) ||
               (Ocass_time_from_epoch == NULL) ||
               (Ocass_date_from_epoch == NULL) ||
               (Ocass_statement_bind_int64 == NULL) ||
               (Ocass_statement_bind_uint32 == NULL)
               )
            {
            printf("#JITServer: Failed to load all the required cassandra symbols\n");
            unloadDBLib(handle);
            return false;
            }
         return true;
         }
#endif // defined(CASSANDRA_LOGGER);
   }
