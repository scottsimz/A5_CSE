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
		int map_elem; 
		struct car car;
	} cell;


//Matrix Prototypes
	cell **malloc_matrix(int n1, int n2);
	void free_matrix(int n1, int n2, cell **a);
	//print functions print to screen
	void print_map_elems(int n1, int n2, cell **a);
	void print_velocities(int n1, int n2, cell **a);
	void print_car_ids(int n1, int n2, cell **a);

	
//Grid Initialization Functions
	cell** create_grid(int L, int gridHeight, int gridWidth);
	void fill_intersection(cell** grid,int L, int midRow,int midCol);
	void random_fill(cell** grid,int gridHeight,int gridWidth,double p);

//Active Car List/Results Functions
	void grid_snapshot(cell** grid,int gridHeight,int gridWidth,double timeStamp,car* active_car_list,FILE *file);
	void save_results(FILE* f, cell** grid, int gridHeight, int gridWidth, double timeStamp);
	//fprint functions print to file
	void fprint_map_elems(int n1, int n2, cell **a,FILE* f);
	void fprint_velocities(int n1, int n2, cell **a,FILE* f);
	void fprint_car_ids(int n1, int n2, cell **a,FILE* f);

//Other Helper Functions
	double uniform(void);
	double urand(int LowerLimit,int UpperLimit);


//The main function
int main(){

	//Set seed rand() for random grid
	srand(time(NULL));

	int L = 10; //length of a road segment
	int n_hor = 3; //number of horizontal blocks
	int n_vert = 3; //number of vertical blocks

	int gridHeight = n_vert * (2*L + 3); //no of rows
	int gridWidth = n_hor * (2*L + 3); //no of columns

	//Create the grid
	cell** grid = create_grid(L,gridHeight,gridWidth);

	//Fill up the grid randomly with cars
	double p = 0.3; //prob a cell has a car
	random_fill(grid,gridHeight,gridWidth,p);	

	//display the grid on screen
	print_map_elems(gridHeight,gridWidth,grid);

	// //display the car IDs on screen
	// print_car_ids(gridHeight,gridWidth,grid);

	// //display the velocities on screen
	// print_velocities(gridHeight,gridWidth,grid);


	//Set up the snapshots results file
	FILE *resultsFile = fopen("snapshots.txt","w");
	if (resultsFile == NULL){printf("Error opening snapshots.txt file!\n");exit(1);};
	fprint_map_elems(gridHeight,gridWidth,grid,resultsFile); //write the grid to file

	//Get active_car_list and also save car pos and velocities to file
	car* active_car_list = (car*)malloc(gridWidth*gridHeight*sizeof(car)); //This needs to be global shared list

	grid_snapshot(grid,gridHeight,gridWidth,1,active_car_list,resultsFile); //The 2nd to last parameter is timestamp
	grid_snapshot(grid,gridHeight,gridWidth,2,active_car_list,resultsFile); //Example of the function being called again at timestep=2
	grid_snapshot(grid,gridHeight,gridWidth,3,active_car_list,resultsFile); //Example of the function being called again at timestep=3

	//End writing to the results file and close connection
	fprintf(resultsFile,"//END:SNAPSHOTS//");
	fclose(resultsFile);

	//free memory reserved for grid
	//Need to adjust this. The grid will be used by other programs so shouldn't be freed here.
	free_matrix(gridHeight,gridWidth,grid);


}

//Function Definitions

	//Matrix Functions

		//Input: number of rows (n1) and columns (n2) required
		//Output: Reserve space for a matrix of size n1 x n2
		cell **malloc_matrix(int n1, int n2) {

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
				printf("Invalid input for malloc_matrix: Make sure n1 > 0 && n2 > 0");
				exit(1);
			}

		    return mat;
		}

		//free the memory reserved for a matrix
		//input: size of matrix n1 and n2, and the pointer to the matrix
		void free_matrix(int n1, int n2, cell **a) {

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

		//print velocities of cars on grid
		//input size of matrix (n1 and n2), and pointer to the matrix
		void print_velocities(int n1, int n2, cell **a) {

			if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

				printf("\n");
				for(int i=0;i<n1;i++){
					for(int j=0;j<n2;j++){
						if(j==n2-1){
							printf("%d",a[i][j].car.v_old); //Doesn't add a comma for the last column
						} else {
							printf("%d,",a[i][j].car.v_old); //Adds a comma after every element to separate them
						}

					}
					printf("\n");//Print the next row on a different line
				}
			} else {
				printf("Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
				exit(1);
			}
		}

		//print ids of cars on grid
		//input size of matrix (n1 and n2), and pointer to the matrix
		void print_car_ids(int n1, int n2, cell **a) {

			if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

				printf("\n");
				for(int i=0;i<n1;i++){
					for(int j=0;j<n2;j++){
						if(j==n2-1){
							printf("%d",a[i][j].car.id); //Doesn't add a comma for the last column
						} else {
							printf("%d,",a[i][j].car.id); //Adds a comma after every element to separate them
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

	cell** create_grid(int L, int gridHeight, int gridWidth){

		printf("Grid Height = %d, Grid Width = %d\n",gridWidth,gridHeight);
		
		// //Reserve space for full grid
		cell** grid = malloc_matrix(gridHeight,gridWidth);

		if(grid == NULL){printf("Error mallocing grid!");exit(1);};

		//Intially fill up grid with EMPTYs
		for (int row = 0; row < gridHeight; row++){
			for(int col = 0; col < gridWidth; col++){
				grid[row][col].map_elem = EMPTY;
			}
		}

		
		// //Add intersections

		for(int row_block = 0; row_block < gridHeight; row_block++){
			if(row_block % (2*L + 3) == 0){
				// grid[row_block][0] = 'H';
				for(int col_block = 0; col_block < gridWidth; col_block++){
					if(col_block % (2*L + 3) == 0){
						// grid[row_block][col_block] = 'H';
						
						int midRow = row_block + (L+1);
						int midCol = col_block + (L+1);

						fill_intersection(grid,L,midRow,midCol);
													
					}
				}			
			}	
		}	

		// // //Return the grid matrix as output
		return grid;

	}

	// //Fill up grid around an intersection
	void fill_intersection(cell** grid,int L, int midRow,int midCol){

		//Create the North-South Lanes
		for(int row = midRow - (L+1);row <= midRow + (L+1);row++){

				if (abs(row - midRow) == 2) //Juntion space needed for ghost cars
				{
					grid[row][midCol - 1].map_elem = GHOST;
					grid[row][midCol + 1].map_elem = GHOST;
					grid[row][midCol].map_elem = MIDDLE_LANE;	

				} else{
					grid[row][midCol - 1].map_elem = NORTH;
					grid[row][midCol + 1].map_elem = SOUTH;
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
				} else {
					grid[midRow - 1][col].map_elem = WEST;
					grid[midRow + 1][col].map_elem = EAST;
					grid[midRow][col].map_elem = MIDDLE_LANE;
				}
		}

		//fill up the junctions
		for(int row = midRow - 1; row <= midRow + 1; row++){
			for(int col = midCol -1; col <= midCol + 1; col++){
				grid[row][col].map_elem = JUNCTION;
			}
		}

		//Add the traffic light at the middle
		grid[midRow][midCol].map_elem = TRAFFIC_LIGHT;
	}		

	//randomly fill up the grid  with some cars
	void random_fill(cell** grid,int gridHeight,int gridWidth,double p){
		//randomly fills the grid with a few cars

		for(int row = 0; row < gridHeight; row++){
			for(int col = 0; col < gridWidth; col++){
				if (grid[row][col].map_elem == NORTH |
					grid[row][col].map_elem == SOUTH |
					grid[row][col].map_elem == EAST |
					grid[row][col].map_elem == WEST){
					
					if(uniform() < p){
						//add an initial car element
						car init_car;
						init_car.id = 1; //this will have to be a unique id later
						init_car.x_old = col;
						init_car.y_old = row;
						init_car.v_old = urand(1,5); //random velocity between 1 and 5

						grid[row][col].car = init_car;
					}
					else {
						car empty_car;
						empty_car.id = 0; //this will have to be a unique id later
					}

				}

			}
		}

	}	

	//Grid Update/Results Functions


	//Take a snapshot of the grid
	//print out the car id or velocities
	//output an array of active cars
	//This array will be sent to car_calculations() and
	//later to update_grid()
	void grid_snapshot(cell** grid,int gridHeight,int gridWidth,double timeStamp,car* active_car_list,FILE *file){

		int active_car_counter = 0; //keep track of how many cars are active

		for(int row = 0; row < gridHeight; row++){
			for(int col = 0; col < gridWidth; col++){
				if (grid[row][col].map_elem == NORTH |
					grid[row][col].map_elem == SOUTH |
					grid[row][col].map_elem == EAST |
					grid[row][col].map_elem == WEST){

						if(grid[row][col].car.id != 0){
							active_car_list[active_car_counter] = grid[row][col].car;
							active_car_counter++;
						}

				}			

			}
		}

		printf("\nNumber of active cars: %d\n",active_car_counter);	

		//create an output file that stores the snapshot
		save_results(file,grid,gridHeight,gridWidth,timeStamp);

		// return active_car_list; //this will have information old_car information
	}

	//Save results to file
	void save_results(FILE* f, cell** grid, int gridHeight, int gridWidth, double timeStamp){

		fprintf(f,"//BEGIN:SNAPSHOTS//\n");
		fprintf(f,"//TIMESTAMP=%f//\n",timeStamp);
		fprintf(f,"//BEGIN:SNAPSHOTS//\n");
		fprintf(f,"//BEGIN:CAR_IDS//\n");
		fprint_car_ids(gridHeight,gridWidth,grid,f);
		fprintf(f,"//END:CAR_IDS//\n");
		fprintf(f,"//BEGIN:CAR_VELOCITIES//\n");
		fprint_velocities(gridHeight,gridWidth,grid,f);
		fprintf(f,"//END:CAR_VELOCITIES//\n");
	}


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
						fprintf(f,"%d",a[i][j].car.id); //Doesn't add a comma for the last column
					} else {
						fprintf(f,"%d,",a[i][j].car.id); //Adds a comma after every element to separate them
					}

				}
				fprintf(f,"\n");//Print the next row on a different line
			}
		} else {
			fprintf(f,"Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
			exit(1);
		}
	}

	//print velocities of cars on grid
	//input size of matrix (n1 and n2), and pointer to the matrix
	void fprint_velocities(int n1, int n2, cell **a,FILE* f) {

		if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

			for(int i=0;i<n1;i++){
				for(int j=0;j<n2;j++){
					if(j==n2-1){
						fprintf(f,"%d",a[i][j].car.v_old); //Doesn't add a comma for the last column
					} else {
						fprintf(f,"%d,",a[i][j].car.v_old); //Adds a comma after every element to separate them
					}

				}
				fprintf(f,"\n");//Print the next row on a different line
			}
		} else {
			fprintf(f,"Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
			exit(1);
		}
	}
	




	

	

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