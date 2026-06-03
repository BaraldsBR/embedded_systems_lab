`include "spi.v" // Include YOUR entity.
`timescale 1ns / 1ps  // Time unit = period, precision
module tb_spi;
  integer     i;
  reg  [31:0] expected_rx = 32'hAAAAAAAA;
  reg         clk;
  reg         rst;
  reg  [31:0] tx_buf;
  wire        transfer_done;
  wire [31:0] rx_buf;

  reg  SPI_CLK;
  reg  SPI_PICO;
  reg  SPI_CS;
  wire SPI_POCI;

  spi dut ( // <- TopEntity dut (Device Under Test) UPDATE TopEntity when relevant!
    .clk(clk),
    .rst(rst),
    .data_in(tx_buf),
    .transfer_done(transfer_done),
    .data_out(rx_buf),
    .sck(SPI_CLK),
    .mosi(SPI_PICO),
    .cs(SPI_CS),
    .miso(SPI_POCI)
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
    $dumpvars(0, tb_spi);  // Signals to dump

    SPI_CLK <= 0;
    SPI_CS <= 1;
    tx_buf = 32'hAAAAAAAA;
    i = 31;
    rst = 0;
    #10;
    rst = 1;
    #10;
    rst = 0; 
    
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
    #200;
    
    $finish;  // end simulation
  end
endmodule
