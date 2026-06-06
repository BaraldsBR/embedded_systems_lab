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
  output SPI_POCI
);
  assign led1 = btn1;

  wire [31:0] rx_to_tx;

  spi spi (
    .clk(clk),
    .rst(btn1),
    .sck(SPI_CLK),
    .mosi(SPI_PICO),
    .cs(SPI_CS),
    .miso(SPI_POCI),
    .data_out(rx_to_tx),
    .new_data(),
    .data_in(rx_to_tx)
  );

endmodule
