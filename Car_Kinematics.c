#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "traffic.h"



	int n_red = 5;
	int n_yellow = 3;
	int n_green = 10;

//	int vmax = 5;
//	double density = 0.4;
//	int num_samples = 200;
//	int car_count = 0;
//	int numfront = 0;
//	double p_rand = 0.4;

////for the env_map
//
//	#define EMPTY ' '
//	#define SOUTH 'S'
//	#define NORTH 'N'
//	#define WEST 'W'
//	#define EAST 'E'
//	#define MIDDLE_LANE 'M'
//	#define JUNCTION 'J'
//	#define TRAFFIC_LIGHT 'T'
//	#define GHOST 'G'
//	#define SPAWN_ENTRY 'I'
//
////for the trafficLights
//	#define GREEN 1
//	#define YELLOW 0
//	#define RED -1

//define the number of timesteps for which each traffic light color stays
//	int n_red = 5;
//	int n_yellow = 3;
//	int n_green = 10;

	int vmax = 5;
//	int density = 0.4;
	int num_samples = 200;
//	int car_count = 0;
	int numfront = 0;
//	double p_rand = 0.2;

//
////define a car struct
//	typedef struct car {
//		char direction; //Car direction
//		int id; //Car ID
//	    int x_old;  //old x position
//	    int y_old;  //old y position
//	    int v_old;  //old velocity
//	    int x_new;  //new x position
//	    int y_new;  //new y position
//	    int v_new;  //new velocity
//	} car;
//
//
////define a cell structure for grid elements
//	typedef struct cell {
//		int map_elem;
//		int car_id;
//	} cell;
//
////define a traffic light structure
//	typedef struct trafficLight{
//		int id;
//		int x;
//		int y;
//		int timer; //(SCOTT)
//		int time_red; //(SCOTT)
//		int time_green; //(SCOTT)
//		int time_yellow; //(SCOTT)
//		int northSouthLight; //the color of the North and South facing lights
//		//eastWestLight = -1*northSouthLight
//	} trafficLight;


////Matrix Prototypes
//	cell **malloc_grid(int n1, int n2);
//	void free_grid(int n1, int n2, cell **a);
//	//print functions print to screen
//	void print_map_elems(int n1, int n2, cell **a);
//	void print_velocities(int n1, int n2, cell **a);
//	void print_car_ids(int n1, int n2, cell **a);
//
//
////Grid Initialization Functions
//	cell** create_grid(int L, int gridHeight, int gridWidth, trafficLight* traffic_lights_list,int* max_cars);
//	void fill_intersection(cell** grid,int L, int midRow,int midCol, int* max_cars);
//	void random_fill(cell** grid,int gridHeight,int gridWidth,double p, car* activeCarList);
//
////Active Car List/Results Functions
//	void grid_snapshot(cell** grid,int gridHeight,int gridWidth,double timeStamp,car* active_car_list,FILE *file);
//	void save_results(FILE* f, cell** grid, int gridHeight, int gridWidth, double timeStamp);
//	//fprint functions print to file
//	void fprint_map_elems(int n1, int n2, cell **a,FILE* f);
//	void fprint_velocities(int n1, int n2, cell **a,FILE* f);
//	void fprint_car_ids(int n1, int n2, cell **a,FILE* f);
//
////Traffic Light Functions
//	void update_trafficLight(trafficLight* light, int timestep, int n_red, int n_yellow, int n_green);
//
////Other Helper Functions
//	double uniform(void);
//	double urand(int LowerLimit,int UpperLimit);

//	int emptycellcount(cell **grid, int gridWidth, int gridHeight, int i, int j, char direction);
//	int update_velocity(struct car *c, cell** grid, int empty);
//	car* update_car(struct car *c, cell** grid, int empty, int ncars, int i, struct car *activeCarList);

/**********************************
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
	cell** grid = init_grid(traffic_lights_list, &max_cars);
	printf("Max Cars: %d",max_cars);
	//display the grid on screen
	printf("\nPrinting the grid:\n");
	print_elements(gridHeight,gridWidth,grid);
	// //create an initial active car list
	int n_cars = 10;
	car* activeCarList = (car*)malloc(max_cars * sizeof(car));
	// Fill up the grid randomly with cars
	double p_fill = 0.3; //prob a cell has a car
	random_fill(grid,gridHeight,gridWidth,p_fill,activeCarList);
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
			printf("The Car ID is %d \n",index);
			char direction = activeCarList[i].direction;
			//activeCarList[i].direction;
			printf("Direction is  %c\n ",direction);
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
***********************************/

//----------------------FELICE--------------------------------------------//

//--------------------------------------------------------------//
//FIND OUT HOW MANY SPACES ARE IN FRONT
int emptycellcount(cell **grid, int gridWidth, int gridHeight, int i, int j, char direction){

	if(gridWidth <= 0 || gridHeight <= 0 || grid == NULL || i < 0 || j < 0) {
		printf("Wrong Input");
		return 1;
	}

	int emptyspaces=0;

	if(direction == EAST){
		int x = j;
		for(int y = i+1; y < gridWidth; y++){
			if(grid[x][y].car_id == -1){
				emptyspaces++;
			}
			else{
				break;
			}
		}
	}

	else if(direction == WEST){
		int x = j;
		for(int y = i-1; y>-1; y--){
			if(grid[x][y].car_id == -1){
				emptyspaces++;
			}
			else{
				break;
			}
		}
	}

	else if(direction == NORTH){
		int y = i;
		for(int x=j-1; x>-1; x--){
			if(grid[x][y].car_id == -1){
				emptyspaces++;

			}
			else{
				break;
			}
		}
	}

	else if(direction == SOUTH){
		int y = i;
		for(int x = j+1; x<gridHeight; x++){
			if(grid[x][y].car_id == -1){
				emptyspaces++;
//				printf("\n|| The next , y location is (%d,%d) ", x,y);
//				printf("# emptyspaces %d && ", emptyspaces);
//				printf("Car ID is %d\n ",grid[x][y].car_id);
			}
			else{
				break;
			}
		}
	}
	return emptyspaces;
}

//int dist_intersect(cell **grid, int gridWidth, int gridHeight, int i, int j, char direction){
//
//	int dist_j = 0;
//
//	if(direction == EAST){
//
//	}
//
//}


//UPDATE VELOCITIES
int update_velocity(struct car *c, cell** grid, int empty){

	//Generate randum number
	double rn = (double)rand() / (double)RAND_MAX;

	//Create a temporary integer for the current velocity
	int v_temp = c->v_new;
	c->v_old = v_temp;

	//If the current velocity is less than v max, increment
	if(v_temp < vmax){
		v_temp ++;
	}

	//Avoid collisions
	if(empty < v_temp){
		v_temp = empty;
	}

	//printf("V temp is --- %d\n",v_temp);
	//Randomization step. If random number less than prob, decrement vtemp
	if( (rn < PROB_SLOW) && (v_temp > 0) ){
		//printf("YES RANDOMIZATION\n");
		v_temp --;
	}
	//The new velocity is v_temp
	c->v_new = v_temp;
	return v_temp;
}



car* update_car(struct car *c, cell** grid, int empty, int ncars, int i, struct car *activeCarList){
	//for(int i = 0; i < ncars; i++){

		//CHANGE: GET DIRECTION FROM CAR STRUCT INSTEAD OF GRID
		char direction = activeCarList[i].direction;

//		if(northSouthLight == RED || northSouthLight == YELLOW){
//			ghost(grid, midRow, midCol, direction, numcars);
//		}
//
//		else(){
//
//		}


		//if this is an east road and there is a car in a cell
		if(direction == EAST){// && (grid[x][y].car_id != -1)){
			//Calculate number of cells in front of car
	//		numfront = emptycellcount(c,grid,direction);

			int v = update_velocity(&activeCarList[i], grid, empty);

	//		printf("V in update_car= %d\n",v);

			//UPDATE X COORDINATES
			int x_current = c->x_new;
			c->x_old = x_current;
			c->x_new = x_current + v;

			//Y STAYS THE SAME IN WEST-BOUND TRAFFIC
			c->y_new = c->y_old;

//			printf("y old %d , y new %d\n", c->y_old, c->y_new);
//			printf("x old %d , x new %d\n", c->x_old, c->x_new);

			//Old spot has car ID of -1 (no car)
			//grid[c->x_old][c->y_old].car_id = -1;

		}//end if east


		else if(direction == WEST){// && (grid[x][y].car_id != -1)){

			//Calculate number of cells in front of car
//			numfront = emptycellcount(c,grid,direction);

			int v = update_velocity(&activeCarList[i], grid, empty);

		//	printf("new velocity in function %d\n",v);

			//UPDATE X COORDINATES
			int x_current = c->x_new;
			c->x_old = x_current;
			c->x_new = x_current - v;

			//Y STAYS THE SAME IN WEST-BOUND TRAFFIC
			c->y_new = c->y_old;

			//printf("y old %d , y new %d\n", c->y_old, c->y_new);
			//printf("x old %d , x new %d\n", c->x_old, c->x_new);

			//Old spot has car ID of -1 (no car)
			//grid[c->x_old][c->y_old].car_id = -1;

		}//end if west

		else if(direction == NORTH){// && (grid[x][y].car_id != -1)){
			//printf("IN IF NORTH\n");
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
			//grid[c->x_old][c->y_old].car_id = -1;

		}//end if north


		else if(direction == SOUTH){// && (grid[x][y].car_id != -1)){
			//printf("In if south\n");

			//Calculate number of cells in front of car
//			numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			int v = update_velocity(&activeCarList[i], grid, empty);

			//printf("new velocity in function %d\n",v);

			//UPDATE Y COORDINATES
			int y_current = c->y_new;
			c->y_old = y_current;
			c->y_new = y_current + v;

			//X STAYS THE SAME IN SOUTH-BOUND TRAFFIC
			c->x_new = c->x_old;

			//printf("y old %d , y new %d\n", c->y_old, c->y_new);
			//printf("x old %d , x new %d\n", c->x_old, c->x_new);

			//Old spot has car ID of 0 (no car)
			//grid[c->x_old][c->y_old].car_id = -1;

		}//end if south

		return c;

}//end update car south function





//GENERATE GHOST CAR FOR RED LIGHTS
void ghost(cell ** grid, int midRow, int midCol, trafficLight *color){

	int light = color->northSouthLight;

	//EAST
	int x_loc_E = midRow + 1;
	int y_loc_E = midCol - 2;

	//WEST
	int x_loc_W = midRow - 1;
	int y_loc_W = midCol + 2;

	//NORTH
	int x_loc_N = midRow + 2;
	int y_loc_N = midCol + 1;

	//SOUTH
	int x_loc_S = midRow - 2;
	int y_loc_S = midCol - 1;


	//If ns yellow or red
	if(light == YELLOW){
		grid[x_loc_E][y_loc_E].car_id = -2;
		grid[x_loc_W][y_loc_W].car_id = -2;

		grid[x_loc_N][y_loc_N].car_id = -2;
		grid[x_loc_S][y_loc_S].car_id = -2;
	}
	else if(light == RED){
		grid[x_loc_N][y_loc_N].car_id = -2;
		grid[x_loc_S][y_loc_S].car_id = -2;

		grid[x_loc_E][y_loc_E].car_id = -1;
		grid[x_loc_W][y_loc_W].car_id = -1;
	}
	else if(light == GREEN){
		grid[x_loc_E][y_loc_E].car_id = -2;
		grid[x_loc_W][y_loc_W].car_id = -2;

		grid[x_loc_N][y_loc_N].car_id = -1;
		grid[x_loc_S][y_loc_S].car_id = -1;
	}
	else{
		//printf("Error - ghost(): invalid color of light");
	}

	//return id;
}
