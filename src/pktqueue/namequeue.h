/*! \file namequeue.h
 * Packet Queue Class Definition
 */
 
/* Copyright (c) 2011, Guy Hutchison
   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _NAMEQUEUE_H
#define _NAMEQUEUE_H

#include <vector>
#include <deque>
#include <map>
#include <stdint.h>
#include <string>
using std::vector;
using std::deque;
using std::map;
using std::string;

//typedef pair<string,int> namePair;

typedef vector<uint8_t> packet_t;
typedef packet_t::iterator packet_i;

typedef deque<packet_t> packet_queue_t;
typedef vector<packet_queue_t> queue_list_t;

/**
 * @brief Name-based packet queue
 */
class namequeue {
 private:
  queue_list_t queue_list;
  map<string,packet_queue_t *> qmap;
  //int nxt_queue;
  void checkName (string name);
 public:
  /// Insert a packet in the named queue
  /// If the named queue does not exist, it will be created prior to
  /// inserting the packet.
  /// @param name Name of queue to insert
  /// @param pkt  Packet vector to insert
  int insert_packet (string name, packet_t &pkt);
  /// Return length of the named queue in number of packets
  /// @param name Name of queue
  int queue_size (string name);

  /// Check queue empty
  /// @param name Name of queue
  /// @returns True if queue is empty, false otherwise
  bool queue_empty (string name);
  /// Return a packet iterator to the packet at the front of the queue.
  /// This call should be qualified by queue_size() or queue_empty() calls
  /// prior to calling.
  /// @param name Name of queue
  packet_i front_iter (string name);
  int front_size (string name);
  int remove_packet (string name);
};

#endif
