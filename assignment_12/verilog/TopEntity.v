`timescale 1 ps / 1 ps
`include "spi.v"

module TopEntity(
  input clk,
  input btn1,

  output led1,
  // SPI
  input  SPI_PICO,
  input  SPI_CLK,
  input  SPI_CS,
  output SPI_POCI,
  output YAW_DIRA,
  output YAW_DIRB
);
  assign led1 = btn1;
  wire        spi_ready;
  wire [31:0] rx_to_tx;

  spi spi (
    .clk(clk),
    .rst(btn1 || SPI_CS),
    .sck(SPI_CLK),
    .state_out({YAW_DIRA, YAW_DIRB}),
    .mosi(SPI_PICO),
    .cs(SPI_CS),
    .miso(SPI_POCI),
    .data_out(rx_to_tx),
    .transfer_done(spi_ready),
    .data_in(rx_to_tx)
  );

endmodule
