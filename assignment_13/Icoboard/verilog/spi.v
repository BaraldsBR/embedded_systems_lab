module spi (
  input             clk,
  input             SPI_CLK,
  input             SPI_PICO,
  input             SPI_CS,
  input      [31:0] tx_buf,
  input             rst,
  output            SPI_POCI,
  output reg        ready,
  output reg [31:0] rx_buf
);

  reg [2:0] SPI_CLKr;
  always @(posedge clk) SPI_CLKr <= {SPI_CLKr[1:0], SPI_CLK};
  wire SPI_CLK_risingedge = (SPI_CLKr[2:1] == 2'b01);
  wire SPI_CLK_fallingedge = (SPI_CLKr[2:1] == 2'b10);

  reg [2:0] SPI_CSr;
  always @(posedge clk) SPI_CSr <= {SPI_CSr[1:0], SPI_CS};
  wire SPI_CS_active = ~SPI_CSr[1];
  wire SPI_CS_startmessage = (SPI_CSr[2:1] == 2'b10); // when chip gets selected
  wire SPI_CS_endmessage = (SPI_CSr[2:1] == 2'b01); // when chip gets deselected

  reg [1:0] SPI_PICOr;
  always @(posedge clk) SPI_PICOr <= {SPI_PICOr[0], SPI_PICO};
  wire SPI_PICO_data = SPI_PICOr[1];

  reg [2:0] bitcnt;
  reg [1:0] bytecnt;
  reg [6:0] data_received;

  reg [31:0] data_sent;

  assign SPI_POCI = data_sent[31];
  
  always @(posedge clk) begin
    if (rst) begin
      SPI_CLKr <= 3'b0;
      SPI_CSr <= 3'b0;
      SPI_PICOr <= 2'b0;
      
      ready <= 1'b0;
      rx_buf <= 32'b0;

      bitcnt <= 3'b000;
      bytecnt <= 2'b00;     
      data_received <= 7'b0;
      data_sent <= 32'b0;
    end else begin
      // Receive data from SPI
      if (~SPI_CS_active) begin 
        bitcnt <= 3'b000;
        bytecnt <= 2'b00;
      end
      else if (SPI_CS_startmessage) begin
        ready <= 0;
      end
      else if (SPI_CLK_risingedge && !ready) begin
        if (bitcnt == 3'b111) begin
          if (bytecnt == 2'b11) begin
            ready <= 1;
          end
          bytecnt <= bytecnt + 2'b01;
        end

        bitcnt <= bitcnt + 3'b001;
        rx_buf <= { rx_buf[30:0], SPI_PICO_data };
      end
      

      // Send addition back over SPI
      if (SPI_CS_active) begin
        if (SPI_CS_startmessage) begin
          data_sent <= tx_buf;
        end
        else if (SPI_CLK_fallingedge) begin
          data_sent <= {data_sent[30:0], 1'b0};
        end
      end
    end
  end

endmodule
