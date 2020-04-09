/*******************************************************************************
 * Copyright (c) 2020, 2020 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include "MongoLogger.hpp"
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include "LoadDBLibs.hpp"

MongoLogger::MongoLogger(const char* databaseIP, uint32_t databasePort, const char* databaseName)
      : BasePersistentLogger(databaseIP, databasePort, databaseName)
   {
   init();
   }

MongoLogger::MongoLogger(const char* databaseIP, uint32_t databasePort, const char* databaseName,
                         const char* databaseUsername, const char* databasePassword)
      : BasePersistentLogger(databaseIP, databasePort, databaseName, databaseUsername, databasePassword)
   {
   init();
   }

MongoLogger::~MongoLogger()
   {
   //Clean up this mongoc logger.
   Omongoc_collection_destroy(_collection);
   Omongoc_database_destroy(_db);
   Omongoc_client_destroy(_client);
   Omongoc_uri_destroy(_uri);
   }

void MongoLogger::init()
   {
   _uri = NULL;
   _collection = NULL;
   _db = NULL;
   _client = NULL;
   }

char * MongoLogger::constructURI()
   {
   // Check if we have the database name
   if (strcmp(_databaseName,"") == 0)
      {
      return "jitserver_logs";
      }

   // Check if we have db IP and Port
   if (strcmp(_databaseIP,"") == 0)
      {
      // No IP try localhost
      _databaseIP = "127.0.0.1";
      }
   if (!_databasePort)
      {
      // No Port try default MongoDB port
      _databasePort = 27017;
      }

   // Check if we have credentials
   if (strcmp(_databaseUsername,"") != 0)
      {
      if (strcmp(_databasePassword,"") != 0)
         {
         snprintf(_uri_string, 256, "mongodb://%s:%s@%s:%u/?authSource=%s", _databaseUsername, _databasePassword,
               _databaseIP, _databasePort, _databaseName);
         }
      else
         {
         snprintf(_uri_string, 256, "mongodb://%s@%s:%u/?authSource=%s", _databaseUsername, _databaseIP, _databasePort,
               _databaseName);
         }
      }
   else
      {
      snprintf(_uri_string, 256, "mongodb://%s:%u/?authSource=%s", _databaseIP, _databasePort, _databaseName);
      }

   return _uri_string;
   }

bool MongoLogger::connect()
   {
   Obson_error_t error;

   //Validate URI.
   _uri = Omongoc_uri_new_with_error(constructURI(), &error);
   if (!_uri)
      {
      fprintf (stderr,
                  "JITServer: Persistent Logger failed to parse URI: %s\n"\
                  "error message: %s\n",
                  constructURI(),
                  error.message);
      return false;
      }

   //Create a client
   _client = Omongoc_client_new_from_uri(_uri);
   if(!_client)
      {
      Omongoc_uri_destroy(_uri);
      fprintf (stderr,
               "JITServer: Persistent Logger failed to create client.\n"\
               "error message: %s\n",
               error.message);
      return false;
      }

   //Register the application name so we can track it in the profile logs
   //on the server if we want.
   Omongoc_client_set_appname(_client, "jitserver");

   //Get a handle on the database and collection.
   _db = Omongoc_client_get_database(_client, _databaseName);
   _collection = Omongoc_client_get_collection (_client, _databaseName, "logs");

   //Mongo is designed to be always available. Thus there is no "Connection" object
   //and you will find that the "Connection" is tested on every read/write.
   return true;
   }

void MongoLogger::disconnect()
   {
   //Does not actually do anything for Mongo as noted in connect()
   return;
   }

bool MongoLogger::logMethod(const char* method, uint64_t clientID, const char* logContent, J9JITConfig* j9JitConfig)
   {
   struct timespec t;
   clock_gettime(CLOCK_REALTIME, &t);
   int64_t timestamp = t.tv_sec * INT64_C(1000) + t.tv_nsec / 1000000;
   char clientIDstr[20];
   snprintf(clientIDstr, 20, "%llu", clientID);
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
   Obson_append_utf8(insert, "method", -1, method, -1);
   Obson_append_utf8(insert, "client_id", -1, clientIDstr, -1);
   Obson_append_utf8(insert, "log", -1, logContent, -1);
   Obson_append_date_time(insert, "timestamp", -1, timestamp);

   if (!Omongoc_collection_insert_one(_collection, insert, NULL, NULL, &error))
      {
      fprintf(stderr, "JITServer: Mongo Logger failed to insert log.\n"
                      "error message: %s\n", error.message);
      }

   Obson_destroy(insert);

   return true;
   }

