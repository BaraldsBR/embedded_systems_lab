`include "spi.v"
`timescale 10ns / 10ns 

module tb_spi;
  parameter WIDTH = 8;
  parameter Tck  = 2;
  parameter Tsck = 20*Tck;

  // spi side
  reg              clk;
  reg              rst;
  reg  [WIDTH-1:0] data_in;   
  wire [WIDTH-1:0] data_out;
  wire             new_data;

  // data side
  reg   cs;
  reg   sck;
  reg   mosi;
  wire  miso;

  spi #(
    .WIDTH(WIDTH)
  ) dut ( 
    .clk(clk),
    .rst(rst),
    .data_in(data_in),
    .new_data(new_data),
    .data_out(data_out),
    .sck(sck),
    .mosi(mosi),
    .cs(cs),
    .miso(miso)
  );

  integer i = (WIDTH-1);
  reg [WIDTH-1:0] mosi_parallel;

  initial forever begin
    clk <= 0;
    #(Tck/2);
    clk <= 1;
    #(Tck/2);
  end

  initial begin
    $dumpfile("signals.vcd"); 
    $dumpvars(0, tb_spi);
    
    cs <= 1;
    sck <= 0;
    mosi <= 0;
    data_in <= 8'hAA;
    mosi_parallel <= 8'hAA;

    rst <= 1;
    #(Tsck/2);
    rst <= 0; 
    #(Tsck);

    cs <= 0;
    #(Tsck);
  
    repeat (WIDTH) begin
      mosi <= mosi_parallel[i];
      #(Tsck/2);
      sck <= 1;
      #(Tsck/2);
      sck <= 0;
      i = i-1;
    end

    #(Tsck);
    cs <= 1;
    #(Tsck);
    
    $finish;
  end
endmodule
