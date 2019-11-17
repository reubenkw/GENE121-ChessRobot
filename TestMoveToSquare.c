void moveToSquare(int row, int col)
{
	//starting position (7, 0)
	int enc_limit_row = 0;
	int enc_limit_col = 0;

	enc_limit_row = (7-row)*(5.72)* 360/(2*PI*0.8);
	enc_limit_col = -col*(5.72) * 360/(2*PI*3.1/2);
	if (nMotorEncoder[motorB] < enc_limit_row)
	{
		motor[motorB] = 35;
		while (nMotorEncoder[motorB] < enc_limit_row)
		{}
		motor[motorB] = 0;
	}
	else if (nMotorEncoder[motorB] > enc_limit_row)
	{
		motor[motorB] = -35;
		while (nMotorEncoder[motorB] > enc_limit_row)
		{}
		motor[motorB] = 0;
	}

	if (nMotorEncoder[motorA] < enc_limit_col)
	{
		motor[motorA] = 35;
		while (nMotorEncoder[motorA] < enc_limit_col)
		{}
		motor[motorA] = 0;
	}
	else if (nMotorEncoder[motorA] > enc_limit_col)
	{
		motor[motorA] = -35;
		while (nMotorEncoder[motorA] > enc_limit_col)
		{}
		motor[motorA] = 0;
	}
}

void return_to_start()
{

	motor[motorA] = 35;
	while (nMotorEncoder[motorA] < 0)
	{}
	motor[motorA] = 0;
	nMotorEncoder[motorA] = 0;

	motor[motorB] = -35;
	while (nMotorEncoder[motorB] > 0)
	{}
	motor[motorB] = 0;
	nMotorEncoder[motorB] = 0;

}
task main()
{


moveToSquare(0, 7);

wait1Msec(1000);



}
