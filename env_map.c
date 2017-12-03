#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "traffic.h"

/****************************
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
		int element;
		int car_id;
	} cell;
//define a traffic light structure
	typedef struct trafficLight {
		int id;
		int x;
		int y;
		int timer; //(SCOTT)
		int time_red; //(SCOTT)
		int time_green; //(SCOTT)
		int time_yellow; //(SCOTT)
		int northSouthLight; //the color of the North and South facing lights
		//eastWestLight = -1*northSouthLight
	} trafficLight;
**************************************/

//Matrix Prototypes
	cell **malloc_grid(int n1, int n2);
//	void free_grid(int n1, int n2, cell **a);

	//print functions print to screen
//	void print_elements(int n1, int n2, cell **a);
	void print_velocities(int n1, int n2, cell **a);
//	void print_car_ids(int n1, int n2, cell **a);

//Grid Initialization Functions
//	cell** init_grid(trafficLight* traffic_lights_list,int* max_cars);
	void fill_intersection(cell** grid, int midRow,int midCol, int* max_cars);
//	void random_fill(cell** grid,int gridHeight,int gridWidth,double p, car* activeCarList){

//Active Car LENGTHist/Results Functions
	void grid_snapshot(cell** grid,double timeStamp,car* active_car_list,FILE *file);
	void save_results(FILE* f, cell** grid, double timeStamp);
	//fprint functions print to file
	void fprint_elements(int n1, int n2, cell **a,FILE* f);
	void fprint_velocities(int n1, int n2, cell **a,FILE* f);
	void fprint_car_ids(int n1, int n2, cell **a,FILE* f);

//Traffic Light Functions
	int update_lights(trafficLight* lights);

//Other Helper Functions
	double uniform(void);
	double urand(int LowerLimit,int UpperLimit);


//=======================================================================================================

	//---------------------------------------------------------------------------------------------------
	//Update Traffic Lights
	int update_lights(trafficLight* lights){

		if(lights == NULL)
		{
			printf("Error - update_lights(): arg 'lights[]' is NULL\n");
			return 1;
		}

		int n_red,n_yellow,n_green,period,lightTimer;

		for(int i=0; i<NUM_LIGHTS; i++)
		{
			n_red = lights[i].time_red;
			n_yellow = lights[i].time_yellow;
			n_green = lights[i].time_green;
			period = n_red + n_yellow + n_green + n_yellow; //the period for a traffic light cycle
			lightTimer = (lights[i].timer++); // (SCOTT)******************************

			if ( (lightTimer > period) | (lightTimer < 0) ) lights[i].timer = lightTimer = 0; // (SCOTT) checks and resets timer ***************************

			if (lightTimer >= 0 && lightTimer <= n_red){ // we are at the start of the light cycle
				lights[i].northSouthLight = RED; //start NS light at RED
			} else if (lightTimer > n_red && lightTimer <= (n_red + n_yellow)){ //we are now in yellow region
				lights[i].northSouthLight = YELLOW;
			} else if (lightTimer > (n_red + n_yellow) && lightTimer <= (n_red + n_yellow + n_green)){ //we are now in green region
				lights[i].northSouthLight = GREEN;
			} else { //we are in the final yellow region of the light cycle
				lights[i].northSouthLight = YELLOW;
			}

		}
		return 0;
	}


//===========================================================================================================
	//Matrix Functions

		//---------------------------------------------------------------------------------------------------
		//Input: number of rows (n1) and columns (n2) required
		//Output: Reserve space for a matrix of size n1 x n2
		cell** malloc_grid(int n1, int n2) {

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

		//---------------------------------------------------------------------------------------------------
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
		void print_elements(int n1, int n2, cell **a) {

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
				printf("Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0\n");
				exit(1);
			}
		}
//=======================================================================================================
	//Grid Initialization Functions
	//---------------------------------------------------------------------------------------------------
	cell** init_grid(trafficLight* traffic_lights_list, int* max_cars){

		// printf("Grid Height = %d, Grid Width = %d\n",GRID_WIDTH,GRID_HEIGHT);

		// //Reserve space for full grid
		cell** grid = malloc_grid(GRID_HEIGHT,GRID_WIDTH);

		if(grid == NULL){printf("Error mallocing grid!");exit(1);};

		//Intially fill up grid with EMPTYs
		for (int row = 0; row < GRID_HEIGHT; row++){
			for(int col = 0; col < GRID_WIDTH; col++){
				grid[row][col].map_elem = EMPTY;
				grid[row][col].car_id = -1; //(SCOTT) initializes map with no cars*****************************
			}
		}

		// //Add intersections

		int intersectionCount = 0;

		for(int row_block = 0; row_block < GRID_HEIGHT; row_block++){
			if(row_block % (2*LENGTH + 3) == 0){
				// grid[row_block][0] = 'H';
				for(int col_block = 0; col_block < GRID_WIDTH; col_block++){
					if(col_block % (2*LENGTH + 3) == 0){
						// grid[row_block][col_block] = 'H';

						int midRow = row_block + (LENGTH+1);
						int midCol = col_block + (LENGTH+1);

						fill_intersection(grid,midRow,midCol,max_cars);

						//Add the spawn entry points
						grid[0][midCol -1].map_elem = SPAWN_ENTRY; //Northbound Spawn
						grid[GRID_HEIGHT-1][midCol + 1].map_elem = SPAWN_ENTRY; //Southbound Spawn
						grid[midRow+1][0].map_elem = SPAWN_ENTRY; //Southbound Spawn
						grid[midRow-1][GRID_WIDTH-1].map_elem = SPAWN_ENTRY; //Southbound Spawn

						//Add a traffic light to the traffic_lights_list
						traffic_lights_list[intersectionCount].id = intersectionCount;
						traffic_lights_list[intersectionCount].x = midRow;  //x-coordinate
						traffic_lights_list[intersectionCount].y = midCol; //y-coordinate
						traffic_lights_list[intersectionCount].northSouthLight = RED; //y-coordinate

						intersectionCount++;

					}
				}
			}
		}

		// // //Return the grid matrix as output
		return grid;

	}
	//---------------------------------------------------------------------------------------------------
	// Fill up grid around an intersection
	void fill_intersection(cell** grid, int midRow,int midCol, int* max_cars){

		//Create the North-South Lanes
		for(int row = midRow - (LENGTH+1);row <= midRow + (LENGTH+1);row++){

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
		//---------------------------------------------------------------------------------------------------
		//Create the East-West Lanes
		for(int col = midCol - (LENGTH+1);col <= midCol + (LENGTH+1);col++){
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
	//---------------------------------------------------------------------------------------------------
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
						(grid[row][col].map_elem == WEST)) {
					//	(grid[row][col].map_elem == GHOST)){

						if(uniform() < p){

							//create an initial car element
							car init_car;
						//	init_car.map_elem =;

							init_car.direction = grid[row][col].map_elem;
							init_car.id = activeCarListCounter; //this should now be a unique id
							init_car.x_old = col;
							init_car.y_old = row;
							init_car.x_new = col;
							init_car.y_new = row;
							init_car.v_old = urand(1,5); //random velocity between 1 and 5
							init_car.v_new = init_car.v_old;
							init_car.stop_time = 0;

							//add init car to active car list
							activeCarList[activeCarListCounter] = init_car;

							//Assign the car to a cell on the grid
							grid[row][col].car_id = init_car.id;

							//Increment counter
							activeCarListCounter ++;

						}
						/* else {
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
						}*/

					}

				}
			}

		}
	//---------------------------------------------------------------------------------------------------
	//Grid Update/Results Functions


	// //Take a snapshot of the grid
	// //print out the car id or velocities
	// //output an array of active cars
	// //This array will be sent to car_calculations() and
	// //later to update_grid()
	// void grid_snapshot(cell** grid,int GRID_HEIGHT,int GRID_WIDTH,double timeStamp,car* active_car_list,FILE *file){

	// 	int active_car_counter = 0; //keep track of how many cars are active

	// 	for(int row = 0; row < GRID_HEIGHT; row++){
	// 		for(int col = 0; col < GRID_WIDTH; col++){
	// 			if (grid[row][col].element == NORTH |
	// 				grid[row][col].element == SOUTH |
	// 				grid[row][col].element == EAST |
	// 				grid[row][col].element == WEST){

	// 					if(grid[row][col].car.id != 0){
	// 						active_car_list[active_car_counter] = grid[row][col].car;
	// 						active_car_counter++;
	// 					}

	// 			}

	// 		}
	// 	}

	// 	// printf("\nNumber of active cars: %d\n",active_car_counter);

	// 	//create an output file that stores the snapshot
	// 	save_results(file,grid,GRID_HEIGHT,GRID_WIDTH,timeStamp);

	// 	// return active_car_list; //this will have information old_car information
	// }

	// //Save results to file
	// void save_results(FILE* f, cell** grid, int GRID_HEIGHT, int GRID_WIDTH, double timeStamp){

	// 	fprintf(f,"//BEGIN:SNAPSHOTS//\n");
	// 	fprintf(f,"//TIMESTAMP=%f//\n",timeStamp);
	// 	fprintf(f,"//BEGIN:SNAPSHOTS//\n");
	// 	fprintf(f,"//BEGIN:CAR_IDS//\n");
	// 	fprint_car_ids(GRID_HEIGHT,GRID_WIDTH,grid,f);
	// 	fprintf(f,"//END:CAR_IDS//\n");
	// 	fprintf(f,"//BEGIN:CAR_VELOCITIES//\n");
	// 	fprint_velocities(GRID_HEIGHT,GRID_WIDTH,grid,f);
	// 	fprintf(f,"//END:CAR_VELOCITIES//\n");
	// }

	//---------------------------------------------------------------------------------------------------
	//print elements to file
	void fprint_elements(int n1, int n2, cell **a,FILE* f) {

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
	//---------------------------------------------------------------------------------------------------
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

//=======================================================================================================
	//---------------------------------------------------------------------------------------------------
	//rand uniform dist from engine
	double uniform(void)
	{
		return ( (double)rand() / (double)RAND_MAX );  //result is between [0,1) because rand() generates a number between [0,RAND_MAX)
	}
	//---------------------------------------------------------------------------------------------------
	//Generate a random number uniformly distributed over an interval [LowerLimit,UpperLimit)
	double urand(int LowerLimit,int UpperLimit){

		double ratio = (double)rand() / (double)RAND_MAX;  //result is between [0,1) because rand() generates a number between [0,RAND_MAX)
		double interval = UpperLimit - LowerLimit;
		double unirand = (ratio * interval);
		return unirand;
	}


void car_mvmt_plot(cell** grid, char direction, int index, int gridWidth, int gridHeight,car* cars, FILE* outputFile) {

	int carID;

	if (direction == NORTH | 
		direction == SOUTH){

		for (int row = 0; row < gridHeight; row++){
			// fprintf(outputFile,"row: %d\n",row);
			carID = grid[row][index].car_id;

			if( carID < 0){
				fprintf(outputFile,"%d",-1);
				if(row != gridHeight -1){
					fprintf(outputFile,",");	
				}						
			} else {
				// fprintf(outputFile,"|%d|",carID);
				fprintf(outputFile,"%d",cars[carID].v_new);	
				if(row != gridHeight -1){
					fprintf(outputFile,",");	
				}
			}
		}	
		fprintf(outputFile,"\n");
		
	} else if (direction == EAST|
		direction == WEST){

		for(int col = 0; col < gridWidth; col++){
			// fprintf(outputFile,"col: %d\n",col);
			carID = grid[index][col].car_id;

			if( carID < 0){
				fprintf(outputFile,"%d",-1);
				if(col != gridWidth  -1){
					fprintf(outputFile,",");	
				}						
			} else {
				fprintf(outputFile,"%d",cars[carID].v_new);
				if(col != gridWidth -1){
					fprintf(outputFile,",");	
				}				
			}
		}
		fprintf(outputFile,"\n");
	}
}