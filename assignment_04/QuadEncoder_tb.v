`include "QuadEncoder.v"
`timescale 1ns / 1ns 
module QuadEncoder_tb;
  // clock and reset
  reg clk;
  reg rst;

  // inputs
  wire signed [12:0] position; 

  // outputs
  reg signal_A;
  reg signal_B;

  QuadEncoder dut (
      .clk (clk),
      .rst(rst),
      .signal_A(signal_A),
      .signal_B(signal_B),
      .pos_out(position)
  );

  // generate the clock
  initial begin
    clk = 1'b0;
    forever #1 clk <= ~clk;
  end

  // Generate the reset
  initial begin
    rst = 1'b0;
    #10;
    rst = 1'b1;
  end

// Start of your testbench script
  initial begin
    $dumpfile("signals.vcd"); 
    $dumpvars(0, QuadEncoder_tb); 

    signal_A <= 1'b0;
    signal_B <= 1'b0;
    #20;
    repeat (1000) begin
      // go one way  
      signal_B <= 1'b1;
      #20;
      signal_A <= 1'b1;
      #20;
      signal_B <= 1'b0;
      #20;
      signal_A <= 1'b0;
      #20;
    end

    #50;
    $display("position: %d", position);

    repeat (2000) begin
      // go one way
      signal_A <= 1'b0;
      #20;
      signal_B <= 1'b0;
      #20;
      signal_A <= 1'b1;
      #20;
      signal_B <= 1'b1;
      #20;
    end

    #50
    $display("position: %d", position);

    repeat (1000) begin
      // go one way  
      signal_B <= 1'b1;
      #20;
      signal_A <= 1'b1;
      #20;
      signal_B <= 1'b0;
      #20;
      signal_A <= 1'b0;
      #20;
    end
    $display("position: %d", position);

    $finish;  // end simulation
  end
endmodule
