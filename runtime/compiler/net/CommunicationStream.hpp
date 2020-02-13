/*******************************************************************************
 * Copyright (c) 2018, 2020 IBM Corp. and others
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

#ifndef COMMUNICATION_STREAM_H
#define COMMUNICATION_STREAM_H

#include <google/protobuf/io/zero_copy_stream_impl.h> // for ZeroCopyInputStream
#include "net/ProtobufTypeConvert.hpp"
#include "net/SSLProtobufStream.hpp"
#include "env/TRMemory.hpp"

namespace JITServer
{
enum JITServerCompatibilityFlags
   {
   JITServerJavaVersionMask    = 0x00000FFF,
   JITServerCompressedRef      = 0x00001000,
   };
// list of features that client and server must match in order for remote compilations to work

using namespace google::protobuf::io;

class CommunicationStream
   {
public:
   static bool useSSL();
   static void initSSL();

   static void initVersion();

   static uint64_t getJITServerVersion()
      {
      return ((((uint64_t)CONFIGURATION_FLAGS) << 32) | (MAJOR_NUMBER << 24) | (MINOR_NUMBER << 8));
      }

protected:
   CommunicationStream()
      : _inputStream(NULL),
      _outputStream(NULL),
      _ssl(NULL),
      _sslInputStream(NULL),
      _sslOutputStream(NULL),
      _connfd(-1)
      {
      // set everything to NULL, in case the child stream fails to call initStream
      // which initializes these variables
      }

   void initStream(int connfd, BIO *ssl)
   {
      _connfd = connfd;
      _ssl = ssl;
      if (_ssl)
         {
         _sslInputStream = new (PERSISTENT_NEW) SSLInputStream(_ssl);
         _sslOutputStream = new (PERSISTENT_NEW) SSLOutputStream(_ssl);
         _inputStream = new (PERSISTENT_NEW) CopyingInputStreamAdaptor(_sslInputStream);
         _outputStream = new (PERSISTENT_NEW) CopyingOutputStreamAdaptor(_sslOutputStream);
         }
      else
         {
         _inputStream = new (PERSISTENT_NEW) FileInputStream(_connfd);
         _outputStream = new (PERSISTENT_NEW) FileOutputStream(_connfd);
         }
      }

   virtual ~CommunicationStream()
      {
      if (_inputStream)
         {
         _inputStream->~ZeroCopyInputStream();
         TR_Memory::jitPersistentFree(_inputStream);
         }
      if (_outputStream)
         {
         _outputStream->~ZeroCopyOutputStream();
         TR_Memory::jitPersistentFree(_outputStream);
         }
      if (_ssl)
         {
         _sslInputStream->~SSLInputStream();
         TR_Memory::jitPersistentFree(_sslInputStream);
         _sslOutputStream->~SSLOutputStream();
         TR_Memory::jitPersistentFree(_sslOutputStream);
         (*OBIO_free_all)(_ssl);
         }
      if (_connfd != -1)
         {
         close(_connfd);
         _connfd = -1;
         }
      }

   template <typename T>
   void readBlocking(T &val)
      {
      val.mutable_data()->clear_data();
      CodedInputStream codedInputStream(_inputStream);
      uint32_t messageSize;
      if (!codedInputStream.ReadLittleEndian32(&messageSize))
         throw JITServer::StreamFailure("JITServer I/O error: reading message size");
      auto limit = codedInputStream.PushLimit(messageSize);
      if (!val.ParseFromCodedStream(&codedInputStream))
         throw JITServer::StreamFailure("JITServer I/O error: reading from stream");
      if (!codedInputStream.ConsumedEntireMessage())
         throw JITServer::StreamFailure("JITServer I/O error: did not receive entire message");
      codedInputStream.PopLimit(limit);
      }
   template <typename T>
   void writeBlocking(const T &val)
      {
         {
         CodedOutputStream codedOutputStream(_outputStream);
         size_t messageSize = val.ByteSizeLong();
         TR_ASSERT(messageSize < 1ul<<32, "message size too big");
         codedOutputStream.WriteLittleEndian32(messageSize);
         val.SerializeWithCachedSizes(&codedOutputStream);
         if (codedOutputStream.HadError())
            throw JITServer::StreamFailure("JITServer I/O error: writing to stream");
         // codedOutputStream must be dropped before calling flush
         }
      if (_ssl ? !((CopyingOutputStreamAdaptor*)_outputStream)->Flush()
               : !((FileOutputStream*)_outputStream)->Flush())
         {
         throw JITServer::StreamFailure("JITServer I/O error: flushing stream: GetErrno " + std::to_string(((FileOutputStream*)_outputStream)->GetErrno()));
         }
      }

   int _connfd; // connection file descriptor

   // re-usable message objects
   J9ServerMessage _sMsg;
   J9ClientMessage _cMsg;

   BIO *_ssl; // SSL connection, null if not using SSL
   SSLInputStream *_sslInputStream;
   SSLOutputStream *_sslOutputStream;
   ZeroCopyInputStream *_inputStream;
   ZeroCopyOutputStream *_outputStream;

   static const uint8_t MAJOR_NUMBER = 0;
   static const uint16_t MINOR_NUMBER = 4;
   static const uint8_t PATCH_NUMBER = 0;
   static uint32_t CONFIGURATION_FLAGS;
   };

};

#endif // COMMUNICATION_STREAM_H
