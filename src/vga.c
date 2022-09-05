#include "vga.h"
#include "io_ports.h"
#include "types.h"
#include "console.h"

/**
 * 16 bit video buffer elements(register ax)
 * 8 bits(ah) higher : 
 * lower 4 bits - forec olor
 * higher 4 bits - back color

 * 8 bits(al) lower :
 * 8 bits : ASCII character to print
 * 
 * returns complete item with fore & back color to be placed at VGA address
*/
uint16 vga_item_entry(uint8 ch, VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color) {
    uint16 ax = 0;
    uint8 ah = 0, al = 0;

    ah = back_color;
    ah <<= 4;
    ah |= fore_color;
    ax = ah;
    ax <<= 8;
    al = ch;
    ax |= al;

    return ax;
}

/**
 * set cursor position to given (x, y)
 * by writing to CRT controller registers
 */
void vga_set_cursor_pos(uint8 x, uint8 y) {
    // The screen is 80 characters wide...
    uint16 cursorLocation = y * VGA_WIDTH + x;
    outportb(0x3D4, 14);
    outportb(0x3D5, cursorLocation >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, cursorLocation);
}

/**
 * disable blinking top-left cursor
 * by writing to CRT controller registers
 */
void vga_disable_cursor() {
    outportb(0x3D4, 0x0A);
	outportb(0x3D5, 0x20);
}

void vga_enable_cursor(uint8 cursor_start, uint8 cursor_end)
{
	outportb(0x3D4, 0x0A);
	outportb(0x3D5, (inportb(0x3D5) & 0xC0) | cursor_start);
 
	outportb(0x3D4, 0x0B);
	outportb(0x3D5, (inportb(0x3D5) & 0xE0) | cursor_end);
}

uint16 vga_get_cursor_position(void)
{
    uint16 pos = 0;
    outportb(0x3D4, 0x0F);
    pos |= inportb(0x3D5);
    outportb(0x3D4, 0x0E);
    pos |= ((uint16)inportb(0x3D5)) << 8;
    return pos;
}

uint16 vga_get_cursor_pos_x(){
    uint16 pos = 0;
    outportb(0x3D4, 0x0F);
    pos |= inportb(0x3D5);
    outportb(0x3D4, 0x0E);
    pos |= ((uint16)inportb(0x3D5)) << 8;
    pos = pos / VGA_WIDTH;
    return pos;
}

uint16 vga_get_cursor_pos_y(){
    uint16 pos = 0;
    outportb(0x3D4, 0x0F);
    pos |= inportb(0x3D5);
    outportb(0x3D4, 0x0E);
    pos |= ((uint16)inportb(0x3D5)) << 8;
    pos = pos % VGA_WIDTH;
    return pos;
}