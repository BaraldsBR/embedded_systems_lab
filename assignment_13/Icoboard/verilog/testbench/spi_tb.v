`include "../spi_base.v" // Include YOUR entity.
`timescale 1ns / 1ps  // Time unit = period, precision
module spi_tb;
  integer     i;
  reg  [31:0] expected_rx = 32'h87654321;
  reg         clk;
  reg         rst;
  reg  [31:0] tx_buf;
  wire        ready;
  wire [31:0] rx_buf;

  reg  SPI_CLK;
  reg  SPI_PICO;
  reg  SPI_CS;
  wire SPI_POCI;

  spi_base dut ( // <- TopEntity dut (Device Under Test) UPDATE TopEntity when relevant!
    .clk(clk),
    .rst(rst),
    .data_in(tx_buf),
    .transfer_done(ready),
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
      #2;
      clk = ~clk;
      #2;
    end
  end

  initial begin
    forever begin
      SPI_CLK = 0;
      #15;
      SPI_CLK = ~SPI_CLK;
      #15;
    end
  end


// Start of your testbench script
  initial begin
    $dumpfile("signals.vcd");  // Name of the signal dump file
    $dumpvars(0, spi_tb);  // Signals to dump

    SPI_CS <= 1;
    tx_buf = 32'h12345678;
    i = 31;
    rst = 0;
    #10;
    rst = 1;
    #10;
    rst = 0; 
    
    #10;
    SPI_CS <= 0;
    SPI_PICO <= expected_rx[31];
    repeat (31) begin
      i <= i-1;
      #30;
      SPI_PICO <= expected_rx[i];
    end
    #50;
    SPI_CS <= 1;
    #200;
    
    $finish;  // end simulation
  end
endmodule
