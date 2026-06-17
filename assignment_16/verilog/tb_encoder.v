`include "encoder.v" // Include YOUR entity.
`timescale 1ns / 1ps  // Time unit = period, precision
module tb_encoder;

    reg  rst;
    reg  clk;

    reg  signal_A;
    reg  signal_B;
    wire [15:0] pos_out;

  encoder dut ( // <- TopEntity dut (Device Under Test) UPDATE TopEntity when relevant!
    .rst(rst),
    .clk(clk),
    .signal_A(signal_A),
    .signal_B(signal_B),
    .pos_out(pos_out)
  );

  // generate input signals
  initial begin
    forever begin
      clk = 0;
      #1;
      clk = ~clk;
      #1;
    end
  end


// Start of your testbench script
  initial begin
    $dumpfile("signals.vcd");  // Name of the signal dump file
    $dumpvars(0, tb_encoder);  // Signals to dump

    signal_A <= 1'b0;
    signal_B <= 1'b0;

    rst = 0;
    #10;
    rst = 1;
    #10;
    rst = 0;

    
    repeat (50) begin
      // go one way  
      signal_B <= 1'b1;
      #100;
      signal_A <= 1'b1;
      #100;
      signal_B <= 1'b0;
      #100;
      signal_A <= 1'b0;
      #100;
    end

    #100;

    repeat (75) begin
      // go one way
      signal_A <= 1'b0;
      #100;
      signal_B <= 1'b0;
      #100;
      signal_A <= 1'b1;
      #100;
      signal_B <= 1'b1;
      #100;
    end
    
    $finish;  // end simulation
  end
endmodule
