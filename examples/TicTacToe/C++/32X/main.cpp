#include <iostream>
#include <string>
#include <cmath>

#include "32x.h"
#include "hw_32x.h"

using namespace std;

class TicTacToe
{
    public:
        TicTacToe();    // constructor
        int Pick_Player();   // member function
        int Pick_Row();    // member functions
        int Pick_Column();  // member functions
        int Check_Board();  // member function
        void Clear_Board();  // member function
        void Choice_by_Player(int);  // member functions
        void Choice_of_Row(int);   // member functions
        void Choice_of_Column(int);  // member functions
        void Tic_Tac_Toe_Board();  // member functions
        bool Check_Move(int,int); // member functions
    private:
        int row;
        int column;
        int player;
        int board[3][3];
        char display_board[3][3];
};

TicTacToe::TicTacToe()//:row(0):column(0):player(1):board(0)(0):display_board(' ')(' ')
{
    row = 0;
    column = 0;
    player = 1;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = 0;
            display_board[i][j] = ' ';
        }
    }
}

int TicTacToe::Pick_Player()
{
    return player;
}

int TicTacToe::Pick_Row()
{
    return row;
}

int TicTacToe::Pick_Column()
{
    return column;
}

void TicTacToe::Choice_by_Player(int a)
{
    player = a;
}

void TicTacToe::Choice_of_Row(int b)
{
    row = b;
}

void TicTacToe::Choice_of_Column(int c)
{
    column = c;
}

bool TicTacToe::Check_Move(int row, int column)
{
    if ( board[row][column] != 0 )
    {
        HwMdPuts((char*)"Space occupied - Try Again", 0x4000, 20-13, 3);
        Hw32xDelay(120);
        HwMdPuts((char*)"                          ", 0x4000, 20-13, 3);
        return 0;
    }
    else
    {
        board[row][column] = player;
        return 1;
    }
}   // end of Check_Move

int TicTacToe::Check_Board()
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

void TicTacToe::Clear_Board()//:row(0):column(0):player(1):board(0)(0):display_board(' ')(' ')
{
    row = 0;
    column = 0;
    player = 1;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = 0;
            display_board[i][j] = ' ';
        }
    }
}

void TicTacToe::Tic_Tac_Toe_Board()
{
    HwMdPuts((char*)"\x80\x82\x82\x82\x88\x82\x82\x82\x88\x82\x82\x82\x85", 0x2000, 20-6, 6);
    HwMdPuts((char*)"\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 7);
    HwMdPuts((char*)"\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 8);
    HwMdPuts((char*)"\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 9);
    HwMdPuts((char*)"\x81\x82\x82\x82\x87\x82\x82\x82\x87\x82\x82\x82\x84", 0x2000, 20-6, 10);
    HwMdPuts((char*)"\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 11);
    HwMdPuts((char*)"\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 12);
    HwMdPuts((char*)"\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 13);
    HwMdPuts((char*)"\x81\x82\x82\x82\x87\x82\x82\x82\x87\x82\x82\x82\x84", 0x2000, 20-6, 14);
    HwMdPuts((char*)"\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 15);
    HwMdPuts((char*)"\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 16);
    HwMdPuts((char*)"\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C\x20\x20\x20\x7C", 0x2000, 20-6, 17);
    HwMdPuts((char*)"\x86\x82\x82\x82\x89\x82\x82\x82\x89\x82\x82\x82\x83", 0x2000, 20-6, 18);

    HwMdPuts((char*)"Current Player: ", 0x2000, 20-8, 21);
    HwMdPutc((player == 1) ? 'X' : 'O', 0x0000, 20+8, 21);

    for ( int row = 0; row < 3; row ++)
    {
        for ( int column = 0; column < 3; column++)
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
            HwMdPutc(display_board[row][column], 0x0000, 20-6+2 + column*4, 6+2 + row*4);
        }  // end of inner for loop
    }  // end of outer for loop

}   // end of Tic_Tac_Toe_Board


int main()
{
    TicTacToe game;
    bool test;
    bool more = true;
    int row = 0;
    int column= 0;
    int player;
    int check = 0;
    short buttons = 0;

    TicTacToe();

    while ( more )
    {
        game.Tic_Tac_Toe_Board();
        player = game.Pick_Player();

        HwMdPuts((char*)"Choose a square", 0x2000, 20-7, 3);
        while ( !(buttons & SEGA_CTRL_A) )
        {
            Hw32xDelay(2);
            buttons = HwMdReadPad(0);
            if ( buttons & SEGA_CTRL_C )
            {
                more = 0;
                break;
            }
        }
        while ( HwMdReadPad(0) & SEGA_CTRL_A ) ;
        HwMdPuts((char*)"               ", 0x2000, 20-7, 3);
        Hw32xDelay(30);

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

        game.Choice_of_Row(row);
        game.Choice_of_Column(column);

        test = game.Check_Move( game.Pick_Row(), game.Pick_Column());
        if (test == 0)
        {
            // invalid move
            continue;
        }
        game.Tic_Tac_Toe_Board();

        check = game.Check_Board();
        if ( (check == 1) || (check == 2) )
        {
            HwMdPutc((check == 1) ? 'X' : 'O', 0x0000, 20-4, 3);
            HwMdPuts((char*)" wins!", 0x0000, 20-3, 3);
            Hw32xDelay(240);
            HwMdPuts((char*)"        ", 0x0000, 20-4, 3);
            // reset the board
            game.Clear_Board();
        }
        else if ( check == 3 )
        {
            HwMdPuts((char*)"The game is tied", 0x0000, 20-8, 3);
            Hw32xDelay(240);
            HwMdPuts((char*)"                ", 0x0000, 20-8, 3);
            // reset the board
            game.Clear_Board();
        }

        if ( player == 1)
        {
            player = 2;
        }
        else
        {
            player = 1;
        }
        game.Choice_by_Player(player);

    } // end of outer while loop

    HwMdClearScreen ();
    return 0;
} // end of main function

