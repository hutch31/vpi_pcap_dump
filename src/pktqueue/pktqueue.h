/*! \file pktqueue.h
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

#ifndef _PKTQUEUE_H
#define _PKTQUEUE_H

#include <vector>
#include <deque>
#include <map>
#include <stdint.h>
using std::vector;
using std::deque;
using std::map;

typedef vector<uint8_t> packet_t;
typedef packet_t::iterator packet_i;

typedef deque<packet_t> packet_queue_t;
typedef vector<packet_queue_t> queue_list_t;

class pktqueue {
 private:
  queue_list_t queue_list;
  int nxt_queue;
 public:
  pktqueue() { nxt_queue = 0; };
  uint32_t request_queue ();
  int num_queues () { return queue_list.size(); };
  int insert_packet (uint32_t queue_id, packet_t &pkt);
  int queue_size (uint32_t queue_id);
  packet_i front_iter (uint32_t queue_id);
  int front_size (uint32_t queue_id);
  int remove_packet (uint32_t queue_id);
};

#endif
