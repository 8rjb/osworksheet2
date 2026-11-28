#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

#define FB_GREEN 2
#define FB_DARK_GREY 8 /* define colour constants */

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);

void fb_move_cursor(unsigned short pos);

int write(char *buf, unsigned int len);

/* Move the cursor to column x and row y. The top-left cell is (0, 0). */
void fb_move(unsigned short x, unsigned short y);

/* Set the foreground (text) and background colours for subsequent output. */
void fb_set_colour(unsigned char fg, unsigned char bg);

/* Clear the screen and return the cursor to the top-left cell. */
void fb_clear(void);

/* Print a string from the current cursor position, stopping at '\0'. */
void fb_print(char *text);

/* Print an unsigned integer as decimal digits at the current cursor position. */
void fb_print_number(unsigned int number);

#endif