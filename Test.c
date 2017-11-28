
#include <stdio.h>
#include <stdlib.h>     // for rand, srand
#include <time.h>       // for time, clock
#include <string.h>     // for strncmp



//for the env_map

	#define EMPTY ' '
	#define SOUTH 'S'
	#define NORTH 'N'
	#define WEST 'W'
	#define EAST 'E'
	#define MIDDLE_LANE 'M'
	#define JUNCTION 'J'
	#define TRAFFIC_LIGHT 'T'
	#define GHOST 'G'
	#define SPAWN_ENTRY 'I'

//for the trafficLights
	#define GREEN 1
	#define YELLOW 0
	#define RED -1

//define the number of timesteps for which each traffic light color stays
	int n_red = 5;
	int n_yellow = 3;
	int n_green = 10;

//define a car struct
	typedef struct car {
		int id; //Car ID
	    int x_old;  //old x position
	    int y_old;  //old y position
	    int v_old;  //old velocity
	    int x_new;  //new x position
	    int y_new;  //new y position
	    int v_new;  //new velocity
	} car;


//define a cell structure for grid elements
	typedef struct cell {
		char map_elem;
		int car_id;
	} cell;

//define a traffic light structure
	typedef struct trafficLight {
		int id;
		int x;
		int y;
		int northSouthLight; //the color of the North and South facing lights
		//eastWestLight = -1*northSouthLight
	} trafficLight;


	double p = 0.2;

	void free_matrix(int n1, int n2, int **a);
	int *malloc_matrix(int n1);
	void print_matrix(int n1, int n2, int **a);
//	int emptycellcount(struct car *c, int **grid,int gridWidth, int gridHeight);
	int emptycellcount(int *grid,int gridWidth, int gridHeight, int i);

//------------------------------------------------------------------------//
	int main(){
		//srand(time(NULL));
		int L = 20; //length of a road segment
		int n_hor = 1; //number of horizontal blocks
		int n_vert = 1; //number of vertical blocks

//		int gridHeight = n_vert * (2*L + 3); //no of rows
//		int gridWidth = n_hor * (2*L + 3); //no of columns

		int gridWidth = 25;
		//int gridHeight = 1;

		int *grid = NULL;
		grid =  malloc_matrix(gridWidth);
		for(int i = 0; i < gridWidth; i++){
			double rn = ((double)rand()) / RAND_MAX;
			if(rn<p){
				grid[i] = rand()%6;
				printf("%d ",grid[i]);
			}
			else{
				grid[i] = 0;
				printf("%d ",grid[i]);
			}
		}

	//FOR EAST
		for(int i = 0; i <gridWidth; i++){
			if(grid[i]!=0){
				int emptygridspaces = 0;
				//int v = grid[i];
				for(int x = i; x < gridWidth; x++){
					if(grid[x+1] == 0){
					emptygridspaces+=1;
					}
					else{
						break;
					}
				}
				printf("\ngrid[i] %d ECC *** %d\n",grid[i],emptygridspaces);

			}
			else{
				continue;
			}

		}

	//FOR WEST?
		for(int i = gridWidth-1; i > -1; i--){
			//printf("I is %d", i);
			if(grid[i]!=0){
				int emptygridspaces = 0;
				for(int x = i; x > 0; x--){
					if(grid[x-1] == 0){
						emptygridspaces+=1;
					}
					else{
						break;
					}
				}
				printf("%d WEST ECC %d\n",grid[i],emptygridspaces);

			}
			else{
				continue;
			}
		}


//		int ecc = emptycellcount(grid,L,1,i);
//		printf("ECC--------%d\n",ecc);

//		for(int col = 0; col < gridWidth; col++){
//			grid[1][col].map_elem = EAST;
//			double rn = rand()/RAND_MAX;
//			if(rn < p){
//				car newcar;
//				newcar.id = col;
//				newcar.v_new = 0;
//				newcar.v_old = rand() % 6;
//				newcar.x_new = 0;
//				newcar.x_old = col;
//				newcar.y_old = 0;
//
//				grid[1][col].car_id = col;
//			}
//			else{
//				grid[1][col].map_elem = EAST;
//				grid[1][col].car_id = 0;
//			}
//		}//end for
		//print_matrix(gridWidth, 1, grid);

	}

//----------------------FELICE--------------------------------------------//



int vmax = 5;
int density = 0.4;
int num_samples = 200;
int car_count = 0;
int numfront = 0;

//FIND OUT HOW MANY SPACES ARE IN FRONT
//int emptycellcount(struct car *c, cell **grid, char direction, int gridWidth, int gridHeight){
int emptycellcount(int *grid,int gridWidth, int gridHeight, int i){
	//	for(int i = 0; i < gridWidth; i++){
	//	for(int j = 0; j < gridHeight; j++){

	//			if((grid[i][j].map_elem == EAST)){
	// && (grid[i][j].car_id != 0)){
	//printf("v- %d- \n",v);
	int emptyspaces=0;
	if(grid[i]!=0){
		int v = grid[i];
	//	printf("V IS%d\n",v);
		for(int x = 0; x < v; x++){
		//	printf("IN HERE\n");
			//int emptyspaces = 0;
			//printf("carid----%d\n",grid[i][j].car_id);
			//if(grid[i][j].car_id == 0){
			printf("grid x %d\n ", grid[x]);
			if(grid[x+1] == 0){
				printf("inside if ");
				emptyspaces++;
			}
			else{
				break;
			}//end else
		}
		//		}
		//		}//end for i
	}//end if east
	else{
		//do nothing
	}
	//
	//	else if(grid[c.x_old][c.y_old].map_elem == WEST){
//		int j = c.y_old;
//		for(int i = 0; i < c.v_new; i++){
//			if(grid[c.x_old-1-i][j].car.id == 0){
//				emptyspaces++;
//			}
//			else{
//				break;
//			}//end else
//		}//end for i
//		//return emptyspaces;
//	}//end if west
//
//	else if(grid[c.x_old][c.y_old].map_elem == SOUTH){
//		int i = c.x_old;
//		for(int j = 0; j < c.v_new; j++){
//			if(grid[i][c.y_old+j+1].car.id == 0){
//				emptyspaces++;
//			}
//			else{
//				break;
//			}//end else
//			return emptyspaces;
//		}//end for j
//	}//end if south
//
//
//	else if(grid[c.x_old][c.y_old].map_elem == NORTH){
//		int i = c.x_old;
//		for(int j = 0; j < c.v_new; j++){
//			if(grid[i][c.y_old-1-j].car.id == 0){
//				emptyspaces++;
//			}
//			else{
//				break;
//			}//end else
//		}//end for j
//	}//end if north
	return emptyspaces;
}//end function



car update_car(struct car c, cell** grid, int active_car_counter, car* active_car_list){
	for(int i = 0; i < active_car_counter; i++){
		char direction = grid[c.x_old][c.y_old].map_elem;

		int x = c.x_old;
		int y = c.y_old;

		//if this is an east road and there is a car in a cell
		if((direction == grid[x][y].map_elem == 'E') && grid[x][y].car_id != 0){

			//see traffic lights
			//					if(TRAFFIC == RED){
			//						car gc = ghost(c,grid,midRow,midCol);
			//					}

			//Calculate number of cells in front of car
	//		numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			c.v_new = update_velocity(c,grid);

			//current positions become old
			c.x_old = c.x_new;
			c.y_old = c.y_new;

			//new positions become current positions
			c.x_new = c.x_old+c.v_new;
			//y direction does not change with east bound traffic
			c.y_new = y;

			//Old spot has car ID of 0 (no car)
			grid[c.x_old][c.y_old].car_id = 0;
			grid[c.x_new][c.y_new].car_id = 1;

		}//end if east


		if((direction == grid[x][y].map_elem == 'W') && grid[x][y].car_id != 0){

			//see traffic lights
			//					if(TRAFFIC == RED){
			//						car gc = ghost(c,grid,midRow,midCol);
			//					}

			//Calculate number of cells in front of car
//			numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			c.v_new = update_velocity(c,grid);

			//current positions become old
			c.x_old = c.x_new;
			c.y_old = c.y_new;

			//new positions become current positions
			c.x_new = c.x_old-c.v_new;
			//y direction does not change with west bound traffic
			c.y_new = y;

			//Old spot has car ID of 0 (no car)
			grid[c.x_old][c.y_old].car_id = 0;
			grid[c.x_new][c.y_new].car_id = 1;

		}//end if west

		if((direction == grid[x][y].map_elem == 'N') && grid[x][y].car_id != 0){


			//see traffic lights
			//					if(TRAFFIC == RED){
			//						car gc = ghost(c,grid,midRow,midCol);
			//					}

			//Calculate number of cells in front of car
//			numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			c.v_new = update_velocity(c,grid);

			//current positions become old
			c.x_old = c.x_new;
			c.y_old = c.y_new;

			//new positions become current positions
			c.y_new = c.y_old-c.v_new;
			//x direction does not change with north bound traffic
			c.x_new = x;

			//Old spot has car ID of 0 (no car)
			grid[c.x_old][c.y_old].car_id = 0;
			grid[c.x_new][c.y_new].car_id = 1;

		}//end if north


		if(direction == grid[x][y].map_elem == 'S' && grid[x][y].car_id != 0){


			//see traffic lights
			//if(TRAFFIC == RED){
			//car gc = ghost(c,grid,midRow,midCol);
			//}

			//Calculate number of cells in front of car
	//		numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			c.v_new = update_velocity(c,grid);

			//current positions become old
			c.x_old = c.x_new;
			c.y_old = c.y_new;

			//new positions
			c.y_new = c.y_old-c.v_new;
			//x direction does not change with south bound traffic
			c.x_new = x;

			//Old spot has car ID of 0 (no car)
			grid[c.x_old][c.y_old].car_id = 0;
			grid[c.x_new][c.y_new].car_id = 1;


		}//end if south


	}//end for i

	return c;

}//end update car east function

//UPDATE VELOCITIES
int update_velocity(struct car c, cell** grid){
	double rn = (double)rand() / RAND_MAX;

	//Set current value velocity as "old velocity"
	c.v_old = c.v_new;

	//Create a temporary integer for the current velocity
	int v_temp = c.v_old;

	//If the current velocity is less than v max, increment
	if(v_temp < vmax){
		v_temp ++;
	}
	//If the new vtemp velocity is greater than number of spaces in front
	//V temp is the number in front minus 1.
	if(v_temp > numfront){
		v_temp = numfront - 1;
	}

	//Randomization step. If random number less than prob, decrement vtemp
	if(rn < p){
		v_temp --;
	}

	//The new velocity is v_temp
	c.v_new = v_temp;
	return c.v_new;
}





//GENERATE GHOST CAR FOR RED LIGHTS
//car ghost(car ghost_car, cell ** grid, int midRow, int midCol){
//
//	malloc_carstruct(ghost_car);
//
//	int row_east = midRow - 1;
//	int col_east = midCol - 1;
//
//	int row_west = midRow + 1;
//	int col_west = midCol + 1;
//
//	ghost_car.id = 2; //Certain ID for ghost cars?
//	ghost_car.x_old = 0;
//	ghost_car.y_old = 0;
//	ghost_car.v_old = 0; //Velocity is 0
//	ghost_car.v_new = 0;
//	ghost_car.y_new = 0;
//	ghost_car.y_new = 0;
//	grid[row_east][col_east].car_id = ghost_car;
//	grid[row_west][row_west].car_id = ghost_car;
//	return ghost_car;
//}


//ALLOCATE MEMORY FOR CAR STRUCTURE?
void malloc_carstruct(car c){
	//Allocate memory for car structure
	car* car_m;
	car_m = (struct car*)malloc(sizeof(car));
	if(car_m == NULL){
		printf("Memory Allocation Failed");
	}
}


/*
 * FUNCTIONS
 */



void print_matrix(int n1, int n2, int **a) {

	if(n1 <= 0 || n2 <= 0 || a == NULL) {
			printf("Wrong Input");
			return;
		}

	for(int i=0; i<n1; i++){
		for(int j=0; j<n2; j++){
			printf("%c  ", a[i][j] );
		} //end for j loop
		printf("\n");
	} //end for i loop
	printf("\n");
} //end print_matrix function

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int *malloc_matrix(int n1) {


    int *mat = NULL;       // pointer to the matrix

   // int i; //Declare i to be type integer (used in for loop)

	//mat = (int **)malloc(n1 * sizeof(int*));        //Create 1D array of pointers
//	for (i=0; i<n1; i++){                           //Loop over every row
		mat = (int *)malloc(n1 * sizeof(int));   //Pointer to array of pointers
//   	} //end for loop

	//Check to see if memory allocation was successful. If memory allocation was unsuccessful, print NULL.
	if (mat==NULL){
		printf("Memory Allocation Failed");
		return NULL;
	}

    return mat;
} //end malloc_matrix function

void free_matrix(int n1, int n2, int **a) {

	if(n1 <= 0 || n2 <= 0 || a == NULL) {
			printf("Wrong Input");
			return;
		}
	for(int i=0; i < n1; i++){
		free(a[i]);
	}
	free(a);
} //end free matrix function
