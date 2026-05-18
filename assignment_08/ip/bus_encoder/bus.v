`timescale 1 ps / 1 ps
module bus #(
		parameter LED_WIDTH = 8,
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
        input  wire        signal_pitch_A,
        input  wire        signal_pitch_B,
        input  wire        signal_yaw_A,
        input  wire        signal_yaw_B,
        input  wire        signal_reset,
		output wire [LED_WIDTH-1:0]  LED         // user_output.new_signal
	);

    reg         [31:0]          mem;
    wire signed [POS_WIDTH-1:0] pitch_out;
    wire signed [POS_WIDTH-1:0] yaw_out;

    wire debounced_pitch_A;
    wire debounced_pitch_B;
    wire debounced_yaw_A;
    wire debounced_yaw_B;

    debouncer #(
        .DEBOUNCE_CYCLES(DEBOUNCE_CYCLES)
    ) debouncer_pitch_A (
        .clk(clk),
        .rst(signal_reset),
        .signal(signal_pitch_A),
        .debounced(debounced_pitch_A)
    );

    debouncer #(
        .DEBOUNCE_CYCLES(DEBOUNCE_CYCLES)
    ) debouncer_pitch_B (
        .clk(clk),
        .rst(signal_reset),
        .signal(signal_pitch_B),
        .debounced(debounced_pitch_B)
    );

    debouncer #(
        .DEBOUNCE_CYCLES(DEBOUNCE_CYCLES)
    ) debouncer_yaw_A (
        .clk(clk),
        .rst(signal_reset),
        .signal(signal_yaw_A),
        .debounced(debounced_yaw_A)
    );

    debouncer #(
        .DEBOUNCE_CYCLES(DEBOUNCE_CYCLES)
    ) debouncer_yaw_B (
        .clk(clk),
        .rst(signal_reset),
        .signal(signal_yaw_B),
        .debounced(debounced_yaw_B)
    );
    
    encoder #(
        .POS_WIDTH(POS_WIDTH)
    ) pitch_encoder (
        .rst(signal_reset),
        .clk(clk),
        .signal_A(debounced_pitch_A),
        .signal_B(debounced_pitch_B),
        .pos_out(pitch_out) 
    );
    
    encoder #(
        .POS_WIDTH(POS_WIDTH)
    ) yaw_encoder (
        .rst(signal_reset),
        .clk(clk),
        .signal_A(debounced_yaw_A),
        .signal_B(debounced_yaw_B),
        .pos_out(yaw_out) 
    );

    assign LED = { pitch_out[3:0], yaw_out[3:0] };

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
