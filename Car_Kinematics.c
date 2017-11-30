#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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

	int vmax = 5;
	int density = 0.4;
	int num_samples = 200;
	int car_count = 0;
	int numfront = 0;
	double p = 0.4;


//define a car struct
	typedef struct car {
		char map_elem; //Car direction
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
		int map_elem;
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


//Matrix Prototypes
	cell **malloc_grid(int n1, int n2);
	void free_grid(int n1, int n2, cell **a);
	//print functions print to screen
	void print_map_elems(int n1, int n2, cell **a);
	void print_velocities(int n1, int n2, cell **a);
	void print_car_ids(int n1, int n2, cell **a);


//Grid Initialization Functions
	cell** create_grid(int L, int gridHeight, int gridWidth, trafficLight* traffic_lights_list,int* max_cars);
	void fill_intersection(cell** grid,int L, int midRow,int midCol, int* max_cars);
	void random_fill(cell** grid,int gridHeight,int gridWidth,double p, car* activeCarList);

//Active Car List/Results Functions
	void grid_snapshot(cell** grid,int gridHeight,int gridWidth,double timeStamp,car* active_car_list,FILE *file);
	void save_results(FILE* f, cell** grid, int gridHeight, int gridWidth, double timeStamp);
	//fprint functions print to file
	void fprint_map_elems(int n1, int n2, cell **a,FILE* f);
	void fprint_velocities(int n1, int n2, cell **a,FILE* f);
	void fprint_car_ids(int n1, int n2, cell **a,FILE* f);

//Traffic Light Functions
	void update_trafficLight(trafficLight* light, int timestep, int n_red, int n_yellow, int n_green);

//Other Helper Functions
	double uniform(void);
	double urand(int LowerLimit,int UpperLimit);

	int emptycellcount(cell **grid, int gridWidth, int gridHeight, int i, int j, char direction);
	int update_velocity(struct car *c, cell** grid, int empty);
	void update_car(struct car *c, cell** grid, int empty, int ncars, int i, struct car *activeCarList);

//The main function
int main(){

	//Set seed rand() for random grid
	//srand(time(NULL));

	int L = 3; //length of a road segment
	int n_hor = 1; //number of horizontal blocks
	int n_vert = 1; //number of vertical blocks

	int gridHeight = n_vert * (2*L + 3); //no of rows
	int gridWidth = n_hor * (2*L + 3); //no of columns

	//reserve memory for the traffic lights list
	int num_trafficLights = n_hor*n_vert;
	trafficLight* traffic_lights_list = (trafficLight*)malloc(num_trafficLights*sizeof(trafficLight));

	//define max_cars
	int max_cars = 0;

	//Create the grid + update the traffic_lights_list
	cell** grid = create_grid(L,gridHeight,gridWidth,traffic_lights_list,&max_cars);

	printf("Max Cars: %d",max_cars);

	//display the grid on screen
	printf("\nPrinting the grid:\n");
	print_map_elems(gridHeight,gridWidth,grid);

	// //create an initial active car list
	int n_cars = 10;
	car* activeCarList = (car*)malloc(max_cars * sizeof(car));

	// Fill up the grid randomly with cars
	double p = 0.3; //prob a cell has a car
	random_fill(grid,gridHeight,gridWidth,p,activeCarList);

	//display the car IDs on screen
	printf("\nPrinting the grid of car ID's\n");
	print_car_ids(gridHeight,gridWidth,grid);

	//print the active car list
	printf("\nPrinting the active car list\n");
	for(int i = 0; i < max_cars; i++){
		printf("%d,",activeCarList[i].id);
	}
	printf("\n");

	//-------------------FELICE------------------------------------------------
	//loop over active car list from counter


	for(int i = 0; i < n_cars; i++){
		if(activeCarList[i].id !=-1){

			int index = activeCarList[i].id; // this is the ID of the car we're considering
			int x_pos = activeCarList[i].x_old; //current x position of car
			int y_pos = activeCarList[i].y_old; //current y position of car
			printf("-----------------------\n");
			printf("x is %d\n",x_pos);
			printf("y is %d\n",y_pos);
			char direction = SOUTH;//activeCarList[i].map_elem;
			printf("GOING %c\n ",direction);

			//CHECK SPACES
			int empty = emptycellcount(grid, gridWidth, gridHeight, x_pos, y_pos, direction);
			printf("Returned emptyspaces count = %d\n", empty);

			//UPDATE VELOCITY
			//printf("VELOCITY OLD Before update %d\n",activeCarList[i].v_old);
			printf("Old Velocity = %d\n",activeCarList[i].v_old);

			//int v = update_velocity(&activeCarList[i], grid, empty);
			//printf("Returned velocity = %d\n",v);
			printf("**** BEFORE UPDATE CAR FUNCTION ********\n");
			update_car(&activeCarList[i], grid, empty, n_cars, i, activeCarList);

		}//end if

	}

	print_car_ids(gridHeight,gridWidth,grid);

}


//----------------------FELICE--------------------------------------------//

//--------------------------------------------------------------//
//FIND OUT HOW MANY SPACES ARE IN FRONT
//int emptycellcount(struct car *c, cell **grid, char direction, int gridWidth, int gridHeight){
int emptycellcount(cell **grid, int gridWidth, int gridHeight, int i, int j, char direction){

	int emptyspaces=0;

	if(direction == EAST){
		for(int x = i; x < gridWidth; x++){
			for(int y = j; y < j+1; y++){
				if(grid[i+1][j].car_id == -1){
					printf("emptyspaces %d\n", emptyspaces);
					emptyspaces++;
				}
				else{
					//printf("in else break\n");
					return emptyspaces;
				}//end else
			}//end for i
		}//end if east
	}


	else if(direction == WEST){
		for(int x = gridWidth-1; x>-1; x--){
			for(int y = j; y < j+1; y++){
				if(grid[i-1][j].car_id == -1){
					emptyspaces++;
				}
				else{
					break;
				}//end else
			}//end for i
		}//return emptyspaces;
	}//end if west

	else if(direction == NORTH){
		for(int x = i; x < i+1; x++){
			for(int y = gridHeight; y > -1; y--){
				if(grid[i][j-1].car_id == -1){
					emptyspaces++;
				}
				else{
					break;
				}//end else
			}//end for i
		}//return emptyspaces;
	}//end if west

	else if(direction == SOUTH){
		//printf("In south direction else statement\n");
		for(int x = i; x < i+1; x++){
			for(int y = j; y < gridHeight; y++){
				//printf("grid x %d\n ", activeCarList[x][y]);
				if(grid[i][j+1].car_id == -1){
					//printf("emptyspaces %d\n", emptyspaces);
					emptyspaces++;
					//printf("grid id  (%d, %d) %d\n", x,y ,grid[i][j+1].car_id);
				}
				else{
					//printf("in else statement");
					break;
				}//end else
			}//end for i
		}//end if east
	}
	return emptyspaces;
}//end function


//UPDATE VELOCITIES
int update_velocity(struct car *c, cell** grid, int empty){

	double rn = (double)rand() / RAND_MAX;

	//Set current value velocity as "old velocity"
	//printf("V NEW %d\n", c->v_new);
	//c->v_new = c->v_old;

	//Create a temporary integer for the current velocity
	int v_temp = c->v_new;
	c->v_old = v_temp;

	//If the current velocity is less than v max, increment
	if(v_temp < vmax){
		v_temp ++;
	}

	//NS Model says d-1, but what if there are 0 spaces?
	//It should just be min(d,v)?
	if(empty < v_temp){
		v_temp = empty;
	}
	printf("V temp is --- %d\n",v_temp);
	//Randomization step. If random number less than prob, decrement vtemp
	if(rn < p){
		printf("YES RANDOMIZATION\n");
		v_temp --;
	}
	//The new velocity is v_temp
	c->v_new = v_temp;
	return v_temp;
}



void update_car(struct car *c, cell** grid, int empty, int ncars, int i, struct car *activeCarList){
	//for(int i = 0; i < ncars; i++){

		//CHANGE: GET DIRECTION FROM CAR STRUCT INSTEAD OF GRID
		char direction = SOUTH;

//		if(northSouthLight == RED || northSouthLight == YELLOW){
//			ghost(grid, midRow, midCol, direction, numcars);
//		}
//
//		else(){
//
//		}


		//if this is an east road and there is a car in a cell
		if((direction == EAST)){// && (grid[x][y].car_id != -1)){
			//Calculate number of cells in front of car
	//		numfront = emptycellcount(c,grid,direction);

			int v = update_velocity(&activeCarList[i], grid, empty);

			printf("V in update_car= %d\n",v);

			//UPDATE X COORDINATES
			int x_current = c->x_new;
			c->x_old = x_current;
			c->x_new = x_current + v;

			//Y STAYS THE SAME IN WEST-BOUND TRAFFIC
			c->y_new = c->y_old;

			printf("y old %d , y new %d\n", c->y_old, c->y_new);
			printf("x old %d , x new %d\n", c->x_old, c->x_new);

			//Old spot has car ID of -1 (no car)
			//grid[c->x_old][c->y_old].car_id = -1;

		}//end if east


		else if((direction == WEST)){// && (grid[x][y].car_id != -1)){

			//Calculate number of cells in front of car
//			numfront = emptycellcount(c,grid,direction);

			int v = update_velocity(&activeCarList[i], grid, empty);

			printf("new velocity in function %d\n",v);

			//UPDATE X COORDINATES
			int x_current = c->x_new;
			c->x_old = x_current;
			c->x_new = x_current - v;

			//Y STAYS THE SAME IN WEST-BOUND TRAFFIC
			c->y_new = c->y_old;

			printf("y old %d , y new %d\n", c->y_old, c->y_new);
			printf("x old %d , x new %d\n", c->x_old, c->x_new);

			//Old spot has car ID of -1 (no car)
			grid[c->x_old][c->y_old].car_id = -1;

		}//end if west

		else if((direction == NORTH)){// && (grid[x][y].car_id != -1)){
			printf("IN IF NORTH\n");
			//Calculate number of cells in front of car
//			numfront = emptycellcount(c,grid,direction);

			int v = update_velocity(&activeCarList[i], grid, empty);

			printf("new velocity in function %d\n",v);

			//UPDATE Y COORDINATES
			int y_current = c->y_new;
			c->y_old = y_current;
			c->y_new = y_current - v;

			//X STAYS THE SAME IN NORTH-BOUND TRAFFIC
			c->x_new = c->x_old;

			printf("y old %d , y new %d\n", c->y_old, c->y_new);
			printf("x old %d , x new %d\n", c->x_old, c->x_new);

			//Old spot has car ID of 0 (no car)
			grid[c->x_old][c->y_old].car_id = -1;

		}//end if north


		else if((direction == SOUTH)){// && (grid[x][y].car_id != -1)){
			printf("In if south\n");

			//Calculate number of cells in front of car
//			numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			int v = update_velocity(&activeCarList[i], grid, empty);

			printf("new velocity in function %d\n",v);

			//UPDATE Y COORDINATES
			int y_current = c->y_new;
			c->y_old = y_current;
			c->y_new = y_current + v;

			//X STAYS THE SAME IN SOUTH-BOUND TRAFFIC
			c->x_new = c->x_old;

			printf("y old %d , y new %d\n", c->y_old, c->y_new);
			printf("x old %d , x new %d\n", c->x_old, c->x_new);

			//Old spot has car ID of 0 (no car)
			grid[c->x_old][c->y_old].car_id = -1;

		}//end if south


}//end update car south function





//GENERATE GHOST CAR FOR RED LIGHTS
void ghost(cell ** grid, int midRow, int midCol, char direction, int numcars, trafficLight *color){

	int light = color->northSouthLight;

	//EAST
	int x_loc_E = midCol - 2;
	int y_loc_E = midRow - 1;

	//WEST
	int x_loc_W = midCol + 2;
	int y_loc_W = midRow + 1;

	//NORTH
	int x_loc_N = midCol - 1;
	int y_loc_N = midRow - 2;

	//SOUTH
	int x_loc_S = midCol + 1;
	int y_loc_S = midRow + 2;


	if(light == 1 || light == 0){
		grid[x_loc_E][y_loc_E].car_id = numcars + 1;
		grid[x_loc_W][y_loc_W].car_id = numcars + 1;
	}

	else{
		grid[x_loc_N][y_loc_N].car_id = numcars + 1;
		grid[x_loc_S][y_loc_S].car_id = numcars + 1;
	}

	//return id;
}

/*
 * if light = red
 * generate ghost car
 *
 *
 */


/*
 * Sandesh's Function -----------------------------------------------
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


//Function Definitions


	//Update Traffic Lights
	void update_trafficLight(trafficLight* light, int timestep, int n_red, int n_yellow, int n_green){

		int period = n_red + n_yellow + n_green + n_yellow; //the period for a traffic light cycle

		int lightTimer = timestep % period;

		if (lightTimer >= 0 && lightTimer <= n_red){ // we are at the start of the light cycle
			light->northSouthLight = RED; //start NS light at RED
		} else if (lightTimer > n_red && lightTimer <= (n_red + n_yellow)){ //we are now in yellow region
			light->northSouthLight = YELLOW;
		} else if (lightTimer > (n_red + n_yellow) && lightTimer <= (n_red + n_yellow + n_green)){ //we are now in green region
			light->northSouthLight = GREEN;
		} else { //we are in the final yellow region of the light cycle
			light->northSouthLight = YELLOW;
		}
	}



	//Matrix Functions

		//Input: number of rows (n1) and columns (n2) required
		//Output: Reserve space for a matrix of size n1 x n2
		cell **malloc_grid(int n1, int n2) {

		    cell **mat = NULL;       // pointer to the matrix

			if(n1>0 && n2 >0){ //checking for invalid inputs
			    mat = (cell**)malloc(n1*sizeof(cell *)); //Allocate memory for n1 rows

			    if(mat == NULL){
			    	printf("Error: Could not reserve memory for matrix.");
			    	exit(1);
			    }

				for(int i = 0; i < n1; i++){
					mat[i] = (cell*)malloc(n2*sizeof(cell)); //Allocate memory for n2 columns for each of the n1 rows

					if (mat[i] == NULL) { //if memory allocation failure
						for (int j = 0; j < i; j++) {  // free memory reserved for all previous cells
							free(mat[j]);
						}

						printf("Could not reserve memory for matrix.");
					    exit(1);
					}


				}
			} else {
				printf("Invalid input for grid: Make sure n1 > 0 && n2 > 0");
				exit(1);
			}

		    return mat;
		}

		//free the memory reserved for a grid
		//input: size of matrix n1 and n2, and the pointer to the matrix
		void free_grid(int n1, int n2, cell **a) {

			if(n1 > 0 && n2 > 0 && a != NULL){ //checking for invalid inputs

				for(int i =0; i<n1; i++){ //Freeing up columns unless they are null
					if(a[i]!=NULL){
						free(a[i]);
					}
				}

				free(a); //Freeing up rows

			} else {
				printf("Invalid input for free_matrix: Make sure n1 > 0 && n2 > 0 && input matrix != NULL");
				exit(1);
			}

		}

		//free the memory reserved for a grid
		//input: size of matrix n1 and n2, and the pointer to the matrix
		void free_int_matrix(int n1, int n2, int **a) {

			if(n1 > 0 && n2 > 0 && a != NULL){ //checking for invalid inputs

				for(int i =0; i<n1; i++){ //Freeing up columns unless they are null
					if(a[i]!=NULL){
						free(a[i]);
					}
				}

				free(a); //Freeing up rows

			} else {
				printf("Invalid input for free_matrix: Make sure n1 > 0 && n2 > 0 && input matrix != NULL");
				exit(1);
			}

		}

		//print a matrix to the console. Useful for debugging
		//input size of matrix (n1 and n2), and pointer to the matrix
		void print_map_elems(int n1, int n2, cell **a) {

			if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

				printf("\n");
				for(int i=0;i<n1;i++){
					for(int j=0;j<n2;j++){
						if(j==n2-1){
							printf("%c",a[i][j].map_elem); //Doesn't add a comma for the last column
						} else {
							printf("%c",a[i][j].map_elem); //Adds a comma after every element to separate them
						}

					}
					printf("\n");//Print the next row on a different line
				}
			} else {
				printf("Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
				exit(1);
			}
		}

		// //print velocities of cars on grid
		// //input size of matrix (n1 and n2), and pointer to the matrix
		// void print_velocities(int n1, int n2, cell **a) {

		// 	if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

		// 		printf("\n");
		// 		for(int i=0;i<n1;i++){
		// 			for(int j=0;j<n2;j++){
		// 				if(j==n2-1){
		// 					printf("%d",a[i][j].car.v_old); //Doesn't add a comma for the last column
		// 				} else {
		// 					printf("%d,",a[i][j].car.v_old); //Adds a comma after every element to separate them
		// 				}

		// 			}
		// 			printf("\n");//Print the next row on a different line
		// 		}
		// 	} else {
		// 		printf("Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
		// 		exit(1);
		// 	}
		// }

		//print ids of cars on grid
		//input size of matrix (n1 and n2), and pointer to the matrix
		void print_car_ids(int n1, int n2, cell **a) {

			if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

				printf("\n");
				for(int i=0;i<n1;i++){
					for(int j=0;j<n2;j++){
						if(j==n2-1){
							printf("%d",a[i][j].car_id); //Doesn't add a comma for the last column
						} else {
							printf("%d,",a[i][j].car_id); //Adds a comma after every element to separate them
						}

					}
					printf("\n");//Print the next row on a different line
				}
			} else {
				printf("Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
				exit(1);
			}
		}

	//Grid Initialization Functions

	cell** create_grid(int L, int gridHeight, int gridWidth,trafficLight* traffic_lights_list, int* max_cars){

		// printf("Grid Height = %d, Grid Width = %d\n",gridWidth,gridHeight);

		// //Reserve space for full grid
		cell** grid = malloc_grid(gridHeight,gridWidth);

		if(grid == NULL){printf("Error mallocing grid!");exit(1);};

		//Intially fill up grid with EMPTYs
		for (int row = 0; row < gridHeight; row++){
			for(int col = 0; col < gridWidth; col++){
				grid[row][col].map_elem = EMPTY;
				grid[row][col].car_id = -1;
			}
		}

		// //Add intersections

		int intersectionCount = 0;

		for(int row_block = 0; row_block < gridHeight; row_block++){
			if(row_block % (2*L + 3) == 0){
				// grid[row_block][0] = 'H';
				for(int col_block = 0; col_block < gridWidth; col_block++){
					if(col_block % (2*L + 3) == 0){
						// grid[row_block][col_block] = 'H';

						int midRow = row_block + (L+1);
						int midCol = col_block + (L+1);

						fill_intersection(grid,L,midRow,midCol,max_cars);

						//Add the spawn entry points
						grid[0][midCol -1].map_elem = SPAWN_ENTRY; //Northbound Spawn
						grid[gridHeight-1][midCol + 1].map_elem = SPAWN_ENTRY; //Southbound Spawn
						grid[midRow+1][0].map_elem = SPAWN_ENTRY; //Southbound Spawn
						grid[midRow-1][gridWidth-1].map_elem = SPAWN_ENTRY; //Southbound Spawn

						//Add a traffic light to the traffic_lights_list
						traffic_lights_list[intersectionCount].id = intersectionCount;
						traffic_lights_list[intersectionCount].x = midRow;  //x-coordinate
						traffic_lights_list[intersectionCount].y = midCol; //y-coordinate
						traffic_lights_list[intersectionCount].northSouthLight = RED; //y-coordinate
						//above might be suspect to seg-faults
						intersectionCount++;

					}
				}
			}
		}

		// // //Return the grid matrix as output
		return grid;

	}

	// //Fill up grid around an intersection
	void fill_intersection(cell** grid,int L, int midRow,int midCol, int* max_cars){

		//Create the North-South Lanes
		for(int row = midRow - (L+1);row <= midRow + (L+1);row++){

				if (abs(row - midRow) == 2) //Juntion space needed for ghost cars
				{
					grid[row][midCol - 1].map_elem = GHOST;
					grid[row][midCol + 1].map_elem = GHOST;
					grid[row][midCol].map_elem = MIDDLE_LANE;
					*max_cars += 2;
					// printf("Add roadcoutner\n");

				} else{
					grid[row][midCol - 1].map_elem = SOUTH;
					grid[row][midCol + 1].map_elem = NORTH;
					*max_cars += 2;
					grid[row][midCol].map_elem = MIDDLE_LANE;
				}

		}

		//Create the East-West Lanes
		for(int col = midCol - (L+1);col <= midCol + (L+1);col++){
				if (abs(col - midCol) == 2) //Juntion space needed for ghost cars
				{
					grid[midRow - 1][col].map_elem = GHOST;
					grid[midRow + 1][col].map_elem = GHOST;
					grid[midRow][col].map_elem = MIDDLE_LANE;
					*max_cars += 2;
				} else {
					grid[midRow - 1][col].map_elem = WEST;
					grid[midRow + 1][col].map_elem = EAST;
					grid[midRow][col].map_elem = MIDDLE_LANE;
					*max_cars += 2;
				}
		}

		//fill up the junctions
		for(int row = midRow - 1; row <= midRow + 1; row++){
			for(int col = midCol -1; col <= midCol + 1; col++){
				grid[row][col].map_elem = JUNCTION;
				*max_cars -= 1; //We will still have 2 extra cars in max_cars
			}
		}

		// *max_cars -= 2;

		//Add the traffic light at the middle
		grid[midRow][midCol].map_elem = TRAFFIC_LIGHT;
		*max_cars -= 1;

	}

	//randomly fill up the grid  with some cars
	void random_fill(cell** grid,int gridHeight,int gridWidth,double p, car* activeCarList){
		//randomly fills the grid with a few cars

		int activeCarListCounter = 0;

		//This function needs to create a random active car list

		for(int row = 0; row < gridHeight; row++){
			for(int col = 0; col < gridWidth; col++){
				if ((grid[row][col].map_elem == NORTH) |
					(grid[row][col].map_elem == SOUTH) |
					(grid[row][col].map_elem == EAST) |
					(grid[row][col].map_elem == WEST) |
					(grid[row][col].map_elem == GHOST)){

					if(uniform() < p){

						//create an initial car element
						car init_car;
//						init_car.map_elem = E;
						init_car.id = activeCarListCounter; //this should now be a unique id
						init_car.x_old = col;
						init_car.y_old = row;
						init_car.x_new = col;
						init_car.y_new = row;
						init_car.v_old = urand(1,5); //random velocity between 1 and 5
						init_car.v_new = init_car.v_old;

						//add init car to active car list
						activeCarList[activeCarListCounter] = init_car;

						//Assign the car to a cell on the grid
						grid[row][col].car_id = init_car.id;

						//Increment counter
						activeCarListCounter ++;

					}
					else {
						// car empty_car;
						car empty_car;
						empty_car.id = -1;
						empty_car.x_old = -1;
						empty_car.y_old = -1;
						empty_car.x_new = -1;
						empty_car.y_new = -1;
						empty_car.v_old = -1;
						empty_car.v_new = -1;

						//add empty car to active car list
						activeCarList[activeCarListCounter] = empty_car;

						//Add info to the grid
						grid[row][col].car_id = -1; //No car in that cell

						//Increment counter
						activeCarListCounter++;
					}

				}

			}
		}

	}

	//Grid Update/Results Functions


	// //Take a snapshot of the grid
	// //print out the car id or velocities
	// //output an array of active cars
	// //This array will be sent to car_calculations() and
	// //later to update_grid()
	// void grid_snapshot(cell** grid,int gridHeight,int gridWidth,double timeStamp,car* active_car_list,FILE *file){

	// 	int active_car_counter = 0; //keep track of how many cars are active

	// 	for(int row = 0; row < gridHeight; row++){
	// 		for(int col = 0; col < gridWidth; col++){
	// 			if (grid[row][col].map_elem == NORTH |
	// 				grid[row][col].map_elem == SOUTH |
	// 				grid[row][col].map_elem == EAST |
	// 				grid[row][col].map_elem == WEST){

	// 					if(grid[row][col].car.id != 0){
	// 						active_car_list[active_car_counter] = grid[row][col].car;
	// 						active_car_counter++;
	// 					}

	// 			}

	// 		}
	// 	}

	// 	// printf("\nNumber of active cars: %d\n",active_car_counter);

	// 	//create an output file that stores the snapshot
	// 	save_results(file,grid,gridHeight,gridWidth,timeStamp);

	// 	// return active_car_list; //this will have information old_car information
	// }

	// //Save results to file
	// void save_results(FILE* f, cell** grid, int gridHeight, int gridWidth, double timeStamp){

	// 	fprintf(f,"//BEGIN:SNAPSHOTS//\n");
	// 	fprintf(f,"//TIMESTAMP=%f//\n",timeStamp);
	// 	fprintf(f,"//BEGIN:SNAPSHOTS//\n");
	// 	fprintf(f,"//BEGIN:CAR_IDS//\n");
	// 	fprint_car_ids(gridHeight,gridWidth,grid,f);
	// 	fprintf(f,"//END:CAR_IDS//\n");
	// 	fprintf(f,"//BEGIN:CAR_VELOCITIES//\n");
	// 	fprint_velocities(gridHeight,gridWidth,grid,f);
	// 	fprintf(f,"//END:CAR_VELOCITIES//\n");
	// }


	//print map_elems to file
	void fprint_map_elems(int n1, int n2, cell **a,FILE* f) {

		if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

			fprintf(f,"//BEGIN:MAP//\n");
			for(int i=0;i<n1;i++){
				for(int j=0;j<n2;j++){
					if(j==n2-1){
						fprintf(f,"%c",a[i][j].map_elem); //Doesn't add a comma for the last column
					} else {
						fprintf(f,"%c,",a[i][j].map_elem); //Adds a comma after every element to separate them
					}

				}
				fprintf(f,"\n");//Print the next row on a different line
			}
			fprintf(f,"//END:MAP//\n");
		} else {
			fprintf(f,"Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
			exit(1);
		}
	}

	//print ids of cars on grid
	//input size of matrix (n1 and n2), and pointer to the matrix
	void fprint_car_ids(int n1, int n2, cell **a,FILE* f) {

		if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

			for(int i=0;i<n1;i++){
				for(int j=0;j<n2;j++){
					if(j==n2-1){
						fprintf(f,"%d",a[i][j].car_id); //Doesn't add a comma for the last column
					} else {
						fprintf(f,"%d,",a[i][j].car_id); //Adds a comma after every element to separate them
					}

				}
				fprintf(f,"\n");//Print the next row on a different line
			}
		} else {
			fprintf(f,"Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
			exit(1);
		}
	}

	// //print velocities of cars on grid
	// //input size of matrix (n1 and n2), and pointer to the matrix
	// void fprint_velocities(int n1, int n2, cell **a,FILE* f) {

	// 	if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

	// 		for(int i=0;i<n1;i++){
	// 			for(int j=0;j<n2;j++){
	// 				if(j==n2-1){
	// 					fprintf(f,"%d",a[i][j].car.v_old); //Doesn't add a comma for the last column
	// 				} else {
	// 					fprintf(f,"%d,",a[i][j].car.v_old); //Adds a comma after every element to separate them
	// 				}

	// 			}
	// 			fprintf(f,"\n");//Print the next row on a different line
	// 		}
	// 	} else {
	// 		fprintf(f,"Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
	// 		exit(1);
	// 	}
	// }



	//rand uniform dist from engine
	double uniform(void)
	{
		return rand() / (RAND_MAX + 1.0);
	}

	//Generate a random number uniformly distributed over an interval [LowerLimit,UpperLimit)
	double urand(int LowerLimit,int UpperLimit){

		double ratio = rand() / ((float)RAND_MAX + 1);  //RAND_MAX + 1 because we don't want to include upper limit
		double interval = UpperLimit - LowerLimit;
		double unirand = (ratio * interval);
		return unirand;
	}


// /////////////////////////////////////////////////////////////////////////////////
// // WAITLIST FUNCTIONS
// /////////////////////////////////////////////////////////////////////////////////
// //Create waitlist
// activeCarList* create_list(int size) {
//     // Allocate memory for the structure
//     activeCarList *W = malloc( sizeof (activeCarList) );
//     if (W == NULL) {
//         printf("Could not create waitlist. Out of Memory!\n");
//         return NULL;
//     }

//     // Allocate space for the array in the WaitList struct
//     // Note that this space is different from that already
//     // allocated to the structure.
//     W->elements = malloc( sizeof(car) * size);
//     if (W->elements == NULL) {
//         printf("Could not create waitlist. Out of Memory!\n");
//         return NULL;
//     }

//     W->counter = 0;     // 0 elements initially in the list
//     W->MAX_SIZE = size; // maximum size the list can hold

//     return W;
// }

// int add_to_end(activeCarList* aList,car* element){
//     if (aList == NULL){
//         printf("Could not add to waitlist. No wait list found!");
//         return 1;
//     }

//     // check if list is full
//     if (aList->counter == aList->MAX_SIZE){
//         //Double the size allocated to the list
//         aList->elements = realloc(aList->elements,aList->MAX_SIZE*2);
//     }

//     // add an element to the end of the list
//     // and increment the counter
//     aList->elements[aList->counter] = *element;
//     aList->counter++;

//     return 0;
// }


// int remove_first_car(activeCarList* aList){

//     if(aList == NULL){
//         printf("Could not remove from waitlist. No wait list found!");
//         return 1;
//     } else if (aList->counter == 0) {
//         return 0; //don't do anything if counter is already 0
//     }

//     //
//     for(int i = 1; i < aList->counter; i++){
//         aList->elements[i] = aList->elements[i+1];
//     }

//     aList->counter = aList->counter - 1; //reduce the counter size by 1
// }


// int free_list(activeCarList *aList) {
//     printf("Free List Called\n");

//     // NULL pointer can't be freed
//     if (aList == NULL){
// 		return 1;
//     }

//     printf("alist is not null\n");

//     // first free the dynamically allocated array
//     if (aList->elements != NULL)
//         free(aList->elements);

// 	printf("Now let's free alist\n");

//     // now free the pointer to the struct
//     free(aList);
//     return 0;
// }
