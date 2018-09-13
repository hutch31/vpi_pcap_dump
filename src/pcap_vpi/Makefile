INCLUDES=-I/usr/local/Cellar/icarus-verilog/10.2_1/include/iverilog

all: pcap_vpi.vpi

test_vpi: pcap_vpi.o test_pcap_vpi.o
	$(CC) pcap_vpi.o test_pcap_vpi.o -lpcap -o test_vpi

#pcap_vpi.o: pcap_vpi.c
#	$(CC) -fPIC -c $(INCLUDES) $^

#pcap_vpi.vpi: pcap_vpi.o pcap_dump.o
#	$(CC) $(INCLUDES) -shared -lpcap -o $@ $^
pcap_vpi.vpi: pcap_vpi.c pcap_dump.c
	iverilog-vpi $^ -lpcap

clean:
	rm -f pcap_vpi.vpi
	rm -f *.o
