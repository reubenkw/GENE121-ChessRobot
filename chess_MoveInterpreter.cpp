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
#include <string>
#include <sstream>
#include <thread>
#include <chrono>

using namespace std;

char position[8][8];
char newPosition[8][8];

//Capital letter means white

void setStartPos() {
	position[0][0] = 'R';
	position[1][0] = 'N';
	position[2][0] = 'B';
	position[3][0] = 'Q';
	position[4][0] = 'K';
	position[5][0] = 'B';
	position[6][0] = 'N';
	position[7][0] = 'R';

	for(int x = 0; x < 8; x++) {
		position[x][1] = 'P';
	}	
	
	for(int y = 2; y < 6; y++) {
		for(int x = 0; x < 8; x++) {
			position[x][y] = '.';
		}
	}
	
	for(int x = 0; x < 8; x++) {
		position[x][6] = 'p';
	}		
	
	position[0][7] = 'r';
	position[1][7] = 'n';
	position[2][7] = 'b';
	position[3][7] = 'q';
	position[4][7] = 'k';
	position[5][7] = 'b';
	position[6][7] = 'n';
	position[7][7] = 'r';
}

void waitForRCFile(int desiredFirstElement) {
	int inNum = desiredFirstElement + 1;
	do {
		this_thread::sleep_for(chrono::milliseconds(2000));
		ifstream finRC("IPC/IPC_RC_to_CPP.txt");
		finRC >> inNum;
		finRC.close();
	} while(inNum != desiredFirstElement);
}

void waitForPYFile(int desiredFirstElement) {
	int inNum = desiredFirstElement + 1;
	do {
		this_thread::sleep_for(chrono::milliseconds(2000));
		ifstream finPY("IPC/IPC_PY_to_CPP.txt");
		finPY >> inNum;
		finPY.close();
	} while(inNum != desiredFirstElement);
}

string rowNumToChar(int row) {
	switch(row) {
		case 0: return "a";
		case 1:	return "b";
		case 2: return "c";
		case 3: return "d";
		case 4: return "e";
		case 5: return "f";
		case 6: return "g";
		case 7: return "h";
	}
}

string coordsToMoveNotation(int Fx_init, int Fy_init, int Fx_fin, int Fy_fin, char & promotedPiece) {
	stringstream returnStr;
	returnStr << rowNumToChar(Fx_init);
	returnStr << Fy_init + 1;
	returnStr << rowNumToChar(Fx_fin);
	returnStr << b                                                   hbb  Fy_fin + 1;
	if(position[Fx_init][Fy_init] == 'P' && Fy_fin == 7) {
		cout << "What piece did you promote your pawn to? (Options: q,n,r,b): ";
		cin >> promotedPiece;
		returnStr << promotedPiece;
	}
	return returnStr.str();
}

void updateBoard(int Fx_init, int Fy_init, int Fx_fin, int Fy_fin, char & promotedPiece) {
	if(position[Fx_init][Fy_init] == 'K' && Fx_init == 4 && Fy_init == 0 && (Fx_fin == 2 || Fx_fin == 6)) {
		if(Fx_fin == 2) {
			position[4][0] = '.';
			position[2][0] = 'K';
			position[3][0] = 'R';
			position[0][0] = '.';
		}
		else {
			position[4][0] = '.';
			position[6][0] = 'K';
			position[5][0] = 'R';
			position[7][0] = '.';
		}
	}
	else if(position[Fx_init][Fy_init] == 'P' && Fy_fin == 7) {
		position[Fx_fin][Fy_fin] = promotedPiece;
		position[Fx_init][Fy_init] = '.';
	}
	else {
		position[Fx_fin][Fy_fin] = position[Fx_init][Fy_init];
		position[Fx_init][Fy_init] = '.';
	}
}

int compareBoardsForMove(int & Fx_init, int & Fy_init, int & Fx_fin, int & Fy_fin) {
	if(position[4][7] == 'k' && (newPosition[2][7] == 'k' || newPosition[6][7] == 'k')) {
		
		if(newPosition[2][7] == 'k') {
			Fx_init = 4;
			Fy_init = 7;
			Fx_fin = 2;
			Fy_fin = 7;
			position[4][7] = '.';
			position[2][7] = 'k';
			position[3][7] = 'r';
			position[0][7] = '.';			
		}
		else if(newPosition[6][7] == 'k') {
			Fx_init = 4;
			Fy_init = 7;
			Fx_fin = 6;
			Fy_fin = 7;
			position[4][7] = '.';
			position[6][7] = 'k';
			position[5][7] = 'r';
			position[7][7] = '.';
		}
	}
	
	for(int y = 0; y < 8; y++) {
		for(int x = 0; x < 8; x++) {
			
			if(position[x][y] != newPosition[x][y]) {
				if(newPosition[x][y] == '.') {
					Fx_init = x;
					Fy_init = y;
					position[x][y] = newPosition[x][y];
				} 
				else {
					Fx_fin = x;
					Fy_fin = y;
					position[x][y] = newPosition[x][y];
				}
			}
		}
	}
}

int main() {
	
	int inNumRC = 0, outNumRC = 0, inNumPY = 0, outNumPY = 0;
	
	//Resets output to python file
	ofstream foutPY("IPC/IPC_CPP_to_PY.txt");
	foutPY << outNumPY;
	outNumPY++;
	foutPY.close();
	
	//Waits for input from robotc file to be reset
	//waitForRCFile(inNumRC);
	//inNumRC++;
	
	char promotedPiece = 0; //used to store users last promoted piece type (initialized to NULL character)
	int x_init = 0, y_init = 0, x_fin = 0, y_fin = 0; //used for both RC to PY and PY to RC
	bool gameOver = false;
	int firstElement = 0;	//not used but required to get past the first element
	string result = "";
	while(gameOver == false) {
		cout << "Your Move" << endl;
		
		//waitForRCFile(inNumRC);
		//inNumPY++;
		
		//ifstream finRC("IPC/IPC_RC_to_CPP.txt");
		
		//finRC >> firstElement;		//just to move to next element
		cin >> x_init >> y_init >> x_fin >> y_fin;
		
		//finRC.close();
		
		ofstream foutPY("IPC/IPC_CPP_to_PY.txt");
		
		result = coordsToMoveNotation(x_init, y_init, x_fin, y_fin, promotedPiece);
		foutPY << outNumPY << endl << result;
		cout << result << endl;
		outNumPY++;
		
		foutPY.close();
		
		updateBoard(x_init, y_init, x_fin, y_fin, promotedPiece);
		
		waitForPYFile(inNumPY);
		inNumPY++;
		
		ifstream finPY("IPC/IPC_PY_to_CPP.txt");
		
		for(int y = 0; y < 8; y++) {
			for(int x = 0; x < 8; x++) {
				finPY >> newPosition[x][y];
			}
		}
		
		finPY.close();
		
		ofstream foutRC("IPC/IPC_CPP_to_RC.txt");
		
		compareBoardsForMove(x_init, y_init, x_fin, y_fin);
		
		foutRC << outNumRC << x_init << y_init << x_fin << y_fin;
		cout << outNumRC << x_init << y_init << x_fin << y_fin << endl;
		outNumRC++;
		
		foutRC.close();
	}
	
	system("PAUSE");
	return EXIT_SUCCESS;
}
