//
// Created by cmbuhler on 2020-03-11.
//
#include "LoadDBLibs.hpp"

#include <dlfcn.h>
#include <iostream>

Obson_new_t * Obson_new = NULL;
Obson_append_utf8_t * Obson_append_utf8 = NULL;
Obson_append_date_time_t * Obson_append_date_time = NULL;
Obson_destroy_t * Obson_destroy = NULL;

Omongoc_init_t * Omongoc_init = NULL;
Omongoc_cleanup_t * Omongoc_cleanup = NULL;
Omongoc_uri_new_with_error_t * Omongoc_uri_new_with_error = NULL;
Omongoc_client_new_from_uri_t * Omongoc_client_new_from_uri = NULL;
Omongoc_client_set_appname_t * Omongoc_client_set_appname = NULL;
Omongoc_client_get_database_t * Omongoc_client_get_database = NULL;
Omongoc_client_get_collection_t * Omongoc_client_get_collection = NULL;
Omongoc_collection_insert_one_t * Omongoc_collection_insert_one = NULL;
Omongoc_collection_destroy_t * Omongoc_collection_destroy = NULL;
Omongoc_database_destroy_t * Omongoc_database_destroy = NULL;
Omongoc_uri_destroy_t * Omongoc_uri_destroy = NULL;
Omongoc_client_destroy_t * Omongoc_client_destroy = NULL;

namespace JITServer
{
    void * loadLibmongoc()
    {
        void *result = NULL;
        result = dlopen("libmongoc-1.0.so", RTLD_NOW);
        return result;
    }

    void * loadLibbson()
    {
        void *result = NULL;
        result = dlopen("libbson-1.0.so", RTLD_NOW);
        return result;
    }

    void unloadDBLib(void * handle)
    {
        (void)dlclose(handle);
    }

    void * findDBLibSymbol(void * handle, const char * sym)
    {
        return dlsym(handle, sym);
    }

    bool loadLibmongocAndSymbols()
    {
        void *handle = NULL;

        handle = loadLibmongoc();
        if(!handle)
        {
            printf("#JITServer: Failed to load libmongoc\n");
            return false;
        }

        Omongoc_init = (Omongoc_init_t *)findDBLibSymbol(handle, "mongoc_init");
        Omongoc_cleanup = (Omongoc_cleanup_t *)findDBLibSymbol(handle, "mongoc_cleanup");
        Omongoc_uri_new_with_error = (Omongoc_uri_new_with_error_t *)findDBLibSymbol(handle, "mongoc_uri_new_with_error");
        Omongoc_client_new_from_uri = (Omongoc_client_new_from_uri_t *)findDBLibSymbol(handle, "mongoc_client_new_from_uri");
        Omongoc_client_set_appname = (Omongoc_client_set_appname_t *)findDBLibSymbol(handle, "mongoc_client_set_appname");
        Omongoc_client_get_database = (Omongoc_client_get_database_t *)findDBLibSymbol(handle, "mongoc_client_get_database");
        Omongoc_client_get_collection = (Omongoc_client_get_collection_t *)findDBLibSymbol(handle, "mongoc_client_get_collection");
        Omongoc_collection_insert_one = (Omongoc_collection_insert_one_t *)findDBLibSymbol(handle, "mongoc_collection_insert_one");
        Omongoc_collection_destroy = (Omongoc_collection_destroy_t *)findDBLibSymbol(handle, "mongoc_collection_destroy");
        Omongoc_database_destroy = (Omongoc_database_destroy_t *)findDBLibSymbol(handle, "mongoc_database_destroy");
        Omongoc_uri_destroy = (Omongoc_uri_destroy_t *)findDBLibSymbol(handle, "mongoc_uri_destroy");
        Omongoc_client_destroy = (Omongoc_client_destroy_t *)findDBLibSymbol(handle, "mongoc_client_destroy");

        if(
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
        if(!handle)
        {
            printf("#JITServer: Failed to load libbson\n");
            return false;
        }

        Obson_new = (Obson_new_t *)findDBLibSymbol(handle, "bson_new");
        Obson_append_utf8 = (Obson_append_utf8_t *)findDBLibSymbol(handle, "bson_append_utf8");
        Obson_append_date_time = (Obson_append_date_time_t *)findDBLibSymbol(handle, "bson_append_date_time");
        Obson_destroy = (Obson_destroy_t *)findDBLibSymbol(handle, "bson_destroy");

        if(
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
}