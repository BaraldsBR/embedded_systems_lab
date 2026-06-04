`timescale 1 ps / 1 ps
`include "debouncer.v"
`include "encoder.v"
`include "pwm.v"
`include "spi.v"

module TopEntity(
  input clk,
  input btn1,
  input btn2,

  // Pitch Motor
  input  PITCH_ENC_A,
  input  PITCH_ENC_B,
  output PITCH_DIRA,
  output PITCH_DIRB,
  output PITCH_PWM_VAL,
  
  // Yaw Motor
  input  YAW_ENC_A,
  input  YAW_ENC_B,
  output YAW_DIRA,
  output YAW_DIRB,
  output YAW_PWM_VAL,

  // SPI
  input  SPI_PICO,
  input  SPI_CLK,
  input  SPI_CS,
  output SPI_POCI
);
  wire        rst = btn1 || btn2;

  wire [15:0] target_pitch = rx_buf[31:16];
  wire [15:0] target_yaw = rx_buf[15:0];

  pwm pitch_pwm (
    .rst(rst),
    .clk(clk),
    .target_value(target_pitch),
    .dir_A(PITCH_DIRA),
    .dir_B(PITCH_DIRB),
    .PWM_VAL(PITCH_PWM_VAL)
  );
  
  pwm yaw_pwm (
    .rst(rst),
    .clk(clk),
    .target_value(target_yaw),
    .dir_A(YAW_DIRA),
    .dir_B(YAW_DIRB),
    .PWM_VAL(YAW_PWM_VAL)
  );

  wire signed [15:0] pitch_out;
  wire signed [15:0] yaw_out;

  wire debounced_pitch_enc_A;
  wire debounced_pitch_enc_B;
  wire debounced_yaw_enc_A;
  wire debounced_yaw_enc_B;

  debouncer debouncer_pitch_A (
    .clk(clk),
    .rst(rst),
    .signal(PITCH_ENC_A),
    .debounced(debounced_pitch_enc_A)
  );

  debouncer debouncer_pitch_B (
    .clk(clk),
    .rst(rst),
    .signal(PITCH_ENC_B),
    .debounced(debounced_pitch_enc_B)
  );

  debouncer debouncer_yaw_A (
    .clk(clk),
    .rst(rst),
    .signal(YAW_ENC_A),
    .debounced(debounced_yaw_enc_A)
  );

  debouncer debouncer_yaw_B (
    .clk(clk),
    .rst(rst),
    .signal(YAW_ENC_B),
    .debounced(debounced_yaw_enc_B)
  );
  
  encoder pitch_encoder (
    .rst(rst),
    .clk(clk),
    .signal_A(debounced_pitch_enc_A),
    .signal_B(debounced_pitch_enc_B),
    .pos_out(pitch_out) 
  );
  
  encoder yaw_encoder (
    .rst(rst),
    .clk(clk),
    .signal_A(debounced_yaw_enc_A),
    .signal_B(debounced_yaw_enc_B),
    .pos_out(yaw_out) 
  );

  wire [31:0] rx_buf;
  wire        spi_ready;

  spi spi (
    .rst(rst),
    .clk(clk),
    .sck(SPI_CLK),
    .mosi(SPI_PICO),
    .cs(SPI_CS),
    .miso(SPI_POCI),
    .data_out(rx_buf),
    .transfer_done(spi_ready),
    .data_in({ pitch_out[15:0], yaw_out[15:0] })
  );

endmodule
