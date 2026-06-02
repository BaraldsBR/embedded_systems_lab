module pwm #(
    parameter PWM_CYCLE_LENGTH = 2500
) (
    input rst,
    input clk,
    input [15:0] target_value,
    
    output dir_A,
    output dir_B,
    output PWM_VAL
);
    reg  [11:0] counted_cycles;
    wire        target_zero = (target_value == 16'b0);
    wire        target_sign = target_value[15];
    wire [15:0] absolute_target = (target_sign) ? (-target_value) : target_value;

    always @(posedge clk) begin
        if(!rst) begin
            counted_cycles <= 0;
        end else begin
            if (counted_cycles >= PWM_CYCLE_LENGTH) begin
                counted_cycles <= 0;
            end else begin
                counted_cycles <= counted_cycles + 1;
            end
        end
    end

    assign dir_A = (target_zero) ? 0 : ~target_sign;
    assign dir_B = (target_zero) ? 0 : target_sign;

    assign PWM_VAL = counted_cycles < absolute_target;
    
endmodule
