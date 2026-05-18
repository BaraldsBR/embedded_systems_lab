`timescale 1 ps / 1 ps
module bus #(
		parameter LED_WIDTH = 8,
        parameter DATA_WIDTH = 32
	) (
		input  wire [7:0]  slave_address,     //      avs_s0.address
		input  wire        slave_read,        //            .read
		output reg  [DATA_WIDTH-1:0] slave_readdata,    //            .readdata
		input  wire        slave_write,       //            .write
		input  wire [DATA_WIDTH-1:0] slave_writedata,   //            .writedata
		input  wire        clk,          //       clock.clk
		input  wire        reset,        //       reset.reset
        input  wire [(DATA_WIDTH/8)-1:0] slave_byteenable,
        input  wire        signal_reset,
        output wire        signal_pitch_dir_A,
        output wire        signal_pitch_dir_B,
        output wire        signal_pitch_pwm_val,
        output wire        signal_yaw_dir_A,
        output wire        signal_yaw_dir_B,
        output wire        signal_yaw_pwm_val,
		output wire [LED_WIDTH-1:0]  LED         // user_output.new_signal
	);

    reg  [31:0] mem;
    wire [15:0] target_pitch = mem[15:0];
    wire [15:0] target_yaw = mem[31:16];
    assign LED = {signal_pitch_dir_A, signal_pitch_dir_B, 4'b0, signal_yaw_dir_A, signal_yaw_dir_B}

    pwm pitch_pwm (
        .rst(signal_reset),
        .clk(clk),
        .target_value(target_pitch),
        .DIR_A(signal_pitch_dir_A),
        .DIR_B(signal_pitch_dir_B),
        .PWM_VAL(signal_pitch_pwm_val)
    );
    
    pwm yaw_pwm (
        .rst(signal_reset),
        .clk(clk),
        .target_value(target_yaw),
        .DIR_A(signal_yaw_dir_A),
        .DIR_B(signal_yaw_dir_B),
        .PWM_VAL(signal_yaw_pwm_val)
    );

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            mem <= 32'b0;
        end else begin
            if (slave_read) begin
                slave_readdata <= mem;
            end
            if (slave_write) begin
                mem <= slave_writedata;
            end
        end;
    end



endmodule
