#include <stdio.h>
#include <stdlib.h>


#define L 3
#define n_hor 3
#define n_vert 3
#define EMPTY '0'
#define SOUTH 'S'
#define NORTH 'N'
#define WEST 'W'
#define EAST 'E'
#define MIDDLE_LANE 'M'
#define JUNCTION 'J'
#define TRAFFIC_LIGHT 'T'


//Matrix Prototypes
char **malloc_matrix(int n1, int n2);
void free_matrix(int n1, int n2, char **a);
void print_matrix(int n1, int n2, char **a);
void fill_intersection(char** grid,int midRow,int midCol);

int main(){

	int gridHeight = n_vert * (2*L + 3);
	int gridWidth = n_hor * (2*L + 3);


	printf("Grid Height = %d, Grid Width = %d\n",gridWidth,gridHeight);
	
	//Reserve space for full grid
	char** grid = malloc_matrix(gridHeight,gridWidth);

	if(grid == NULL){printf("Error mallocing grid!");exit(1);};

	//Intially fill up grid with EMPTYs
	for (int row = 0; row < gridHeight; row++){
		for(int col = 0; col < gridWidth; col++){
			grid[row][col] = EMPTY;
		}
	}

	
	//Add intersections

	for(int row_block = 0; row_block < gridHeight; row_block++){
		if(row_block % (2*L + 3) == 0){
			// grid[row_block][0] = 'H';
			for(int col_block = 0; col_block < gridWidth; col_block++){
				if(col_block % (2*L + 3) == 0){
					// grid[row_block][col_block] = 'H';
					
					int midRow = row_block + (L+1);
					int midCol = col_block + (L+1);

					fill_intersection(grid,midRow,midCol);
												
				}
			}			
		}	
	}	


	//display the grid
	print_matrix(gridHeight,gridWidth,grid);

	//free memory reserved for grid
	free_matrix(gridHeight,gridWidth,grid);

}
	


//Function Definitions


	//Fill up grid around an intersection
	void fill_intersection(char** grid,int midRow,int midCol){

		//Create the North-South Lanes
		for(int row = midRow - (L+1);row <= midRow + (L+1);row++){
				grid[row][midCol - 1] = NORTH;
				grid[row][midCol + 1] = SOUTH;
				grid[row][midCol] = MIDDLE_LANE;
		}

		//Create the East-West Lanes
		for(int col = midCol - (L+1);col <= midCol + (L+1);col++){
				grid[midRow - 1][col] = WEST;
				grid[midRow + 1][col] = EAST;
				grid[midRow][col] = MIDDLE_LANE;
		}

		//fill up the junctions
		for(int row = midRow - 1; row <= midRow + 1; row++){
			for(int col = midCol -1; col <= midCol + 1; col++){
				grid[row][col] = JUNCTION;
			}
		}

		//Add the traffic light at the middle
		grid[midRow][midCol] = TRAFFIC_LIGHT;
	}

	
	//Matrix Functions

	//Input: number of rows (n1) and columns (n2) required
	//Output: Reserve space for a matrix of size n1 x n2
	char **malloc_matrix(int n1, int n2) {

	    char **mat = NULL;       // pointer to the matrix

		if(n1>0 && n2 >0){ //checking for invalid inputs
		    mat = (char**)malloc(n1*sizeof(char *)); //Allocate memory for n1 rows

		    if(mat == NULL){
		    	printf("Error: Could not reserve memory for matrix.");
		    	exit(1);
		    }

			for(int i = 0; i < n1; i++){
				mat[i] = (char*)malloc(n2*sizeof(char)); //Allocate memory for n2 columns for each of the n1 rows

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
	void free_matrix(int n1, int n2, char **a) {

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
	void print_matrix(int n1, int n2, char **a) {

		if(n1 > 0 && n2 > 0 && a != NULL){ //Checking for invalid inputs

			printf("\n");
			for(int i=0;i<n1;i++){
				for(int j=0;j<n2;j++){
					if(j==n2-1){
						printf("%c",a[i][j]); //Doesn't add a comma for the last column
					} else {
						printf("%c,",a[i][j]); //Adds a comma after every element to separate them
					}

				}
				printf("\n");//Print the next row on a different line
			}
		} else {
			printf("Invalid input for print_matrix: Make sure n1 > 0 && n2 > 0");
			exit(1);
		}
	}
