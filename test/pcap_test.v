`timescale 1ns/1ns

`define PV_READ 1
`define PV_WRITE 0

module pcap_test;

  reg [7:0] mypacket [0:2048];
  reg [7:0] inpacket [0:2048];
  integer i, h, k;
  integer len, nstime;

  initial
    begin
      for (i=0; i<100; i=i+1)
        mypacket[i] = i;

      $pv_open (h, "packet_dump.pcap", `PV_WRITE);
      repeat (10)
        begin
          #100;
          $pv_dump_packet (h, i, mypacket);
          mypacket[0] = mypacket[0] + 1;
        end

      #1000;
      $display ("Calling shutdown");
      $pv_shutdown (h);

      // now read in the array we wrote out and print out
      // each packet

      $pv_open (k, "packet_dump.pcap", `PV_READ);
      repeat (10)
        begin
          $pv_get_packet (k, len, inpacket, nstime);
          print_pkt (len);
        end
      $finish;
    end

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

