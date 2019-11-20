//global array for robotC
char chessboard[8][8];
//movement based on file
#include "PC_FileIO.c"
//desiredFirstElement = 0;
bool pickUpPiece(float SIZE_OF_WHEEL, float & movedDown);
int moveDownTilTouch(int enc_limit_claw, float SIZE_OF_WHEEL);
void moveDistancePos(tMotor motorPort, float dist, float SIZE_OF_WHEEL);
void moveDistanceNeg(tMotor motorPort, float dist, float SIZE_OF_WHEEL);
void openClaw(int enc_limit);
void closeClaw();
void dropPiece(float SIZE_OF_WHEEL, float & movedDown);
void moveToSquare(int x, int y);
void return_to_start();
bool movePiece (int ix, int iy, int fx, int fy, float SIZE_OF_WHEEL, float & movedDown);
bool checkMissing(int posX, int posY, float SIZE_OF_WHEEL);
bool checkFound(int posX, int posY, bool movement, float SIZE_OF_WHEEL);
bool checkUserPiece(int z_wheel_size, float SIZE_OF_WHEEL, float&moveDown);
bool testThreeTimes (int initX, int initY, int fX, int fY, bool pass, int trials, float & movedDown);

bool pickUpPiece (float SIZE_OF_WHEEL, float & movedDown)
{
	bool successful = true;

	movedDown = moveDownTilTouch(-250, SIZE_OF_WHEEL);


	if(SensorValue[S1] == 1)
	{
		moveDistanceNeg (motorC, movedDown, SIZE_OF_WHEEL);
		closeClaw();
		openClaw (-65);

		moveDistancePos(motorC, movedDown, SIZE_OF_WHEEL);
		closeClaw();
		moveDistanceNeg(motorC, movedDown, SIZE_OF_WHEEL);


		return successful;
	}
	else
	{
		moveDistanceNeg (motorC, movedDown, SIZE_OF_WHEEL);
		return false;
	}
}


int moveDownTilTouch (int enc_limit_claw, float SIZE_OF_WHEEL)
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


void moveDistancePos (tMotor motorPort, float dist, float SIZE_OF_WHEEL) //we can decide on what positive is based on how we installed the motors
{
	nMotorEncoder[motorPort] = 0;
	int rotations = 0;
	rotations = dist;
	motor[motorPort] = 45;
	while (nMotorEncoder[motorPort] < rotations)
	{}

	motor[motorPort] = 0;
}

void moveDistanceNeg (tMotor motorPort, float dist, float SIZE_OF_WHEEL)
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

void dropPiece(float SIZE_OF_WHEEL, float & movedDown)
{
	int enc_limit = -65;

	moveDistancePos (motorC, movedDown, SIZE_OF_WHEEL);

	openClaw(enc_limit);
	moveDistanceNeg (motorC, movedDown, SIZE_OF_WHEEL);

	closeClaw();
}

void moveToSquare(int x, int y)
{
	//starting position (0, 7)
	int enc_limit_x = 0;
	int enc_limit_y = 0;

	enc_limit_x = -x*(217);
	enc_limit_y = (7-y)*420;
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
		if (pickUpPiece(1, movedDown)) {
			moveToSquare(fX,fY);
			dropPiece(1, movedDown);
			bool passFound = false;

			for (int tried = 0; tried < 3 && !passFound; tried++) {
				passFound = checkFound(fX, fY, false, 1);
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

bool movePiece (int ix, int iy, int fx, int fy, float SIZE_OF_WHEEL, float & movedDown)
{
	bool successful = true;
	successful = testThreeTimes(ix, iy, fx, fy, false, 0, movedDown);
	return_to_start();
	return successful;
}

bool checkMissing(int posX, int posY, float SIZE_OF_WHEEL) {
	bool found = false;
	int enc_limit = -250;
	int zDist = 0;
	moveToSquare(posX, posY);
	zDist = moveDownTilTouch(enc_limit, SIZE_OF_WHEEL);
	if (SensorValue[S1] == 0 && chessboard[posX][posY] != '.') { //the piece is not there
		found = true;
	}
	moveDistanceNeg(motorC, zDist, 1);
	return found;
}

bool checkFound(int posX, int posY, bool movement, float SIZE_OF_WHEEL)
{
	bool found = false;
	int enc_limit = -250;
	float zDist = 0;
	if (movement) {
		moveToSquare(posX, posY);
		zDist = moveDownTilTouch(enc_limit, SIZE_OF_WHEEL);
		wait1Msec(100);
		if (SensorValue[S1] == 1 && '.' == '.') { //the piece was previously there
			if (checkUserPiece(1, 0.8, zDist))
				found = true;
		}
	} else {
		zDist = moveDownTilTouch(enc_limit, SIZE_OF_WHEEL);
		if (SensorValue[S1] == 1)
			found = true;
	}

	moveDistanceNeg(motorC,zDist, 1);
	closeClaw();
	return found;
}

bool checkUserPiece(int z_wheel_size, float SIZE_OF_WHEEL, float&moveDown) {
	//assume user is already in the correct piece position
	//move some distance up to measure colour

	float moveDist = 120;

	bool isUsers = false;

	moveDistanceNeg(motorC, 100, z_wheel_size);
	moveDistancePos(motorA, moveDist, SIZE_OF_WHEEL);
	moveDistancePos(motorC, 100, z_wheel_size);
	wait1Msec(500);

	if (SensorValue[S2] == (int)colorRed) //piece is a user piece
		isUsers = true;
	//undo movements
	wait1Msec(500);
	moveDistanceNeg(motorC, 100, SIZE_OF_WHEEL);
	moveDistanceNeg(motorA, moveDist, SIZE_OF_WHEEL);
	moveDistancePos(motorC, 100, SIZE_OF_WHEEL);

	return isUsers;
}

task main()
{
	SensorType[S1] = sensorEV3_Touch;
	wait1Msec(50);
	SensorType[S2] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S2] = modeEV3Color_Color;
	wait1Msec(50);

	if (checkFound(2,6, true, 1))
		displayString(3, "The piece is here");
	else
		displayString (3, "The piece is MIA");

	wait1Msec(5000);

	float movedDown = 0;
//	movePiece(0,7,7,0,1, movedDown);
}
