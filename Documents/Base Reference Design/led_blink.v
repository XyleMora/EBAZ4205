`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// https://github.com/botnursery/EBAZ4205-Minimum-Board-Operating-Configuration
// Create Date: 06.07.2023 16:45:43
// Design Name: 
// Module Name: led_blink
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// v.1
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// LEDs control and blink module fot EBAZ4205
//////////////////////////////////////////////////////////////////////////////////

module led_blink(
     input clk,
     input rst_n,
     input [1:0] led_in,
     output [1:0] led_out
    );
    
 parameter T1MS = 26'd50_000_000 ; //50MHz
 reg [25:0]time_count;//timer counter
 reg led_g;
 always@(posedge clk or negedge rst_n)
     if(!rst_n)begin
        time_count<=26'd0;
        led_g<=1'b0;
     end
     else begin
         if(time_count>=T1MS)begin
             time_count<=26'd0;
             led_g<=~led_g;
         end
         else time_count<=time_count+1'b1;
     end
 assign led_out[0]=led_in[0]; // Red
 assign led_out[1]=led_g&led_in[1]; //Green
// initial state - red lights with 0 and green blink enabled with 1
// AXI default output value should be 0x00000002
endmodule
