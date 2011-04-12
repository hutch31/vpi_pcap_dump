`timescale 1ns/1ns

`define PV_READ 1
`define PV_WRITE 0

module pcap_test;

  reg [7:0] mypacket [0:2048];
  integer i, h;

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
      $finish;
    end

endmodule

