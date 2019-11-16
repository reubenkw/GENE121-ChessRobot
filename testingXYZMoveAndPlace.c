void openClaw (int enc_limit)
{
	nMotorEncoder[motorD] = 0;
	motor[motorD] = -35;
	while(nMotorEncoder[motorD] > enc_limit)
	{}
	motor[motorD] = 0;
}

void moveDownTilTouch (int enc_limit_claw, float SIZE_OF_WHEEL)
{
	openClaw(enc_limit_claw);
	//opens all the way
	int zENC_LIMIT = 20 * 360/ (2*PI*SIZE_OF_WHEEL);
	nMotorEncoder[motorC] = 0;
	motor[motorC] = 35;
	while(SensorValue[S1] == 0 && nMotorEncoder[motorC] < zENC_LIMIT) //touch sensor
	{}
	motor[motorC] = 0;
}

void closeClaw()
{
	motor[motorD] = 35;
	while (nMotorEncoder[motorD] < 0)
	{}
	motor[motorD] = 0;
}

void moveDistancePos (tMotor motorPort, float dist, float SIZE_OF_WHEEL) //we can decide on what positive is based on how we installed the motors
{
	nMotorEncoder[motorPort] = 0;
	motor[motorPort] = 35;
	int rotations = 0;
	rotations = dist * 360/ (2*PI*SIZE_OF_WHEEL);
	while (nMotorEncoder[motorPort] < rotations)
	{}

	motor[motorPort] = 0;
}

void moveDistanceNeg (tMotor motorPort, float dist, float SIZE_OF_WHEEL)
{

	nMotorEncoder[motorPort] = 0;
	motor[motorPort] = -35;
	int rotations = 0;
	rotations = -dist * 360/ (2*PI*SIZE_OF_WHEEL);
	while (nMotorEncoder[motorPort] > rotations)
	{}

	motor[motorPort] = 0;
}

bool pickUpPiece (float SIZE_OF_WHEEL)
{
	bool successful = true;

	moveDownTilTouch(-250, SIZE_OF_WHEEL);

	if(SensorValue[S1] == 1)
	{
		moveDistanceNeg (motorC, 10, SIZE_OF_WHEEL);
		closeClaw();
		openClaw (-45);
		moveDistancePos(motorC, 8, SIZE_OF_WHEEL);
		closeClaw();
		moveDistanceNeg(motorC, 15, SIZE_OF_WHEEL);
		return successful;
	}
	else
	{
		return false;
	}
}

void dropPiece(float SIZE_OF_WHEEL)
{
	int enc_limit = -45;
	int dist = 15;
	moveDistancePos (motorC, dist, SIZE_OF_WHEEL);

	openClaw(enc_limit);
	moveDistanceNeg (motorC, dist, SIZE_OF_WHEEL);

	closeClaw();

}

task main()
{
	moveDistancePos(motorB, 5.72, 2.75/2);
	moveDistancePos(motorA, 5.72, (2.75/4));
	pickUpPiece(1);

	moveDistanceNeg(motorB, 5.72, 2.75/2);
	moveDistanceNeg(motorA, 5.72, (2.75/4));
	dropPiece(1);

}
