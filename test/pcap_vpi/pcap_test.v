`timescale 1ns/1ns

`define PV_READ 1
`define PV_WRITE 0

`define NUM_PKTS 200

module pcap_test;

  reg [7:0] mypacket [0:2048];
  reg [7:0] inpacket [0:2048];
  integer h, k;
  integer len, nstime;
  reg     fail;

  initial
    begin
      fail = 0;
      
      $pv_open (h, "packet_dump.pcap", `PV_WRITE);
      repeat (`NUM_PKTS)
        begin
          #100;
          len = {$random} % 128 + 64;
          $display ("Creating packet with length %0d", len);
          create_pkt (len);
          $pv_dump_packet (h, len, mypacket);
        end

      #1000;
      $display ("Calling shutdown");
      $pv_shutdown (h);

      // now read in the array we wrote out and print out
      // each packet
      
      $pv_open (k, "packet_dump.pcap", `PV_READ);
      $display ("Checking packets");
      repeat (`NUM_PKTS)
        begin
          $pv_get_packet (k, len, inpacket, nstime);
          check_pkt (len);
          //print_pkt (len);
        end
      $pv_shutdown(k);
      
      end_check();
      $finish;
    end // initial begin

  initial
    begin
      #50000;
      $display ("ERROR   : Timeout");
      fail = 1;
      end_check();
      $finish;
    end

  task end_check;
    begin
      if (fail)
        $display ("!!!!!!!!!! TEST FAILED !!!!!!!!!!");
      else
        $display ("---------- TEST PASSED ----------");
    end
  endtask

  // create a random packet and put it in the buffer
  task create_pkt;
    input [31:0] len;
    integer      i;
    reg [7:0]    checksum;
    begin
      // start by randomizing entire packet
      for (i=0; i<(len-1); i=i+1)
        mypacket[i] = {$random};

      // change ethertype to be one of several known ethertypes
      case ({$random} % 4)
        0 : { mypacket[12],mypacket[13] } = 16'h8100; // VLAN
        1 : { mypacket[12],mypacket[13] } = 16'h0800; // IPv4
        2 : { mypacket[12],mypacket[13] } = 16'h86DD; // IPv6
        3 : { mypacket[12],mypacket[13] } = 16'h88F7; // PTP
      endcase // case ({$random} % 4)

      // create packet checksum
      checksum = 0;
      for (i=0; i<(len-1); i=i+1)
        checksum = checksum + mypacket[i];
      mypacket[len-1] = checksum;
    end
  endtask // create_pkt

  task check_pkt;
    input [31:0] len;
    reg [7:0]    checksum;
    integer      i;
    begin
      // create packet checksum
      checksum = 0;
      for (i=0; i<(len-1); i=i+1)
        checksum = checksum + inpacket[i];

      if (inpacket[len-1] !== checksum)
        begin
          $display ("ERROR   : Packet failed checksum (exp %x, rcv %x)", checksum, inpacket[len-1]);
          fail = 1;
          print_pkt(len);
        end
    end
  endtask
      
      
  

  task print_pkt;
    input [31:0] len;
    integer      i;
    begin
      
      $display ("%t: INFO    : %m: Received packet length %0d", $time, len);

      for (i=0; i<len; i=i+1)
	begin
	  if (i % 16 == 0) $write ("%x: ", i[15:0]);
	  $write ("%x ", inpacket[i]);
	  if (i % 16 == 7) $write ("| ");
	  if (i % 16 == 15) $write ("\n");
	end
      if (i % 16 != 0) $write ("\n");
    end
  endtask // print_pkt
  
 endmodule

