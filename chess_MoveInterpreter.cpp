/*
	incoming file should contain the original and final coordinates of the 
	piece that moved
	
	program should wait for file to update, read file, produce a FEN string,
	send FEN string off to stockfish, await stockfish response, list of pickup 
	and dropoff coordinates, then update and close file
	
	note that FEN is a position notation convention that is used by stockfish.
	stockfish will most likely take in a FEN position and then produce a move
	(eg. B7/2P2N2/6Q1/4p3/k6N/2P1p2P/1P6/K7 w - - 0 1 ------> ke5)
*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

int position[8][8];

/*
	0 - nothing
	1_ - white piece
	2_ - black piece
	_1 - pawn
	_3 - knight
	_4 - bishop
	_2 - rook
	_5 - queen
	_6 - king
*/

void setStartPos() {
	position[0][0] = 12;
	position[1][0] = 13;
	position[2][0] = 14;
	position[3][0] = 15;
	position[4][0] = 16;
	position[5][0] = 14;
	position[6][0] = 13;
	position[7][0] = 12;

	for(int x = 0; x < 8; x++) {
		position[x][1] = 11;
	}	
	
	for(int y = 2; y < 6; y++) {
		for(int x = 0; x < 8; x++) {
			position[x][y] = 0;
		}
	}
	
	for(int x = 0; x < 8; x++) {
		position[x][6] = 21;
	}		
	
	position[0][7] = 22;
	position[1][7] = 23;
	position[2][7] = 24;
	position[3][7] = 25;
	position[4][7] = 26;
	position[5][7] = 24;
	position[6][7] = 23;
	position[7][7] = 22;
}

string posToFen() {
	stringstream FEN;
	int emptyCnt = 0;
	int crntPiece = -1;
	
	for(int y = 7; y >= 0; y--) {
		
		for(int x = 0; x < 8; x++) {
			crntPiece = position[x][y];
			if(crntPiece == 0)
				emptyCnt++;
			else {
				if(emptyCnt > 0) {
					FEN << emptyCnt;
					emptyCnt = 0;
				}
				
				if(crntPiece == 11)
					FEN << "P";
				else if(crntPiece == 12)
					FEN << "R";
				else if(crntPiece == 13)
					FEN << "N";
				else if(crntPiece == 14)
					FEN << "B";
				else if(crntPiece == 15)
					FEN << "Q";
				else if(crntPiece == 16)
					FEN << "K";
				else if(crntPiece == 21)
					FEN << "p";
				else if(crntPiece == 22)
					FEN << "r";
				else if(crntPiece == 23)
					FEN << "n";
				else if(crntPiece == 24)
					FEN << "b";
				else if(crntPiece == 25)
					FEN << "q";
				else if(crntPiece == 26)
					FEN << "k";
			}
		}
		
		if(emptyCnt > 0) {
			FEN << emptyCnt;
			emptyCnt = 0;
		}
		
		if(y>0) {
			FEN << "/";
		}
	}
	
	FEN << " b"; //As it will be blacks move (robot is playing black)
	
	FEN << " --"; //Does not allow for castling (should be implemented later)
	
	FEN << " 0"; //Should increment for every halfmove and reset after a pawn is moved or captured (draw available after >50)
	
	FEN << " 0"; //Should increment for every full move after black has moved
	
	return FEN.str();
}

int main() {

	//wait for file to update
	
	//open file
	
	setStartPos();
	
	cout << posToFen() << endl;
	
	system("PAUSE");
	return EXIT_SUCCESS;
}
