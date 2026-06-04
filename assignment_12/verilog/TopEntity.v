`timescale 1 ps / 1 ps
`include "spi.v"

module TopEntity(
  input clk,
  input btn1,
  input btn2,

  // SPI
  input  SPI_PICO,
  input  SPI_CLK,
  input  SPI_CS,
  output SPI_POCI
);
  wire        rst = btn1 || btn2;
  wire        spi_ready;
  wire [31:0] rx_to_tx;

  spi spi (
    .clk(clk),
    .sck(SPI_CLK),
    .mosi(SPI_PICO),
    .cs(SPI_CS),
    .miso(SPI_POCI),
    .data_out(rx_to_tx),
    .transfer_done(spi_ready),
    .data_in(rx_to_tx)
  );

endmodule
