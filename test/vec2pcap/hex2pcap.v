`timescale 1ns/1ns

`define PV_READ 1
`define PV_WRITE 0

module hex2pcap;

  reg [7:0] mypacket [0:2048];
  reg [7:0] inpacket [0:2048];
  integer hi, ho, k, p;
  integer len, nstime;
  reg     fail;
  reg [127:0] key;

  task copy_bytes;
    input [31:0] ptr;
    input [31:0] len;
    integer i;
    begin
      for (i=0; i<len; i=i+1)
        mypacket[i] = inpacket[ptr+i];
    end
  endtask

  task get_packet;
    input [7:0] index;
    begin
      case (index)
        0 : $readmemh ("vpacket1.hex", inpacket);
        1 : $readmemh ("vpacket2.hex", inpacket);
        2 : $readmemh ("vpacket3.hex", inpacket);
        3 : $readmemh ("vpacket4.hex", inpacket);
        4 : $readmemh ("vpacket5.hex", inpacket);
        5 : $readmemh ("vpacket6.hex", inpacket);
        6 : $readmemh ("vpacket7.hex", inpacket);
        7 : $readmemh ("vpacket8.hex", inpacket);
        8 : $readmemh ("vpacket9.hex", inpacket);
      endcase
    end
  endtask

  initial
    begin
      fail = 0;
      $pv_open (hi, "hex_in.pcap", `PV_WRITE);
      $pv_open (ho, "hex_out.pcap", `PV_WRITE);
      
      for (k=0; k<8; k=k+1)
        begin
          get_packet (k);
          p = 12;
          len = { inpacket[p], inpacket[p+1] };
          p = p + 2;
          copy_bytes (p, len);
          $pv_dump_packet (hi, len, mypacket);
      p = p + len;
      //print_pkt (len);
      // Skip past key, SCI, DA, SA, PN, TCI
      p = p + 16 + 8 + 6 + 6 + 4 + 1;
      len = { inpacket[p], inpacket[p+1] };
      p = p + 2;

      //key=0;
      //for (k=len; k<len+16; k=k+1)
      //  key = key << 8 | inpacket[k];
      //$display ("KEY=%x", key);
      copy_bytes (p, len);
      $pv_dump_packet (ho, len, mypacket);
        end

      $display ("Calling shutdown");
      $pv_shutdown (hi);
      $pv_shutdown (ho);

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

