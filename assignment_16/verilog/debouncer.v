module debouncer #(
  parameter DEBOUNCE_CYCLES = 10
) (
  input      clk,
  input      rst,
  input      signal,
  output reg debounced
);
  reg [$clog2(DEBOUNCE_CYCLES)-1:0] counter;

  always @(posedge clk) begin
    if (rst) begin
      debounced <= signal;
      counter <= 0;
    end else begin
      if (signal == debounced) begin
        counter <= 0;
        debounced <= debounced;
      end else begin
        if (counter >= DEBOUNCE_CYCLES) begin
          debounced <= signal;
          counter <= 0;
        end else begin
          debounced <= debounced;
          counter <= counter + 1;
        end
      end
    end
  end
endmodule