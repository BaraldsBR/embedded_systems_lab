module TopEntity (
    input clk,
    output reg led1 = 0,
    output reg led2 = 0,
    output reg led3 = 0
);
  reg [31:0] count = 0;
  always @(posedge clk) begin
    if (count == 0) begin 
      led1 <= ~led1;
    end
    if (count == 33333333) begin 
      led2 <= ~led2;
    end
    if (count == 66666666) begin 
      led3 <= ~led3;
    end

    if (count == 99999999) begin  //Time is up
      count <= 0;  //Reset count register
    end else begin
      count <= count + 1;  //Counts 100MHz clock
    end
  end
endmodule
