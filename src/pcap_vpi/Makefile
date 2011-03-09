
all: pcap_vpi.vpi

test_vpi: pcap_vpi.o test_pcap_vpi.o
	$(CC) pcap_vpi.o test_pcap_vpi.o -lpcap -o test_vpi

#pcap_vpi.vpi: pcap_vpi.o pcap_dump.o
#	$(CC) -shared -o $@ $^
pcap_vpi.vpi: pcap_vpi.c pcap_dump.c
	iverilog-vpi $^

