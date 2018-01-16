#include <iostream>
#include <iomanip>
#include <fstream>
#include "board.h"

using namespace std;


int main( int argc, char **argv ){

	Board board;

	bool winf = false;
	bool wins = false;

	board.inputPositions(argv[1]);
	//show initial board
	board.print();

	winf = board.checkFoxWin();
	wins = board.checkSheepWin();



	if(!winf && !wins){
		//move player
		cout << endl << "Your move? " << board.getSheepMove();
		board.moveSheep();
		board.print();

		winf = board.checkFoxWin();
		wins = board.checkSheepWin();

		if(!winf && !wins){
			//move fox
			//
			board.moveFox();
			board.print();

			winf = board.checkFoxWin();
			wins = board.checkSheepWin();
		}
	}





	return 0;
}

