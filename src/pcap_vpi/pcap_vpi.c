#include <stdio.h>
#include <stdlib.h>
#include "pcap_vpi.h"

void pcap_add_pkt (pcap_dumper_t *dump, packet_info_t *p) {
  struct pcap_pkthdr hdr;
		
  hdr.ts.tv_sec = p->sec;
  hdr.ts.tv_usec = p->usec;
  hdr.caplen = p->length;
  hdr.len = p->length;
  pcap_dump ((u_char *) dump, &hdr, p->pdata);
}

pcap_handle_t pcap_open (char *filename, int bufsize)
{
  pcap_handle_t h;

  h.ctx = pcap_open_dead (DLT_EN10MB, bufsize);
  h.dump = pcap_dump_open (h.ctx, filename);

  return h;
}

void pcap_shutdown (pcap_handle_t *h) {
  pcap_dump_close (h->dump);
  pcap_close (h->ctx);
}
