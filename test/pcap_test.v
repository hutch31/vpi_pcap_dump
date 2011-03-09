`timescale 1ns/1ns

module pcap_test;

  reg [7:0] mypacket [0:2048];
  integer i, h;

  initial
    begin
      for (i=0; i<100; i=i+1)
        mypacket[i] = i;

      $pv_open (h);
      repeat (10)
        begin
          #100;
          $pv_dump_packet (h, i, mypacket);
        end

      #1000;
      $pv_shutdown (h);
      $finish;
    end

endmodule

