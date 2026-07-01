module pwm #(

  // 20kHz PWM for 100MHz FPGA clock
  parameter PWM_CYCLE_LENGTH = 5000,

  // Hardcoded 25% pwm safety limit
  parameter PWM_MAX = PWM_CYCLE_LENGTH * 0.25
) (
  input rst,
  input clk,
  input [15:0] target_value,

  output dir_A,
  output dir_B,
  output PWM_VAL
);
  reg  [15:0] counted_cycles;
  wire        target_zero = (target_value == 16'b0);
  wire        target_sign = target_value[15];
  wire [15:0] abs_target = (target_sign) ? (-target_value) : target_value;
  wire [15:0] capped_target = (abs_target > PWM_MAX) ? PWM_MAX : abs_target;

  always @(posedge clk) begin
    if(rst) begin
      counted_cycles <= 0;
    end else begin
      if (counted_cycles >= PWM_CYCLE_LENGTH) counted_cycles <= 0;
      else counted_cycles <= counted_cycles + 1;
    end
  end

  assign PWM_VAL = (counted_cycles < capped_target);
  assign dir_A = (target_zero) ? 0 : ~target_sign;
  assign dir_B = (target_zero) ? 0 : target_sign;
endmodule