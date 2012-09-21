#include <stdlib.h>

#include "hw_md.h"

static int row;
static int column;
static int player;
static int board[3][3];
static char display_board[3][3];


void TicTacToe(void)
{
    int i, j;

    row = 0;
    column = 0;
    player = 1;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            board[i][j] = 0;
            display_board[i][j] = ' ';
        }
    }
}

int Pick_Player(void)
{
    return player;
}

int Pick_Row(void)
{
    return row;
}

int Pick_Column(void)
{
    return column;
}

void Choice_by_Player(int a)
{
    player = a;
}

void Choice_of_Row(int b)
{
    row = b;
}

void Choice_of_Column(int c)
{
    column = c;
}

int Check_Move(int row, int column)
{
    if ( board[row][column] != 0 )
    {
        put_str("Space occupied - Try Again", 0x4000, 20-13, 3);
        delay(120);
        put_str("                          ", 0x4000, 20-13, 3);
        return 0;
    }
    else
    {
        board[row][column] = player;
        return 1;
    }
}   // end of Check_Move

int Check_Board(void)
{
    int i = 0;

    // check for tie
    for (i = 0; i < 9; i++)
    {
        if (board[i/3][i%3] == 0)
            break;
    } // end of for loop
    if ( i == 9 )
        return 3; // tie game!

    // check rows
    if (( (board[0][0] == player) && (board[0][1] == player) && (board[0][2] == player) ) ||
        ( (board[1][0] == player) && (board[1][1] == player) && (board[1][2] == player) ) ||
        ( (board[2][0] == player) && (board[2][1] == player) && (board[2][2] == player) ))
        return player;

    // check columns
    if (( (board[0][0] == player) && (board[1][0] == player) && (board[2][0] == player) ) ||
        ( (board[0][1] == player) && (board[1][1] == player) && (board[2][1] == player) ) ||
        ( (board[0][2] == player) && (board[1][2] == player) && (board[2][2] == player) ))
        return player;

    // check diagonals
    if (( (board[0][0] == player) && (board[1][1] == player) && (board[2][2] == player) ) ||
        ( (board[0][2] == player) && (board[1][1] == player) && (board[2][0] == player) ))
        return player;

    return 0;
} // end of Check_Board

void Clear_Board(void)
{
    int i, j;

    row = 0;
    column = 0;
    player = 1;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            board[i][j] = 0;
            display_board[i][j] = ' ';
        }
    }
}

void Tic_Tac_Toe_Board(void)
{
    int row, column;

    put_str("\x80\x82\x82\x82\x88\x82\x82\x82\x88\x82\x82\x82\x85", 0x2000, 20-6, 6);
    put_str("\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 7);
    put_str("\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 8);
    put_str("\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 9);
    put_str("\x81\x82\x82\x82\x87\x82\x82\x82\x87\x82\x82\x82\x84", 0x2000, 20-6, 10);
    put_str("\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 11);
    put_str("\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 12);
    put_str("\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 13);
    put_str("\x81\x82\x82\x82\x87\x82\x82\x82\x87\x82\x82\x82\x84", 0x2000, 20-6, 14);
    put_str("\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 15);
    put_str("\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 16);
    put_str("\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 17);
    put_str("\x86\x82\x82\x82\x89\x82\x82\x82\x89\x82\x82\x82\x83", 0x2000, 20-6, 18);

    put_str("Current Player: ", 0x2000, 20-8, 21);
    put_chr((player == 1) ? 'X' : 'O', 0x0000, 20+8, 21);

    for ( row = 0; row < 3; row ++)
    {
        for ( column = 0; column < 3; column++)
        {
            if ( board[row][column] == 0)
            {
                display_board[row][column] = ' ';
            }
            if ( board[row][column] == 1)
            {
                display_board[row][column] = 'X';
            }
            if ( board[row][column] == 2)
            {
                display_board[row][column] = 'O';
            }
            put_chr(display_board[row][column], 0x0000, 20-6+2 + column*4, 6+2 + row*4);
        }  // end of inner for loop
    }  // end of outer for loop

}   // end of Tic_Tac_Toe_Board


int main(void)
{
    int test;
    int more = 1;
    int row = 0;
    int column= 0;
    int player;
    int check = 0;
    short buttons = 0;

    TicTacToe();

    while ( more )
    {
        Tic_Tac_Toe_Board();
        player = Pick_Player();

        put_str("Choose a square", 0x2000, 20-7, 3);
        while ( !(buttons & SEGA_CTRL_A) )
        {
            delay(2);
            buttons = get_pad(0);
            if ( buttons & SEGA_CTRL_C )
            {
                more = 0;
                break;
            }
        }
        while ( get_pad(0) & SEGA_CTRL_A ) ;
        put_str("               ", 0x2000, 20-7, 3);
        delay(30);

        if ( !more )
            break;

        switch ( buttons & 0x000F )
        {
            case SEGA_CTRL_UP:
            row = 0;
            column = 1;
            break;
            case SEGA_CTRL_DOWN:
            row = 2;
            column = 1;
            break;
            case SEGA_CTRL_LEFT:
            row = 1;
            column = 0;
            break;
            case SEGA_CTRL_UP+SEGA_CTRL_LEFT:
            row = 0;
            column = 0;
            break;
            case SEGA_CTRL_DOWN+SEGA_CTRL_LEFT:
            row = 2;
            column = 0;
            break;
            case SEGA_CTRL_RIGHT:
            row = 1;
            column = 2;
            break;
            case SEGA_CTRL_UP+SEGA_CTRL_RIGHT:
            row = 0;
            column = 2;
            break;
            case SEGA_CTRL_DOWN+SEGA_CTRL_RIGHT:
            row = 2;
            column = 2;
            break;
            default:
            row = 1;
            column = 1;
            break;
        }
        buttons = 0;

        Choice_of_Row(row);
        Choice_of_Column(column);

        test = Check_Move( Pick_Row(), Pick_Column());
        if (test == 0)
        {
            // invalid move
            continue;
        }
        Tic_Tac_Toe_Board();

        check = Check_Board();
        if ( (check == 1) || (check == 2) )
        {
            put_chr((check == 1) ? 'X' : 'O', 0x0000, 20-4, 3);
            put_str(" wins!", 0x0000, 20-3, 3);
            delay(240);
            put_str("        ", 0x0000, 20-4, 3);
            // reset the board
            Clear_Board();
        }
        else if ( check == 3 )
        {
            put_str("The game is tied", 0x0000, 20-8, 3);
            delay(240);
            put_str("                ", 0x0000, 20-8, 3);
            // reset the board
            Clear_Board();
        }

        if ( player == 1)
        {
            player = 2;
        }
        else
        {
            player = 1;
        }
        Choice_by_Player(player);

    } // end of outer while loop

    clear_screen ();
    return 0;
} // end of main function

