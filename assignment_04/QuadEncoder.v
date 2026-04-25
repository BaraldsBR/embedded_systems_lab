module QuadEncoder #(
    parameter STEPS = 1000
) (
    input rst,
    input clk,

    input signal_A,
    input signal_B,
    output wire [$clog2(STEPS)+2:0] pos_out // create a counter with ceil(log2(STEPS)) + 3 bits
);
    reg old_A;
    reg old_B; 
    reg signed [$clog2(STEPS)+2:0] position;
    assign pos_out = position;

    wire rising_A  = !old_A && signal_A;
    wire falling_A = old_A  && !signal_A; 
    wire rising_B  = !old_B && signal_B;
    wire falling_B = old_B  && !signal_B;
    
    wire increment = (rising_A  &&  signal_B)
                   || (falling_B &&  signal_A)
                   || (falling_A && !signal_B)
                   || (rising_B  && !signal_A);
    
    wire decrement = (rising_A  && !signal_B)
                   || (rising_B  &&  signal_A)
                   || (falling_A &&  signal_B)
                   || (falling_B && !signal_A);
                   
    always @(posedge clk) begin
        if(!rst) begin
            position <= 0;
            old_A <= signal_A;
            old_B <= signal_B;
        end else begin
            if (increment) begin
                position <= position + 1;
            end
            if (decrement) begin
                position <= position - 1;
            end

            old_A <= signal_A;
            old_B <= signal_B;
        end
    end

endmodule
