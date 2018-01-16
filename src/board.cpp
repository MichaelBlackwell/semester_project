#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "board.h"

//TO DO = ADD CHECK FOR RING AREA FOR FOXES

using namespace std;

Board::Board(){

	columnMoveS = -1;
	rowMoveS = -1;
	columnMoveF = -1;
	rowMoveF = -1;

	fox0col = -1;
	fox0row = -1;
	fox1col = -1;
	fox1row = -1;

	foxColMoveS = -1;
	foxRowMoveS = -1;
	foxColMoveF = -1;
	foxRowMoveF = -1;

	fox0takeSheep = false;

	fox0takeSheepCol = -1;
	fox0takeSheepRow = -1;
	fox1takeSheepCol = -1;
	fox1takeSheepRow = -1;

	fox0threatenCol = -1;
	fox0threatenRow = -1;
	fox1threatenCol = -1;
	fox1threatenRow = -1;

	fox0highThreat = -1;
	fox1highThreat = -1;

	win = false;
}

void Board::inputPositions(char* filename){
	ifstream word_file( filename );

	string next_word;
	string sheep_move;
	for(int i = 0; i < 7; i++){

		getline(word_file, next_word);

		trim(next_word);

		if(i < 2 || i > 4){
			next_word.insert(0, "  ");
			next_word.insert(5, "  ");
		}

		boardPositions[i] = next_word;
	}

	getline(word_file, sheep_move);

	trim(sheep_move);

	sheepMove= sheep_move;

	//change string to int
	for(int i = 0; i < 7; i++){
		for(int j = 0; j < 7; j++){
			if(boardPositions[j].substr(i,1) == "."){
				boardPos[i][j] = 1;
			}
			else if(boardPositions[j].substr(i,1) == "S"){
				boardPos[i][j] = 2;
			}
			else if(boardPositions[j].substr(i,1) == "F"){
				boardPos[i][j] = 3;
			}
			else{
				boardPos[i][j] = 0;
			}
		}
	}

	findFoxes();

	word_file.close();
}

void Board::moveSheep(){

	columnMoveS = convertSToI(sheepMove.substr(0,1)) - 1;
	rowMoveS = 6 - (convertSToI(sheepMove.substr(1,1)) - 1);
	columnMoveF = convertSToI(sheepMove.substr(3,1)) - 1;
	rowMoveF = 6 - (convertSToI(sheepMove.substr(4,1)) - 1);

	if(legalSheepMove(columnMoveS, rowMoveS, columnMoveF, rowMoveF) &&
			boardPos[columnMoveS][rowMoveS] == 2 &&
			boardPos[columnMoveF][rowMoveF] == 1){
		boardPositionUpdate(1, ".", columnMoveS, rowMoveS);
		boardPositionUpdate(2, "S", columnMoveF, rowMoveF);
	}
	else{
		cout << endl << "That is not a legal move.";
		cout << endl << "Your move? ";
		cin >> sheepMove;
		moveSheep();


	}

	if(sheepCaptureFox(fox0col, fox0row)){
		boardPositionUpdate(1, ".", fox0col, fox0row);

		if(fox1col >= 0){
			fox0col = fox1col;
			fox0row = fox1row;
		}
		else{
			fox0col = -1;
			fox0row = -1;
		}
	}

	if(sheepCaptureFox(fox1col, fox1row) && fox1col >= 0){
		boardPositionUpdate(1, ".", fox1col, fox1row);

		fox1col = -1;
		fox1row = -1;
	}
}

bool Board::legalSheepMove(int colS, int rowS, int colF, int rowF){
	//offboard
	//edges
	if(colS < 0 || rowS < 0){
		return false;
	}
	if(colF < 0 || rowF < 0){
		return false;
	}
	if(colS > 6 || rowS > 6){
		return false;
	}
	if(colF > 6 || rowF > 6){
		return false;
	}
	//corners
	//check start position
	if(colS < 2 && rowS < 2){
		return false;
	}
	if(colS < 2 && rowS > 4){
		return false;
	}
	if(colS > 4 && rowS < 2){
		return false;
	}
	if(colS > 4 && rowS > 4){
		return false;
	}

	//check final position
	if(colF < 2 && rowF < 2){
		return false;
	}
	if(colF < 2 && rowF > 4){
		return false;
	}
	if(colF > 4 && rowF < 2){
		return false;
	}
	if(colF > 4 && rowF > 4){
		return false;
	}
	if(!legalSheepMoveAdj(colS, rowS, colF, rowF)){
		return false;
	}

	return true;
}

bool Board::legalSheepMoveAdj(int colS, int rowS, int colF, int rowF){
	if(colS - colF >= -1 && colS - colF <= 1 && rowS - rowF >= -1 && rowS - rowF <= 1){
		if(colS - colF != 0 && rowS - rowF != 0){
			return legalSheepMoveDiag(colS, rowS);
		}else{
			return true;
		}
	}else{
		return false;
	}
}

bool Board::legalSheepMoveDiag(int col, int row){
	if((col + row) % 2){
		return false;
	}else{
		return true;
	}
}

int Board::convertSToI(string pos){

	if(pos == "a"){
		return 1;
	}
	if(pos == "b"){
			return 2;
	}
	if(pos == "c"){
			return 3;
	}
	if(pos == "d"){
			return 4;
	}
	if(pos == "e"){
			return 5;
	}
	if(pos == "f"){
			return 6;
	}
	if(pos == "g"){
			return 7;
	}
	if(pos == "1"){
			return 1;
	}
	if(pos == "2"){
			return 2;
	}
	if(pos == "3"){
			return 3;
	}
	if(pos == "4"){
			return 4;
	}
	if(pos == "5"){
			return 5;
	}
	if(pos == "6"){
			return 6;
	}
	if(pos == "7"){
			return 7;
	}

	return -1;
}

void Board::moveFox(){
	//do all general Fox logic here
	//FOX 0
	bool foxMoved = false;


	if(sheepRipe(fox0col, fox0row, fox0takeSheepCol, fox0takeSheepRow)){
		while(sheepRipe(fox0col, fox0row, fox0takeSheepCol, fox0takeSheepRow)){


			foxTakesSheep(fox0col, fox0row, fox0takeSheepCol, fox0takeSheepRow);
			foxMoved = true;
		}
	}
	else if(fox1col >= 0 && !foxMoved){

		while(sheepRipe(fox1col, fox1row, fox1takeSheepCol, fox1takeSheepRow)){

			foxTakesSheep(fox1col, fox1row, fox1takeSheepCol, fox1takeSheepRow);
			foxMoved = true;
		}
	}
	//FOX 0
	if(!foxMoved && (sheepTwoAway(fox0col, fox0row, fox0threatenCol, fox0threatenRow, fox0highThreat) ||
			sheepTwoAway(fox1col, fox1row, fox1threatenCol, fox1threatenRow, fox1highThreat))){

		if(sheepTwoAway(fox1col, fox1row, fox1threatenCol, fox1threatenRow, fox1highThreat)){

		}
		if(fox0highThreat > fox1highThreat){
			foxThreatensSheep(fox0col, fox0row, fox0threatenCol, fox0threatenRow);
			foxMoved = true;
		}
		else{
			foxThreatensSheep(fox1col, fox1row, fox1threatenCol, fox1threatenRow);
			foxMoved = true;
		}

	}
	else{
		if(!foxMoved){
			foxRandomlyMoves(fox0col, fox0row);
			foxMoved = true;
		}
	}



}

void Board::print(){

	cout << endl << "7     " << boardPositions[0].substr(2,1) << "-" << boardPositions[0].substr(3,1) << "-" << boardPositions[0].substr(4,1) << endl;
	cout << "      |\\|/|" << endl;
	cout << "6     " << boardPositions[1].substr(2,1) << "-" << boardPositions[1].substr(3,1) << "-" << boardPositions[1].substr(4,1) << endl;
	cout << "      |/|\\|" << endl;
	cout << "5 " << boardPositions[2].substr(0,1) << "-" << boardPositions[2].substr(1,1) << "-" << boardPositions[2].substr(2,1) << "-" << boardPositions[2].substr(3,1) << "-"
				 << boardPositions[2].substr(4,1) << "-" << boardPositions[2].substr(5,1) << "-" << boardPositions[2].substr(6,1) << endl;
	cout << "  |\\|/|\\|/|\\|/|" << endl;
	cout << "4 " << boardPositions[3].substr(0,1) << "-" << boardPositions[3].substr(1,1) << "-" << boardPositions[3].substr(2,1) << "-" << boardPositions[3].substr(3,1) << "-"
				 << boardPositions[3].substr(4,1) << "-" << boardPositions[3].substr(5,1) << "-" << boardPositions[3].substr(6,1) << endl;
	cout << "  |/|\\|/|\\|/|\\|" << endl;
	cout << "3 " << boardPositions[4].substr(0,1) << "-" << boardPositions[4].substr(1,1) << "-" << boardPositions[4].substr(2,1) << "-" << boardPositions[4].substr(3,1) << "-"
				 << boardPositions[4].substr(4,1) << "-" << boardPositions[4].substr(5,1) << "-" << boardPositions[4].substr(6,1) << endl;
	cout << "      |\\|/|" << endl;
	cout << "2     " << boardPositions[5].substr(2,1) << "-" << boardPositions[5].substr(3,1) << "-" << boardPositions[5].substr(4,1) << endl;
	cout << "      |/|\\|" << endl;
	cout << "1     " << boardPositions[6].substr(2,1) << "-" << boardPositions[6].substr(3,1) << "-" << boardPositions[6].substr(4,1) << endl;
	cout << "  a b c d e f g";

}

void Board::trim(string &str){
	// trim trailing spaces
	size_t endpos = str.find_last_not_of(" \t");
	if( string::npos != endpos )
	{
	    str = str.substr( 0, endpos+1 );
	}

	// trim leading spaces
	size_t startpos = str.find_first_not_of(" \t");
	if( string::npos != startpos )
	{
	    str = str.substr( startpos );
	}
}

void Board::findFoxes(){
	for(int i = 2; i < 5; i++){
		for(int j = 0; j < 2; j++){
			if(boardPositions[j].substr(i,1) == "F" && fox0col == -1){
				fox0col = i;
				fox0row = j;
			}
			else if(boardPositions[j].substr(i,1) == "F" && fox0col != -1){
				fox1col = i;
				fox1row = j;
			}
		}
	}

	for(int i = 0; i < 7; i++){
		for(int j = 2; j < 5; j++){
			if(boardPositions[j].substr(i,1) == "F" && fox0col == -1){
				fox0col = i;
				fox0row = j;
			}
			else if(boardPositions[j].substr(i,1) == "F" && fox0col != -1){
				fox1col = i;
				fox1row = j;
			}
		}
	}

	for(int i = 2; i < 5; i++){
		for(int j = 5; j < 7; j++){
			if(boardPositions[j].substr(i,1) == "F" && fox0col == -1){
				fox0col = i;
				fox0row = j;
			}
			else if(boardPositions[j].substr(i,1) == "F" && fox0col != -1){
				fox1col = i;
				fox1row = j;
			}
		}
	}
}

void Board::foxThreatensSheep(int &fCol, int &fRow, int fThreatenCol, int fThreatenRow){
	boardPositionUpdate(3,"F",fThreatenCol, fThreatenRow);
	boardPositionUpdate(1,".",fCol,fRow);

	cout << endl << "The foxes move: " << convertItoS(fCol) << 7 - fRow << "-" << convertItoS(fThreatenCol) << 7 - fThreatenRow;

	fCol = fThreatenCol;
	fRow = fThreatenRow;
}
void Board::foxRandomlyMoves(int &fCol, int &fRow){
	bool foxM = false;
	for(int i = 0; i < 7 && !foxM; i++){
		for(int j = 0; j < 7 && !foxM; j++){
			if((i + 1 == fCol && j + 1 == fRow) ||
					(i == fCol && j + 1 == fRow) ||
					(i - 1 == fCol && j + 1 == fRow) ||
					(i - 1 == fCol && j == fRow) ||
					(i - 1 == fCol && j - 1 == fRow) ||
					(i == fCol && j - 1 == fRow) ||
					(i + 1 == fCol && j - 1 == fRow) ||
					(i + 1 == fCol && j == fRow)){
				if(boardPos[i][j] == 1){

					boardPositionUpdate(3, "F", i, j);
					boardPositionUpdate(1, ".", fCol, fRow);

					cout << endl << "The foxes move: " << convertItoS(fCol) << 7 - fRow << "-" << convertItoS(i) << 7 - j;

					fCol = i;
					fRow = j;
					foxM = true;
				}
			}
		}
	}
}
bool Board::checkFoxWin(){
	if(countSheep() < 9){
		cout << endl << "The foxes win!";
		return true;
	}
	else{
		return false;
	}
}
bool Board::checkSheepWin(){
	if(sheepInPaddock()){
		cout << endl << "The sheep win!";
		return true;
	}
	if(fox0col < 0 && fox1col < 0){
		cout << endl << "The sheep win!";
		return true;
	}
	else{
		return false;
	}


}

bool Board::sheepRipe(int fCol, int fRow, int &fTakeSheepCol, int &fTakeSheepRow){

	//check for diag
	if(!((fCol + fRow) % 2)){
		//check if fox is in center area
		if(fCol > 1 && fCol < 5 && fRow > 1 && fRow < 5){
			//check all directions for sheep
			if( boardPos[fCol + 1][fRow + 1] == 2 &&
					boardPos[fCol + 2][fRow + 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else if(boardPos[fCol + 1][fRow] == 2 &&
					boardPos[fCol + 2][fRow] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(boardPos[fCol][fRow + 1] == 2 &&
					boardPos[fCol][fRow + 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else if(boardPos[fCol + 1][fRow - 1] == 2 &&
					boardPos[fCol + 2][fRow - 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(boardPos[fCol - 1][fRow + 1] == 2 &&
					boardPos[fCol - 2][fRow + 2] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else if(boardPos[fCol - 1][fRow - 1] == 2 &&
					boardPos[fCol - 2][fRow - 2] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(boardPos[fCol - 1][fRow] == 2 &&
					boardPos[fCol - 2][fRow] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(boardPos[fCol][fRow - 1] == 2 &&
					boardPos[fCol][fRow - 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else{
				return false;
			}
		}

		//fox is on ring
		else if(fCol == 1 || fCol == 5 || fRow == 1 || fRow == 5){
			//left ring
			if(fCol == 1 &&
					boardPos[fCol][fRow - 1] == 2 &&
					boardPos[fCol][fRow - 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fCol == 1 &&
					boardPos[fCol + 1][fRow - 1] == 2 &&
					boardPos[fCol + 2][fRow - 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fCol == 1 &&
					boardPos[fCol + 1][fRow] == 2 &&
					boardPos[fCol + 2][fRow] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(fCol == 1 &&
					boardPos[fCol + 1][fRow + 1] == 2 &&
					boardPos[fCol + 2][fRow + 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else if(fCol == 1 &&
					boardPos[fCol][fRow + 1] == 2 &&
					boardPos[fCol][fRow + 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}

			//right ring
			else if(fCol == 5 &&
					boardPos[fCol][fRow - 1] == 2 &&
					boardPos[fCol][fRow - 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fCol == 5 &&
					boardPos[fCol - 1][fRow - 1] == 2 &&
					boardPos[fCol - 2][fRow - 2] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fCol == 5 &&
					boardPos[fCol - 1][fRow] == 2 &&
					boardPos[fCol - 2][fRow] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(fCol == 5 &&
					boardPos[fCol - 1][fRow + 1] == 2 &&
					boardPos[fCol - 2][fRow + 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(fCol == 5 &&
					boardPos[fCol][fRow + 1] == 2 &&
					boardPos[fCol][fRow + 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			//top ring
			else if(fRow == 1 &&
					boardPos[fCol - 1][fRow] == 2 &&
					boardPos[fCol - 2][fRow] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(fRow == 1 &&
					boardPos[fCol - 1][fRow + 1] == 2 &&
					boardPos[fCol - 2][fRow + 2] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else if(fRow == 1 &&
					boardPos[fCol][fRow + 1] == 2 &&
					boardPos[fCol][fRow + 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else if(fRow == 1 &&
					boardPos[fCol + 1][fRow + 1] == 2 &&
					boardPos[fCol + 2][fRow + 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else if(fRow == 1 &&
					boardPos[fCol + 1][fRow] == 2 &&
					boardPos[fCol + 2][fRow] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			//bottom ring
			else if(fRow == 6 &&
					boardPos[fCol - 1][fRow] == 2 &&
					boardPos[fCol - 2][fRow] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fRow == 6 &&
					boardPos[fCol - 1][fRow - 1] == 2 &&
					boardPos[fCol - 2][fRow - 2] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fRow == 6 &&
					boardPos[fCol][fRow - 1] == 2 &&
					boardPos[fCol][fRow - 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fRow == 6 &&
					boardPos[fCol + 1][fRow - 1] == 2 &&
					boardPos[fCol + 2][fRow - 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(fRow == 6 &&
					boardPos[fCol][fRow + 1] == 2 &&
					boardPos[fCol][fRow + 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else{
				return false;
			}

		}
		//fox is on edge
		else{

			//left edge
			if(fCol == 0){
				if(boardPos[fCol][fRow - 1] == 2 &&
						boardPos[fCol][fRow - 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol + 1][fRow - 1] == 2 &&
						boardPos[fCol + 2][fRow - 2] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol + 1][fRow] == 2 &&
						boardPos[fCol + 2][fRow] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else if(boardPos[fCol + 1][fRow + 1] == 2 &&
						boardPos[fCol + 2][fRow + 2] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else if(boardPos[fCol][fRow + 1] == 2 &&
						boardPos[fCol][fRow + 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else{
					return false;
				}
			}
			//right edge
			if(fCol == 6){
				if(boardPos[fCol][fRow - 1] == 2 &&
						boardPos[fCol][fRow - 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol - 1][fRow - 1] == 2 &&
						boardPos[fCol - 2][fRow - 2] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol - 1][fRow] == 2 &&
						boardPos[fCol - 2][fRow] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else if(boardPos[fCol - 1][fRow + 1] == 2 &&
						boardPos[fCol - 2][fRow + 2] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else if(boardPos[fCol][fRow + 1] == 2 &&
						boardPos[fCol][fRow + 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else{
					return false;
				}
			}
			//top edge
			if(fRow == 0){
				if(boardPos[fCol - 1][fRow] == 2 &&
						boardPos[fCol - 2][fRow] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else if(boardPos[fCol - 1][fRow + 1] == 2 &&
						boardPos[fCol - 2][fRow + 2] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else if(boardPos[fCol][fRow + 1] == 2 &&
						boardPos[fCol][fRow + 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else if(boardPos[fCol + 1][fRow + 1] == 2 &&
						boardPos[fCol + 2][fRow + 2] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else if(boardPos[fCol + 1][fRow] == 2 &&
						boardPos[fCol + 2][fRow] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else {
					return false;
				}
			}
			//bottom edge
			if(fRow == 6){
				if(boardPos[fCol - 1][fRow] == 2 &&
						boardPos[fCol - 2][fRow] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else if(boardPos[fCol - 1][fRow - 1] == 2 &&
						boardPos[fCol - 2][fRow - 2] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol][fRow - 1] == 2 &&
						boardPos[fCol][fRow - 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol + 1][fRow - 1] == 2 &&
						boardPos[fCol + 2][fRow - 2] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol + 1][fRow] == 2 &&
						boardPos[fCol + 2][fRow] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else{
					return false;
				}
			}

		}
	}
	//fox is not on a diagonal
	else{
		//check if fox is in center area
		if(fCol > 0 && fCol < 6 && fRow > 0 && fRow < 6){
			//check all directions for sheep
			if(boardPos[fCol + 1][fRow] == 2 &&
					boardPos[fCol + 2][fRow] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(boardPos[fCol][fRow + 1] == 2 &&
					boardPos[fCol][fRow + 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else if(boardPos[fCol - 1][fRow] == 2 &&
					boardPos[fCol - 2][fRow] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(boardPos[fCol][fRow - 1] == 2 &&
					boardPos[fCol][fRow - 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else{
				return false;


			}
		}
		//fox is on ring
		else if(fCol == 1 || fCol == 5 || fRow == 1 || fRow == 5){
			//left ring
			if(fCol == 1 &&
					boardPos[fCol][fRow - 1] == 2 &&
					boardPos[fCol][fRow - 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fCol == 1 &&
					boardPos[fCol + 1][fRow] == 2 &&
					boardPos[fCol + 2][fRow] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(fCol == 1 &&
					boardPos[fCol][fRow + 1] == 2 &&
					boardPos[fCol][fRow + 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			//right ring
			if(fCol == 5 &&
					boardPos[fCol][fRow - 1] == 2 &&
					boardPos[fCol][fRow - 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fCol == 5 &&
					boardPos[fCol - 1][fRow] == 2 &&
					boardPos[fCol - 2][fRow] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(fCol == 5 &&
					boardPos[fCol][fRow + 1] == 2 &&
					boardPos[fCol][fRow + 2] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			//top ring
			if(fRow == 1 &&
					boardPos[fCol - 1][fRow] == 2 &&
					boardPos[fCol - 2][fRow] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(fRow == 1 &&
					boardPos[fCol][fRow + 1] == 2 &&
					boardPos[fCol][fRow + 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow + 1;
				return true;
			}
			else if(fRow == 1 &&
					boardPos[fCol + 1][fRow] == 2 &&
					boardPos[fCol + 2][fRow] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			//bottom ring
			if(fRow == 5 &&
					boardPos[fCol - 1][fRow] == 2 &&
					boardPos[fCol - 2][fRow] == 1){
				fTakeSheepCol = fCol - 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else if(fRow == 5 &&
					boardPos[fCol][fRow - 1] == 2 &&
					boardPos[fCol][fRow - 2] == 1){
				fTakeSheepCol = fCol;
				fTakeSheepRow = fRow - 1;
				return true;
			}
			else if(fRow == 5 &&
					boardPos[fCol + 1][fRow] == 2 &&
					boardPos[fCol + 2][fRow] == 1){
				fTakeSheepCol = fCol + 1;
				fTakeSheepRow = fRow;
				return true;
			}
			else{
				return false;
			}
		}
		//fox is on edge
		else{

			//left edge
			if(fCol == 0){
				if(boardPos[fCol][fRow - 1] == 2 &&
						boardPos[fCol][fRow - 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol + 1][fRow] == 2 &&
						boardPos[fCol + 2][fRow] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else if(boardPos[fCol][fRow + 1] == 2 &&
						boardPos[fCol][fRow + 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else{
					return false;
				}
			}
			//right edge
			if(fCol == 6){
				if(boardPos[fCol][fRow - 1] == 2 &&
						boardPos[fCol][fRow - 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol - 1][fRow] == 2 &&
						boardPos[fCol - 2][fRow] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else if(boardPos[fCol][fRow + 1] == 2 &&
						boardPos[fCol][fRow + 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else{
					return false;
				}
			}
			//top edge
			if(fRow == 0){
				if(boardPos[fCol - 1][fRow] == 2 &&
						boardPos[fCol - 2][fRow] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else if(boardPos[fCol][fRow + 1] == 2 &&
						boardPos[fCol][fRow + 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow + 1;
					return true;
				}
				else if(boardPos[fCol + 1][fRow] == 2 &&
						boardPos[fCol + 2][fRow] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else {
					return false;
				}
			}
			//bottom edge
			if(fRow == 6){
				if(boardPos[fCol - 1][fRow] == 2 &&
						boardPos[fCol - 2][fRow] == 1){
					fTakeSheepCol = fCol - 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else if(boardPos[fCol][fRow - 1] == 2 &&
						boardPos[fCol][fRow - 2] == 1){
					fTakeSheepCol = fCol;
					fTakeSheepRow = fRow - 1;
					return true;
				}
				else if(boardPos[fCol + 1][fRow] == 2 &&
						boardPos[fCol + 2][fRow] == 1){
					fTakeSheepCol = fCol + 1;
					fTakeSheepRow = fRow;
					return true;
				}
				else{
					return false;
				}
			}
			else{
				return false;
			}
		}

	}

}

void Board::foxTakesSheep(int &fCol, int &fRow , int takeCol, int takeRow){
	int jumpCol = ((takeCol - fCol) * 2) + fCol;
	int jumpRow = ((takeRow - fRow) * 2) + fRow;
	boardPositionUpdate(3,"F",jumpCol,jumpRow);
	boardPositionUpdate(1,".",fCol,fRow);
	boardPositionUpdate(1,".",takeCol,takeRow);

	cout << endl << "The foxes move: " << convertItoS(fCol) << 7 - fRow << "-" << convertItoS(jumpCol) << 7 - jumpRow;

	fCol = jumpCol;
	fRow = jumpRow;
}

bool Board::sheepTwoAway(int fCol, int fRow, int &fThreatCol, int &fThreatRow, int &highThreat){
	int highestThreat = 0;
	int currentThreat = 0;

	//within bounds of array
	for(int i = 0; i < 7; i++){
		for(int j = 0; j < 7; j++){
			//check neighboring spaces for threat
			if((i + 1 == fCol && j + 1 == fRow) ||
								(i == fCol && j + 1 == fRow) ||
								(i - 1 == fCol && j + 1 == fRow) ||
								(i - 1 == fCol && j == fRow) ||
								(i - 1 == fCol && j - 1 == fRow) ||
								(i == fCol && j - 1 == fRow) ||
								(i + 1 == fCol && j - 1 == fRow) ||
								(i + 1 == fCol && j == fRow)){
				if(boardPos[i][j] == 1){

					currentThreat = checkThreat(i, j);
					if(currentThreat > highestThreat){
						fThreatCol = i;
						fThreatRow = j;
						highestThreat = currentThreat;
					}
				}
			}
		}
	}
	if(highestThreat == 0){
		return false;
	}
	else{
		highThreat = highestThreat;
		return true;
	}
}

int Board::checkThreat(int col, int row){
	int threat = 0;

	for(int i = 0; i < 7; i++){
		for(int j = 0; j < 7; j++){
			if((i + 1 == col && j + 1 == row) ||
					(i == col && j + 1 == row) ||
					(i - 1 == col && j + 1 == row) ||
					(i - 1 == col && j == row) ||
					(i - 1 == col && j - 1 == row) ||
					(i == col && j - 1 == row) ||
					(i + 1 == col && j - 1 == row) ||
					(i + 1 == col && j == row)){
				if(boardPos[i][j] == 2){
					threat++;
				}
			}
		}
	}
	return threat;
}

void Board::boardPositionUpdate(int n, string s, int col, int row){
	boardPos[col][row] = n;
	boardPositions[row].replace(col,1,s);
}

int Board::countSheep(){
	int numSheep = 0;
	for (int i = 0; i < 7; i++){
		for (int j = 0; j < 7; j++){
			if(boardPos[i][j] == 2){
				numSheep++;
			}
		}
	}
	return numSheep;
}

bool Board::sheepInPaddock(){
	if(boardPos[2][0] == 2 &&
			boardPos[3][0] == 2 &&
			boardPos[4][0] == 2 &&
			boardPos[2][1] == 2 &&
			boardPos[3][1] == 2 &&
			boardPos[4][1] == 2 &&
			boardPos[2][2] == 2 &&
			boardPos[3][2] == 2 &&
			boardPos[4][2] == 2){
		return true;
	}
	else{
		return false;
	}
}

string Board::getSheepMove(){
	return sheepMove;
}

string Board::convertItoS(int col){
	switch(col){
	case 0:
		return "a";
		break;
	case 1:
		return "b";
		break;
	case 2:
		return "c";
		break;
	case 3:
		return "d";
		break;
	case 4:
		return "e";
		break;
	case 5:
		return "f";
		break;
	case 6:
		return "g";
		break;
	default:
		return " ";
		break;
	}
}

bool Board::sheepCaptureFox(int col, int row){
	int possibleMoves = 0;

		for(int i = 0; i < 7; i++){
			for(int j = 0; j < 7; j++){
				if((i + 1 == col && j + 1 == row) ||
						(i == col && j + 1 == row) ||
						(i - 1 == col && j + 1 == row) ||
						(i - 1 == col && j == row) ||
						(i - 1 == col && j - 1 == row) ||
						(i == col && j - 1 == row) ||
						(i + 1 == col && j - 1 == row) ||
						(i + 1 == col && j == row)){
					if(boardPos[i][j] == 1){
						possibleMoves++;
					}
				}
			}
		}

	if(possibleMoves == 0){
		return true;
	}
	else{
		return false;
	}
}
