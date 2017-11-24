#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#define EMPTY '0'
#define SOUTH 'S'
#define NORTH 'N'
#define WEST 'W'
#define EAST 'E'
#define MIDDLE_LANE 'M'
#define JUNCTION 'J'
#define TRAFFIC_LIGHT 'T'

//define a car struct
typedef struct car {
	int id; //Car ID
    int x;  //position (on range of [0; L])
    int y;  //position (on range of [0; L])
    int v;  //current velocity
} car;


//define a cell structure for grid elements
typedef struct cell {
	int map_elem; //for print outs
	struct car car;
} cell;


//Matrix Prototypes
cell **malloc_matrix(int n1, int n2);
void free_matrix(int n1, int n2, cell **a);
void print_map_elems(int n1, int n2, cell **a);
void print_velocities(int n1, int n2, cell **a);
void print_car_ids(int n1, int n2, cell **a);
cell** initialize_grid(int L, int n_hor, int n_vert);
void fill_intersection(cell** grid,int L, int midRow,int midCol);
void random_fill(cell** grid,int gridHeight,int gridWidth);
double uniform(void);
double urand(int LowerLimit,int UpperLimit);



int main(){

	//Seed rand()
	srand(time(NULL));

	int L = 3; //length of road segment
	int n_hor = 1; //number of horizontal blocks
	int n_vert = 1; //number of vertical blocks

	int gridHeight = n_vert * (2*L + 3);
	int gridWidth = n_hor * (2*L + 3);

	//Initialize the grid
	cell** grid = initialize_grid(L,gridHeight,gridWidth);

	//update the grid
	random_fill(grid,gridHeight,gridWidth);	

	//display the grid
	print_map_elems(gridHeight,gridWidth,grid);

	//display the velocities
	print_velocities(gridHeight,gridWidth,grid);

	//display the velocities
	print_car_ids(gridHeight,gridWidth,grid);

	//free memory reserved for grid
	//Need to adjust this. The grid will be used by other programs so shouldn't be freed here.
	free_matrix(gridHeight,gridWidth,grid);

}

void random_fill(cell** grid,int gridHeight,int gridWidth){
	//randomly fills the grid with a few cars

	double p = 0.3; //probability of a road cell having a car

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
					init_car.x = col;
					init_car.y = row;
					init_car.v = urand(1,5); //random velocity between 1 and 5

					grid[row][col].car = init_car;
				}

			}

		}
	}

}



cell** initialize_grid(int L, int gridHeight, int gridWidth){

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
	


//Function Definitions


	// //Fill up grid around an intersection
	void fill_intersection(cell** grid,int L, int midRow,int midCol){
	
		// char test = grid[1][2].map_elem;
		// printf("%c",test);

		//Create the North-South Lanes
		for(int row = midRow - (L+1);row <= midRow + (L+1);row++){
				grid[row][midCol - 1].map_elem = NORTH;
				grid[row][midCol + 1].map_elem = SOUTH;
				grid[row][midCol].map_elem = MIDDLE_LANE;
		}

		//Create the East-West Lanes
		for(int col = midCol - (L+1);col <= midCol + (L+1);col++){
				grid[midRow - 1][col].map_elem = WEST;
				grid[midRow + 1][col].map_elem = EAST;
				grid[midRow][col].map_elem = MIDDLE_LANE;
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
						printf("%c,",a[i][j].map_elem); //Adds a comma after every element to separate them
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
						printf("%d",a[i][j].car.v); //Doesn't add a comma for the last column
					} else {
						printf("%d,",a[i][j].car.v); //Adds a comma after every element to separate them
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
