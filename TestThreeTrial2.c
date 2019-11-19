void openClaw(int enc_limit);
int moveDownTilTouch(int enc_limit_claw, float SIZE_OF_WHEEL);
bool testThreeTimes (int initX, int initY, int fX, int fY, bool pass, int trials, float & movedDown);
bool checkFound(int posX, int posY, bool movement, float SIZE_OF_WHEEL);
char chessboard[8][8];

void openClaw (int enc_limit)
{
	nMotorEncoder[motorD] = 0;
	motor[motorD] = -45;
	while(nMotorEncoder[motorD] > enc_limit)
	{}
	motor[motorD] = 0;
}

//made it int
int moveDownTilTouch (int enc_limit_claw, float SIZE_OF_WHEEL)
{
	openClaw(enc_limit_claw);
	//opens all the way
	int zENC_LIMIT = 20 * 360/ (2*PI*SIZE_OF_WHEEL);
	nMotorEncoder[motorC] = 0;
	motor[motorC] = 45;
	while(SensorValue[S1] == 0 && nMotorEncoder[motorC] < zENC_LIMIT) //touch sensor
	{}
	motor[motorC] = 0;
	return nMotorEncoder[motorC];
}

void closeClaw()
{
	motor[motorD] = 45;
	while (nMotorEncoder[motorD] < 0)
	{}
	motor[motorD] = 0;
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

bool pickUpPiece (float SIZE_OF_WHEEL, float & movedDown)
{
	bool successful = true;

	movedDown = moveDownTilTouch(-250, SIZE_OF_WHEEL);


	if(SensorValue[S1] == 1)
	{
		moveDistanceNeg (motorC, movedDown, SIZE_OF_WHEEL);
		closeClaw();
		openClaw (-45);

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

void dropPiece(float SIZE_OF_WHEEL, float & movedDown)
{
	int enc_limit = -45;

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

	enc_limit_x = -x*(5.72)* 360/(2*PI*3.1/2);
	enc_limit_y = (7-y)*(5.72) * 360/(2*PI*0.8);
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

//function to test the pickup and drop off five times (MAIN FUNCTION TO BE TESTED HERE)
// values are INCORRECT for passing
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

//not same as original checkFound in dumdumdum
bool checkFound(int posX, int posY, bool movement, float SIZE_OF_WHEEL)
{
	bool found = false;
	int enc_limit = -250;
	float zDist = 0;
	if (movement) {
		moveToSquare(posX, posY);
		zDist = moveDownTilTouch(enc_limit, SIZE_OF_WHEEL);
		if (SensorValue[S1] == 1 && chessboard[posX][posY] == '.') { //the piece was previously there
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

bool movePiece (int ix, int iy, int fx, int fy, float SIZE_OF_WHEEL, float & movedDown)
{
	bool successful = true;
	successful = testThreeTimes(ix, iy, fx, fy, false, 0, movedDown);
	return_to_start();
	return successful;
}

task main()
{
	SensorType[S1] = sensorEV3_Touch;
	float movedDown = 0;
	movePiece(0,7,0,7,1, movedDown);
}
