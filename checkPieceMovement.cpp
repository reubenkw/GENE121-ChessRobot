/*
DESC: CHECKS IF A PIECE IS MISSING FROM THE PLACEMENT & WHERE THE PIECE HAS MOVED
CAN RETURN A PLACEMENT AND A PIECE
	
*/

const double SIDE_LENGTH = 5.6;
const double PICKUP_HEIGHT = 8.0; //HOLDING VALUE UNTIL WE FIGURE OUT HOW LOW IT GOES
const int SQUARES_PER_SIDE = 8;


//passed by reference positions to return two values
void checkMissingPiece(int&positionX, int&positionY; char&prevPositions[][]){
	//assume that the starting position is THE MIDDLE of the robot
	
	//move the robot to the bottom left part of the player's side
	move(XMOTOR, -SIDE_LENGTH*SQUARES_PER_SIDE/2);
	move (YMOTOR, SIDE_LENGTH/2);
	

	int touchEngaged = -1;
	char prevPos = '';
	//double for-loop to account for grid system
	for (int i = 0; i < SQUARES_PER_SIDE/2; i++) {
		move(XMOTOR, SIDE_LENGTH);
		for (int n = 0; n < SQUARES_PER_SIDE/2; n++) {
			move(ZMOTOR, PICKUP_HEIGHT);
			touchEngaged = SensorValue[TOUCH_SENSOR_PLACEHOLDER];
			prevPos = prevPositions[i][n];
			
			if (prevPos != '.' &&  touchEngaged == 0) { //piece has moved
				positionX = i;
				positionY = n;
				cout << "There is a missing piece at " << positionX << "," << positionY << endl;
			}
			
		}
		move(ZMOTOR, -PICKUP_HEIGHT);
		move(YMOTOR, SIDE_LENGTH);		
	}
}

//int&checkPosition[][][] stores: [NTH POSSIBLE MOVE][XCOORDINATE = 0, YCOORDINATE = 1)
void checkPossibleMoves(int&positionX, int&positionY, char&prevPositions[][], int&checkPosition[][]) {
	//pass in the positions and check what the possible moves are
	//use stockfish bro im confused abt this sorry ):
}

void findPosition (int&positionXMove, int&positionYMove, char&prevPositions[], int&checkPosition[][][]){
	for (int i = 0; i < 30; i++) {
		move(XMOTOR, checkPositionX[i][0];
		move(YMOTOR, checkPosition[i, 1]);
		move(ZMOTOR, PICKUP_HEIGHT);
		
		touchEngaged = SensorValue[TOUCH_SENSOR_PLACEHOLDER];
		prevPos = prevPositions[i][n];
			
		if (prevPos == '.' &&  touchEngaged == 1) { //piece has moved
			positionXMove = i;
			positionYMove = n;
			cout << "The piece is at " << positionX << "," << positionY << endl;
		}
		
	}
	
	move(ZMOTOR, -PICKUP_HEIGHT);
	 
}

//send values to file and update
void updateValues(int&positionX, int&positionY, char&prevPositions[][] ofstream&fout) {
	prevPositions[positionXMove][positionYMove] = prevPositions[positionX][positionY];
	prevPositions[positionX][positionY] = '.';
	for (int i = 0; i < SQUARES_PER_SIDE; i++) {
		for (int n = 0; n < SQUARES_PER_SIDE; n++) {
			fout << prevPositions[i][n] << " ";
		}
		fout << endl;
	}
}

task main() {
	
}


