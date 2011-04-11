//----------------------------------------------------------------------
//  Packet Queue PLI Register
//
//  Register routines with the simulator.  This registration stub
//  optimized for Icarus Verilog.
//----------------------------------------------------------------------

/* Copyright (c) 2011, Guy Hutchison
   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "pqpli.h"
#include "vpi_user.h"

// Register VPI routines with the simulator
void pqpli_register(void)
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
