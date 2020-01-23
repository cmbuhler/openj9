//
// Created by cmbuhler on 2020-03-11.
//

#ifndef JITSERVER_LOADDBLIBS_HPP
#define JITSERVER_LOADDBLIBS_HPP

#include <stdint.h>

/*
 * libbson Function Pointers and typedefs
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


namespace JITServer
   {
      static bool is_mongoc_init = 0;

      void *loadLibmongoc();

      void *loadLibbson();

      void unloadDBLib(void *handle);

      void *findDBLibSymbol(void *handle, const char *sym);

      bool loadLibmongocAndSymbols();

      bool loadLibbsonAndSymbols();
   }
#endif //JITSERVER_LOADDBLIBS_HPP
