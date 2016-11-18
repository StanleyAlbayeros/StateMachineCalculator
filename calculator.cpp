#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "calculator.h"


int f, c, step, errcod;


int MAX_ERRCOD = 10;
int MAX_ERRLFN = 10;

enum calc_state{
	IDLE,
	WAIT,
	GO,
	FIRST,
	DIGIT,
	DISPY,
	ADDX,
	INTRO,
	SUB,
	ADD,
	DELETE,
	INIK,
	NOP,
	RES1,
	RES2,
	ERROR,
	WAIT2,
	GO2,
	NERR
} state;

enum {
	curr = 0,
	next = 1
};

struct calc_data_s{
	calc_state state;
	int X,Y,K,A,D,m,v,q;
} calc_data[2];

struct kbd_data_s {
	int r,c;
	int p;
} kbd_data[2];

/////////////////////////////StateMachine//////////////////////////////////

void calc_init(){
	calc_data[curr].state = IDLE;
	calc_data[curr].D = 0;
	calc_data[curr].A = 0;
	calc_data[curr].v = 0;
	calc_data[curr].Y = 0;
	calc_data[curr].K = 0;
	calc_data[next] = calc_data[curr];
}
void calc_step() {

	int encod;
	encod = 0;

	switch (calc_data[curr].state){

		case IDLE:
			calc_data[next].Y = 0;
			calc_data[next].A = 0;
			calc_data[next].K = 0;
			calc_data[next].m = 0;
			calc_data[next].D = 0;
			calc_data[next].v = 0;
			calc_data[next].state = WAIT;
		break;

		case WAIT:
			calc_data[next].q = 1;
			if (kbd_data[curr].p == 1){
				calc_data[next].state = GO;
			} else {
				calc_data[next].state = WAIT;
			}
		break;

		case GO:
			calc_data[next].q = 0;
			if ( kbd_data[curr].c == 11){
				switch (kbd_data[curr].r){
		 			case 0:
						calc_data[next].state = INTRO;
		 			break;

		 			case 1:
						calc_data[next].state = ADD;
		 			break;

		 			case 10:
						calc_data[next].state = SUB;
		 			break;

		 			case 11:
						calc_data[next].state = DELETE;
		 			break;
				}
			} else {
				if (calc_data[curr].K == 0){
					if (kbd_data[curr].r ==0){
						calc_data[next].state = WAIT;			
					} else {
						calc_data[next].state = FIRST;
					}
				} else if (calc_data[curr].K == 4){
					calc_data[next].state = WAIT;
				} else {
					calc_data[next].state = DIGIT;
				}
			}
		break;

		case FIRST:
			calc_data[next].Y = 0;
			calc_data[next].state = DIGIT;
		break;

		case DIGIT:		
			calc_data[next].K = calc_data[curr].K + 1;
			calc_data[next].Y = calc_data[curr].Y * 10;
			calc_data[next].X = getKeyValue(kbd_data[curr].r, kbd_data[curr].c);

			if (kbd_data[curr].r == 0){
				calc_data[next].state = DISPY;				
			} else {
				calc_data[next].state = ADDX;
			}
		break;

		case DISPY:
			calc_data[next].D = calc_data[curr].Y;
			calc_data[next].m = 0;
			calc_data[next].v = 0;
			calc_data[next].state = WAIT;
		break;

		case ADDX:
			calc_data[next].Y = calc_data[curr].Y + calc_data[curr].X;
			calc_data[next].state = DISPY;
		break;

		case INTRO:
			calc_data[next].A = calc_data[curr].Y ;
			calc_data[next].state = INIK;
		break;

		case SUB:
			calc_data[next].Y = calc_data[curr].A - calc_data[curr].Y;
			calc_data[next].state = NOP;
		break;

		case ADD:
			calc_data[next].Y = calc_data[curr].A + calc_data[curr].Y;
			calc_data[next].state = NOP;
		break;

		case DELETE:
			calc_data[next].Y = 0;
			calc_data[next].D = 0;
			calc_data[next].m = 0;
			calc_data[next].state = INIK;
		break;

		case INIK:
			calc_data[next].K = 0;
			calc_data[next].state = WAIT;
		break;

		case NOP:
			if ((calc_data[curr].Y < -9999)||(calc_data[curr].Y > 9999)){
				calc_data[next].state = ERROR;
			} else if (calc_data[curr].Y < 0) {				
				calc_data[next].state = RES1;
			} else {
				calc_data[next].state = RES2;
			}
		break;

		case RES1:
			calc_data[next].D = (-1 * calc_data[curr].Y) + 1;
			calc_data[next].m = 1;
			calc_data[next].state = INIK;
		break;

		case RES2:
			calc_data[next].D = calc_data[curr].Y;
			calc_data[next].m = 0;
			calc_data[next].state = INIK;
		break;

		case ERROR:
			calc_data[next].v = 1;
			calc_data[next].state = WAIT2;
		break;

		case WAIT2:
			calc_data[next].q =  1;
			if (kbd_data[curr].p == 1){
				calc_data[next].state = GO2;
			} else {
				calc_data[next].state = WAIT2;
			}
		break;

		case GO2:
			calc_data[next].q = 0;
			if ( (kbd_data[curr].r == 11)&&(kbd_data[curr].c == 11) ){
				calc_data[next].state = NERR;
			} else {				
				calc_data[next].state = WAIT2;
			}
		break;

		case NERR:
			calc_data[next].Y = 0;
			calc_data[next].D = 0;
			calc_data[next].m = 0;
			calc_data[next].v = 0;
			calc_data[next].state = INIK;
		break;
	}
	calc_data[curr] = calc_data[next];
}

//////////////////////////////////////////////////////////////////////////
////////////////////////////TestBench///////////////////////////////////

void readInput(){

	int row, column;
	int p=0;

	printf("\n__________________________\n");
	printf("| 7 |  | 8 |  | 9 |  | C |\n");
	printf("| 4 |  | 5 |  | 6 |  | - |\n");	
	printf("| 1 |  | 2 |  | 3 |  | + |\n");
	printf("|        0        |  | = |\n");
	printf("__________________________\n");
	printf("\nInput row number (1,2,3,4): ");
	scanf("%d", &row);
	printf("\nInput column number (1,2,3,4): ");
	scanf("%d", &column);
	if ( ( ((row < 4)||(column >=1)) && ((column < 4)||(column>=1)) ) ){
		kbd_data[curr].r = rowToKey(row);
		kbd_data[curr].c = columnToKey(column);
		kbd_data[curr].p = 1;
	}
	printf("\n");
}

int rowToKey(int row){
	int result;
	switch (row){
		case 1:
		result = 11; 
		break;

		case 2:
		result = 10; 

		break;

		case 3:
		result = 1; 

		break;

		case 4:
		result = 0; 

		break;

		default:
		break;

	}
	return result;

}

int columnToKey(int column){
	int result;
	switch (column){
		case 1:
		result = 0; 

		break;

		case 2:
		result = 1; 

		break;

		case 3:
		result = 10; 

		break;

		case 4:
		result = 11; 

		break;

		default:
		break;

	}
	return result;
}

int getKeyValue(int row, int column){
	int value;

	switch (column){
	 	case 0:
	 		switch (row){
	 			case 0:
	 			value = 0;
	 			break;

	 			case 1:
	 			value = 1;
	 			break;

	 			case 10:
	 			value = 4;
	 			break;

	 			case 11:
	 			value = 7;
	 			break;
	 		}
	 	break;

	 	case 1:
	 		switch (row){
	 			case 0:
	 			value = 0;
	 			break;

	 			case 1:
	 			value = 2;
	 			break;

	 			case 10:
	 			value = 5;
	 			break;

	 			case 11:
	 			value = 8;
	 			break;
	 		}
	 	break;

	 	case 10:
	 		switch (row){
	 			case 0:
	 			value = 0;
	 			break;

	 			case 1:
	 			value = 3;
	 			break;

	 			case 10:
	 			value = 6;
	 			break;

	 			case 11:
	 			value = 9;
	 			break;
	 		}
	 	break;

	 	case 11:
	 		switch (row){
	 			case 0:
	 			value = -1;
	 			break;

	 			case 1:
	 			value = -2;
	 			break;

	 			case 10:
	 			value = -3;
	 			break;

	 			case 11:
	 			value = -4;
	 			break;
	 		}
	 	break;
	}
	return value;
}

void main (){
	calc_init();
	//readInput();
	while (1){
		calc_step();
		if (calc_data[curr].state == WAIT){
			readInput();
			// cutre?
			// calc_step(); x6
			// cutre?
		} else if (calc_data[curr].state == WAIT2){
			printf("Error, press the C button\n");
			readInput();
		}
		printf( "\r Display: %d", calc_data[curr].D);
	}
}
//////////////////////////////////////////////////////////////////////////
