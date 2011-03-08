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
