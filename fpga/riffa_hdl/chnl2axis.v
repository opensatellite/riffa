//----------------------------------------------------------------------------
// Filename:			chnl2axis.v
// Version:				1.00.a
// Verilog Standard:	Verilog-2001
// Description:			Convert RIFFA channel to axi-stream interface
// Author:				WarMonkey
// History:				
//-----------------------------------------------------------------------------
`timescale 1ns/1ns
module chnl2axis #(
	parameter C_PCI_DATA_WIDTH = 9'd32,
	parameter DEBUG = 0
)
(
    (* X_INTERFACE_PARAMETER = "ASSOCIATED_BUSIF s_axis_in:m_axis_out, ASSOCIATED_RESET RST, FREQ_HZ 250000000" *)
     (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 CLK CLK" *)
	input CLK,
	input RST,
         
	input CHNL_RX, 
	output reg CHNL_RX_ACK, 
	input CHNL_RX_LAST, 
	input [31:0] CHNL_RX_LEN, 
	input [30:0] CHNL_RX_OFF, 
	input [C_PCI_DATA_WIDTH-1:0] CHNL_RX_DATA, 
	input CHNL_RX_DATA_VALID, 
	output CHNL_RX_DATA_REN,
	
	output reg CHNL_TX, 
	input CHNL_TX_ACK, 
	output CHNL_TX_LAST, 
	output [31:0] CHNL_TX_LEN, 
	output [30:0] CHNL_TX_OFF, 
	output [C_PCI_DATA_WIDTH-1:0] CHNL_TX_DATA, 
	output CHNL_TX_DATA_VALID, 
	input  CHNL_TX_DATA_REN,

	output [C_PCI_DATA_WIDTH-1:0] m_axis_out_tdata,
	output m_axis_out_tlast,
	output m_axis_out_tvalid,
	input  m_axis_out_tready,
	output [32:0] m_axis_out_tuser,	//{rx_last, packet len}

	input  [C_PCI_DATA_WIDTH-1:0] s_axis_in_tdata,
	input  s_axis_in_tlast,
	input  s_axis_in_tvalid,
	output s_axis_in_tready,
	input  [32:0] s_axis_in_tuser	//{tx_last, packet len}

	//output err_out_tlast_missing,
	//output err_out_tlast_unexpected,
	//output err_in_tlast_missing,
	//output err_in_tlast_unexpected
);

reg [1:0] rx_state;
reg rx_pkt_last;
reg [31:0] rx_len, rx_cnt;

reg [1:0] tx_state;
reg tx_pkt_last;
reg [31:0] tx_len, tx_cnt;

assign m_axis_out_tlast = (rx_cnt + (C_PCI_DATA_WIDTH/32)) == rx_len;
assign CHNL_RX_DATA_REN = m_axis_out_tready && (rx_state != 0);
assign m_axis_out_tdata = CHNL_RX_DATA;
assign m_axis_out_tvalid = CHNL_RX_DATA_VALID && (rx_state != 0);
assign m_axis_out_tuser[31:0] = rx_len;
assign m_axis_out_tuser[32] = rx_pkt_last;

always @(posedge CLK or posedge RST) begin
	if (RST) begin
		CHNL_RX_ACK <= 0;
		rx_state <= 0;
		rx_pkt_last <= 0;
		rx_len <= 0;
		rx_cnt <= 0;
	end else begin
		case (rx_state)
			0: if(CHNL_RX && m_axis_out_tready) begin	//wait for rx
				CHNL_RX_ACK <= 1;
				rx_len <= CHNL_RX_LEN;
				rx_pkt_last <= CHNL_RX_LAST;
				rx_cnt <= 0;
				rx_state <= 1;
			end
			1: begin	//rx running
				CHNL_RX_ACK <= 0;
				if(!CHNL_RX) rx_state <= 2;
				if(CHNL_RX_DATA_VALID & CHNL_RX_DATA_REN) begin
					if(rx_cnt + (C_PCI_DATA_WIDTH/32) >= rx_len) begin
						rx_state <= 0;
					end else begin
						rx_cnt <= rx_cnt + (C_PCI_DATA_WIDTH/32);
					end
				end
			end 
			2: begin	//rx stopping
				if(CHNL_RX_DATA_VALID & CHNL_RX_DATA_REN) begin
                    if(rx_cnt + (C_PCI_DATA_WIDTH/32) >= rx_len) begin
                        rx_state <= 0;
                    end else begin
                        rx_cnt <= rx_cnt + (C_PCI_DATA_WIDTH/32);
                    end
                end
			end
			default: rx_state <= 0;
		endcase
	end
end

assign CHNL_TX_OFF = 0;
assign CHNL_TX_LAST = tx_pkt_last;
assign CHNL_TX_LEN = tx_len;
reg [C_PCI_DATA_WIDTH-1:0] tx_data1;
reg tx_data1_vld;
assign CHNL_TX_DATA = tx_data1_vld ? tx_data1 : s_axis_in_tdata;
assign CHNL_TX_DATA_VALID =  tx_data1_vld || (s_axis_in_tvalid && (tx_state == 2));
assign s_axis_in_tready = (tx_state == 0) || (CHNL_TX_DATA_REN && (tx_state == 2) && ~tx_data1_vld);

always @(posedge CLK or posedge RST) begin
	if (RST) begin
		CHNL_TX <= 0;
		tx_data1 <= 0;
		tx_data1_vld <= 0;
		tx_len <= 0;
		//tx_len <= 4096;
		tx_pkt_last <= 0;
		//tx_pkt_last <= 1;
		tx_cnt <= 0;
		tx_state <= 0;
	end else begin
		case (tx_state)
		  0: begin
              if(s_axis_in_tvalid) begin	//tx pending
                CHNL_TX <= 1;
                tx_data1 <= s_axis_in_tdata;
                tx_data1_vld <= 1;
                tx_len <= s_axis_in_tuser[31:0];
                tx_pkt_last <= s_axis_in_tuser[32];
                tx_cnt <= 0;
                tx_state <= 1;
                //if(tx_cnt >= tx_len) 
                //s_axis_in_tlast expected here
              end
		  end
		  1: begin	//wait for tx ack
              if(CHNL_TX_ACK) begin
			      tx_state <= 2;
		      end
		  end
		  2: begin	//tx running
			if((CHNL_TX_DATA_VALID || tx_data1_vld) && CHNL_TX_DATA_REN) begin
			    if(tx_data1_vld) tx_data1_vld <= 0;
				if(tx_cnt + (C_PCI_DATA_WIDTH/32) >= tx_len) begin //transfer complete
					CHNL_TX <= 0;
					tx_state <= 0;
					//s_axis_in_tlast expected here
				end else begin //transfer until stop
					tx_cnt <= tx_cnt + (C_PCI_DATA_WIDTH/32);
				end
			end 
		  end
		  default: tx_state <= 0;
	    endcase
	end
end

generate if(DEBUG) begin
    chnl_ila ila0_inst(
        .clk(CLK),
        .probe0(CHNL_RX),
        .probe1(CHNL_RX_ACK),
        .probe2(CHNL_RX_LAST),
        .probe3(CHNL_RX_LEN),
        .probe4(CHNL_RX_OFF),
        .probe5(CHNL_RX_DATA),
        .probe6(CHNL_RX_DATA_VALID),
        .probe7(CHNL_RX_DATA_REN),
        .probe8(rx_state),
        .probe9(rx_cnt),
        .probe10(m_axis_out_tready)
    );
    
    chnl_ila ila1_inst(
        .clk(CLK),
        .probe0(CHNL_TX),
        .probe1(CHNL_TX_ACK),
        .probe2(CHNL_TX_LAST),
        .probe3(CHNL_TX_LEN),
        .probe4(CHNL_TX_OFF),
        .probe5(CHNL_TX_DATA),
        .probe6(CHNL_TX_DATA_VALID),
        .probe7(CHNL_TX_DATA_REN),
        .probe8(tx_state),
        .probe9(tx_cnt),
        .probe10(s_axis_in_tvalid)
    );
end endgenerate

endmodule
