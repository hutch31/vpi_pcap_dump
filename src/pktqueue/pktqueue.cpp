//----------------------------------------------------------------------
//  Packet Queue Class Methods
//----------------------------------------------------------------------

/* Copyright (c) 2011, Guy Hutchison
   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "pktqueue.h"
#include <assert.h>

/*
class pktqueue {
 public:
  uint32_t request_queue ();
  int insert_packet (uint32_t queue_id, packet_t pkt);
  int queue_size (uint32_t queue_id);
  pq_entry_t remove_packet (uint32_t queue_id);
 private:
  queue_list_t queue_list;
};
*/

uint32_t pktqueue::request_queue ()
{
  queue_list.resize(++nxt_queue);
  return (nxt_queue-1);
}

int pktqueue::insert_packet (uint32_t queue_id, packet_t &pkt)
{
  if ((queue_id >= 0) && (queue_id < queue_list.size())) {
    queue_list[queue_id].push_back (pkt);
    return 1;
  } else return 0;
}

int pktqueue::queue_size (uint32_t queue_id) {
   if ((queue_id >= 0) && (queue_id < queue_list.size())) {
     return queue_list[queue_id].size();
   } else return -1;
}

int pktqueue::remove_packet (uint32_t queue_id) 
{
   if ((queue_id >= 0) && (queue_id < queue_list.size())) {
     if (!queue_list[queue_id].empty()) {
       queue_list[queue_id].front().clear();
       queue_list[queue_id].pop_front();
       return 1;
     } else return 0;
   } else return 0;
}


packet_i pktqueue::front_iter (uint32_t queue_id)
{
  assert ((queue_id >= 0) && (queue_id < queue_list.size()));
  return queue_list[queue_id].front().begin();
}

int pktqueue::front_size (uint32_t queue_id)
{
  if ((queue_id >= 0) && (queue_id < queue_list.size()))
    return queue_list[queue_id].front().size();
  else
    return -1;
}
