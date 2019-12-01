//global array for robotC
char chessboard[8][8];
//movement based on file
#include "PC_FileIO.c"
//desiredFirstElement = 0;
bool pickUpPiece(float & movedDown);
int moveDownTilTouch(int enc_limit_claw);
void moveDistancePos(tMotor motorPort, float dist);
void moveDistanceNeg(tMotor motorPort, float dist);
void openClaw(int enc_limit);
void closeClaw();
void dropPiece(float & movedDown);
void moveToSquare(int x, int y);
void return_to_start();
bool movePiece (int ix, int iy, int fx, int fy, float & movedDown);
bool checkMissing(int posX, int posY);
bool checkFound(int posX, int posY, bool movement);
bool checkUserPiece(float&moveDown);
bool makeMoveFindMove(TFileHandle&fin, int*moveLocation, int*userMove, float & movedDown);
void moveColourSens(bool positive);
int initializeChessboard(TFileHandle&fin);
void initialCheck(int*userMove);
void writingToCPP(TFileHandle&fout, int writeFirstElement,int*userMove);
void writingToRobot(TFileHandle&fout, int update);
bool testThreeTimes (int initX, int initY, int fX, int fY, bool pass, int trials, float & movedDown);

/*
NAME: pickUpPiece
PARAMETERS: float & movedDown
DESC: moves the claw down until it contacts a piece, picks up the piece, and returns true if a piece is detected.
*/
bool pickUpPiece (float & movedDown)
{
	bool successful = true;

	movedDown = moveDownTilTouch(-250);

	if(SensorValue[S1] == 1)
	{
		moveDistanceNeg (motorC, movedDown);
		closeClaw();
		openClaw (-55);

		moveDistancePos(motorC, movedDown);
		closeClaw();
		moveDistanceNeg(motorC, movedDown);

		return successful;
	}
	else
	{
		moveDistanceNeg (motorC, movedDown);
		return false;
	}
}

/*
NAME: moveDownTilTouch
PARAMETERS: int enc_limit_claw
DESC: takes in the amount that the claw should open based on encoder values, engages the Z motor and moves down until either it touches
or reaches a preset limit, it returns the motor encoder value to tell other functions how far to move
*/

int moveDownTilTouch (int enc_limit_claw)
{
	openClaw(enc_limit_claw);
	//opens all the way
	int zENC_LIMIT = 1200;
	nMotorEncoder[motorC] = 0;
	motor[motorC] = 45;
	while(SensorValue[S1] == 0 && nMotorEncoder[motorC] < zENC_LIMIT) //touch sensor
	{}
	motor[motorC] = 0;
	return nMotorEncoder[motorC];
}
/*
NAME: moveDistancePos
PARAMETERS: tMotor motorport, float dist
DESC: turns on a motor to a positive value, turns off the motor when it reaches distance limit that was passed in
*/


void moveDistancePos (tMotor motorPort, float dist) //we can decide on what positive is based on how we installed the motors
{
	nMotorEncoder[motorPort] = 0;
	int rotations = 0;
	rotations = dist;
	motor[motorPort] = 45;
	while (nMotorEncoder[motorPort] < rotations)
	{}

	motor[motorPort] = 0;
}
/*
NAME: moveDistanceNeg
PARAMETERS: tMotor motorport, float dist
DESC: turns on a motor to a negative value, turns off when it reaches the distance limit that was passed in
*/

void moveDistanceNeg (tMotor motorPort, float dist)
{

	nMotorEncoder[motorPort] = 0;
	int rotations = 0;
	rotations = -dist;
	motor[motorPort] = -45;
	while (nMotorEncoder[motorPort] > rotations)
	{}

	motor[motorPort] = 0;
}
/*
NAME: openClaw
PARAMETERS: int enc_limit
DESC: opens claw to desired encoder value
*/

void openClaw (int enc_limit)
{
	time1[T2] = 0;
	nMotorEncoder[motorD] = 0;
	motor[motorD] = -45;
	while(nMotorEncoder[motorD] > enc_limit && time1[T2] < 2000)
	{}
	motor[motorD] = 0;
}

/*
NAME: closeClaw
PARAMETERS: NONE
DESC: closes claw until motor encoder resets to 0 or the timer runs out
*/

void closeClaw()
{
	time1[T2] = 0;
	motor[motorD] = 45;
	while (nMotorEncoder[motorD] < 0 && time1[T2] < 2000)
	{}
	motor[motorD] = 0;
}

/*
NAME: dropPiece
PARAMETERS: float & movedDown
DESC: takes in a value indicating how far the claw should move, moves the claw down, opens the claw to release the piece
and moves the claw back up
*/

void dropPiece(float & movedDown)
{
	int enc_limit = -65;

	moveDistancePos (motorC, movedDown);

	openClaw(enc_limit);
	moveDistanceNeg (motorC, movedDown);

	closeClaw();
}

/*
NAME: moveToSquare
PARAMETERS: int x, int y
DESC: takes in a coordinate based the bottom left square being (0,0), and runs the motor until it reaches the square
the starting position of the claw is (0,7)
*/
void moveToSquare(int x, int y)
{
	//starting position (0, 7)
	int enc_limit_x = 0;
	int enc_limit_y = 0;

	enc_limit_x = -x*(220);
	enc_limit_y = (7-y)*405;
	if (nMotorEncoder[motorB] < enc_limit_y)
	{
		motor[motorB] = 45;
		while (nMotorEncoder[motorB] < enc_limit_y)
		{}
		motor[motorB] = 0;
	}
	else if (nMotorEncoder[motorB] > enc_limit_y)
	{
		motor[motorB] = -45;
		while (nMotorEncoder[motorB] > enc_limit_y)
		{}
		motor[motorB] = 0;
	}

	if (nMotorEncoder[motorA] < enc_limit_x)
	{
		motor[motorA] = 45;
		while (nMotorEncoder[motorA] < enc_limit_x)
		{}
		motor[motorA] = 0;
	}
	else if (nMotorEncoder[motorA] > enc_limit_x)
	{
		motor[motorA] = -45;
		while (nMotorEncoder[motorA] > enc_limit_x)
		{}
		motor[motorA] = 0;
	}
}


/*
NAME: testThreeTimes
PARAMETERS: int initX, int initY, int fX, int fY, bool pass, int trials, float & movedDown
DESC: takes in initial position and movement position, in addition to a pass boolean and trials for recurstion. the value for 
z-motor encoder movement has also been passed on. the function checks for placement of the piece successfully: the piece is 
moved to the pick up position, tries to pick up the piece, and places the piece. it then checks if the piece was placed 
successfully three times. if it was not placed successfully, it goes back to the initial position to try and pick the piece up again. 
if the return to initial position fails three times, the function returns false.
*/
bool testThreeTimes (int initX, int initY, int fX, int fY, bool pass, int trials, float & movedDown) {
	if (!pass) {
		pass = true; //reset to true
		moveToSquare(initX, initY);
		if (pickUpPiece(movedDown)) {
			moveToSquare(fX,fY);
			dropPiece(movedDown);
			bool passFound = false;

			for (int tried = 0; tried < 3 && !passFound; tried++) {
				passFound = checkFound(fX, fY, false);
			}

			if (!passFound) {
				moveToSquare(initX, initY);
				pass = false;
				trials++;
			}

			} else {
			pass = false;
			trials++;
			closeClaw();
		}
	}

	if (trials == 3 && !pass)
		return false;

	if (!pass && trials < 3) {
		testThreeTimes(initX, initY, fX, fY, false, trials, movedDown);
	}

	return true;
}

/*
NAME: return_to_start
PARAMETERS: NONE
DESC: runs the motors in the opposite direction until all the values reset to 0, resets the encoder values, 
the resting position is (0,7) based on a coordinate system with the bottom left corner being (0,0)
*/
void return_to_start()
{

	motor[motorA] = 45;
	while (nMotorEncoder[motorA] < 0)
	{}
	motor[motorA] = 0;
	nMotorEncoder[motorA] = 0;

	motor[motorB] = -45;
	while (nMotorEncoder[motorB] > 0)
	{}
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;

}

/*
NAME: movePiece
PARAMETERS: int ix, int iy, int fx, int fy, float & movedDown
DESC: takes in initial and final locations, calls on testThreeTimes function to pick up and deposit a piece, returns
to resting position after the piece was moved and returns successful if the piece was moved
*/
bool movePiece (int ix, int iy, int fx, int fy, float & movedDown)
{
	bool successful = true;
	successful = testThreeTimes(ix, iy, fx, fy, false, 0, movedDown);
	return_to_start();
	return successful;
}

/*
NAME: checkMissing
PARAMETERS: int posX, int posY
DESC: checks if a piece that was there previously (by comparison to the global 2d chess array) is now missing
*/
bool checkMissing(int posX, int posY) {
	bool found = false;
	int enc_limit = -250;
	int zDist = 0;
	moveToSquare(posX, posY);
	zDist = moveDownTilTouch(enc_limit);
	if (SensorValue[S1] == 0) { //the piece is not there
		found = true;
	}
	moveDistanceNeg(motorC, zDist);
	closeClaw();
	return found;
}

/*
NAME: checkFound
PARAMETERS: int posX, int posY, bool movement
DESC: checks if the piece has been found in two cases:
1) when checking if a user piece has moved to a given position (movement is true)
2) when checking of a piece has been placed successfully (movement is false)
returns true or false based on if the piece has been found
*/
bool checkFound(int posX, int posY, bool movement)
{
	bool found = false;
	int enc_limit = -250;
	float zDist = 0;
	if (movement) {
		moveToSquare(posX, posY);
		zDist = moveDownTilTouch(enc_limit);
		wait1Msec(100);
		if (SensorValue[S1] == 1) { //the piece was previously there
			if (chessboard[posY][posX] == '.') {
				found = true;
			} else {
				if (checkUserPiece(zDist))
					found = true;
			}
		}
	} else {
		zDist = moveDownTilTouch(enc_limit);
		if (SensorValue[S1] == 1)
			found = true;
	}

	moveDistanceNeg(motorC,zDist);
	closeClaw();
	return found;
}

/*
NAME: checkUserPiece
PARAMETERS: float&moveDown
DESC: checks the piece colour to see if it is a user piece: this is used in the case a piece is "killed"
returns true if the piece chosen is the user's piece (checked via colour sensor detecting if piece has red sticker on it)
*/
bool checkUserPiece(float&moveDown) {
	//assume user is already in the correct piece position
	//move some distance up to measure colour

	float moveDistZ = 100;

	bool isUsers = false;

	moveDistanceNeg(motorC, moveDistZ);
	moveColourSens(true);
	moveDistancePos(motorC, moveDistZ);
	wait1Msec(500);

	if (SensorValue[S2] == (int)colorRed) //piece is a user piece
		isUsers = true;
	//undo movements
	wait1Msec(500);
	moveDistanceNeg(motorC, moveDistZ);
	moveColourSens(false);
	moveDistancePos(motorC, moveDistZ);

	return isUsers;
}

/*
*/
void moveColourSens(bool positive) {
	int rotations = nMotorEncoder[motorA];

	if (positive) {
		motor[motorA] = 45;
		while (nMotorEncoder[motorA] < rotations+120){}
	}
	else {
		motor[motorA] = -45;
		while (nMotorEncoder[motorA] > rotations-120){}
	}
	motor[motorA] = 0;
}
bool makeMoveFindMove(TFileHandle&fin, int*moveLocation, int*userMove, float & movedDown)
{
	openReadPC(fin,"IPC_CPP_to_RC.txt");
	int x0 = 0, y0 = 0, x = 0, y = 0;
	bool missing = false, found = false, over = false, successfulMove = false, failure = false;
	int a = 0;
	readIntPC(fin,a);

	for(int counter = 0; counter < 4; counter++)
	{
		readIntPC(fin,a);
		moveLocation[counter] = a;
	}
	//check for eaten pieces
	int checkX = moveLocation[2];
	int checkY = moveLocation[3];
	char checkValue = chessboard[checkY][checkX];
	if(checkValue != '.' && (moveLocation[0] != 0 || moveLocation[1] != 0 || moveLocation[2] != 0 || moveLocation[3] != 0))
	{
		movePiece(checkX,checkY,0,8, movedDown);
		moveDistancePos(motorB, 405);
		nMotorEncoder[motorB] = 0;
		chessboard[checkY][checkX] = '.';
	}
	//check for castling
	else if(checkValue == 'K' &&
		(moveLocation[3] == moveLocation[0] + 2 || moveLocation[3] == moveLocation[0] - 2))
	{
		if(moveLocation[3] == moveLocation[0] + 2)
		{
			movePiece(7,7,5,7, movedDown);
			chessboard[7][5] = 'B';
			chessboard[7][7] = '.';
		}
		else
		{
			movePiece(0,7,3,7, movedDown);
			chessboard[7][3] = 'B';
			chessboard[7][0] = '.';
		}
	}
	if(moveLocation[0] != 0 ||
		moveLocation[1] != 0 ||
	moveLocation[2] != 0 ||
	moveLocation[3] != 0)
	{
		successfulMove = movePiece(moveLocation[0],moveLocation[1],moveLocation[2],moveLocation[3], movedDown);
	}
	else
	{
		successfulMove = true;
		failure = true;
  }

	if(!successfulMove)
	{
		x0 = 1;
		y0 = 1;
		x = 1;
		y = 1;
		char piece = chessboard[moveLocation[1]][moveLocation[0]];
		chessboard[moveLocation[1]][moveLocation[0]] = '.';
		chessboard[moveLocation[3]][moveLocation[2]] = piece;
		over = false;
		return over;
	}
	else if(successfulMove && !failure)
	{
		char piece = chessboard[moveLocation[1]][moveLocation[0]];
		chessboard[moveLocation[1]][moveLocation[0]] = '.';
		chessboard[moveLocation[3]][moveLocation[2]] = piece;
	}

	//finished our turn
	time1[T1] = 0;

	if(!failure)
	{
		while(!getButtonPress(buttonEnter))
		{
			if(time1[T1]>180000)
				playSound(soundBeepBeep);
		}
		while(getButtonPress(buttonEnter))
		{
			if(time1[T1]>180000)
				playSound(soundBeepBeep);
		}
	}

	writeDebugStreamLine("waited for user turn");
	string s = "";
	//user made a move

	//checking legal moves
	if(!readTextPC(fin,s))
		over = true;
	if(!over)
	{
		do
		{
			readIntPC(fin,x0);
			readIntPC(fin,y0);
			writeDebugStreamLine("checking if %d,%d was moved",x0,y0);
			missing = checkMissing(x0,y0);

			if(missing)
			{
				writeDebugStreamLine("%d,%d was moved",x0,y0);
				readTextPC(fin,s);
				do
				{
					readIntPC(fin,x);
					readIntPC(fin,y);
					writeDebugStreamLine("checking if %d,%d was moved to %d,%d",x0,y0,x,y);
					found = checkFound(x,y,true);
					writeDebugStreamLine("checking final location that user can move");
					readTextPC(fin,s);
				}while(s!="piecePos"&&!found);
				writeDebugStreamLine("checking if %d,%d was moved to %d,%d",x0,y0,x,y);
			}
			else
			{
				do
				{
					if(!readTextPC(fin,s))
						break;
				}while(s!="piecePos");
			}
		}while(!missing && !found);
	}
	writeDebugStreamLine("finished checking user move");
	if((x0!=0||y0!=0||x!=0||y!=0)&&missing&&found)
	{
		char piece = chessboard[y0][x0];
		chessboard[y0][x0] = '.';
		chessboard[y][x] = piece;
	}
	else
	{
		x0 = 0;
		y0 = 0;
		x = 0;
		y = 0;
	}
	//updating array
	userMove[0] = x0;
	userMove[1] = y0;
	userMove[2] = x;
	userMove[3] = y;
	return_to_start();
	closeFilePC(fin);

	return over;
}


int initializeChessboard(TFileHandle&fin)
{
	openReadPC(fin, "SETUP_CHESSBOARD.txt");
	int update = 0;
	char temp = '0';
	readIntPC(fin,update);
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			readCharPC(fin,temp);
			writeDebugStreamLine("%d %d cr read is %d", i,j,(int)temp);
			chessboard[i][j] = (char)temp;
		}
	}
	closeFilePC(fin);
	return update;
}


void initialCheck(int*userMove)
{
	bool missing = false, found = false;
	int x0 = 0, y0 = 0, x = 0, y = 0;
	for(int j = 0; j < 8 && !missing; j++)
	{
		missing = checkMissing(j,1); //check for pawn
		if(missing)
		{
			x0 = j;
			y0 = 1;
			for (int i = 2; i <4 &&!found; i++)
			{
				found = checkFound(j,i,true);
				if(found)
				{
					x = j;
					y = i;
				}
			}
		}
	}
	if(!missing)
	{
		missing = checkMissing(1,0); //check for left horse
		if(missing)
		{
			x0 = 1;
			y0 = 0;
			found = checkFound(0,2,true);
			if(found)
			{
				x = 0;
				y = 2;
			}
			else{
				found = checkFound(2,2,true);
				if(found)
				{
					x = 2;
					y = 2;
				}
			}
		}
	}
	if(!missing)
	{
		missing = checkMissing(6,0); //check for right horse
		if(missing)
		{
			x0 = 6;
			y0 = 0;
			found = checkFound(5,2,true);
			if(found)
			{
				x = 5;
				y = 2;
			}
			else
			{
				found = checkFound(7,2,true);
				if(found)
				{
					x = 7;
					y = 2;
				}
			}
		}
	}

	if((x0 != 0 || y0 != 0||x != 0||y !=0) && missing && found)
	{
		chessboard[y0][x0] = '.';
		chessboard[y][x] = 'W';
	}
	else
	{
		x0 = 0;
		y0 = 0;
		x = 0;
		y = 0;
	}
	writeDebugStreamLine("508 ");
	writeDebugStreamLine("%d",x0 );
	writeDebugStreamLine("%d",y0 );
	writeDebugStreamLine("%d",x );
	writeDebugStreamLine("%d",y );
	//updating array

	userMove[0] = x0;
	userMove[1] = y0;
	userMove[2] = x;
	userMove[3] = y;
	writeDebugStreamLine("user moved: ");
	writeDebugStreamLine("%d",userMove[0] );
	writeDebugStreamLine("%d",userMove[1] );
	writeDebugStreamLine("%d",userMove[2] );
	writeDebugStreamLine("%d",userMove[3] );
}

void writingToCPP(TFileHandle&fout, int writeFirstElement,int*userMove)
{
	openWritePC(fout,"IPC_RC_to_CPP.txt");
	writeLongPC(fout,writeFirstElement);
	for(int counter = 0; counter < 4; counter++)
	{
		writeTextPC(fout," ");
		writeLongPC(fout,userMove[counter]);
	}
	closeFilePC(fout);
}
void writingToRobot(TFileHandle&fout, int update)
{
	openWritePC(fout,"SETUP_CHESSBOARD.txt");
	writeLongPC(fout,update);
	writeEndlPC(fout);
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			char temp = chessboard[i][j] ;
			writeCharPC(fout,temp);
		}
		writeEndlPC(fout);
	}
	closeFilePC(fout);

}

task main()
{
	//X move = motorA
	//Y move = motorB
	//Z move = motorC
	//claw motor = motorD
	displayString(0," ");
	clearDebugStream();
	float movedDown = 0;
	SensorType[S1] = sensorEV3_Touch;
	SensorType[S2] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S2] = modeEV3Color_Color;
	int moveLocation[4] = {0,0,0,0};
	int userMove[4] = {0,0,0,0};
	int update = 0;
	bool end = false;

	TFileHandle foutCPP;
	TFileHandle finCPP;
	TFileHandle finRobot;
	TFileHandle foutRobot;
	bool fileWriteCPPOkay = openWritePC(foutCPP,"IPC_RC_to_CPP.txt");
	bool fileReadCPPOkay = openReadPC(finCPP,"IPC_CPP_to_RC.txt");
	bool fileReadRobotOkay = openReadPC(finRobot,"SETUP_CHESSBOARD.txt");
	//	bool fileWriteRobotOkay = openWritePC(foutRobot,"SETUP_CHESSBOARD.txt");
	//	closeFilePC(foutRobot);
	if(fileWriteCPPOkay && fileReadCPPOkay && fileReadRobotOkay)
	{
		closeFilePC(foutCPP);
		closeFilePC(finCPP);
		closeFilePC(finRobot);
		update = initializeChessboard(finRobot);

		if(update == 0)
		{
			while(!getButtonPress(buttonEnter))
			{}
			while(getButtonPress(buttonEnter))
			{}
			initialCheck(userMove);
			return_to_start();
			writingToCPP(foutCPP,update+1,userMove);
			writingToRobot(foutRobot,update+1);
		}
		else
		{
			end = makeMoveFindMove(finCPP, moveLocation, userMove, movedDown);
			closeFilePC(finCPP);
			writeDebugStreamLine("finished read location input");

			if(!end)
			{
				writingToCPP(foutCPP,update+1,userMove);
				closeFilePC(foutCPP);
				writingToRobot(foutRobot, update + 1);
				closeFilePC(foutRobot);
				writeDebugStreamLine("updated chessboard");
			}

		}

	}
	else
	{
		displayString(2,"Error with file IO");
		wait1Msec(5000);
	}

}
