`timescale 1 ps / 1 ps
module encoder_adapter #(
        parameter DATA_WIDTH = 32,
        parameter POS_WIDTH = 16,
        parameter DEBOUNCE_CYCLES = 1000
	) (
		input  wire [7:0]  slave_address,     //      avs_s0.address
		input  wire        slave_read,        //            .read
		output reg  [DATA_WIDTH-1:0] slave_readdata,    //            .readdata
		input  wire        slave_write,       //            .write
		input  wire [DATA_WIDTH-1:0] slave_writedata,   //            .writedata
		input  wire        clk,          //       clock.clk
		input  wire        reset,        //       reset.reset
        input  wire [(DATA_WIDTH/8)-1:0] slave_byteenable,
        input  wire        signal_pitch_enc_A,
        input  wire        signal_pitch_enc_B,
        input  wire        signal_yaw_enc_A,
        input  wire        signal_yaw_enc_B,
        output wire        signal_pitch_dir_A,
        output wire        signal_pitch_dir_B,
        output wire        signal_pitch_pwm_val,
        output wire        signal_yaw_dir_A,
        output wire        signal_yaw_dir_B,
        output wire        signal_yaw_pwm_val,
        input  wire        signal_reset
	);

    reg  [31:0] mem;
    wire [15:0] target_pitch = mem[31:16];
    wire [15:0] target_yaw = mem[15:0];

    pwm pitch_pwm (
        .rst(signal_reset),
        .clk(clk),
        .target_value(target_pitch),
        .dir_A(signal_pitch_dir_A),
        .dir_B(signal_pitch_dir_B),
        .PWM_VAL(signal_pitch_pwm_val)
    );
    
    pwm yaw_pwm (
        .rst(signal_reset),
        .clk(clk),
        .target_value(target_yaw),
        .dir_A(signal_yaw_dir_A),
        .dir_B(signal_yaw_dir_B),
        .PWM_VAL(signal_yaw_pwm_val)
    );

    wire signed [POS_WIDTH-1:0] pitch_out;
    wire signed [POS_WIDTH-1:0] yaw_out;

    wire debounced_pitch_enc_A;
    wire debounced_pitch_enc_B;
    wire debounced_yaw_enc_A;
    wire debounced_yaw_enc_B;

    debouncer #(
        .DEBOUNCE_CYCLES(DEBOUNCE_CYCLES)
    ) debouncer_pitch_A (
        .clk(clk),
        .rst(signal_reset),
        .signal(signal_pitch_enc_A),
        .debounced(debounced_pitch_enc_A)
    );

    debouncer #(
        .DEBOUNCE_CYCLES(DEBOUNCE_CYCLES)
    ) debouncer_pitch_B (
        .clk(clk),
        .rst(signal_reset),
        .signal(signal_pitch_enc_B),
        .debounced(debounced_pitch_enc_B)
    );

    debouncer #(
        .DEBOUNCE_CYCLES(DEBOUNCE_CYCLES)
    ) debouncer_yaw_A (
        .clk(clk),
        .rst(signal_reset),
        .signal(signal_yaw_enc_A),
        .debounced(debounced_yaw_enc_A)
    );

    debouncer #(
        .DEBOUNCE_CYCLES(DEBOUNCE_CYCLES)
    ) debouncer_yaw_B (
        .clk(clk),
        .rst(signal_reset),
        .signal(signal_yaw_enc_B),
        .debounced(debounced_yaw_enc_B)
    );
    
    encoder #(
        .POS_WIDTH(POS_WIDTH)
    ) pitch_encoder (
        .rst(signal_reset),
        .clk(clk),
        .signal_A(debounced_pitch_enc_A),
        .signal_B(debounced_pitch_enc_B),
        .pos_out(pitch_out) 
    );
    
    encoder #(
        .POS_WIDTH(POS_WIDTH)
    ) yaw_encoder (
        .rst(signal_reset),
        .clk(clk),
        .signal_A(debounced_yaw_enc_A),
        .signal_B(debounced_yaw_enc_B),
        .pos_out(yaw_out) 
    );

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            mem <= 32'b0;
        end else begin
            if (slave_read) begin
                slave_readdata <= { pitch_out, yaw_out };
            end
            if (slave_write) begin
                mem <= slave_writedata;
            end
        end;
    end



endmodule
