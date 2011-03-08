/*! \file test_pcap_vpi.c */

/* Copyright (c) 2011, Guy Hutchison
   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "pcap_vpi.h"

static int sec;

packet_info_t *stub_make_pkt () {
  int i;
  uint8_t *d;
  packet_info_t *p = (packet_info_t *) malloc (sizeof(packet_info_t));
  p->length = random() % 1000 + 64;
  p->sec = sec++;
  p->usec = random() % 1000000;
  
  d = (uint8_t *) malloc (p->length);
  
  for (i=0; i<p->length; i++)
    d[i] = random() % 256;
  p->pdata = d;
  return p;
}

int main (int argc, char **argv)
{
  char buf[256];
  int i;
  pcap_handle_t h;
  packet_info_t *p;
  sec = 0;

  h = pcap_open ("packet.pcap", 2048);
  
  for (i=0; i<20; i++) {
  	p = stub_make_pkt();
  	pcap_add_pkt (h.dump, p);
  	free (p);
  }

  pcap_shutdown (&h);
}
