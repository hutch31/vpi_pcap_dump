//----------------------------------------------------------------------
//  Packet Queue PLI Interface
//
//  Creates an instance of the packet queue class and the five
//  functions that will be exported to the simulator.  Note that
//  functions exported to Verilog need to be defined as extern "C".
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
#include <vector>
#include <stdint.h>
#include <assert.h>
using std::vector;

extern "C" {
#include "vpi_user.h"
#include "cv_vpi_user.h"
  //#include "vpi_utils.h"
}

// create single global packet queue for simulation
pktqueue pq;

// create __dso_handle symbol to make linker happy.  Exact usage of
// this symbol is FM to me, but apparently needs to point to an
// address of something somewhere in the share library.  Probably
// not an issue since this lib doesn't get unloaded until end of
// simulation.
void *__dso_handle = &pq;

void get_args (vector<vpiHandle> &args, int expected) {
  vpiHandle call_h, iref_h, arg_h;
   // get handles to argument list
 
  call_h = vpi_handle(vpiSysTfCall, NULL);
  assert ((iref_h = vpi_iterate(vpiArgument, call_h)) != NULL);

  while ((arg_h = vpi_scan(iref_h)) != NULL) {
    args.push_back (arg_h);
  }

  if ((expected != -1) && (args.size() != expected)) {
    vpi_printf ("PLI ERROR: expected %d arguments, got %d\n",
		expected, args.size());
  }
}

// $pq_request_queue (out qid)
extern "C" PLI_INT32 pq_request_queue_call ()
{
  vector<vpiHandle> args;
  s_vpi_value arg_info;

  get_args (args, 1);
  arg_info.format = vpiIntVal;
  arg_info.value.scalar = pq.request_queue();
  vpi_put_value (args[0], &arg_info, NULL, vpiNoDelay);
  //v_error_check();
}

// $pq_num_queues (out num_queues)
extern "C" PLI_INT32 pq_num_queues_call ()
{
  vector<vpiHandle> args;
  s_vpi_value arg_info;

  get_args (args, 1);
  arg_info.format = vpiIntVal;
  arg_info.value.scalar = pq.num_queues();
  vpi_put_value (args[0], &arg_info, NULL, vpiNoDelay);
  //v_error_check();
}

// $pq_insert_packet (qid, len, buffer)
extern "C" PLI_INT32 pq_insert_packet_call ()
{
  vpiHandle arg_h;
  vector<vpiHandle> args;
  s_vpi_value arg_info;
  int qid, len;
  packet_t pkt;

  get_args (args, 3);

  arg_info.format = vpiIntVal;
  vpi_get_value (args[0], &arg_info);
  qid = arg_info.value.scalar;

  arg_info.format = vpiIntVal;
  vpi_get_value (args[1], &arg_info);
  len = arg_info.value.scalar;

  //check_memory_element (args[2], len);

  pkt.resize (len);
  for (int i=0; i<len; i++) {
    arg_h = vpi_handle_by_index (args[2], i);
    vpi_get_value (arg_h, &arg_info);
    pkt[i] = (uint8_t) arg_info.value.scalar;
  }

  pq.insert_packet (qid, pkt);
  //v_error_check();
}

// $pq_queue_size (qid, out size)
extern "C" PLI_INT32 pq_queue_size_call ()
{
  vector<vpiHandle> args;
  s_vpi_value arg_info;
  int qid, size;

  get_args (args, 2);

  arg_info.format = vpiIntVal;
  vpi_get_value (args[0], &arg_info);
  qid = arg_info.value.scalar;

  size = pq.queue_size (qid);

  arg_info.value.scalar = size;
  vpi_put_value (args[1], &arg_info, NULL, vpiNoDelay);
}

// $pq_remove_packet (qid, out len, out buffer)
extern "C" PLI_INT32 pq_remove_packet_call ()
{
  vector<vpiHandle> args;
  vpiHandle element;
  s_vpi_value arg_info;
  int qid, size;
  packet_i pi;

  get_args (args, 3);

  arg_info.format = vpiIntVal;
  vpi_get_value (args[0], &arg_info);
  qid = arg_info.value.scalar;

  size = pq.front_size (qid);
  arg_info.value.scalar = size;
  vpi_put_value (args[1], &arg_info, NULL, vpiNoDelay);

  // do some sanity checks
  if (qid >= pq.num_queues()) {
    vpi_printf ("ERROR   : qid %d out of range (%d)\n", qid, pq.num_queues());
    return 0;
  } else if (pq.queue_size(qid) == 0) {
    vpi_printf ("WARNING : requested queue %d is empty\n", qid);
    return 0;
  }

  //check_memory_element (args[2], size);

  pi = pq.front_iter (qid);
  arg_info.format = vpiIntVal;
  for (int i=0; i<size; i++) {
    element = vpi_handle_by_index (args[2], i);
    arg_info.value.scalar = (unsigned int) *pi; pi++;
    vpi_put_value (element, &arg_info, NULL, vpiNoDelay);
    //v_error_check();
  } 

  pq.remove_packet (qid);
}

// Register VPI routines with the simulator
extern "C" void pqpli_register(void)
{
 p_vpi_systf_data systf_data_p;

 /* use predefined table form - could fill systf_data_list dynamically */
 static s_vpi_systf_data systf_data_list[] = {

  { vpiSysTask, 0, "$pq_request_queue", pq_request_queue_call, NULL, NULL, NULL },
  { vpiSysTask, 0, "$pq_num_queues", pq_num_queues_call, NULL, NULL, NULL },
  { vpiSysTask, 0, "$pq_insert_packet", pq_insert_packet_call, NULL, NULL, NULL },
  { vpiSysTask, 0, "$pq_queue_size", pq_queue_size_call, NULL, NULL, NULL },
  { vpiSysTask, 0, "$pq_remove_packet", pq_remove_packet_call, NULL, NULL, NULL },

  { 0, 0, NULL, NULL, NULL, NULL, NULL }
 };

 systf_data_p = &(systf_data_list[0]);
 while (systf_data_p->type != 0) vpi_register_systf(systf_data_p++);
}
