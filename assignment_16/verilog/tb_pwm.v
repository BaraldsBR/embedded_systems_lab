`include "pwm.v" // Include YOUR entity.
`timescale 1ns / 1ps  // Time unit = period, precision
module tb_pwm;
  
  reg rst;
  reg clk;
  reg [15:0] target_value;
  wire dir_A;
  wire dir_B;
  wire PWM_VAL;

  pwm #(
    .PWM_CYCLE_LENGTH(100)
  ) dut ( // <- TopEntity dut (Device Under Test) UPDATE TopEntity when relevant!
    .rst(rst),
    .clk(clk),
    .target_value(target_value),
    .dir_A(dir_A),
    .dir_B(dir_B),
    .PWM_VAL(PWM_VAL)
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
    $dumpvars(0, tb_pwm);  // Signals to dump

    target_value = 0;
    rst = 0;
    #10;
    rst = 1;
    #10;
    rst = 0; 
    #200;
    target_value = 20;
    #400;
    target_value = 80;
    #400;
    target_value = -20;
    #400;
    target_value = -80;
  #400;
    
    $finish;  // end simulation
  end
endmodule
