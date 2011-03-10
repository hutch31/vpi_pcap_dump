/*! \file pcap_dpi.c
 * Contains the DPI routines used to allocate pcap dumpers, dump individual
 * packets to a dumpfile, and shutdown afterwards.  Default has a maximum
 * of 32 open dumpfiles for a simulation.
 */
/* Copyright (c) 2011, Sachin Gandhi
   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <svdpi.h>
#include "pcap_dump.h"
#include <assert.h>

#define MAX_OPEN_PCAP 32
#define PCAP_BUFSIZE  2048

#if defined(__cplusplus)
extern "C"
{
#endif

pcap_handle_t pcap_handle[MAX_OPEN_PCAP];

/*! \brief Create a new dumper (port)
 *
 * Usage: pv_open (phandle, filename)
 *
 * Creates a single dumper file.  phandle must be a integer or 32-bit
 * reg, filename should be a string.  The index of the newly
 * created dumper in phandle should be passed to future calls of
 * pv_dump_packet() and pv_shutdown().
 */

  void pv_open(int phandle, char *pcap_file) 
  {
    assert (phandle <  MAX_OPEN_PCAP);
    pcap_handle[phandle].ctx  = NULL;
    pcap_handle[phandle].dump = NULL;
    pcap_handle[phandle]      = pcap_open (pcap_file, PCAP_BUFSIZE);
  }

/*! \brief Dump a packet to an active dumper
 *
 * Usage: pv_dump_packet (phandle, len, pkt, stime);
 *
 * Takes a packet residing in buffer pkt of length len and stores it in
 * the dumper referenced by phandle.  The packet is stored using the
 * current simulation time as its time.
 */
  void pv_dump_pkt(int                phandle,
                   int                pkt_len,
                   svOpenArrayHandle  pkt,
                   svBitVec32        *stime) // simulation time in ns
  {
    svBitVec32   *pkt_ptr;
    packet_info_t p;
    uint64_t      s_time;
    int           i;
    pkt_ptr = (svBitVec32*) svGetArrayPtr(pkt); 
    p.pdata = (uint8_t *) malloc (pkt_len);
    s_time  = ((uint64_t) stime[0]) | ((uint64_t) stime[1]) << 32;
    for (i = 0; i < pkt_len; i++) 
    {
      p.pdata[i] = (uint8_t) pkt_ptr[i];
    }
    p.length = pkt_len;
    p.usec   = s_time / 1000000LL;
    p.sec    = p.usec / 1000000LL;
    assert (phandle < MAX_OPEN_PCAP);
    pcap_add_pkt (pcap_handle[phandle].dump, &p);
  }

/*! \brief Shutdown a dumper after use
 *
 * Usage: pv_shutdown (handle)
 *
 * Shut down the dumper and close the file once simulation is complete.
 * Handle should be value passed by the original pv_open() call.
 */
  void pv_shutdown(int phandle)
  {
    assert (phandle < MAX_OPEN_PCAP);
    assert (pcap_handle[phandle].ctx != NULL);
    assert (pcap_handle[phandle].dump != NULL);
    pcap_shutdown (&pcap_handle[phandle]);
  }

#if defined(__cplusplus)
}
#endif
