`timescale 1 ps / 1 ps
module bus #(
		parameter LED_WIDTH = 8,
        parameter DATA_WIDTH = 32
        parameter POS_WIDTH = 16
	) (
		input  wire [7:0]  slave_address,     //      avs_s0.address
		input  wire        slave_read,        //            .read
		output reg  [DATA_WIDTH-1:0] slave_readdata,    //            .readdata
		input  wire        slave_write,       //            .write
		input  wire [DATA_WIDTH-1:0] slave_writedata,   //            .writedata
		input  wire        clk,          //       clock.clk
		input  wire        reset,        //       reset.reset
        input  wire [(DATA_WIDTH/8)-1:0] slave_byteenable,
        input  wire        signal_A,
        input  wire        signal_B,
		output wire [LED_WIDTH-1:0]  LED         // user_output.new_signal
	);

    reg         [31:0]          mem;
    wire signed [POS_WIDTH-1:0] pos_out;

    encoder #(
        .POS_WIDTH(POS_WIDTH)
    ) my_ip (
        .rst(reset),
        .clk(clk),
        .signal_A(signal_A),
        .signal_B(signal_B),
        .pos_out(pos_out) 
    );

    assign LED = pos_out[LED_WIDTH-1:0];

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            mem <= 32'b0;
        end else begin
            if (slave_read) begin
                slave_readdata <= pos_out;
            end
            if (slave_write) begin
                mem <= slave_writedata;
            end
        end;
    end



endmodule