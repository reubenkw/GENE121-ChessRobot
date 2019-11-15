//global array for robotC
char chessboard[8][8];
//movement based on file
#include "PC_FileIO.c"
//desiredFirstElement = 0;
bool pickUpPiece(int enc_limit_for_claw);
void moveDownTilTouch(int enc_limit_claw);
void moveDistancePos(tMotor motorPort, int dist, float sizeOfWheel);
void moveDistanceNeg(tMotor motorPort, int dist, float sizeOfWheel);
void openClaw(int enc_limit);
void closeClaw();
void dropPiece();
void moveToSquare(int x, int y);
void return_to_start();
bool movePiece (int ix, int iy, int fx, int fy);
bool checkMissing(int posX, int posY);
bool checkFound(int posX, int posY);
bool checkUserPiece();
void waitForCPPFile(int desiredFirstElement);
void readLocationInput(TFileHandle fin, int*moveLocation, int*userMove, int&desiredFirstElement);

bool pickUpPiece (int enc_limit_for_claw)
{
	bool successful = true;
	int sizeOfWheel = 2.75;

	moveDownTilTouch(enc_limit_for_claw);

	if(SensorValue[S1] == 1)
	{
		moveDistanceNeg (motorC, 10, sizeOfWheel);
		closeClaw();
		openClaw (enc_limit_for_claw);
		moveDistancePos (motorC, 10, sizeOfWheel);
		closeClaw();
		moveDistanceNeg (motorC, 10, sizeOfWheel);
		return successful;
	}
	else
	{
		return false;
	}
}


void moveDownTilTouch (int enc_limit_claw)
{
	int sizeOfWheel = 2.75;
	openClaw(enc_limit_claw);
	//opens all the way
	int tenCmENC_LIMIT = 10 * 360/ (2*PI*sizeOfWheel);
	nMotorEncoder[motorC] = 0;
	motor[motorC] = 10;
	while(SensorValue[S1] == 0 && nMotorEncoder[motorC] < tenCmENC_LIMIT) //touch sensor
	{}
	motor[motorC] = 0;
}


void moveDistancePos (tMotor motorPort, int dist, float sizeOfWheel) //we can decide on what positive is based on how we installed the motors
{
	nMotorEncoder[motorPort] = 0;
	motor[motorPort] = 10;
	int rotations = 0;
	rotations = dist * 360/ (2*PI*sizeOfWheel);
	while (nMotorEncoder[motorPort] < rotations)
	{}

	motor[motorPort] = 0;
}

void moveDistanceNeg (tMotor motorPort, int dist, float sizeOfWheel)
{

	nMotorEncoder[motorPort] = 0;
	motor[motorPort] = -10;
	int rotations = 0;
	rotations = -dist * 360/ (2*PI*sizeOfWheel);
	while (nMotorEncoder[motorPort] > rotations)
	{}

	motor[motorPort] = 0;
}

void openClaw (int enc_limit)
{
	nMotorEncoder[motorD] = 0;
	motor[motorD] = 10;
	while(nMotorEncoder[motorD] < enc_limit)
	{}
	motor[motorD] = 0;
}

void closeClaw()
{
	motor[motorD] = -10;
	while (nMotorEncoder[motorD] > 0)
	{}
	motor[motorD] = 0;
}

void dropPiece()
{
	int enc_limit = -55;
	int sizeOfWheel = 10;
	int dist = 10;
	moveDistancePos (motorC, dist, sizeOfWheel);

	openClaw(enc_limit);
	moveDistanceNeg (motorC, dist, sizeOfWheel);

	closeClaw();

}

void moveToSquare(int x, int y)
{
	int enc_limit_x = 0;
	int enc_limit_y = 0;

	enc_limit_x = x*(5.5) * 360/(2*PI*2.75);
	enc_limit_y = y*(5.5) * 360/(2*PI*0.5);
	if (nMotorEncoder[motorA] < enc_limit_x)
	{
		motor[motorA] = 20;
		while (nMotorEncoder[motorA] < enc_limit_x)
		{}
		motor[motorA] = 0;
	}
	else if (nMotorEncoder[motorA] > enc_limit_x)
	{
		motor[motorA] = -20;
		while (nMotorEncoder[motorA] > enc_limit_x)
		{}
		motor[motorA] = 0;
	}

	if (nMotorEncoder[motorB] < enc_limit_y)
	{
		motor[motorB] = 20;
		while (nMotorEncoder[motorB] < enc_limit_y)
		{}
		motor[motorB] = 0;
	}
	else if (nMotorEncoder[motorB] > enc_limit_y)
	{
		motor[motorB] = -20;
		while (nMotorEncoder[motorB] > enc_limit_y)
		{}
		motor[motorB] = 0;
	}
}

void return_to_start()
{

	motor[motorB] = -20;
	while (nMotorEncoder[motorB] > 0)
	{}
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;

	motor[motorA] = -20;
	while (nMotorEncoder[motorA] > 0)
	{}
	motor[motorA] = 0;

}

bool movePiece (int ix, int iy, int fx, int fy)
{
	int enc_limit_for_claw = -195;
	bool successful = true;
	moveToSquare(ix, iy);
	successful = pickUpPiece(enc_limit_for_claw);
	if (successful)
	{
		moveToSquare(fx, fy);
		dropPiece();
		return_to_start();
		return true;
	}
	else
		return false;

}
bool checkMissing(int posX, int posY) {
	bool found = false;
	int enc_limit = -55;
	moveToSquare(posX, posY);
	moveDownTilTouch(enc_limit);
	if (SensorValue[S1] == 0 && chessboard[posX][posY] != '.') { //the piece is not there
		found = true;
	}
	return found;
}

bool checkFound(int posX, int posY){
	bool found = false;
	int enc_limit = -55;
	moveToSquare(posX, posY);
	moveDownTilTouch(enc_limit);
	if (SensorValue[S1] == 1 && chessboard[posX][posY] == '.') { //the piece was previously there
		found = true;
		} else if (SensorValue[S1] == 1){ //check colour of the piece
		if (checkUserPiece())
			found = true;
	}
	return found;
}

bool checkUserPiece() {
	//assume user is already in the correct piece position
	//move some distance up to measure colour
	float moveDist = 3.3; //3.3 cm
	float moveUp = 0.5;
	float z_wheel_size = 1;
	float wheel_size = 2.75;

	bool isUsers = false;
	moveDistanceNeg(motorC, moveUp, z_wheel_size);
	moveDistancePos(motorA, moveDist, wheel_size);

	if (SensorValue[S2] == (int)colorRed) //piece is a user piece
		isUsers = true;
	//undo movements
	moveDistanceNeg(motorA, moveDist, wheel_size);
	moveDistancePos(motorC, moveUp, wheel_size);

	return isUsers;

}

void waitForCPPFile(int desiredFirstElement) {
	int inNum = desiredFirstElement + 1;
	do{
		wait1Msec(2000);
		TFileHandle fin;
		readIntPC(fin,inNum);
		//close file
	} while(inNum != desiredFirstElement);
}

void readLocationInput(TFileHandle fin, int*moveLocation, int*userMove, int&desiredFirstElement)
{
	int numEaten = 0;
	int x0 = 0, y0 = 0, x = 0, y = 0;
	bool fileOkay = openReadPC(fin,"IPC_CPP_TO_RC.txt");
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
		int checkX = moveLocation[2];
		int checkY = moveLocation[3];
		char checkValue = chessboard[checkX][checkY];
		if(checkValue != '.')
		{
			numEaten++;

			movePiece(checkX,checkY,9, numEaten);
		}
		//check for castling
		else if(checkValue == 'K' &&
			(moveLocation[3] == moveLocation[0] + 2 || moveLocation[3] == moveLocation[0] - 2))
		{
			if(moveLocation[3] == moveLocation[0] + 2)
			{
				movePiece(8,0,6,0);
			}
			else
			{
				movePiece(0,0,3,0);
				moveToSquare(3,0);
			}
		}

		movePiece(moveLocation[0],moveLocation[1],moveLocation[2],moveLocation[3]);


		//checking for legal positions

		string s = "";
		bool missing = false;
		bool found = false;
		while(readTextPC(fin,s))
		{
			if(s == "piecePos")
			{
				readIntPC(fin,x0);
				readIntPC(fin,y0);
				missing = checkMissing(x0,y0);
			}
			if(missing)
			{
				readTextPC(fin,s);
				while(s!="piecePos")
				{
					readIntPC(fin,x);
					readIntPC(fin,y);
					found = checkFound(x,y);
					readTextPC(fin,s);
				}
			}
			while(s!="piecePos")
			{
				readTextPC(fin,s);
			}
		}
	}

	if(x0!=0||y0!=0||x!=0||y!=0)
	{
		char piece = chessboard[x0][y0];
		chessboard[x0][y0] = '.';
		chessboard[x][y] = piece;
	}
	//updating array
	userMove[0] = x0;
	userMove[1] = y0;
	userMove[2] = x;
	userMove[3] = y;
}

void initializeChessboard()
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
	for(int i = 0; i < 3 ; i++)
	{
		chessboard[6][i] = 'B';
	}
	chessboard[6][4] = 'K';
	for(int i = 5; i < 8; i++)
	{
		chessboard[6][i] = 'B';
	}
	for(int i = 0; i < 8; i++)
	{
		chessboard[7][i] = 'B';
	}
}

task main()
{

	//X move = motorA
	//Y move = motorB
	//Z move = motorC
	//claw motor = motorD
	SensorType[S1] = sensorEV3_Touch;
	SensorType[S2] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S2] = modeEV3Color_Color;
	int moveLocation[4] = {0,0,0,0};
	int userMove[4] = {0,0,0,0};
	int desiredFirstElement = 0;
	int writeFirstElement = 0;
	initializeChessboard();
	bool over = false;

	waitForCPPFile(desiredFirstElement);

	TFileHandle fout;
	bool fileOkay = openWritePC(fout,"IPC_RC_TO_CPP.txt");
	if(fileOkay)
	{
		writeLongPC(fout,writeFirstElement);
		writeFirstElement++;
	}

	while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonEnter))
	{}

	//check pawn then horse


}
