/* Copyright (c) 2011, Guy Hutchison
   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <string.h>
#include <pcap.h>
#include <sys/time.h>

#include "vpi_user.h"
#include "cv_vpi_user.h"

#define TRUE 1
#define FALSE 0

#define MEMDUMP_MAX_ARGS 2

/* local prototypes */
static int count_systf_args(vpiHandle);

/* global prototypes */
extern void register_my_systfs(void); 
void get_byte_array (vpiHandle array, int len, unsigned char *dest);
void print_byte_array (int len, unsigned char *data);

/*
 * possible pattern for register vpi_ system tasks and functions
 */

void inline error_check()
{
	s_vpi_error_info err;
	
	if (vpi_chk_error(&err))
		vpi_printf ("ERROR: File %s, line %d: %s\n", err.file, err.line, err.message);
}

/*
 * Initialize packet lib dump file
 */

#define PCAP_BUF_SIZE 2048

struct {
  pcap_t *ctx;
  pcap_dumper_t *dump;
  u_char *buf;
} pc_context;

void pcap_add_pkt (int len, u_char *data, int time) {
	struct timeval ts;
	struct pcap_pkthdr hdr;
		
	ts.tv_sec = time / 1000000;
	ts.tv_usec = time % 1000000;
	hdr.ts = ts;
	hdr.caplen = len;
	hdr.len = len;
	pcap_dump (pc_context.dump, &hdr, data);
}

void pcap_shutdown () {
  // close up shop
  pcap_dump_close (pc_context.dump);
  pcap_close (pc_context.ctx);
  free (pc_context.buf);
}

PLI_INT32 capture_init_vpi (s_cb_data *foo) {
  //init_pcap_dump ("packet.pcap");
  pc_context.buf = (u_char *) malloc (PCAP_BUF_SIZE);
  pc_context.ctx = pcap_open_dead (DLT_EN10MB, PCAP_BUF_SIZE);
  pc_context.dump = pcap_dump_open (pc_context.ctx, "packet.pcap");
}

PLI_INT32 capture_shutdown_vpi (s_cb_data *foo) {
  pcap_shutdown();
}

// Register callbacks at start and end of simulation,
// to initialize PCap dump file, and shutdown file,
// respectively
PLI_INT32 capture_compile (void) {
  s_cb_data *callback;

  callback = malloc (sizeof(s_cb_data));

  callback->reason = cbStartOfSimulation;
  callback->cb_rtn = capture_init_vpi;
 
  vpi_register_cb (callback);
  
  callback->reason = cbEndOfSimulation;
  callback->cb_rtn = capture_shutdown_vpi;

  vpi_register_cb (callback);
  
  free (callback);
}

// get current simulation time
int get_sim_time (vpiHandle obj) {
	s_vpi_time stime;

	stime.type = vpiSimTime;
        vpi_get_time (obj, &stime);

	return stime.low;
}

/*
 * Packet Dump PLI routines
 * Takes 2 arguments, length and a reference to a memory where
 * the packet is stored.
 */
PLI_INT32 capture_pkt_vpi (void)
{
 	vpiHandle href, iref;
	int i, nargs, len;
	unsigned char *mem;
	int time;
	
	s_vpi_value arg_info;
	s_vpi_error_info err;
	
 	vpiHandle tfargs[MEMDUMP_MAX_ARGS];

 	href = vpi_handle(vpiSysTfCall, NULL); 
	error_check();
	
	if ((iref = vpi_iterate(vpiArgument, href)) != NULL)
		nargs = get_systf_args (iref, tfargs);
	
	// get first arguments
	arg_info.format = vpiIntVal;
	vpi_get_value (tfargs[0], &arg_info);
	len = arg_info.value.scalar;
	error_check();
  
	// dump array in second argument
	mem = (unsigned char *) malloc (len);
	get_byte_array (tfargs[1], len, mem);
	error_check();

        // get current simulation time
        time = get_sim_time(href);

	pcap_add_pkt (len, mem, time);
	free (mem);
	return(0);
}

/*
 * this is routine to implement registered systf call back
 *
 * notice vpi_user.h requires function return int so using dummy 0
 */
PLI_INT32 memdump (void)
{
 	vpiHandle href, iref;
	int i, nargs, len;
	unsigned char *mem;
	
	s_vpi_value arg_info;
	s_vpi_error_info err;
	
 	vpiHandle tfargs[MEMDUMP_MAX_ARGS];

 	href = vpi_handle(vpiSysTfCall, NULL); 
	error_check();
	
	if ((iref = vpi_iterate(vpiArgument, href)) != NULL)
		nargs = get_systf_args (iref, tfargs);
	
	// get first arguments
	arg_info.format = vpiIntVal;
	vpi_get_value (tfargs[0], &arg_info);
	len = arg_info.value.scalar;
	error_check();
  
	// dump array in second argument
	mem = (unsigned char *) malloc (len);
	get_byte_array (tfargs[1], len, mem);
	error_check();
	print_byte_array (len, mem);
	free (mem);
	return(0);
}

void get_byte_array (vpiHandle array, int len, unsigned char *dest) {
	int mem_size, i;
	vpiHandle element;
	s_vpi_value arg_info;

	// check that type is a memory
	if (vpi_get (vpiType, array) != vpiMemory) {
		vpi_printf ("ERROR: Only memory elements can be dumped\n");
		return;
	}
	// check size of memory
	mem_size = vpi_get(vpiSize, array);
	if (mem_size < len) {
		vpi_printf ("ERROR: Memory size (%d) is smaller than requested amount (%d)\n",
		mem_size, len);
		return;
	}
	
	// iterate through memory and copy out
	arg_info.format = vpiIntVal;
	for (i=0; i<len; i++) {
		element = vpi_handle_by_index (array, i);
		vpi_get_value (element, &arg_info);
		dest[i] = (unsigned char) arg_info.value.scalar;
	}
}

void print_byte_array (int len, unsigned char *data) {
	int i;
	
	for (i=0; i<len; i++) {
		if ((i % 16) == 0)
			vpi_printf ("%04x: ", i);
		vpi_printf ("%02x ", data[i]);
		if ((i % 16) == 7)
			vpi_printf ("| ");
		else if ((i%16)==15)
			vpi_printf ("\n");
	}
	if ((i%16)!=15) vpi_printf ("\n");
}

int get_systf_args (vpiHandle iref, vpiHandle *args)
{
	int anum = 0;
	vpiHandle arg_h;
	
	while ((arg_h = vpi_scan(iref)) != NULL) {
		args[anum++] = arg_h;
	}
	return anum;
}
/*
 * count arguments
 */
static int count_systf_args(vpiHandle iref)
{
 int anum = 0;

 while (vpi_scan(iref) != NULL) anum++;
 return(anum);
}

/* Template functin table for added user systf tasks and functions.
   See file vpi_user.h for structure definition
   Note only vpi_register_systf and vpi_ or tf_ utility routines that 
   do not access the simulation data base may be called from these routines
*/ 

/* routine to do the systf registering - probably should go in other file */
/* usually only vpi_ PLI 2.0 systf registering is done here */

/*
 * register all vpi_ PLI 2.0 style user system tasks and functions
 */
void pktcap_register(void)
{
 p_vpi_systf_data systf_data_p;

 /* use predefined table form - could fill systf_data_list dynamically */
 static s_vpi_systf_data systf_data_list[] = {
  { vpiSysTask, 0, "$pktdump", memdump, NULL, NULL, NULL },
  { vpiSysTask, 0, "$capture_pkt", capture_pkt_vpi, capture_compile, NULL, NULL },
  { 0, 0, NULL, NULL, NULL, NULL, NULL }
 };

 systf_data_p = &(systf_data_list[0]);
 while (systf_data_p->type != 0) vpi_register_systf(systf_data_p++);
}
