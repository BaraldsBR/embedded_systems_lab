module QuadEncoderDE10 (
    input            FPGA_CLK1_50,
    input      [3:0] SW,
    output reg [7:0] LED
);
  wire signed [7:0] pos_out;

  QuadEncoder #(.STEPS(25)) dut (
    .clk(FPGA_CLK1_50),
    .rst(SW[3]),
    .signal_A(SW[0]),
    .signal_B(SW[1]),
    .pos_out(pos_out)
  );

  always @(posedge FPGA_CLK1_50) begin
    LED <= pos_out;
  end

endmodule