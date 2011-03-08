#ifndef PCAP_VPI_H_
#define PCAP_VPI_H_
#include <stdint.h>
#include <pcap.h>

typedef struct {
  pcap_t *ctx;
  pcap_dumper_t *dump;
} pcap_handle_t;

typedef struct {
  int length;
  uint8_t *pdata;
  uint32_t sec;
  uint32_t usec;
} packet_info_t;

pcap_handle_t pcap_open (char *filename, int bufsize);
void pcap_add_pkt (pcap_dumper_t *dump, packet_info_t *p);
void pcap_shutdown (pcap_handle_t *h);

#endif /*PCAP_VPI_H_*/
