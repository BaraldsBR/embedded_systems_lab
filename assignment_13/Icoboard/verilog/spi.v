module spi #(
    parameter WIDTH = 32
)(
  input                  clk,        
  input                  rst,
  input                  mosi,           
  input                  sck,           
  input                  cs,             
  input      [WIDTH-1:0] data_in,

  output                 miso,
  output reg             new_data,
  output reg [WIDTH-1:0] data_out
);

  // stabilise cs and sck with 2 ffs 
  reg [1:0] cs_reg;
  reg [2:0] sck_reg;
  always @(posedge clk) begin
    if (rst) begin
      cs_reg  <= 2'b11;
      sck_reg <= 3'b000;
    end else begin
      cs_reg  <= {cs_reg[0], cs};
      sck_reg <= {sck_reg[1:0], sck};
    end
  end
  wire sck_re = (sck_reg==3'b011);
  wire sck_fe = (sck_reg==3'b100);
  wire cs_syn = cs_reg[1];

  // FSM
  localparam IDLE     = 1'b0;
  localparam TRANSFER = 1'b1;
  reg state;
  reg next_state;
  always @(posedge clk) begin
    if (rst) state <= IDLE;
    else     state <= next_state;
  end

  // next state block
  always @(*) begin
    case (state)
      IDLE:     next_state = (!cs_syn) ? TRANSFER : IDLE;
      TRANSFER: next_state = (cs_syn)  ? IDLE : TRANSFER;
      default:  next_state = IDLE;
    endcase
  end

  // datapath sortof
  reg [WIDTH-1:0] tx_reg, rx_reg;

  always @(posedge clk) begin
    if (rst) begin
      tx_reg   <= 0;
      rx_reg   <= 0;
      data_out <= 0;
      new_data <= 1'b0;
    end else begin
      case (state)
        IDLE: begin
          if (cs_syn) 
            new_data <= 0;
          else if (!cs_syn)
            tx_reg <= data_in;
        end

        TRANSFER: begin
          if (!cs_syn) begin
            rx_reg <= (sck_re)? {rx_reg[WIDTH-2:0], mosi} : rx_reg;
            tx_reg <= (sck_fe)? {tx_reg[WIDTH-2:0], 1'b0} : tx_reg;  
          end else if (cs_syn) begin
            new_data <= 1;
            data_out <= rx_reg;
          end
        end

        default: ;
      endcase
    end
  end

  assign miso = tx_reg[WIDTH-1];
endmodule