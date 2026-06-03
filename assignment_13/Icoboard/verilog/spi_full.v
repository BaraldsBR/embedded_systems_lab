`include "spi_base.v"
module spi_full (
  input clk,        
  input rst,
  input mosi,                 // Master Out Slave In
  input sck,                  // SPI clock from master (SCK)  
  input cs,                   // Chip Select (Active LOW)
  input [31:0] data_in,        // Data to transmit to master
  output miso,            // Master In Slave Out
  output reg [31:0] data_out   // Data received from master
);
  reg  [1:0] transfer_count;
  reg [23:0] internal_data;

  wire [7:0] base_data_in = (transfer_count == 2'b00) ? data_in[7:0]  :
                            (transfer_count == 2'b01) ? data_in[15:8] :
                            (transfer_count == 2'b10) ? data_in[23:16] : data_in[31:24];

  spi_base base (
    .clk(clk),        
    .rst(rst),
    .mosi(mosi),                 // Master Out Slave In
    .sck(sck),                  // SPI clock from master (SCK)  
    .cs(cs),                   // Chip Select (Active LOW)
    .data_in(base_data_in),        // Data to transmit to master
    .miso(miso),            // Master In Slave Out
    .transfer_done(base_transfer_done),
    .data_out(base_data_out)   // Data received from master
  );

  always @(posedge clk) begin
    if (rst) begin
      transfer_count <= 0;
      internal_data <= 0;
      data_out <= 0;
    end else if (base_transfer_done) begin
      if (transfer_count == 2'b11) begin
        transfer_count <= 0;
        internal_data <= 0;
        data_out <= { internal_data, base_data_out };
      end else begin
        transfer_count <= transfer_count + 1;
        internal_data <= { internal_data[15:0], base_data_out };
      end
    end
  end
    
endmodule