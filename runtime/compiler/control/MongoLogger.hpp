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

#ifndef MONGOLOGGER_HPP
#define MONGOLOGGER_HPP

#include "BasePersistentLogger.hpp"
#include "LoadDBLibs.hpp"

class MongoLogger : public BasePersistentLogger
   {
private:
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

   bool logMethod(const char* method, uint64_t clientID, const char* logContent, J9JITConfig* j9JitConfig) override;
   };

#endif //MONGOLOGGER_HPP
