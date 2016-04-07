/*	File:		Tetris.cpp
*
*	Author:		Ryan Pickelsimer
*	Class:		Programming 2, Section 1
*	Date:		September 14, 2014
*
*	Purpose:	The game of Tetris.
*/

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <vector>
#include <string>
#include <windows.h>
#include <cstdlib>
#include <ctime>
using namespace std;


/*********************/
/* program constants */

const static int SCREEN_WIDTH = 365, SCREEN_HEIGHT = 435; // used to size console
const static int COLUMNS = 41, ROWS = 30, MID_COLUMN = COLUMNS - 20; // used for memory map 
const static int NEXT_TETRIS_ROW = 9; // position for next piece
const static int NEXT_TETRIS_COLUMN = COLUMNS - 12; // position for next piece
const static int CURRENT_TETRIS_ROW = 5; // position for current piece
const static int CURRENT_TETRIS_COLUMN = (COLUMNS - 20) / 2; // position for current piece
const static int CURRENT = 0, NEXT = 1; // used for vector index
const static int NUM_SHAPES = 7; // number of different Tetris subclasses
const static int LEVEL_1_SPEED = 300;	// beginning game speed
const static int SPEED_MULTIPLIER = 20; // speed increment between levels
const static int ROWS_2_NEXT_LEVEL = 10; // rows needed for next level


/***********************************************/
/* program class declaration and definitons */

/* Board object allows for easy board passing and modification */
class Board {

protected:
	char board[ROWS][COLUMNS];

public:
	friend void gotoXY(int row, int column);	
	void setBoard(int row, int column, char character) { board[row][column] = character; }
	char getBoard(int row, int column) { return board[row][column]; }
	void initializeBoard();
	void drawBoard();
	int testForFullRows();
};

/* initializes a board memory map with borders */
void Board::initializeBoard(){

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++){

			/**  create borders and initialize a game board array **/
			if ((i == 0) || (j == 0) || (i == ROWS - 1) || (j == COLUMNS - 1) || (j == MID_COLUMN)) {
				
				board[i][j] = (char)219;
				gotoXY(i, j);
				cout << board[i][j];
			}
			else
				board[i][j] = ' ';
		}
	}
}

/* draws a current state of the game board. */
void Board::drawBoard(){

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLUMNS; j++){
			
			gotoXY(i, j);
			cout << board[i][j];			
		}
	}
}

/* removes full rows and returns an int value equal to number rows removed */
int Board::testForFullRows() {

	bool fullRow = false;
	int rows = 0;

	// test each row for full row.
	for (int i = ROWS - 2; i > 4; i--) {
				
		for (int j = MID_COLUMN - 1; j > 0; j--){			
			if (board[i][j] != ' ')
				fullRow = true;
			else 
				fullRow = false;	

			// skip to next row if there is a space
			if (!fullRow)
				break;
		} 		
		
		// move rows down if full row
		if (fullRow){
			rows++;
			for (int k = i; k > 4; k--)	{
				for (int l = MID_COLUMN - 1; l > 0; l--) {
					board[k][l] = board[k - 1][l];
				}
			}
			i++; // check same row index with new contents.
		}
	}
	return rows;
}

/* Abstract class for various tetris shapes */
class Tetris { 

protected: 
	
	int rowPosition, columnPosition;
	int rotation;
	char graphic;

public:	

	Tetris(){		
		rotation = 0;
		rowPosition = NEXT_TETRIS_ROW;
		columnPosition = NEXT_TETRIS_COLUMN;
	}

	friend void gotoXY(int row, int column);

	/* Display, erase, and manipulate memory map*/
	/* bool erase == true removes characters from screen and memory map*/
	virtual void tetrisDisplay(Board&, bool) =0;
	/* return true for tetris landing on a surface*/
	virtual bool testForStop(Board, int) =0;
	
	void incrementRotation(){ rotation++; }
	void incrementRowPosition(){ rowPosition++; }
	void decrementRowPosition(){ rowPosition--; }
	void incrementColumnPosition(){ columnPosition++; }
	void decrementColumnPosition(){ columnPosition--; }
	int getColumnPosition(){ return columnPosition; }
	int getRowPosition(){ return rowPosition; }
	void setPosition(int row, int col) {
		rowPosition = row;
		columnPosition = col;
	}
};

/* Square class inherits Tetris */
class Square : public Tetris {

public:

	Square() :Tetris(){ graphic = char(219); };

	/* simulates tetris motion by erasing at old positions and drawing at new ones */
	/* also modifies board array */
	void tetrisDisplay(Board &board, bool erase);
	bool testForStop(Board board, int horizChange);
};

/* Square class virtual function definition */
void Square::tetrisDisplay(Board &board, bool erase){

	char symbol;
	if (erase == true)
		symbol = ' ';
	else symbol = graphic;

	board.setBoard(rowPosition, columnPosition, symbol);
	gotoXY(rowPosition, columnPosition);
	cout << symbol;

	board.setBoard(rowPosition, columnPosition + 1, symbol);
	gotoXY(rowPosition, columnPosition + 1);
	cout << symbol;

	board.setBoard(rowPosition - 1, columnPosition, symbol);
	gotoXY(rowPosition - 1, columnPosition);
	cout << symbol;

	board.setBoard(rowPosition - 1, columnPosition + 1, symbol);
	gotoXY(rowPosition - 1, columnPosition + 1);
	cout << symbol;

	gotoXY(0, 0); //moves cursor to corner of screen
};

/* Square class virtual function definition */
bool Square::testForStop(Board board, int horizChange){

	if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
		board.getBoard(rowPosition + 1, columnPosition + 1 + horizChange) != ' ')
		return true;
	else return false;	
}

/* Line Class inherits Tetris */
class Line : public Tetris {

public:

	Line() :Tetris(){ graphic = char(177); };

	/* simulates tetris motion by erasing at old positions and drawing at new ones */
	/* also modifies board array */
	void tetrisDisplay(Board &board, bool erase);
	bool testForStop(Board board, int horizChange);
};

/* Line Class virtual function definition */
void Line::tetrisDisplay(Board &board, bool erase) {

	char symbol;
	if (erase == true)
		symbol = ' ';
	else symbol = graphic;

	// vertical position
	if (rotation % 2 == 0){
		for (int i = 0; i < 4; i++) {

			board.setBoard(rowPosition - i, columnPosition, symbol);
			gotoXY(rowPosition - i, columnPosition);
			cout << symbol;
		}
	}
	// horizontal position
	else if (rotation % 2 == 1) {
		for (int i = 0; i < 4; i++) {

			board.setBoard(rowPosition, columnPosition + i, symbol);
			gotoXY(rowPosition, columnPosition + i);
			cout << symbol;
		}
	}
	gotoXY(0, 0); //moves cursor to corner of screen
}

/* Line Class virtual function definition */
bool Line::testForStop(Board board, int horizChange) {

	// test vertical position
	if (rotation % 2 == 0) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ')
			return true;
	}
	// test horizontal position
	else if (rotation % 2 == 1) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 1 + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 2 + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 3 + horizChange) != ' ')
			return true;
	}
	// else piece is still falling
	return false;
}

/* Z Class inherits Tetris */
class Z : public Tetris {

public:

	Z() :Tetris(){ graphic = char(176); };

	/* simulates tetris motion by erasing at old positions and drawing at new ones */
	/* also modifies board array */
	void tetrisDisplay(Board &board, bool erase);
	bool testForStop(Board board, int horizChange);
};

/* Z Class virtual function definition */
void Z::tetrisDisplay(Board &board, bool erase) {

	char symbol;
	if (erase == true)
		symbol = ' ';
	else symbol = graphic;

	// horizontal position
	if (rotation % 2 == 0){		

		board.setBoard(rowPosition, columnPosition, symbol);
		gotoXY(rowPosition, columnPosition);
		cout << symbol;

		board.setBoard(rowPosition, columnPosition + 1, symbol);
		gotoXY(rowPosition, columnPosition + 1);
		cout << symbol;

		board.setBoard(rowPosition - 1, columnPosition - 1, symbol);
		gotoXY(rowPosition - 1, columnPosition - 1);
		cout << symbol;

		board.setBoard(rowPosition - 1, columnPosition, symbol);
		gotoXY(rowPosition - 1, columnPosition);
		cout << symbol;		
	}
	// vertical position
	else if (rotation % 2 == 1) {		

		board.setBoard(rowPosition, columnPosition, symbol);
		gotoXY(rowPosition, columnPosition);
		cout << symbol;

		board.setBoard(rowPosition - 1, columnPosition , symbol);
		gotoXY(rowPosition - 1, columnPosition);
		cout << symbol;

		board.setBoard(rowPosition - 1, columnPosition + 1, symbol);
		gotoXY(rowPosition - 1, columnPosition + 1);
		cout << symbol;

		board.setBoard(rowPosition - 2, columnPosition + 1, symbol);
		gotoXY(rowPosition - 2, columnPosition +1);
		cout << symbol;		
	}
	gotoXY(0, 0); //moves cursor to corner of screen
}

/* Z Class virtual function definition */
bool Z::testForStop(Board board, int horizChange) {

	// test horizontal position
	if (rotation % 2 == 0) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 1 + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 1, columnPosition - 1) != ' ')
			return true;
		else if (horizChange < 0 && board.getBoard(rowPosition + 1 - 1, columnPosition - 2) != ' ') 
			return true;
	}
	// test vertical position
	else if (rotation % 2 == 1) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 1, columnPosition + 1) != ' ' )
			return true;
		else if (horizChange > 0 && board.getBoard(rowPosition + 1 - 1, columnPosition + 2) != ' ')
			return true;
	}
	// else piece is still falling
	return false;
}

/* S Class inherits Tetris */
class S : public Tetris {

public:

	S() :Tetris(){ graphic = char(178); };

	/* simulates tetris motion by erasing at old positions and drawing at new ones */
	/* also modifies board array */
	void tetrisDisplay(Board &board, bool erase);
	bool testForStop(Board board, int horizChange);
};

/* S Class virtual function definition */
void S::tetrisDisplay(Board &board, bool erase) {

	char symbol;
	if (erase == true)
		symbol = ' ';
	else symbol = graphic;

	// horizontal position
	if (rotation % 2 == 0){

		board.setBoard(rowPosition, columnPosition, symbol);
		gotoXY(rowPosition, columnPosition);
		cout << symbol;

		board.setBoard(rowPosition, columnPosition + 1, symbol);
		gotoXY(rowPosition, columnPosition + 1);
		cout << symbol;

		board.setBoard(rowPosition - 1, columnPosition + 1, symbol);
		gotoXY(rowPosition - 1, columnPosition + 1);
		cout << symbol;

		board.setBoard(rowPosition - 1, columnPosition + 2, symbol);
		gotoXY(rowPosition - 1, columnPosition + 2);
		cout << symbol;
	}
	// vertical position
	else if (rotation % 2 == 1) {

		board.setBoard(rowPosition, columnPosition, symbol);
		gotoXY(rowPosition, columnPosition);
		cout << symbol;

		board.setBoard(rowPosition - 1, columnPosition, symbol);
		gotoXY(rowPosition - 1, columnPosition);
		cout << symbol;

		board.setBoard(rowPosition - 1, columnPosition - 1, symbol);
		gotoXY(rowPosition - 1, columnPosition - 1);
		cout << symbol;

		board.setBoard(rowPosition - 2, columnPosition - 1, symbol);
		gotoXY(rowPosition - 2, columnPosition - 1);
		cout << symbol;
	}
	gotoXY(0, 0); //moves cursor to corner of screen
}

/* S Class virtual function definition */
bool S::testForStop(Board board, int horizChange) {

	// test horizontal position
	if (rotation % 2 == 0) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 1 + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 1, columnPosition + 2) != ' ')
			return true;
		else if (horizChange > 0 && board.getBoard(rowPosition + 1 - 1, columnPosition + 3) != ' ')
			return true;
	}
	// test vertical position
	else if (rotation % 2 == 1) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 1, columnPosition - 1) != ' ')
			return true;
		else if (horizChange < 0 && board.getBoard(rowPosition + 1 - 1, columnPosition - 2) != ' ')
			return true;
	}
	// else piece is still falling
	return false;
}

/* J Class inherits Tetris */
class J : public Tetris {

public:

	J() :Tetris(){ graphic = char(176); };

	/* simulates tetris motion by erasing at old positions and drawing at new ones */
	/* also modifies board array */
	void tetrisDisplay(Board &board, bool erase);
	bool testForStop(Board board, int horizChange);
};

/* J Class virtual function definition */
void J::tetrisDisplay(Board &board, bool erase) {

	char symbol;
	if (erase == true)
		symbol = ' ';
	else symbol = graphic;

	// position 0
	if (rotation % 4 == 0){

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition, columnPosition + i, symbol);
			gotoXY(rowPosition, columnPosition + i);
			cout << symbol;
		}		

		board.setBoard(rowPosition - 1, columnPosition , symbol);
		gotoXY(rowPosition - 1, columnPosition);
		cout << symbol;
	}
	// position 1
	else if (rotation % 4 == 1) {

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition - i, columnPosition, symbol);
			gotoXY(rowPosition - i, columnPosition);
			cout << symbol;
		}

		board.setBoard(rowPosition - 2, columnPosition + 1, symbol);
		gotoXY(rowPosition - 2, columnPosition + 1);
		cout << symbol;
	}
	// position 2
	else if (rotation % 4 == 2) {

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition - 1, columnPosition - i, symbol);
			gotoXY(rowPosition - 1, columnPosition - i);
			cout << symbol;
		}

		board.setBoard(rowPosition, columnPosition, symbol);
		gotoXY(rowPosition, columnPosition);
		cout << symbol;
	}
	// position 3
	else if (rotation % 4 == 3) {

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition - i, columnPosition + 1, symbol);
			gotoXY(rowPosition - i, columnPosition + 1);
			cout << symbol;
		}

		board.setBoard(rowPosition, columnPosition, symbol);
		gotoXY(rowPosition, columnPosition);
		cout << symbol;
	}
	gotoXY(0, 0); //moves cursor to corner of screen
}

/* J Class virtual function definition */
bool J::testForStop(Board board, int horizChange) {

	// test position 0
	if (rotation % 4 == 0) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 1 + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 2 + horizChange) != ' ')
			return true;
	}
	// test position 1
	else if (rotation % 4 == 1) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 2, columnPosition + 1) != ' ')
			return true;
		else if (horizChange > 0 &&	board.getBoard(rowPosition + 1 - 2, columnPosition + 2) != ' ')
			return true;
	}
	// test position 2
	else if (rotation % 4 == 2) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1 - 1, columnPosition - 2 + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 1, columnPosition - 1) != ' ')
			return true;
	}
	// test position 3
	else if (rotation % 4 == 3) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 1 + horizChange) != ' ')
			return true;
	}
	// else piece is still falling
	return false;
}

/* L Class inherits Tetris */
class L : public Tetris {

public:

	L() :Tetris(){ graphic = char(178); };

	/* simulates tetris motion by erasing at old positions and drawing at new ones */
	/* also modifies board array */
	void tetrisDisplay(Board &board, bool erase);
	bool testForStop(Board board, int horizChange);
};

/* L Class virtual function definition */
void L::tetrisDisplay(Board &board, bool erase) {

	char symbol;
	if (erase == true)
		symbol = ' ';
	else symbol = graphic;

	// position 0
	if (rotation % 4 == 0){

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition, columnPosition + i, symbol);
			gotoXY(rowPosition, columnPosition + i);
			cout << symbol;
		}

		board.setBoard(rowPosition - 1, columnPosition + 2, symbol);
		gotoXY(rowPosition - 1, columnPosition + 2);
		cout << symbol;
	}
	// position 1
	else if (rotation % 4 == 1) {

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition - i, columnPosition, symbol);
			gotoXY(rowPosition - i, columnPosition);
			cout << symbol;
		}

		board.setBoard(rowPosition, columnPosition + 1, symbol);
		gotoXY(rowPosition, columnPosition + 1);
		cout << symbol;
	}
	// position 2
	else if (rotation % 4 == 2) {

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition - 1, columnPosition + i, symbol);
			gotoXY(rowPosition - 1, columnPosition + i);
			cout << symbol;
		}

		board.setBoard(rowPosition, columnPosition, symbol);
		gotoXY(rowPosition, columnPosition);
		cout << symbol;
	}
	// position 3
	else if (rotation % 4 == 3) {

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition - i, columnPosition, symbol);
			gotoXY(rowPosition - i, columnPosition);
			cout << symbol;
		}

		board.setBoard(rowPosition - 2, columnPosition - 1, symbol);
		gotoXY(rowPosition - 2, columnPosition - 1);
		cout << symbol;
	}
	gotoXY(0, 0); //moves cursor to corner of screen
}

/* L Class virtual function definition */
bool L::testForStop(Board board, int horizChange) {

	// test position 0
	if (rotation % 4 == 0) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 1 + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 2 + horizChange) != ' ')
			return true;
	}
	// test position 1
	else if (rotation % 4 == 1) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 1 + horizChange) != ' ')
			return true;		
	}
	// test position 2
	else if (rotation % 4 == 2) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1 - 1, columnPosition + 2 + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 1, columnPosition + 1) != ' ')
			return true;
	}
	// test position 3
	else if (rotation % 4 == 3) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 2, columnPosition - 1) != ' ')
			return true;
		else if (horizChange < 0 && board.getBoard(rowPosition + 1 - 2, columnPosition - 2) != ' ')
			return true;
	}
	// else piece is still falling
	return false;
}

/* T Class inherits Tetris */
class T : public Tetris {

public:

	T() :Tetris(){ graphic = char(219); };

	/* simulates tetris motion by erasing at old positions and drawing at new ones */
	/* also modifies board array */
	void tetrisDisplay(Board &board, bool erase);
	bool testForStop(Board board, int horizChange);
};

/* T Class virtual function definition */
void T::tetrisDisplay(Board &board, bool erase) {

	char symbol;
	if (erase == true)
		symbol = ' ';
	else symbol = graphic;

	// position 0
	if (rotation % 4 == 0){

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition, columnPosition + i, symbol);
			gotoXY(rowPosition, columnPosition + i);
			cout << symbol;
		}

		board.setBoard(rowPosition - 1, columnPosition + 1, symbol);
		gotoXY(rowPosition - 1, columnPosition + 1);
		cout << symbol;
	}
	// position 1
	else if (rotation % 4 == 1) {

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition - i, columnPosition, symbol);
			gotoXY(rowPosition - i, columnPosition);
			cout << symbol;
		}

		board.setBoard(rowPosition - 1, columnPosition + 1, symbol);
		gotoXY(rowPosition - 1, columnPosition + 1);
		cout << symbol;
	}
	// position 2
	else if (rotation % 4 == 2) {

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition - 1, columnPosition - 1 + i, symbol);
			gotoXY(rowPosition - 1, columnPosition - 1 + i);
			cout << symbol;
		}

		board.setBoard(rowPosition, columnPosition, symbol);
		gotoXY(rowPosition, columnPosition);
		cout << symbol;
	}
	// position 3
	else if (rotation % 4 == 3) {

		for (int i = 0; i < 3; i++) {

			board.setBoard(rowPosition - i, columnPosition, symbol);
			gotoXY(rowPosition - i, columnPosition);
			cout << symbol;
		}

		board.setBoard(rowPosition - 1, columnPosition - 1, symbol);
		gotoXY(rowPosition - 1, columnPosition - 1);
		cout << symbol;
	}
	gotoXY(0, 0); //moves cursor to corner of screen
}

/* T Class virtual function definition */
bool T::testForStop(Board board, int horizChange) {

	// test position 0
	if (rotation % 4 == 0) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 1 + horizChange) != ' ' ||
			board.getBoard(rowPosition + 1, columnPosition + 2 + horizChange) != ' ')
			return true;
	}
	// test position 1
	else if (rotation % 4 == 1) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 1, columnPosition + 1) != ' ')
			return true;
		else if (horizChange > 0 && board.getBoard(rowPosition + 1 - 1, columnPosition + 2) != ' ')
			return true;
	}
	// test position 2
	else if (rotation % 4 == 2) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && (board.getBoard(rowPosition + 1 - 1, columnPosition - 1) != ' ' ||
			board.getBoard(rowPosition + 1 - 1, columnPosition + 1) != ' '))
			return true;
		else if (horizChange < 0 && board.getBoard(rowPosition + 1 - 1, columnPosition - 2) != ' ')
			return true;		
		else if (horizChange > 0 && board.getBoard(rowPosition + 1 - 1, columnPosition + 2) != ' ')
			return true;
	}
	// test position 3
	else if (rotation % 4 == 3) {

		if (board.getBoard(rowPosition + 1, columnPosition + horizChange) != ' ')
			return true;
		else if (horizChange == 0 && board.getBoard(rowPosition + 1 - 1, columnPosition - 1) != ' ')
			return true;
		else if (horizChange < 0 && board.getBoard(rowPosition + 1 - 1, columnPosition - 2) != ' ')
			return true;
	}
	// else piece is still falling
	return false;
}


/***********************************************/
/* program function declaration and definitons */

/* move cursor to a location */
void gotoXY(int row, int column)
{
	COORD coord;
	coord.X = column;
	coord.Y = row;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
		);
}

/* clear screen */
void clear() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
	SetConsoleCursorPosition(console, topLeft);
}

/*  display game information  (use with displayCurrentStats()) */
void gameInformation() {	

	gotoXY(4, COLUMNS - 12);
	cout << "Next";

	gotoXY(13, COLUMNS - 13);
	cout << "Level";

	gotoXY(17, COLUMNS - 13);
	cout << "Points";

	gotoXY(21, COLUMNS - 13);
	cout << "Rows to ";
	gotoXY(22, COLUMNS - 15);
	cout << "Next Level";
}

/* update game statistics to display */
void displayCurrentStats(int level, int score, int rows2next) {

	gotoXY(15, COLUMNS - 11);
	cout << level;

	gotoXY(19, COLUMNS - 11);
	cout << score;

	gotoXY(24, COLUMNS - 11);
	cout << rows2next;
}

/* game instructions */
void welcomeScreen() {

	gotoXY(3, COLUMNS / 2 - 3);
	cout << "TETRIS ";

	gotoXY(8, COLUMNS / 2 - 7);
	cout << "Direction Keys";
	gotoXY(9, COLUMNS / 2 - 8);
	cout << "----------------";

	gotoXY(12, COLUMNS / 2 - 9);
	cout << "W -> Rotate Tetris";

	gotoXY(16, COLUMNS / 3 - 10);
	cout << "A -> Move Left";

	gotoXY(16, COLUMNS * 2 / 3 - 4);
	cout << "D -> Move Right";

	gotoXY(20, COLUMNS / 2 - 8);
	cout << "S -> Drop Tetris";

	gotoXY(23, COLUMNS / 2 - 5);
	cout << "P -> Pause";

	gotoXY(25, COLUMNS / 2 - 6);
	cout << "ESC -> Quit";

	gotoXY(27, COLUMNS / 2 - 13);
	cout << "Press enter when ready.";
	cin.sync();
	cin.get();
}

/* creates and adds a tetris shape to a vector*/
void setTetrisVector(vector<Tetris*> &tetris, int index){

	switch (index) {

	case 0:
		tetris.push_back(new Square());		
		break;
	case 1:
		tetris.push_back(new Line());
		break;
	case 2:
		tetris.push_back(new Z());
		break;
	case 3:
		tetris.push_back(new S());
		break;
	case 4:
		tetris.push_back(new L());
		break;
	case 5:
		tetris.push_back(new J());
		break;
	case 6:
		tetris.push_back(new T());
		break;
	}
 }

/* return a score for number of rows removed and current level */
int calculateScore(int rows, int level) {

	int score;
	switch (rows) {
	case 1:
		score = level * 40 + 40;
		break;
	case 2:
		score = level * 100 + 100;
		break;
	case 3:
		score = level * 300 + 300;
		break;
	case 4:
		score = level * 1200 + 1200;
		break;
	default:
		score = 0;
	}
	return score;
}

/* returns true for a piece at row 5 (called after piece stops) */
bool testForGameOver(Tetris* tetris) {
	
	if (tetris->getRowPosition() < 6)
		return true; 

	else return false;
}


/*************************************************/
/***************  main function  *****************/

int _tmain(int argc, _TCHAR* argv[])
{
	/* resize console window */
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, r.left, r.top, SCREEN_WIDTH, SCREEN_HEIGHT, TRUE);

	int rowsToNextLevel = 0;
	int keyPressed = 0;
	int level = 0;
	int score = 0;
	bool playAgain = true;


	/********************/
	/* Begin game loop */

	while (playAgain) {

		/* random number generator */
		srand(static_cast<unsigned int>(time(0)));		
		
		Board board;
		vector<Tetris*> tetris;
		
		//increment level
		level++;

		// deduct earned rows from last round
		rowsToNextLevel = ROWS_2_NEXT_LEVEL + rowsToNextLevel;		

		// display graphical elements
		if (level < 2)
			welcomeScreen();
		clear();
		board.initializeBoard();
		gameInformation();
		displayCurrentStats(level, score, rowsToNextLevel);		

		// display current tetris (random) in starting field
		setTetrisVector(tetris, rand() % NUM_SHAPES);
		tetris[CURRENT]->setPosition(CURRENT_TETRIS_ROW, CURRENT_TETRIS_COLUMN);
		tetris[CURRENT]->tetrisDisplay(board, false);

		// show the next tetris (random)
		setTetrisVector(tetris, rand() % NUM_SHAPES);
		tetris[NEXT]->tetrisDisplay(board, false);


		/********************/
		/* Begin level loop */

		while (rowsToNextLevel > 0){

			int horizontalChange = 0;

			// display current tetris
			tetris[CURRENT]->tetrisDisplay(board, false);

			// control game speed
			int	levelSpeed = LEVEL_1_SPEED - (SPEED_MULTIPLIER * level);

			if (keyPressed == 's'){
				Sleep(50);
				keyPressed = 0;
			}
			else
				Sleep(levelSpeed);

			//test for player input
			if (_kbhit()) {

				// process user request
				// only valid input from the keyboard is accepted
				try {
					keyPressed = tolower(_getch());

					// pressed left -> move left if possible
					if (keyPressed == 'a'){
						horizontalChange = -1;

						// no horizontal change if any obstruction
						if (tetris[CURRENT]->testForStop(board, horizontalChange))
							horizontalChange = 0;
					}
					// pressed right -> move right if possible
					else if (keyPressed == 'd'){
						horizontalChange = 1;

						// no horizontal change if any obstruction
						if (tetris[CURRENT]->testForStop(board, horizontalChange))
							horizontalChange = 0;
					}
					// pressed up -> rotate shape
					else if (keyPressed == 'w'){

						// erase current tetris position display
						tetris[CURRENT]->tetrisDisplay(board, true);
						tetris[CURRENT]->incrementRotation();
					}
					// pressed pause -> pause play
					else if (keyPressed == 'p'){

						gotoXY(10, COLUMNS / 2 - 13);
						cout << "Press enter to continue.";
						cin.sync();
						cin.get();

						// on return reset to previous state
						clear();
						board.drawBoard();
						gameInformation();
						displayCurrentStats(level, score, rowsToNextLevel);
						tetris[NEXT]->tetrisDisplay(board, false);
					}
					// pressed ESC -> exit program
					else if (keyPressed == 27){
						return 0;
					}
				}
				catch (const char* msg){
					cerr << msg << endl;
				}
			}

			// if the tetris stops falling
			bool stopped = tetris[CURRENT]->testForStop(board, horizontalChange);

			if (stopped){

				// test for any full rows 
				int rows = board.testForFullRows();

				if (rows > 0) {
					// calculate score, rows, and level and update display
					score = calculateScore(rows, level);					
					rowsToNextLevel = rowsToNextLevel - rows;
					if (rowsToNextLevel < 1)
						board.initializeBoard();					

					// redraw new state
					clear();
					board.drawBoard();
					gameInformation();
					displayCurrentStats(level, score, rowsToNextLevel);
					tetris[NEXT]->tetrisDisplay(board, false);
				}
				else {
					// test whether game is over					
					if (testForGameOver(tetris[CURRENT])){

						gotoXY(10, 15);
						cout << "GAME OVER";

						gotoXY(14, 15);
						cout << "Continue? (Y,N)";						
						int c = tolower(cin.get());

						if (c == 'y') {
							playAgain = true;
							level = 0;
							score = 0;
							rowsToNextLevel = ROWS_2_NEXT_LEVEL;
						}
						else 
							return 0;
					}
				}

				//move next tetris to starting field and replace with new next tetris
				tetris[NEXT]->tetrisDisplay(board, true);
				tetris[NEXT]->setPosition(CURRENT_TETRIS_ROW, CURRENT_TETRIS_COLUMN);

				// delete current element from vector
				tetris.erase(tetris.begin());
				
				// display next shape (random)
				setTetrisVector(tetris, rand() % NUM_SHAPES);
				tetris[NEXT]->tetrisDisplay(board, false);	
			}
			// if still falling
			else {
				// erase current tetris position display
				tetris[CURRENT]->tetrisDisplay(board, true);

				// set new position values
				tetris[CURRENT]->incrementRowPosition();

				if (horizontalChange == -1)
					tetris[CURRENT]->decrementColumnPosition();
				if (horizontalChange == 1)
					tetris[CURRENT]->incrementColumnPosition();

				horizontalChange = 0;
			}
		}
	} 	
	return 0;
}