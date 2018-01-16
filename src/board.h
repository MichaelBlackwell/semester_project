/*
 * board.h
 *
 *  Created on: Dec 2, 2015
 *      Author: mike
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <string>
using namespace std;

class Board {
    private:
		string boardPositions[7];
		string sheepMove;

		int boardPos[7][7];
		int columnMoveS;
		int rowMoveS;
		int columnMoveF;
		int rowMoveF;
		int fox0col;
		int fox0row;
		int fox1col;
		int fox1row;

		int foxColMoveS;
		int foxRowMoveS;
		int foxColMoveF;
		int foxRowMoveF;

		bool fox0takeSheep;

		int fox0takeSheepCol;
		int fox0takeSheepRow;
		int fox1takeSheepCol;
		int fox1takeSheepRow;

		int fox0threatenCol;
		int fox0threatenRow;
		int fox1threatenCol;
		int fox1threatenRow;

		int fox0highThreat;
		int fox1highThreat;

		bool win;
    public:
		Board();
		void inputPositions(char* filename);
		int convertSToI(string pos);
		void print();
		void moveSheep();

		bool legalSheepMove(int colS, int rowS, int colF, int rowF);
		bool legalSheepMoveAdj(int colS, int rowS, int colF, int rowF);
		bool legalSheepMoveDiag(int col, int row);

		void moveFox();
		void trim(string &str );
		void findFoxes();

		//can a sheep be captured
		void foxTakesSheep(int &fCol, int &fRow , int takeCol, int takeRow);
		//can a fox move adjacent to the most number of sheep
		void foxThreatensSheep(int &fCol, int &fRow, int fThreatenCol, int fThreatenRow);
		//else make a random move
		void foxRandomlyMoves(int &fCol, int &fRow);

		bool checkFoxWin();
		bool checkSheepWin();

		bool sheepRipe(int col, int row, int &foxTakeSheepCol, int &foxTakeSheepRow);
		bool sheepTwoAway(int fCol, int fRow, int &fThreatCol, int &fThreatRow, int &highThreat);

		void boardPositionUpdate(int n, string s, int col, int row);

		int checkThreat(int col, int row);

		int countSheep();
		bool sheepInPaddock();

		string getSheepMove();

		string convertItoS(int col);

		bool sheepCaptureFox(int col, int row);



};



#endif /* BOARD_H_ */
