//global array for robotC
char chessboard[8][8];

//movement based on file
#include "PC_FileIO.c"
//desiredFirstElement = 0;

void waitForCPPFile(int desiredFirstElement) {
	int inNum = desiredFirstElement + 1;
	do{
		wait1Msec(2000);
		TFileHandle fin;
		readIntPC(fin,inNum);
		//close file
	} while(inNum != desiredFirstElement);
}

void readLocationInput(TFileHandle&fin, int*moveLocation, int*board, int*userMove, int&desiredFirstElement)
{
	//move location will be [xinitial,yinitial,xfinal,yfinal]
	int numEaten = 0;
	bool fileOkay = openReadPC(fin,"location.txt");
	if(!fileOkay)
	{
		displayString(2,"Could not find file");
		wait1Msec(5000);
	}
	else
	{
		waitForCPPFile(desiredFirstElement);

		for(int counter = 0; counter < 4; counter++)
		{
			int a = 0;
			readIntPC(fin,a);
			moveLocation[counter] = a;
		}

		//check for eaten pieces
		//assume that no chess piece value is 0
		if(board[moveLocation[2]][moveLocation[3]] != 0)
		{
			numEaten++;
			movePosition(moveLocation[2],moveLocation[3]);
			//pick up piece
			movePosition(9, numEaten);
		}
		//check for castling
		else if(board[(moveLocation[0])][(moveLocation[1])] == 7 &&
		(moveLocation[3] == moveLocation[0] + 2 || moveLocation[3] == moveLocation[0] - 2))
		{
			if(moveLocation[3] == moveLocation[0] + 2)
			{
				movePosition(8,0);
				//pick up piece
				movePosition(6,0);
			}
			else
			{
				movePosition(0,0);
				//pick up piece
				movePosition(3,0);
			}
		}

		movePosition(moveLocation[0],moveLocation[1]);
		//pick up piece
		movePosition(moveLocation[2],moveLocation[3]);

		//checking for legal positions

		string s = "";
		bool missing = false;
		bool found = false;
		int x0 = 0, y0 = 0, x = 0, y = 0;
		while(readIntPC(TFileHandle,s))
		{
			if(s=="piecePos")
			{
				readIntPC(TFileHandle,x0);
				readIntPC(TFileHandle,y0);
				//move(x0,y0);
				missing = true; //check if piece missing through touch sensor return a boolean;
			}
			if(missing)
			{
				while(readIntPC(TFileHandle,s)!="piecePos")
				{
					readIntPC(TFileHandle,x);
					readIntPC(TFileHandle,y);
					//move(x,y);
					found = true; //check if piece missing through touch sensor and color sensor return a boolean;
				}
			}
			while(readIntPC(TFileHandle,s)!="piecePos")
			{}
		}
		// check for illegal moves
		if(missing == false || found == false)
		{
			for(int i = 0; i < 7; i++)
			{
				for(int j = 0; j < 7 && !missing; j++)
				{
					movePiece(i,j);
					missing = true;//check if piece missing through touch sensor return a boolean;
					if(missing)
					{
						x0 = i;
						y0 = j;
					}
				}
			}
			for(int i = 0; i < 7; i++)
			{
				for(int j = 0; j < 7 && !found; j++)
				{
					movePiece(i,j);
					found = true;//check if piece missing through touch sensor return a boolean;
					if(missing)
					{
						x = i;
						y = j;
					}
				}
			}
		}

		//updating array
		char piece = chessboard[x0][y0];
		chessboard[x0][y0] = '.';
		chessboard[x][y] = piece;

		userMove[0] = x0;
		userMove[1] = y0;
		userMove[2] = x;
		userMove[3] = y;
}

task main()
{
	for(int i = 0; i < 8; i++)
	{
		chessboard[i][0] = 'W';
	}

	for(int x = 0; x < 8; x++)
	{
		chessboard[x][1] = 'W';
	}

	for(int y = 2; y < 6; y++)
	{
		for(int x = 0; x < 8; x++) {
			chessboard[x][y] = '.';
		}
	}

	for(int x = 0; x < 8; x++)
	{
		chessboard[x][6] = 'p';
	}
	for(int i = 0; i < 8; i++)
	{
		chessboard[i][6] = 'B';
	}
	for(int i = 0; i < 8; i++)
	{
		chessboard[i][7] = 'B';
	}
}
