module encoder #(
  parameter POS_WIDTH = 16
) (
  input rst,
  input clk,

  input signal_A,
  input signal_B,
  output reg [POS_WIDTH-1:0] pos_out
);
  reg old_A;
  reg old_B;

  wire rising_A  = !old_A && signal_A;
  wire falling_A = old_A  && !signal_A; 
  wire rising_B  = !old_B && signal_B;
  wire falling_B = old_B  && !signal_B;

  wire increment =  (rising_A  &&  signal_B)
                 || (falling_B &&  signal_A)
                 || (falling_A && !signal_B)
                 || (rising_B  && !signal_A);

  wire decrement =  (rising_A  && !signal_B)
                 || (rising_B  &&  signal_A)
                 || (falling_A &&  signal_B)
                 || (falling_B && !signal_A);

  always @(posedge clk) begin
    if (rst) begin
      pos_out <= 0;
      old_A <= signal_A;
      old_B <= signal_B;
    end else begin
      if (increment) pos_out <= pos_out + 1;
      if (decrement) pos_out <= pos_out - 1;

      old_A <= signal_A;
      old_B <= signal_B;
    end
  end
endmodule
