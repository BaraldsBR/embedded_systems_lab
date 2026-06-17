`include "TopEntity.v" // Include YOUR entity.
`timescale 1ns / 1ps  // Time unit = period, precision
module tb_TopEntity;
  
  reg rst;
  reg clk;

  reg  [31:0] expected_rx = 32'h800F00AA;
  integer i;

  reg  PITCH_ENC_A;
  reg  PITCH_ENC_B;
  wire PITCH_DIRA;
  wire PITCH_DIRB;
  wire PITCH_PWM_VAL;
  
  reg  YAW_ENC_A;
  reg  YAW_ENC_B;
  wire YAW_DIRA;
  wire YAW_DIRB;
  wire YAW_PWM_VAL;

  reg  SPI_PICO;
  reg  SPI_CLK;
  reg  SPI_CS;
  wire SPI_POCI;

  TopEntity dut (
    .clk(clk),
    .btn1(rst),
    .btn2(rst),
    .PITCH_ENC_A(PITCH_ENC_A),
    .PITCH_ENC_B(PITCH_ENC_B),
    .PITCH_DIRA(PITCH_DIRA),
    .PITCH_DIRB(PITCH_DIRB),
    .PITCH_PWM_VAL(PITCH_PWM_VAL),
    .YAW_ENC_A(YAW_ENC_A),
    .YAW_ENC_B(YAW_ENC_B),
    .YAW_DIRA(YAW_DIRA),
    .YAW_DIRB(YAW_DIRB),
    .YAW_PWM_VAL(YAW_PWM_VAL),
    .SPI_PICO(SPI_PICO),
    .SPI_CLK(SPI_CLK),
    .SPI_CS(SPI_CS),
    .SPI_POCI(SPI_POCI)
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
    $dumpvars(0, tb_TopEntity);  // Signals to dump

    PITCH_ENC_A = 0;
    PITCH_ENC_B = 0;
    YAW_ENC_A = 0;
    YAW_ENC_B = 0;
    SPI_PICO = 0;
    SPI_CLK = 0;
    SPI_CS = 1;
    i = 31;
    
    rst = 0;
    #10;
    rst = 1;
    #10;
    rst = 0;
    
    
    repeat (50) begin  
      PITCH_ENC_B <= 1'b1;
      YAW_ENC_A <= 1'b1;
      #120;
      PITCH_ENC_A <= 1'b1;
      YAW_ENC_B <= 1'b1;
      #120;
      PITCH_ENC_B <= 1'b0;
      YAW_ENC_A <= 1'b0;
      #120;
      PITCH_ENC_A <= 1'b0;
      YAW_ENC_B <= 1'b0;
      #120;
    end

    #100;

    repeat (75) begin
      PITCH_ENC_A <= 1'b1;
      YAW_ENC_B <= 1'b1;
      #120;
      PITCH_ENC_B <= 1'b1;
      YAW_ENC_A <= 1'b1;
      #120;
      PITCH_ENC_A <= 1'b0;
      YAW_ENC_B <= 1'b0;
      #120;
      PITCH_ENC_B <= 1'b0;
      YAW_ENC_A <= 1'b0;
      #120;
    end

    #10;
    SPI_CS <= 0;
    repeat (32) begin
      SPI_PICO <= expected_rx[i];
      i <= i-1;
      SPI_CLK = 0;
      #8;
      SPI_CLK = ~SPI_CLK;
      #8;
    end
    #150;
    SPI_CS <= 1;
    SPI_CLK = 0;
    #10000;

    $finish;  // end simulation
  end
endmodule
