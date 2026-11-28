#include "../drivers/framebuffer.h"

int sum_of_three(int arg1, int arg2, int arg3)
{
    return arg1 + arg2 + arg3; /* compiler returns this int in EAX */
}

int difference(int left, int right)
{
    return left - right;
}

int square(int value)
{
    return value * value; 
}

void framebuffer_test_old(void)
{
    /* Page 25's example call. */
    //fb_write_cell(0, 'A', FB_GREEN, FB_DARK_GREY);

    /* Page 25's example position: first column of the second row. */
    //fb_move_cursor(80);

    write("AB", 2); // write 2 characters 
}

void framebuffer_test12345(void)
{
    /* This message should disappear when the screen is cleared. */
    fb_print("This text should be cleared");

    /* White text on blue. Clearing applies the blue background to every cell. */
    fb_set_colour(15, 1);
    fb_clear();

    /* fb_clear() returned the cursor to the top-left corner. */
    fb_print("Task 3");

    /* Column 5, row 2 is cell (2 * 80) + 5 = 165. */
    fb_move(5, 2);
    fb_print("Cell ");
    fb_print_number(165);

    /* Green text on the same blue background. */
    fb_set_colour(FB_GREEN, 1);
    fb_move(5, 4);
    fb_print("Square of 9 = ");
    fb_print_number(square(9));

    /* Put a label at the start of the bottom row. */
    fb_move(0, 24);
    fb_print("This row should move up");

    /* Start at the final cell on the screen. */
    fb_move(79, 24);

    /* Pause so the bottom row is visible before X triggers scrolling. */
    for (volatile unsigned int i = 0; i < 100000000U; i++) {
    }
    
    fb_print("XY");

    
}


void framebuffer_test(void)
{
    /* This message disappears when the screen is cleared. */
    fb_print("This text should be cleared");

    /* Clear the screen to blue and print the heading in white. */
    fb_set_colour(15, 1);
    fb_clear();
    fb_print("Task 3");

    /* Column 5, row 2 is cell (2 * 80) + 5 = 165. */
    fb_move(5, 2);
    fb_print("Cell ");
    fb_print_number(165);

    /* Switch to green text and keep the blue background. */
    fb_set_colour(FB_GREEN, 1);
    fb_move(5, 4);
    fb_print("Square of 9 = ");
    fb_print_number(square(9));
}