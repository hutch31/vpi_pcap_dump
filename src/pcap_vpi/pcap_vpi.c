/* Copyright (c) 2011, Guy Hutchison
   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "pcap_vpi.h"
#include "pcap_dump.h"
#include <assert.h>

#define MAX_OPEN_PCAP 32
#define PCAP_BUFSIZE 2048

pcap_handle_t pcap_handle[MAX_OPEN_PCAP];
static int pcap_used;

vpiHandle *get_args (int expected) {
  vpiHandle call_h, iref_h, arg_h;
  vpiHandle *alist;
  int hindex = 0;
  
  // allocate storage for handle list
  alist = (vpiHandle *) malloc (sizeof(vpiHandle) * expected);
  
   // get handles to argument list
 
  call_h = vpi_handle(vpiSysTfCall, NULL);
  assert ((iref_h = vpi_iterate(vpiArgument, call_h)) != NULL);

  while ((arg_h = vpi_scan(iref_h)) != NULL) {
    //args.push_back (arg_h);
    alist[hindex++] = arg_h;
  }

  if ((expected != -1) && (hindex != expected)) {
    vpi_printf ("PLI ERROR: expected %d argument MAX_OPEN_PCAP);s, got %d\n",
		expected, hindex);
  }
  return alist;
}

// $pv_open (phandle, filename);
void pv_open () {
  vpiHandle *args;
  int phandle;
  char *filename;
  
  phandle = pcap_used++;
  
  assert (phandle <  MAX_OPEN_PCAP);
  sprintf (filename, "pvdump%03d.pcap", phandle);

  pcap_handle[phandle] = pcap_open (filename, PCAP_BUFSIZE);
}

// $pv_dump_packet (phandle, len, pkt);
void pv_dump_packet () {
  vpiHandle *args;
  s_vpi_value arg_info;
  vpiHandle arg_h;
  int phandle, len;
  packet_info_t p;
  int i;
  
  args = get_args (3);

  arg_info.format = vpiIntVal;
  vpi_get_value (args[0], &arg_info);
  phandle = arg_info.value.scalar;

  arg_info.format = vpiIntVal;
  vpi_get_value (args[1], &arg_info);
  len = arg_info.value.scalar;

  p.pdata = (uint8_t *) malloc (len);
  for (i=0; i<len; i++) {
    arg_h = vpi_handle_by_index (args[2], i);
    vpi_get_value (arg_h, &arg_info);
    p.pdata[i] = (uint8_t) arg_info.value.scalar;
  }
  
  // fill in remainder of p struct
  p.length = len;
  p.sec = 0;
  p.usec = 0;

  assert (phandle < MAX_OPEN_PCAP);
  pcap_add_pkt (pcap_handle[phandle].dump, &p);
  //pq.insert_packet (qid, pkt);
  //v_error_check();  
}

void pv_shutdown () {
}

// Register VPI routines with the simulator
extern void pv_register(void)
{
 p_vpi_systf_data systf_data_p;

 pcap_used = 0;
 
 /* use predefined table form - could fill systf_data_list dynamically */
 static s_vpi_systf_data systf_data_list[] = {

  { vpiSysTask, 0, "$pv_open", pv_open, NULL, NULL, NULL },
  { vpiSysTask, 0, "$pv_dump_packet", pv_dump_packet, NULL, NULL, NULL },
  { vpiSysTask, 0, "$pv_shutdown", pv_open, NULL, NULL, NULL },
  { 0, 0, NULL, NULL, NULL, NULL, NULL }
 };

 systf_data_p = &(systf_data_list[0]);
 while (systf_data_p->type != 0) vpi_register_systf(systf_data_p++);
}

// entry point for simulator
void (*vlog_startup_routines[]) () = { pv_register, 0 };

