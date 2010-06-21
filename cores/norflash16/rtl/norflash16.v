/*
 * Milkymist VJ SoC
 * Copyright (C) 2007, 2008, 2009, 2010 Sebastien Bourdeauducq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

module norflash16 #(
	parameter adr_width = 22,
	parameter timing = 4'd12
) (
	input sys_clk,
	input sys_rst,

	input [31:0] wb_adr_i,
	output reg [31:0] wb_dat_o,
	input wb_stb_i,
	input wb_cyc_i,
	output reg wb_ack_o,
	
	output [adr_width-1:0] flash_adr,
	input [15:0] flash_d
);

reg [adr_width-1-1:0] flash_adr_msb;
reg flash_adr_lsb;

assign flash_adr = {flash_adr_msb, flash_adr_lsb};

reg load;
always @(posedge sys_clk) begin
	/* Use IOB registers to prevent glitches on address lines */
	if(wb_cyc_i & wb_stb_i) /* register only when needed to reduce EMI */
		flash_adr_msb <= wb_adr_i[adr_width-1:2];
	if(load) begin
		case(flash_adr_lsb)
			1'b0: wb_dat_o[31:16] <= {flash_d[7:0], flash_d[15:8]};
			1'b1: wb_dat_o[15:0] <= {flash_d[7:0], flash_d[15:8]};
		endcase
		flash_adr_lsb <= ~flash_adr_lsb;
	end
	if(sys_rst)
		flash_adr_lsb <= 1'b0;
end

/*
 * Timing of the flash chips is typically 110ns.
 */
reg [3:0] counter;
reg counter_en;
wire counter_done = (counter == timing);
always @(posedge sys_clk) begin
	if(sys_rst)
		counter <= 4'd0;
	else begin
		if(counter_en & ~counter_done)
			counter <= counter + 4'd1;
		else
			counter <= 4'd0;
	end
end

reg [1:0] state;
reg [1:0] next_state;
always @(posedge sys_clk) begin
	if(sys_rst)
		state <= 1'b0;
	else
		state <= next_state;
end

always @(*) begin
	next_state = state;
	counter_en = 1'b0;
	load = 1'b0;
	wb_ack_o = 1'b0;

	case(state)
		2'd0: begin
			if(wb_cyc_i & wb_stb_i)
				next_state = 2'd1;
		end

		2'd1: begin
			counter_en = 1'b1;
			if(counter_done) begin
				load = 1'b1;
				if(flash_adr_lsb)
					next_state = 2'd2;
			end
		end

		2'd2: begin
			wb_ack_o = 1'b1;
			next_state = 2'd0;
		end
	endcase
end

endmodule
