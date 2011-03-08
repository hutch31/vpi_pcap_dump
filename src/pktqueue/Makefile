#----------------------------------------------------------------------
#  Author: Guy Hutchison
#
#  Copyright (c) 2005 D5 Networks
#  All Rights Reserved
#----------------------------------------------------------------------

INCLUDES=-I$(CVER_HOME)/pli_incs/ 
CXX=g++
CXXFLAGS=-g -fpic -m32 $(INCLUDES)
CFLAGS=-g -O -fpic -m32 $(INCLUDES)
LFLAGS= -G -melf_i386 -shared -export-dynamic -lstdc++ -L$(LIBPCAP) -lpcap
LIBPCAP=$(HOME)/env/libpcap-0.8.3

%.so:     %.o
	$(LD) $(LFLAGS) $< -o $@

pqpli.so : pqpli.o pktqueue.o pqpli_register.o pktcap.o
	$(LD) $(LFLAGS) pqpli.o pktqueue.o pqpli_register.o -o $@

clean:
	rm -f *.o *.so


