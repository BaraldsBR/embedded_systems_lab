
// source: https://github.com/Spurthy-S/SPI-Full-Duplex/blob/main/Source/slave.v

module spi_base (
  input clk,        
  input rst,
  input mosi,                 // Master Out Slave In
  input sck,                  // SPI clock from master (SCK)  
  input cs,                   // Chip Select (Active LOW)
  input [7:0] data_in,        // Data to transmit to master
  output reg miso,            // Master In Slave Out
  output reg transfer_done,
  output reg [7:0] data_out   // Data received from master
);

  parameter CPOL = 0;
  parameter CPHA = 0;

  localparam IDLE=0, TRANSFER=1, DONE=2;
  reg [1:0] state;

  reg [7:0] tx_reg, rx_reg;
  reg [2:0] bit_count;
  
  // after this pretend code is for CPOL=0
  wire sck_norm = (CPOL==0)? sck : !sck;

  reg [1:0] sck_sync;
  always @(posedge clk)
  begin
    if (rst) 
      sck_sync <= {1'b0, 1'b0};
    else 
      sck_sync <= {sck_sync[0], sck_norm};
  end

  // sample edge and pushing edge
  wire sck_se = (CPHA==0)? (sck_sync==2'b01) : (sck_sync==2'b10);
  wire sck_pe = (CPHA==0)? (sck_sync==2'b10) : (sck_sync==2'b01);

  // CS detect
  reg cs_d;
  always @(posedge clk)
  begin
    if(rst) 
      cs_d <= 1;
    else 
      cs_d <= cs;
  end
  
  wire cs_falling = (cs_d==1 && cs==0);

  always @(posedge clk) 
  begin
    if(rst) begin
      state <= IDLE;
      miso <= 0;
      tx_reg <= 0;
      rx_reg <= 0;
      bit_count <= 7;
      data_out <= 0;
      transfer_done <= 0;
    end else begin
      
      transfer_done <= 0;
    
      case(state)
        IDLE: begin
          if(!cs) begin
            tx_reg <= data_in;
            rx_reg <= 0;
            bit_count <= 7;
            transfer_done <= 0;
            state <= TRANSFER;
            if(!CPHA)
              miso <= data_in[7]; // preload
          end
        end

        TRANSFER: begin
          if (cs)
            state <= IDLE;
          else begin
      
            // SAMPLE
            if (sck_se) begin
              rx_reg <= {rx_reg[6:0], mosi};
              if (bit_count==0)
                state <= DONE;
              else
                bit_count <= bit_count-1;
            end

            // SHIFT
            if (sck_pe) begin
              tx_reg <= {tx_reg[6:0],1'b0};
              miso   <= tx_reg[7];
            end
          end
        end

        DONE: begin
          data_out <= rx_reg;
          transfer_done <= 1;
          bit_count <= 7;
          tx_reg <= 0;
          rx_reg <= 0;
          state <= IDLE;
        end
      endcase 
    end
  end
endmodule