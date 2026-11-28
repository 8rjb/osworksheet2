#include "framebuffer.h"
#include "io.h"

/* VGA text-mode screen dimensions. Each cell occupies two bytes. */
#define FB_COLUMNS 80
#define FB_ROWS    25
#define FB_CELLS   (FB_COLUMNS * FB_ROWS)

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/* Start of VGA text memory. fb[0] and fb[1] belong to the top-left cell. */
char *fb = (char *) 0x000B8000;

/* Cursor position tracked by the driver, counted in cells from the top left. */
static unsigned short cursor_pos = 0;

/* Initial values reproduce the book's 0x28 colour attribute. */
static unsigned char foreground = FB_DARK_GREY;
static unsigned char background = FB_GREEN;

/* fb_write_cell:
 * Writes a character with the given foreground and background to position i
 * in the framebuffer.
 *
 * @param i The location in the framebuffer
 * @param c The character
 * @param fg The foreground color
 * @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
    /* i is a byte offset. For cell number n, pass 2 * n. */
    fb[i] = c;                                      /* Character byte. */

    /* The foreground goes in the low four bits. The background starts at bit 4. */
    fb[i + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

/* fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos)
{
    /* pos is a cell number. Position 80 is the first cell of the second row.
     * The hardware accepts one byte at a time, so send pos in two parts.
     */
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND); /* Select high cursor byte. */
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));   /* Send high byte. */

    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);  /* Select low cursor byte. */
    outb(FB_DATA_PORT, pos & 0x00FF);            /* Send low byte. */
}

/* Write one character in the current colours. Multiplying the cell number
 * by two gives the byte offset expected by fb_write_cell.
 */
static void write_at(unsigned short pos, char c)
{
    /* Pass the colours in the same order as the fb_write_cell declaration. */
    fb_write_cell(pos * 2, c, foreground, background);
}

/* Shift the screen contents up one row and leave the cursor at the
 * start of the newly cleared bottom row.
 */
static void scroll(void)
{
    unsigned int i;
    unsigned int row_bytes = FB_COLUMNS * 2;  /* 80 cells × 2 bytes = 160 bytes. */

    /* Copy rows 1–24 up one row, including their colour bytes. */
    /* Start at the top so each source row is read before it is overwritten. */
    for (i = 0; i < (FB_ROWS - 1) * row_bytes; i++) {
        fb[i] = fb[i + row_bytes];
    }

    /* The old bottom row is now available for new text. */
    cursor_pos = (FB_ROWS - 1) * FB_COLUMNS;  /* Row 24 starts at cell 1920. */

    /* Clear cells 1920–1999 with spaces in the current colours.
     * Using i leaves cursor_pos at the first cell of the cleared row.
     */
    for (i = cursor_pos; i < FB_CELLS; i++) {
        write_at(i, ' ');
    }
}

/* Write len characters from buf, starting at the driver's current position.
 * The caller supplies the length, so buf does not need a terminating '\0'.
 */
int write(char *buf, unsigned int len)
{
    unsigned int i;

    for (i = 0; i < len; i++) {
        write_at(cursor_pos, buf[i]);
        cursor_pos++;

        /* After writing the bottom-right cell (1999), cursor_pos reaches
         * 2000. Scrolling moves it back to the start of the bottom row.
         */
        if (cursor_pos == FB_CELLS) {
            scroll();
        }

        /* Show the hardware cursor where the next character will be written. */
        fb_move_cursor(cursor_pos);
    }

    return len;  /* Number of characters written. */
}

/* Move the cursor to column x and row y, counting from (0, 0) at the top left. */
void fb_move(unsigned short x, unsigned short y)
{
    /* Leave the cursor where it is if either coordinate is outside the screen. */
    if (x >= FB_COLUMNS || y >= FB_ROWS) {
        return;
    }

    /* Skip y full rows, then count x cells across to get the cell index. */
    cursor_pos = y * FB_COLUMNS + x;

    /* Move the visible hardware cursor to the cell the driver now tracks. */
    fb_move_cursor(cursor_pos);
}

/* Set the colours used for subsequent writes to the framebuffer. */
void fb_set_colour(unsigned char fg, unsigned char bg)
{
    foreground = fg;
    background = bg;
}

/* Fill every cell with a space in the current colours, then move
 * the cursor back to the top-left corner.
 */
void fb_clear(void)
{
    unsigned short pos;

    for (pos = 0; pos < FB_CELLS; pos++) {
        write_at(pos, ' ');
    }

    fb_move(0, 0);
}

/* Print a string from the current cursor position. */
void fb_print(char *text)
{
    unsigned int len = 0;

    /* Count the characters up to the string's terminating null byte. */
    while (text[len] != '\0') {
        len++;
    }

    write(text, len);
}

/* Print an unsigned integer as decimal digits. */
void fb_print_number(unsigned int number)
{
    char digit;

    /* Print the digits before the final digit first. */
    if (number >= 10) {
        fb_print_number(number / 10);
    }

    /* Convert the final digit to its character and write one byte. */
    digit = '0' + (number % 10);
    write(&digit, 1);
}