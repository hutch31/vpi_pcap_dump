/*! \file pcap_vpi.c
 * Contains the VPI routines used to allocate pcap dumpers, dump individual
 * packets to a dumpfile, and shutdown afterwards.  Default has a maximum
 * of 32 open dumpfiles for a simulation.
 */
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
#include "vpi_user.h"
#include "pcap_dump.h"
#include <assert.h>
#include <string.h>

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
    alist[hindex++] = arg_h;
  }

  if ((expected != -1) && (hindex != expected)) {
    vpi_printf ("PLI ERROR: expected %d arguments, got %d\n",
		expected, hindex);
  }
  return alist;
}

int inline getIntegerArgument (vpiHandle vh)
{
  s_vpi_value arg_info;

  arg_info.format = vpiIntVal;
  vpi_get_value (vh, &arg_info);
  return arg_info.value.integer;
}

inline char *getStringArgument (vpiHandle vh)
{
  s_vpi_value arg_info;
  char *buf;

  arg_info.format = vpiStringVal;
  vpi_get_value(vh, &arg_info);
  buf = (char *) malloc (sizeof(arg_info.value.str)+1);
  strcpy (buf, arg_info.value.str);

  return buf;
}

void inline putIntegerArgument (vpiHandle vh, int a)
{
  s_vpi_value arg_info;
  
  arg_info.format = vpiIntVal;
  arg_info.value.integer = a;
  vpi_put_value (vh, &arg_info, NULL, vpiNoDelay);
}

/*! \brief Create a new dumper (port)
 * 
 * Usage: $pv_open (phandle, filename, filetype)
 * 
 * Creates a single dumper file.  phandle must be a integer or 32-bit
 * reg, filename should be a string.  Returns the index of the newly
 * created dumper in phandle, which should be passed to future calls of
 * pv_dump_packet() and pv_shutdown().
 * 
 * Filetype should be 0 for writing or 1 for reading
 */
void pv_open () {
  vpiHandle *args;
  int phandle;
  char *filename;
  int filetype;
  s_vpi_value arg_info;
   
  phandle = pcap_used++;
  
  assert (phandle <  MAX_OPEN_PCAP);

  args = get_args (3);

  filename = getStringArgument (args[1]);
  
  arg_info.format = vpiIntVal;
  vpi_get_value (args[2], &arg_info);
  filetype = arg_info.value.integer;
  
  pcap_handle[phandle] = pcap_open (filename, PCAP_BUFSIZE, filetype);

  putIntegerArgument (args[0], phandle);
  free (filename);
}

/*! \brief Dump a packet to an active dumper
 * 
 * Usage: $pv_dump_packet (phandle, len, pkt);
 * 
 * Takes a packet residing in buffer pkt of length len and stores it in
 * the dumper referenced by phandle.  The packet is stored using the
 * current simulation time as its time.
 */
void pv_dump_packet () {
  vpiHandle *args;
  s_vpi_value arg_info;
  vpiHandle arg_h;
  int phandle, len;
  packet_info_t p;
  int i;
  s_vpi_time simtime;
  uint64_t itime;
  
  args = get_args (3);

/*
  arg_info.format = vpiIntVal;
  vpi_get_value (args[0], &arg_info);
  phandle = arg_info.value.scalar;

  arg_info.format = vpiIntVal;
  vpi_get_value (args[1], &arg_info);
  len = arg_info.value.scalar;
  */
  phandle = getIntegerArgument (args[0]);
  len = getIntegerArgument (args[1]);

  p.pdata = (uint8_t *) malloc (len);
  arg_info.format = vpiIntVal;
  for (i=0; i<len; i++) {
    arg_h = vpi_handle_by_index (args[2], i);
    vpi_get_value (arg_h, &arg_info);
    p.pdata[i] = (uint8_t) arg_info.value.integer;
  }
  
  // get simulation time
  simtime.type = vpiSimTime;
  vpi_get_time (args[2], &simtime);
  
  // fill in remainder of p struct
  p.length = len;
  itime = ((uint64_t) simtime.high << 32) + (uint64_t) simtime.low;
  p.sec = (itime * 1000LL) / 1000000LL;
  p.usec = (itime * 1000LL) % 1000000LL; // assume ns timescale

  assert (phandle < MAX_OPEN_PCAP);
  pcap_add_pkt (pcap_handle[phandle].dump, &p);
}
/*! \brief Get a packet from an open dumpfile
 * 
 * Usage: $pv_get_packet (phandle, len, pkt, nstime);
 * 
 * Retrieve a packet from open dumpfile referenced by phandle.
 * Packet length is placed in len, which should be an integer or
 * reg[31:0].  pkt should be array of reg[7:0],  nstime is the
 * timestamp of the packet in nanoseconds, and should also be an
 * integer.
 */
 
 void pv_get_packet () {
   vpiHandle *args, element;
   s_vpi_value arg_info;
   //vpiHandle arg_h;
   packet_info_t p;
   int i;
   int phandle;
 
   args = get_args (4);
   
   phandle = getIntegerArgument (args[0]);
   assert ((phandle < MAX_OPEN_PCAP) && (phandle >= 0));
   assert (pcap_handle[phandle].ctx != NULL);
   
   // read packet from pcap file
   pcap_get_pkt (pcap_handle[phandle].ctx, &p);
   
   // store packet in array
   arg_info.format = vpiIntVal;
   for (i=0; i<p.length; i++) {
     element = vpi_handle_by_index (args[2], i);
     arg_info.value.integer = p.pdata[i];
     vpi_put_value (element, &arg_info, NULL, vpiNoDelay);
   }
   
   putIntegerArgument (args[1], p.length);
   putIntegerArgument (args[3], p.sec * 1000000 + p.usec * 1000); 
 }
 
/*! \brief Shutdown a dumper after use
 * 
 * Usage: $pv_shutdown (handle)
 * 
 * Shut down the dumper and close the file once simulation is complete.
 * Handle should be value passed by the original pv_open() call.
 */
void pv_shutdown () {
  vpiHandle *args;
  //s_vpi_value arg_info;
  int phandle;
  
  args = get_args (1);
	
  phandle = getIntegerArgument (args[0]);
  
  assert (pcap_handle[phandle].ctx != NULL);
  pcap_shutdown (&pcap_handle[phandle]);
}

// Register VPI routines with the simulator
extern void pv_register(void)
{
  p_vpi_systf_data systf_data_p;
  int i;

  pcap_used = 0;
  for (i=0; i<MAX_OPEN_PCAP; i++) {
    pcap_handle[i].ctx = NULL;
    pcap_handle[i].dump = NULL;
  }
 
  /* use predefined table form - could fill systf_data_list dynamically */
  static s_vpi_systf_data systf_data_list[] = {

    { vpiSysTask, 0, "$pv_open", pv_open, NULL, NULL, NULL },
    { vpiSysTask, 0, "$pv_dump_packet", pv_dump_packet, NULL, NULL, NULL },
    { vpiSysTask, 0, "$pv_get_packet", pv_get_packet, NULL, NULL, NULL },
    { vpiSysTask, 0, "$pv_shutdown", pv_shutdown, NULL, NULL, NULL },
    { 0, 0, NULL, NULL, NULL, NULL, NULL }
  };
  
  systf_data_p = &(systf_data_list[0]);
  while (systf_data_p->type != 0) vpi_register_systf(systf_data_p++);
}

// entry point for simulator
void (*vlog_startup_routines[]) () = { pv_register, 0 };

