module spi (
  input             clk,
  input             SPI_CLK,
  input             SPI_PICO,
  input             SPI_CS,
  input      [31:0] tx_buf,
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

  // Receive data from SPI
  reg [2:0] bitcnt;
  reg [1:0] bytecnt;
  reg data_received_ready;
  reg [6:0] data_received;

  always @(posedge clk) begin
    if (~SPI_CS_active) begin 
      bitcnt <= 3'b000;
      bytecnt <= 2'b00;
    end
    else if (SPI_CLK_risingedge) begin
      if (bitcnt == 3'b111) begin
        bitcnt <= 3'b000;
        bytecnt <= bytecnt + 2'b01;
        rx_buf <= { rx_buf[23:0], data_received[6:0], SPI_PICO_data };
      end else begin
        bitcnt <= bitcnt + 3'b001;
      end
      data_received <= {data_received[5:0], SPI_PICO_data};
    end
  end

  always @(posedge clk) begin
    if(SPI_CS_startmessage) begin
      ready <= 1'b0;
    end
    else if (bytecnt == 2'b11 && bitcnt == 3'b111) begin
      ready <= 1'b1;
    end
  end

  // Send addition back over SPI
  reg [31:0] data_sent;

  always @(posedge clk)
    if (SPI_CS_active) begin
      if (SPI_CS_startmessage) begin
        data_sent <= tx_buf;
      end
      else if (SPI_CLK_fallingedge) begin
        data_sent <= {data_sent[30:0], 1'b0};
      end
    end

  assign SPI_POCI = data_sent[31];

endmodule
