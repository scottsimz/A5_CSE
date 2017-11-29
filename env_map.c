#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
	void free_grid(int n1, int n2, cell **a);
	//print functions print to screen
	void print_elements(int n1, int n2, cell **a);
	void print_velocities(int n1, int n2, cell **a);
	void print_car_ids(int n1, int n2, cell **a);

	
//Grid Initialization Functions
	cell** init_grid(trafficLight* traffic_lights_list,int* max_cars);
	void fill_intersection(cell** grid, int midRow,int midCol, int* max_cars);
	void random_fill(cell** grid,double p, car* carlist);

//Active Car LENGTHist/Results Functions
	void grid_snapshot(cell** grid,double timeStamp,car* active_car_list,FILE *file);
	void save_results(FILE* f, cell** grid, double timeStamp);
	//fprint functions print to file
	void fprint_elements(int n1, int n2, cell **a,FILE* f);
	void fprint_velocities(int n1, int n2, cell **a,FILE* f);
	void fprint_car_ids(int n1, int n2, cell **a,FILE* f);

//Traffic Light Functions
	void update_lights(trafficLight* lights);

//Other Helper Functions
	double uniform(void);
	double urand(int LowerLimit,int UpperLimit);

//=======================================================================================================
//The main function
/***
int main(){

	//Set seed rand() for random grid
	srand(time(NULL));

	int L = 2; //length of a road segment
	int n_hor = 1; //number of horizontal blocks
	int n_vert = 1; //number of vertical blocks

	int gridHeight = NUM_LIGHTS_VERT * (2*LENGTH + 3); //no of rows
	int gridWidth = NUM_LIGHTS_HOR * (2*LENGTH + 3); //no of columns

	//reserve memory for the traffic lights list
	int num_trafficLights = NUM_LIGHTS_HOR*NUM_LIGHTS_VERT;
	trafficLight* traffic_lights_list = (trafficLight*)malloc(num_trafficLights*sizeof(trafficLight));

	//define max_cars
	int max_cars = 0;

	//Create the grid + update the traffic_lights_list
	cell** grid = init_grid(traffic_lights_list,&max_cars);

	printf("Max Cars: %d",max_cars);

	//display the grid on screen
	printf("\nPrinting the grid:\n");
	print_elements(grid);

	// //create an initial active car list
	int n_cars = 10;
	car* carlist = (car*)malloc(max_cars * sizeof(car));


	// Fill up the grid randomly with cars
	double p = 0.3; //prob a cell has a car
	random_fill(grid,p,carlist);

	//display the car IDs on screen
	printf("\nPrinting the grid of car ID's\n");
	print_car_ids(grid);

	//print the active car list
	printf("\nPrinting the active car list\n");
	for(int i = 0; i < max_cars; i++){
		printf("%d,",carlist[i].id);
	}
	printf("\n");	

	// Test out the traffic_lights_list
	printf("\nPrinting the positions of all traffic lights:\n");
	printf("  (id,x,y)\n");
	for(int i = 0; i < num_trafficLights; i++){
		// printf("traffic_lights_list[%d].id = %d\n",i,traffic_lights_list[i].id);	
		// printf("traffic_lights_list[%d].x = %d\n",i,traffic_lights_list[i].x);
		// printf("traffic_lights_list[%d].y = %d\n",i,traffic_lights_list[i].y);
		printf("  %d,%d,%d\n",traffic_lights_list[i].id,traffic_lights_list[i].x,traffic_lights_list[i].y);
		// printf("--\n");	
	}


	// Test the update_trafficLight() function
	trafficLight light = traffic_lights_list[0]; //Pick the 0th traffic light
	int del_t = n_red + n_yellow + n_green; //the period for a traffic light cycle

	//call update_trafficLight on a trafficLight at incremental timesteps
	printf("\nPrinting the North-South light for the %d-th traffic light at different timesteps:\n",0);
	printf("  TimeStep,NorthSouthLight\n");
	for(int i = 0; i < del_t*3; i++){ //run for 3 lightCycles
		update_trafficLight(&light,i,n_red,n_yellow,n_green);
		printf("  %d,%d\n",i,light.northSouthLight); //print out the timestep,color-of-the-north-traffic-light		
	}
***/

/*********************************************
 	 //display the velocities on screen
	printf("\nPrinting the grid of car velocities\n");
	print_velocities(grid);


	//Set up the snapshots results file
	FILE *resultsFile = fopen("snapshots.txt","w");
	if (resultsFile == NULL){printf("Error opening snapshots.txt file!\n");exit(1);};
	fprint_elements(grid,resultsFile); //write the grid to file

	//Get active_car_list and also save car pos and velocities to file
	car* active_car_list = (car*)malloc(gridWidth*gridHeight*sizeof(car)); //This needs to be global shared list

	//Uncomment to test the
	grid_snapshot(grid,GRID_HEIGHT,GRID_WIDTH,1,active_car_list,resultsFile); //The 2nd to last parameter is timestamp
	grid_snapshot(grid,GRID_HEIGHT,GRID_WIDTH,2,active_car_list,resultsFile); //Example of the function being called again at timestep=2
	grid_snapshot(grid,GRID_HEIGHT,GRID_WIDTH,3,active_car_list,resultsFile); //Example of the function being called again at timestep=3

	//End writing to the results file and close connection
	fprintf(resultsFile,"//END:SNAPSHOTS//");
	fclose(resultsFile);

	//free memory reserved for grid
	//Need to adjust this. The grid will be used by other programs so shouldn't be freed here.
	free_int_matrix(NUM_LIGHTS_HOR*NUM_LIGHTS_VERT,2,traffic_lights_list);
	free_grid(GRID_HEIGHT,GRID_WIDTH,grid);

}
***********************************************/

//=======================================================================================================

	//---------------------------------------------------------------------------------------------------
	//Update Traffic Lights
	void update_lights(trafficLight* lights){

		for(int i=0; i<NUM_LIGHTS; i++)
		{
			int n_red = lights[i].time_red;
			int n_yellow = lights[i].time_yellow;
			int n_green = lights[i].time_green;
			int period = n_red + n_yellow + n_green + n_yellow; //the period for a traffic light cycle
			int lightTimer = (lights[i].timer++); // (SCOTT)******************************

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
							printf("%c",a[i][j].element); //Doesn't add a comma for the last column
						} else {
							printf("%c",a[i][j].element); //Adds a comma after every element to separate them
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
				grid[row][col].element = EMPTY;
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
						grid[0][midCol -1].element = SPAWN_ENTRY; //Northbound Spawn
						grid[GRID_HEIGHT-1][midCol + 1].element = SPAWN_ENTRY; //Southbound Spawn
						grid[midRow+1][0].element = SPAWN_ENTRY; //Southbound Spawn
						grid[midRow-1][GRID_WIDTH-1].element = SPAWN_ENTRY; //Southbound Spawn

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
	//---------------------------------------------------------------------------------------------------
	// Fill up grid around an intersection
	void fill_intersection(cell** grid, int midRow,int midCol, int* max_cars){

		//Create the North-South Lanes
		for(int row = midRow - (LENGTH+1);row <= midRow + (LENGTH+1);row++){

				if (abs(row - midRow) == 2) //Juntion space needed for ghost cars
				{
					grid[row][midCol - 1].element = GHOST;
					grid[row][midCol + 1].element = GHOST;
					grid[row][midCol].element = MIDDLE_LANE;
					*max_cars += 2;
					// printf("Add roadcoutner\n");

				} else{
					grid[row][midCol - 1].element = SOUTH;
					grid[row][midCol + 1].element = NORTH;
					*max_cars += 2;
					grid[row][midCol].element = MIDDLE_LANE;
				}
				
		}
		//---------------------------------------------------------------------------------------------------
		//Create the East-West Lanes
		for(int col = midCol - (LENGTH+1);col <= midCol + (LENGTH+1);col++){
				if (abs(col - midCol) == 2) //Juntion space needed for ghost cars
				{
					grid[midRow - 1][col].element = GHOST;
					grid[midRow + 1][col].element = GHOST;
					grid[midRow][col].element = MIDDLE_LANE;
					*max_cars += 2;
				} else {
					grid[midRow - 1][col].element = WEST;
					grid[midRow + 1][col].element = EAST;
					grid[midRow][col].element = MIDDLE_LANE;
					*max_cars += 2;
				}
		}

		//fill up the junctions
		for(int row = midRow - 1; row <= midRow + 1; row++){
			for(int col = midCol -1; col <= midCol + 1; col++){
				grid[row][col].element = JUNCTION;
				*max_cars -= 1; //We will still have 2 extra cars in max_cars
			}
		}

		// *max_cars -= 2;

		//Add the traffic light at the middle
		grid[midRow][midCol].element = TRAFFIC_LIGHT;
		*max_cars -= 1;

	}		
	//---------------------------------------------------------------------------------------------------
	//randomly fill up the grid  with some cars
	void random_fill(cell** grid,double p, car* carlist){
		//randomly fills the grid with a few cars

		int car_index = 0;
		char element; //(SCOTT)*****************************
		//This function needs to create a random active car list

		for(int row = 0; row < GRID_HEIGHT; row++){
			for(int col = 0; col < GRID_WIDTH; col++){

				element = grid[row][col].element; // (SCOTT)*****************************

				if (element == NORTH |
					element == SOUTH |
					element == EAST |
					element == WEST){
					
					if(uniform() < p){

						//create an initial car element
						carlist[car_index].id = car_index; //this should now be a unique id
						carlist[car_index].x = col;
						carlist[car_index].y = row;
						carlist[car_index].speed = urand(1,5); //random velocity between 1 and 5
						carlist[car_index].direction = element; //(SCOTT) direction of car is same as road*****************************

						//Assign the car to a cell on the grid
						grid[row][col].car_id = car_index;
						
						//Increment counter (index)
						car_index ++;
					}
				} // (SCOTT) removed ELSE condition because all grid.car_id's were initialized to -1 in a previous function*****************************
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
						fprintf(f,"%c",a[i][j].element); //Doesn't add a comma for the last column
					} else {
						fprintf(f,"%c,",a[i][j].element); //Adds a comma after every element to separate them
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
		return rand() / (RAND_MAX + 1.0);
	}
	//---------------------------------------------------------------------------------------------------
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
// carlist* create_list(int size) {
//     // Allocate memory for the structure
//     carlist *W = malloc( sizeof (carlist) );
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

// int add_to_end(carlist* aList,car* element){
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


// int remove_first_car(carlist* aList){

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


// int free_list(carlist *aList) {
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
