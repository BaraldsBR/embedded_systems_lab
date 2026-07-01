`timescale 1 ps / 1 ps
`include "debouncer.v"
`include "encoder.v"
`include "pwm.v"
`include "spi.v"

module TopEntity(
  input clk,
  input btn1,
  output led1,

  // Pitch
  input  PITCH_ENC_A,
  input  PITCH_ENC_B,
  output PITCH_DIRA,
  output PITCH_DIRB,
  output PITCH_PWM_VAL,
  
  // Yaw
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
  wire   rst = btn1;
  assign led1 = rst;

  // SPI module
  wire [31:0] rx_buf;
  wire [31:0] tx_buf;

  spi spi (
    .rst(rst),
    .clk(clk),
    .sck(SPI_CLK),
    .mosi(SPI_PICO),
    .cs(SPI_CS),
    .miso(SPI_POCI),
    .data_out(rx_buf),
    .new_data(),
    .data_in(tx_buf)
  );

  // PWM module
  // Descramble SPI coming from RPi
  wire [15:0] target_pitch = { rx_buf[23:16] , rx_buf[31:24] };
  wire [15:0] target_yaw   = { rx_buf[7:0]   , rx_buf[15:8]  };

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

  // Debounce encoder inputs
  wire debounced_enc_A_pitch;
  wire debounced_enc_B_pitch;
  wire debounced_enc_A_yaw;
  wire debounced_enc_B_yaw;

  debouncer debouncer_pitch_A (
    .clk(clk),
    .rst(rst),
    .signal(PITCH_ENC_A),
    .debounced(debounced_enc_A_pitch)
  );

  debouncer debouncer_pitch_B (
    .clk(clk),
    .rst(rst),
    .signal(PITCH_ENC_B),
    .debounced(debounced_enc_B_pitch)
  );

  debouncer debouncer_yaw_A (
    .clk(clk),
    .rst(rst),
    .signal(YAW_ENC_A),
    .debounced(debounced_enc_A_yaw)
  );

  debouncer debouncer_yaw_B (
    .clk(clk),
    .rst(rst),
    .signal(YAW_ENC_B),
    .debounced(debounced_enc_B_yaw)
  );

  // Encoder module
  wire signed [15:0] pitch_out;
  wire signed [15:0] yaw_out;

  encoder encoder_pitch (
    .rst(rst),
    .clk(clk),
    .signal_A(debounced_enc_A_pitch),
    .signal_B(debounced_enc_B_pitch),
    .pos_out(pitch_out) 
  );
  
  encoder encoder_yaw (
    .rst(rst),
    .clk(clk),
    .signal_A(debounced_enc_A_yaw),
    .signal_B(debounced_enc_B_yaw),
    .pos_out(yaw_out) 
  );

  // Pre-scramble SPI content for RPi
  assign tx_buf= {pitch_out[7:0], pitch_out[15:8], yaw_out[7:0], yaw_out[15:8]};

endmodule
