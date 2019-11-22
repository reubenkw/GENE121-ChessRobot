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
bool readLocationInput(TFileHandle&fin, int*moveLocation, int*userMove, float & movedDown);
void moveColourSens(bool positive);
int initializeChessboard(TFileHandle&fin);
void initialCheck(int*userMove);
void writingToCPP(TFileHandle&fout, int writeFirstElement,int*userMove);
void writingToRobot(TFileHandle&fout, int update);
bool testThreeTimes (int initX, int initY, int fX, int fY, bool pass, int trials, float & movedDown);

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

void openClaw (int enc_limit)
{
	nMotorEncoder[motorD] = 0;
	motor[motorD] = -45;
	while(nMotorEncoder[motorD] > enc_limit)
	{}
	motor[motorD] = 0;
}

void closeClaw()
{
	motor[motorD] = 45;
	while (nMotorEncoder[motorD] < 0)
	{}
	motor[motorD] = 0;
}

void dropPiece(float & movedDown)
{
	int enc_limit = -65;

	moveDistancePos (motorC, movedDown);

	openClaw(enc_limit);
	moveDistanceNeg (motorC, movedDown);

	closeClaw();
}

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

bool movePiece (int ix, int iy, int fx, int fy, float & movedDown)
{
	bool successful = true;
	successful = testThreeTimes(ix, iy, fx, fy, false, 0, movedDown);
	return_to_start();
	return successful;
}

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
			if (chessboard[posX][posY] == '.') {
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
bool readLocationInput(TFileHandle&fin, int*moveLocation, int*userMove, float & movedDown)
{
	openReadPC(fin,"IPC_CPP_TO_RC.txt");
	int x0 = 0, y0 = 0, x = 0, y = 0;
	bool missing = false, found = false, over = false;

	for(int counter = 0; counter < 4; counter++)
	{
		int a = 0;
		readIntPC(fin,a);
		moveLocation[counter] = a;
		displayString(counter + 2, "moving to: %d", a);
	}
	//check for eaten pieces
	int checkX = moveLocation[2];
	int checkY = moveLocation[3];
	char checkValue = chessboard[checkX][checkY];
	if(checkValue != '.')
	{
		movePiece(checkX,checkY,0,8, movedDown);
		chessboard[checkX][checkY] = '.';
	}
	//check for castling
	else if(checkValue == 'K' &&
		(moveLocation[3] == moveLocation[0] + 2 || moveLocation[3] == moveLocation[0] - 3))
	{
		if(moveLocation[3] == moveLocation[0] + 2)
		{
			movePiece(7,7,5,7, movedDown);
			chessboard[5][7] = 'B';
			chessboard[7][7] = '.';
		}
		else
		{
			movePiece(0,7,2,7, movedDown);
			chessboard[2][7] = 'B';
			chessboard[0][7] = '.';
		}
	}
	writeDebugStreamLine("checked for eaten castling");
	writeDebugStreamLine("moving to: ");
	writeDebugStreamLine("%d ",moveLocation[0] );
	writeDebugStreamLine("%d ",moveLocation[1] );
	writeDebugStreamLine("%d ",moveLocation[2] );
	writeDebugStreamLine("%d ",moveLocation[3] );
	bool successfulMove = movePiece(moveLocation[0],moveLocation[1],moveLocation[2],moveLocation[3], movedDown);

	if(!successfulMove)
	{
		x0 = 1;
		y0 = 1;
		x = 1;
		y = 1;
		return over;
	}
	else
	{
		char piece = chessboard[moveLocation[1]][moveLocation[0]];
		chessboard[moveLocation[1]][moveLocation[0]] = '.';
		chessboard[moveLocation[3]][moveLocation[2]] = piece;
	}
	writeDebugStreamLine("checked for eaten pieces");

	//finished our turn
	time1[T1] = 0;

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
					readTextPC(fin,s);
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
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				writeDebugStreamLine("%d",(int)chessboard[i][j]);
			}
		}
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
		writeDebugStreamLine("%d",(int)chessboard[x0][y0]);
		chessboard[y0][x0] = '.';
		chessboard[y][x] = 'W';
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				writeDebugStreamLine("Printing %d,%d",i,j);
				writeDebugStreamLine("");
				writeDebugStreamLine("%d",(int)chessboard[i][j]);
			}
		}
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
	openWritePC(fout,"IPC_RC_TO_CPP.txt");
	writeLongPC(fout,writeFirstElement);
	for(int counter = 0; counter < 4; counter++)
	{
		writeLongPC(fout,userMove[counter]);
		writeTextPC(fout,"");
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
	bool fileWriteCPPOkay = openWritePC(foutCPP,"IPC_RC_TO_CPP.txt");
	bool fileReadCPPOkay = openReadPC(finCPP,"IPC_CPP_TO_RC.txt");
	bool fileReadRobotOkay = openReadPC(finRobot,"SETUP_CHESSBOARD.txt");
//	bool fileWriteRobotOkay = openWritePC(foutRobot,"SETUP_CHESSBOARD.txt");
//	closeFilePC(foutRobot);
	if(fileWriteCPPOkay && fileReadCPPOkay && fileReadRobotOkay)
	{
		closeFilePC(foutCPP);
		closeFilePC(finCPP);
		closeFilePC(finRobot);
		update = initializeChessboard(finRobot);

		writeDebugStreamLine("%d",update);
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				writeDebugStreamLine("%d",(int)chessboard[i][j]);
			}
		}
		if(update == 0)
		{
			while(!getButtonPress(buttonEnter))
			{}
			while(getButtonPress(buttonEnter))
			{}
			initialCheck(userMove);
			return_to_start();
			writeDebugStreamLine("finished initial check");
			writingToCPP(foutCPP,update+1,userMove);
			writingToRobot(foutRobot,update+1);
		}
		else
		{
			end = readLocationInput(finCPP, moveLocation, userMove, movedDown);
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
