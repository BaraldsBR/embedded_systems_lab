`include "encoder.v"
`timescale 1ns / 1ps
module tb_encoder;
  localparam POS_WIDTH = 16;
  localparam POSITIVE_PULSES = 50;
  localparam NEGATIVE_PULSES = 75;

  reg  rst;
  reg  clk;

  reg  signal_A;
  reg  signal_B;
  wire [POS_WIDTH - 1:0] pos_out;

  encoder #(
    .POS_WIDTH(POS_WIDTH)
  ) dut ( 
    .rst(rst),
    .clk(clk),
    .signal_A(signal_A),
    .signal_B(signal_B),
    .pos_out(pos_out)
  );

  // generate clock
  initial begin
    forever begin
      clk = 0;
      #1;
      clk = ~clk;
      #1;
    end
  end

  initial begin
    $dumpfile("signals.vcd");
    $dumpvars(0, tb_encoder);

    signal_A <= 1'b0;
    signal_B <= 1'b0;

    rst = 0;
    #10;
    rst = 1;
    #10;
    rst = 0;
    #10;

    repeat (POSITIVE_PULSES) begin
      signal_B <= 1'b1;
      #50;
      signal_A <= 1'b1;
      #50;
      signal_B <= 1'b0;
      #50;
      signal_A <= 1'b0;
      #50;
    end

    #50;

    repeat (NEGATIVE_PULSES) begin
      signal_A <= 1'b1;
      #50;
      signal_B <= 1'b1;
      #50;
      signal_A <= 1'b0;
      #50;
      signal_B <= 1'b0;
      #50;
    end
    
    $finish;
  end
endmodule
