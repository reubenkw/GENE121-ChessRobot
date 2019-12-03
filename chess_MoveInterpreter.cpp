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

/*
NAME: chessMove
PARAMETERS: int const x_init0, int const y_init0, int const x_fin0, int const y_fin0
DISC: Stores a single chess move for the purposes of sending legal moves to robotC
*/
class chessMove
{
	private:
		int x_init, y_init, x_fin, y_fin;
	public:
		chessMove()
		{
			x_init = 0;
			y_init = 0;
			x_fin = 0;
			y_fin = 0;
		}
		chessMove(int const x_init0, int const y_init0, int const x_fin0, int const y_fin0)
		{
			x_init = x_init0;
			y_init = y_init0;
			x_fin = x_fin0;
			y_fin = y_fin0;
		}
		int get_xinit() const
		{
			return x_init;
		}
		int get_yinit() const
		{
			return y_init;
		}
		int get_xfin() const
		{
			return x_fin;
		}
		int get_yfin() const
		{
			return y_fin;
		}
};

/*
NAME: waitForRCFile
PARAMETERS: int desiredFirstElement
DESC: Checks robotC to C++ file for update every two seconds and exits once update is found
*/
void waitForRCFile(int desiredFirstElement) {
	int inNum = desiredFirstElement + 1;
	do {
		this_thread::sleep_for(chrono::milliseconds(2000));
		ifstream finRC("IPC/IPC_RC_to_CPP.txt");
		finRC >> inNum;
		finRC.close();
	} while (inNum != desiredFirstElement);
}

/*
NAME: waitForPYFile
PARAMETERS: int desiredFirstElement
DESC: Checks python to C++ file for update every two seconds and exits once update is found
*/
void waitForPYFile(int desiredFirstElement) {
	int inNum = desiredFirstElement + 1;
	do {
		this_thread::sleep_for(chrono::milliseconds(2000));
		ifstream finPY("IPC/IPC_PY_to_CPP.txt");
		finPY >> inNum;
		finPY.close();
	} while (inNum != desiredFirstElement);
}

/*
NAME: rowNumToChar
PARAMETERS: int row
DESC: Converts a row in coordinate form to a letter for chess board notation
*/
string rowNumToChar(int row) {
	switch (row) {
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

/*
NAME: coordsToMoveNotation
PARAMETERS: int Fx_init, int Fy_init, int Fx_fin, int Fy_fin, char& promotedPiece, char position[8][8]
DESC: Converts move in coordinates form to long algebriac chess move notation
*/
string coordsToMoveNotation(int Fx_init, int Fy_init, int Fx_fin, int Fy_fin, char& promotedPiece, char position[8][8]) {
	stringstream returnStr;
	returnStr << rowNumToChar(Fx_init);
	returnStr << Fy_init + 1;
	returnStr << rowNumToChar(Fx_fin);
	returnStr << Fy_fin + 1;
	if (position[Fx_init][Fy_init] == 'P' && Fy_fin == 7) {
		cout << "What piece did you promote your pawn to? (Options: q,n,r,b): ";
		cin >> promotedPiece;
		returnStr << promotedPiece;
	}
	return returnStr.str();
}

/*
NAME: updateBoard
PARAMETERS: int Fx_init, int Fy_init, int Fx_fin, int Fy_fin, char& promotedPiece, char position[8][8]
DESC: Updates board array by replacing piece in the final position with the peice in the original location
*/
void updateBoard(int Fx_init, int Fy_init, int Fx_fin, int Fy_fin, char& promotedPiece, char position[8][8]) {
	if (position[Fx_init][Fy_init] == 'K' && Fx_init == 4 && Fy_init == 0 && (Fx_fin == 2 || Fx_fin == 6)) {
		if (Fx_fin == 2) {
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
	else if (position[Fx_init][Fy_init] == 'P' && Fy_fin == 7) {
		position[Fx_fin][Fy_fin] = promotedPiece;
		promotedPiece = 0;
		position[Fx_init][Fy_init] = '.';
	}
	else {
		position[Fx_fin][Fy_fin] = position[Fx_init][Fy_init];
		position[Fx_init][Fy_init] = '.';
	}
}

/*
NAME: moveNotationToCoords
PARAMETERS: int& Fx_init, int& Fy_init, int& Fx_fin, int& Fy_fin, string move, char& promotedPiece
DESC: Converts move in long algebriac notation to coordinate form
*/
void moveNotationToCoords(int& Fx_init, int& Fy_init, int& Fx_fin, int& Fy_fin, string move, char& promotedPiece) {
	Fx_init = move[0] - 97;
	Fy_init = move[1] - 49;

	Fx_fin = move[2] - 97;
	Fy_fin = move[3] - 49;

	if(move[4] != 0)
		promotedPiece = move[4];
}

/*
NAME: deleteLegalMove
PARAMETERS: int moveNum, int& totalLM, chessMove legalMoves[]
DESC: Deletes move in legal moves array by shifting all moves above it down one to overide the move
*/
void deleteLegalMove(int moveNum, int& totalLM, chessMove legalMoves[])
{
	while (moveNum < totalLM - 1)
	{
		legalMoves[moveNum] = legalMoves[moveNum + 1];
		moveNum++;
	}
	totalLM--;
}

/*
NAME: printBoard
PARAMETERS: char position[8][8]
DESC: Outputs board to screen for user varification
*/
void printBoard(char position[8][8])
{
	for(int y = 7; y >= 0; y--)
	{
		for(int x = 0; x < 8; x++)
		{
			cout << position[x][y] << " ";
		}
		cout << endl;
	}
}

/*
NAME: sendValidMoves
PARAMETERS: ostream& foutRC, char& promotedPiece
DESC: Outputs legal next moves to robotC communication file for optimized searching
*/
void sendValidMoves(ostream& foutRC, char& promotedPiece) {
	ifstream finPY("IPC/IPC_PY_to_CPP.txt");
	chessMove legalMoves[250];
	string elmt = "";
	int x_init = 0, y_init = 0, x_fin = 0, y_fin = 0;
	
	finPY << elmt << elmt;
	
	int moveNum = 0;
	string legal_move;
	
	while(finPY >> legal_move)
	{
		moveNotationToCoords(x_init, y_init, x_fin, y_fin, legal_move, promotedPiece);
	
		legalMoves[moveNum] = chessMove(x_init, y_init, x_fin, y_fin);
		moveNum++;
	}

	int numLM = moveNum;

	moveNum = 0;

	while (finPY >> legal_move)
	{
		moveNotationToCoords(x_init, y_init, x_fin, y_fin, legal_move, promotedPiece);

		legalMoves[moveNum] = chessMove(x_init, y_init, x_fin, y_fin);
		moveNum++;
	}
	numLM = moveNum;
	moveNum = 0;

	while (moveNum < numLM)
	{
		x_init = legalMoves[moveNum].get_xinit();
		y_init = legalMoves[moveNum].get_yinit();

		foutRC << endl << "piecePos " << x_init << " " << y_init << endl
			<< "destinationPos " << legalMoves[moveNum].get_xfin() << " " << legalMoves[moveNum].get_yfin();
		
		int checkMoveNum = moveNum + 1;
		while (checkMoveNum < numLM)
		{
			if (x_init == legalMoves[checkMoveNum].get_xinit() && y_init == legalMoves[checkMoveNum].get_yinit())
			{
				foutRC << endl << "destinationPos " << legalMoves[checkMoveNum].get_xfin() 
					<< " " << legalMoves[checkMoveNum].get_yfin();
				deleteLegalMove(checkMoveNum, numLM, legalMoves);
			}
			else
				checkMoveNum++;
		}
		moveNum++;
	}
	finPY.close();
}

/*
NAME: Main
DESC: Facilitates all aspects of program including game startup, play, error handling and shutdown proceedures
*/
int main() {	
	char position[8][8] = {
		{'R', 'P', '.', '.', '.', '.', 'p', 'r'},
		{'N', 'P', '.', '.', '.', '.', 'p', 'n'},
		{'B', 'P', '.', '.', '.', '.', 'p', 'b'},
		{'Q', 'P', '.', '.', '.', '.', 'p', 'q'},
		{'K', 'P', '.', '.', '.', '.', 'p', 'k'},
		{'B', 'P', '.', '.', '.', '.', 'p', 'b'},
		{'N', 'P', '.', '.', '.', '.', 'p', 'n'},
		{'R', 'P', '.', '.', '.', '.', 'p', 'r'}
	};
	
	int inNumRC = 0, outNumRC = 1, inNumPY = 0, outNumPY = 0;

	//Resets output to python file
	ofstream foutPY("IPC/IPC_CPP_to_PY.txt");
	foutPY << outNumPY;
	outNumPY++;
	foutPY.close();
	
	//Resets output to robotC file
	ofstream foutRC("IPC/IPC_CPP_to_RC.txt");
	foutRC << outNumRC;
	outNumRC++;
	foutRC.close();

	cout << "Waiting for file sync" << endl;
	
	//Waits for python file to reset to 0
	waitForPYFile(inNumPY);
	inNumPY++;
	
	char promotedPiece = 0; 							//used to store users last promoted piece type
	int x_init = 0, y_init = 0, x_fin = 0, y_fin = 0; 	//used for both CPP to PY and PY to RC
	bool gameOver = false;
	int firstElement = 0;								//not used but required to get past the first element
	string userMove = "";
	string pyCmd = "";
	string compMove = "";
	int moveNum = 0;
	string legal_move = "";
	int numLM = 0;
	chessMove prevCompMove;
	string waitElmt = "";

	cout << "File sync successful" << endl;

	while (!gameOver)
	{
		printBoard(position);
		cout << "Waiting for your move" << endl;
		
		waitForRCFile(inNumRC);
		inNumRC++;

		ifstream finRC("IPC/IPC_RC_to_CPP.txt");

		finRC >> firstElement;		//just to move to next element
		finRC >> x_init >> y_init >> x_fin >> y_fin;
		
		finRC.close();

		//(1,1) to (1,1) is an error code for piece not able to move
		if(x_init == 1 && y_init == 1 && x_fin == 1 && y_fin == 1)
		{
			cout << "Please move " << rowNumToChar(prevCompMove.get_xinit()) << prevCompMove.get_yinit() + 1 
				 << rowNumToChar(prevCompMove.get_xfin()) << prevCompMove.get_yfin() + 1
				 << "enter any character, make your move" << endl 
				 << "and then press the centre button on the robot when finished." << endl;
			
			cin >> waitElmt;
			
			if(waitElmt == "forfeit")
			{
				gameOver = true;
				pyCmd = "Computer_wins";
			}
			else
			{
				ofstream foutRC("IPC/IPC_CPP_to_RC.txt");
				foutRC << outNumRC << endl << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
				sendValidMoves(foutRC, promotedPiece);
				outNumRC++;
				foutRC.close();
			}
		}
		//(0,0) to (0,0) is an error code for piece not found
		else if(x_init == 0 && y_init == 0 && x_fin == 0 && y_fin == 0)
		{
			cout << "Invalid Move. Please rearrange the board to the above position, " << endl 
			<< "enter any character and then make valid move" << endl;
			
			cin >> waitElmt;
			
			ofstream foutRC("IPC/IPC_CPP_to_RC.txt");
			foutRC << outNumRC << endl << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
			sendValidMoves(foutRC, promotedPiece);
			outNumRC++;
			foutRC.close();
		}
		else
		{
			ofstream foutPY("IPC/IPC_CPP_to_PY.txt");
	
			userMove = coordsToMoveNotation(x_init, y_init, x_fin, y_fin, promotedPiece, position);
			foutPY << outNumPY << endl << userMove;
			cout << "Human move: " << userMove << endl;
			outNumPY++;
	
			foutPY.close();
	
			cout << "Waiting for python file" << endl;
			waitForPYFile(inNumPY);
			inNumPY++;
			cout << "Python file recieved" << endl;
	
			ifstream finPY("IPC/IPC_PY_to_CPP.txt");
	
			finPY >> firstElement >> pyCmd;
			
			if (pyCmd == "none")
			{
				updateBoard(x_init, y_init, x_fin, y_fin, promotedPiece, position);
				
				printBoard(position);
				
				finPY >> compMove;
				
				cout << "Computer Move: " << compMove << endl;
				
				moveNotationToCoords(x_init, y_init, x_fin, y_fin, compMove, promotedPiece);
	
				updateBoard(x_init, y_init, x_fin, y_fin, promotedPiece, position);
				
				ofstream foutRC("IPC/IPC_CPP_to_RC.txt");
	
				foutRC << outNumRC << endl << x_init << " " << y_init << " " << x_fin << " " << y_fin;
				
				prevCompMove = chessMove(x_init, y_init, x_fin, y_fin);
				
				sendValidMoves(foutRC, promotedPiece);
				
				outNumRC++;
				foutRC.close();
			}
			else if(pyCmd == "Illegal_Move")
			{
				cout << "Invalid Move. Please rearrange the board to the above position, " << endl 
				<< "enter any character and then make valid move" << endl;
				
				cin >> waitElmt;
				
				ofstream foutRC("IPC/IPC_CPP_to_RC.txt");
				foutRC << outNumRC << endl << 0 << " " << 0 << " " << 0 << " " << 0 << endl;
				sendValidMoves(foutRC, promotedPiece);
				outNumRC++;
				foutRC.close();
			}
			else
			{
				gameOver = true;
				if (pyCmd == "Computer_wins")
				{
					finPY >> compMove;
	
					moveNotationToCoords(x_init, y_init, x_fin, y_fin, compMove, promotedPiece);
	
					ofstream foutRC("IPC/IPC_CPP_to_RC.txt");
	
					foutRC << outNumRC << endl << x_init << " "
						<< y_init << " " << x_fin << " " << y_fin;
	
					foutRC.close();
				}
			}
			finPY.close();
		}
	}
	cout << pyCmd << endl << "Good game!" << endl 
		 << "Please move cursor to top left corner of the screen" << endl;

	system("PAUSE");
	return EXIT_SUCCESS;
}
