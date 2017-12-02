/*
 * simulate.c
 *
 *  Created on: Dec 1, 2017
 *      Author: Scott Sims
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "traffic.h"			// Use shared definitions

#//define MAX_CELLS ( 2*( (NUM_LIGHTS_VERT * GRID_WIDTH) + (NUM_LIGHTS_HOR * GRID_HEIGHT) - (NUM_LIGHTS) ) )
//----------------------------------
// FUNCTION PROTOTYPES
spawner* init_spawners(cell** grid);
trafficLight* init_lights(int green,int yellow, int red, int shift);
int get_bounds( int y, int x);
int update_spawners(int max_cars,spawner* spawners,car* cars,cell** grid);
int get_timer( char dir );
void init_cars(int max_cars, car* cars);
int fprint_vel(int n1, int n2, int timer, car* cars, cell **a,FILE* f);
void fprint_density( int num_cars, int max_cars, int sim_time,FILE* f);
	//void spawn( struct spawner *ptr );
	//int get_max_cars(void);
//----------------------------------

// MAIN SIM-LOOP
int main(void)//(int argc, char *argv[])
{
	/**************************************************
	// check invalid number of arguments
	if (argc != 2)
	{
		printf("Error main() - Expected one argument, an integer for the simulation duration\n");
		return 1;
	}
	// store argument
	int sim_time = strtol(argv[1],NULL,10);
	**************************************************/
	// check invalid argument range
	if (SIM_TIME < 1 )
	{
		printf("Error main() - simulation time must be a positive integer, not %d\n",SIM_TIME);
		return 1;
	}

	int gridHeight = NUM_LIGHTS_VERT * (2*LENGTH + 3); //no of rows
	int gridWidth = NUM_LIGHTS_HOR * (2*LENGTH + 3); //no of columns


	// open file for writing
	FILE *output_vel = fopen( "results_velocities.txt","w");
	FILE *output_den = fopen( "results_density.txt","w");
	if( (output_vel == NULL) || (output_den == NULL) )
	{
		printf("Error - fopen(): files created for writing results are NULL\n");
		return 1;
	}
	printf("Files opened for writing\n");

	// declared variables
	int max_cars=0,error_check=0,timer=0;
	srand( 0 ); // seeds the rand() function

	//-------------------------------------------------------------
	// INITIALIZATIONS
	printf("INITIALIZATIONS:\n");
	// (1) Initialize Lights
	trafficLight* lights = init_lights(TIME_GREEN,TIME_YELLOW,TIME_RED,TIME_SHIFT); // arguments held in header (traffic.h)
	printf("(1) traffic lights\n");

	// (2) Initialize Map
	cell** grid = init_grid(lights,&max_cars);
	printf("(2) map (max_cars = %d)\n",max_cars);

	// (3) Initialize Cars stored in one array
	car cars[max_cars]; // declares array of cars
	init_cars(max_cars,cars); // initializes all cars in list to inactive
		error_check = fprint_vel(GRID_HEIGHT,GRID_WIDTH,timer,cars,grid,output_vel);
		if( error_check == 1 ) return 1;
	//random_fill(grid,DENSITY,cars); // initializes map with cars, modifying both the car-list and grid-matrix
	random_fill(grid,gridHeight,gridWidth,DENSITY,cars);
	printf("(3) cars\n");
		error_check = fprint_vel(GRID_HEIGHT,GRID_WIDTH,timer,cars,grid,output_vel);
		if( error_check == 1 ) return 1;

	// (4) Initialize Ghost Cars
	int light_row = LENGTH + 1;
	int light_col =	LENGTH + 1;

	// (5) Initialize Spawners
	spawner* spawners = init_spawners(grid);
	printf("(5) spawners\n");

	//-------------------------------------------------------------
	// SIM-LOOP

	printf("SIM-LOOP:\n");

		// print header for file 'output_den'
		fprintf(output_den,"Initial Density ( %f ) Max_Cars ( %d ) Length ( %d )\n",(float)DENSITY,max_cars,LENGTH);
		fprintf(output_den,"NS_Light: Green ( %d ) Yellow ( %d ) Red ( %d )\n",TIME_GREEN,TIME_YELLOW,TIME_RED);
		fprintf(output_den,"Spawn Ranges: N S E W\n");
		fprintf(output_den," %d %d \n",N_SPAWN_MIN, N_SPAWN_MAX);
		fprintf(output_den," %d %d \n",S_SPAWN_MIN, S_SPAWN_MAX);
		fprintf(output_den," %d %d \n",E_SPAWN_MIN, E_SPAWN_MAX);
		fprintf(output_den," %d %d \n",W_SPAWN_MIN, W_SPAWN_MAX);
		fprintf(output_den,"|   Time   | Density |\n");

	int x,y,i,num_cars;

	while(timer < SIM_TIME)
	{
		timer ++;
		printf("time = %d\n",timer);
	// (1) Update Lights
		error_check = update_lights(lights);
		if( error_check == 1 ) return 1;
		fprintf(output_vel,"NS_LIGHT(%d)\n",lights[0].northSouthLight);
		printf("(1) update lights\n");

	// (2) Update Ghost Cars
		ghost(grid, light_row, light_col, &lights[0]);

	// (3) Update Cars
			/*error_check = update_cars( max_cars, cars, grid);
			if( error_check == 1 ) return 1; */

		num_cars=0; // reset counter for active cars

		for(int i = 0; i < max_cars; i++){
			if(cars[i].id > -1){

				num_cars++; // count number of active cars
				int index = cars[i].id; // this is the ID of the car we're considering
				int x_pos = cars[i].x_old; //current x position of car
				int y_pos = cars[i].y_old; //current y position of car

				printf("-----------------------\n");
				printf("x is %d\n",x_pos);
				printf("y is %d\n",y_pos);
				printf("The Car ID is %d \n",index);

				char direction = cars[i].direction;
				//cars[i].map_elem;
				//printf("Direction is  %c\n ",direction);

				//CHECK SPACES
				int empty = emptycellcount(grid, gridWidth, gridHeight, x_pos, y_pos, direction);
				//printf("Returned emptyspaces count = %d\n", empty);

				//UPDATE VELOCITY
				//printf("VELOCITY OLD Before update %d\n",cars[i].v_old);
				//printf("Old Velocity = %d\n",cars[i].v_old);

				//int v = update_velocity(&cars[i], grid, empty);
				//printf("Returned velocity = %d\n",v);
				//printf("**** BEFORE UPDATE CAR FUNCTION ********\n");
				update_car(&cars[i], grid, empty, max_cars, i, cars);

			}//end if
		}
		fprint_density(num_cars,max_cars,timer,output_den);
		printf("(3) update cars\n");

	// (4) Update Map and copy cars for next iteration
		for(i=0; i<max_cars; i++)
		{
			if( cars[i].id > -1) // check if car is active on map
			{
				grid[cars[i].y_old][cars[i].x_old].car_id = -1; // remove old car from old map location

				y = cars[i].y_new ; // store new car positions
				x = cars[i].x_new ;

				if( get_bounds(y,x) == 1) // true=1 if car inside boundaries
				{
					grid[y][x].car_id = i ; // add index of car to new map location
					cars[i].x_old = cars[i].x_new;
					cars[i].y_old = cars[i].y_new;
					cars[i].v_old = cars[i].v_new;
				}
				else // if car outside boundaries (along edges of map)
				{
					cars[i].id = -1; // remove car from old array
				}

			}
		}
		printf("(4) update map\n");

	// (5) Update Spawners
		error_check = update_spawners( max_cars, spawners, cars, grid);
		if( error_check == 1 ) return 1;
		printf("(5) update spawners\n");

	// (6) Print Velocities of cars on map
		ghost(grid, light_row, light_col, &lights[0]);
		error_check = fprint_vel(GRID_HEIGHT,GRID_WIDTH,timer,cars,grid,output_vel);
		if( error_check == 1 ) return 1;

	}

	printf("SIMULATION COMPLETE!\n");
	//----------------------------------------------------------------
	//FREE ALLOCATED MEMORY
	free_grid(gridHeight,gridWidth,grid);
	free(lights);
	free(spawners);

	//----------------------------------------------------------------
	//EXIT
	fclose(output_vel);
	fclose(output_den);
	printf("Closed file traffic_results.txt\n");
	return 0;
}

//--------------------------------------------------------------------------------------
// Function call determines if location is inside edge boundaries of map.
// When a car is out of bounds, get_bounds() returns 0 and the car should be removed.
int get_bounds( int y, int x )
{
	// check inputs
	if(y<0){
		printf("Error get_bounds() - arg 'y' cannot be a negative integer\n");
	}
	if(x<0){
		printf("Error get_bounds() - arg 'x' cannot be a negative integer\n");
	}

	if( (y < S_EDGE) || (y > N_EDGE) ) return 0; // check top and bottom edges
	else if( (x < W_EDGE) || (x > E_EDGE) ) return 0; // check left and right edges
	else return 1; // return 1 if inside outer edges of map
}

//--------------------------------------------------------------------------------------
// Function call allocates memory for traffic lights which will be defined later in init_grid()
trafficLight* init_lights(int green,int yellow, int red, int shift)
{
	trafficLight* lights = (trafficLight*) malloc( NUM_LIGHTS*sizeof(trafficLight) ) ;

	for(int i=0; i<NUM_LIGHTS; i++)
	{
		lights[i].time_green = green;
		lights[i].time_yellow = yellow;
		lights[i].time_red = red;
		lights[i].timer = 0 + i*shift;
	}

	return lights;
}

//--------------------------------------------------------------------------------------
// Function call initializes values for all cars in list to default values
void init_cars(int max_cars, car* cars)
{
	if(cars == NULL)
	{
		printf("Error - init_cars(): arg 'cars[]' is NULL\n");
	}
	if(max_cars < 1)
	{
		printf("Error - init_cars(): arg 'max_cars' must be a positive integer\n");
	}

	car car_default;

	car_default.id = -1;
	car_default.v_old = car_default.v_new = V_MAX;
	car_default.x_old = car_default.x_new = 0;
	car_default.y_old = car_default.y_new = 0;
	car_default.direction = EMPTY;

	for(int i=0; i<max_cars; i++)
	{
		cars[i]=car_default;
	}

}

//--------------------------------------------------------------------------------------
// Function call counts-down the timer of each spawner. When a timer reaches zero,
// a new car is "spawned" at specific location moving in a specific direction determined
// by the spawn-point. The timer is reset to a randomly generated duration.

int update_spawners(int max_cars,spawner* spawners,car* cars,cell** grid)
{
	// check invalid inputs
	if( grid == NULL )
	{
		printf("Error - update_spawners(): arg 'grid[][]' is NULL\n");
		return 1;
	}
	if( cars == NULL )
	{
		printf("Error - update_spawners(): arg 'cars[]' is NULL\n");
		return 1;
	}
	if( spawners == NULL )
	{
		printf("Error - update_spawners(): arg 'spawners[][]' is NULL\n");
		return 1;
	}
	if( max_cars < 1 )
	{
		printf("Error - update_spawners(): arg 'max_cars' must be positive integer\n");
		return 1;
	}

	int x,y;

	for( int j=0; j<NUM_SPAWNERS; j++ )
	{
		spawners[j].timer --; // decrement spawn timer
		x = spawners[j].x ; // stores x-position of spawn point
		y = spawners[j].y ; // stores y-position of spawn point

		// checks if timer is zero AND if spawn-point is clear of cars
		if( (spawners[j].timer <= 0) && (grid[y][x].car_id == -1) )
		{
			for(int i=0; i<max_cars; i++) // search through index of car-array
			{
				if(cars[i].id == -1) // if index is inactive
				{
					cars[i].id = i; // current car-array index is activated
					cars[i].v_old = cars[i].v_new = V_MAX; // initial speed set to speed limit
					cars[i].x_old = cars[i].x_new = x; // spawned at location of spawn-point
					cars[i].y_old = cars[i].y_new = y;
					cars[i].direction = spawners[j].direction; // spawned in direction of spawn-point
					grid[y][x].car_id = i; // map is updated with car's location
					break; // exit for-loop
				}
			}
			// Resets timer if car is spawned
			spawners[j].timer = get_timer(spawners[j].direction) ;
		}
	}

	return 0; // no errors found
}

/*** LINKED-LIST VERSION OF UPDATE_SPAWNERS()
void spawn( struct spawner *ptr )
{
	// error check
	if (ptr == NULL) {
		printf("Error spawn() - list of spawners is empty\n");
		return;
	}

	// declarations
	struct car cars[MAX_CARS]; // external array from main()

	for( ; ptr!=NULL; ptr = ptr->next )
	{
		ptr->timer --; // decrement spawn timer
		int x = ptr->x ; // stores x-position of spawn point
		int y = ptr->y ; // stores y-position of spawn point

		// checks if timer is zero AND if spawn-point is clear of cars
		if( (ptr->timer <= 0) && (cell[y][x].car == 0) )
		{
			for(int i=0; i<MAX_CARS; i++) // search through index of car-array
			{
				if(cars[i].active == 0) // if index is inactive
				{
					cars[i].active = 1; // current car-array index is activated
					cars[i].speed = V_MAX; // initial speed set to speed limit
					cars[i].x = x; // spawned at location of spawn-point
					cars[i].y = y;
					cars[i].direction = ptr->direction; // spawned in direction of spawn-point
					cell[y][x].car = i; // map is updated with car's location
					break; // exit for-loop
				}
			}
			// Resets timer. Currently set to a random number from 1-10.
			ptr->timer = get_timer(ptr->direction) ;
		}
	}
}
***/

//--------------------------------------------------------------------------------------
// Initializes a linked-list of spawners and returns a head pointer for the linked-list.

spawner* init_spawners(cell** grid)
{
	// check invalid inputs
	if( grid == NULL )
	{
		printf("Error - init_spawners(): arg 'grid[][]' is NULL\n");
		return NULL;
	}

	// declare variables
	int x,y,i=0;
	spawner* spawners = (spawner*) malloc( NUM_SPAWNERS*sizeof(spawner) );

	// scan left edge of map
	x=0;
	for(y=0; y<GRID_HEIGHT; y++)
	{
		if(grid[y][x].map_elem == 'I')
		{
			spawners[i].x = W_EDGE ;
			spawners[i].y = y;
			spawners[i].direction = 'E';
			spawners[i].timer = get_timer('E');
			i++;
		}
	}

	// scan right edge of map
	x=GRID_WIDTH-1;
	for(y=0; y<GRID_HEIGHT; y++)
	{
		if(grid[y][x].map_elem == 'I')
		{
			spawners[i].x = E_EDGE ;
			spawners[i].y = y;
			spawners[i].direction = 'W';
			spawners[i].timer = get_timer('W');
			i++;
		}
	}

	// scan bottom edge of map
	y=0;
	for(x=0; x<GRID_WIDTH; x++)
	{
		if(grid[y][x].map_elem == 'I')
		{
			spawners[i].x = x ;
			spawners[i].y = S_EDGE;
			spawners[i].direction = 'N';
			spawners[i].timer = get_timer('N');
			i++;
		}
	}

	// scan top edge of map
	y=GRID_HEIGHT-1;
	for(x=0; x<GRID_WIDTH; x++)
	{
		if(grid[y][x].map_elem == 'I')
		{
			spawners[i].x = x ;
			spawners[i].y = N_EDGE;
			spawners[i].direction = 'S';
			spawners[i].timer = get_timer('S');
			i++;
		}
	}
	// return head of linked-list of spawners
	return spawners;
}

/*** LINKED-LIST VERSION OF INIT_SPAWNERS()
spawner* init_spawn(void)
{
	int x,y;
	struct spawner *ptr=NULL,*head=NULL;

	// scan left edge of map
	x=0;
	for(y=0; y<HEIGHT; y++)
	{
		if(cell[y][x].element == 'I')
		{
			ptr = (struct spawner*) malloc( sizeof(struct spawner));
			ptr->x = W_EDGE ;
			ptr->y = y;
			ptr->direction = 'E';
			ptr->timer = get_timer('E');
			ptr->next = head;
			head = ptr;
		}
	}

	// scan right edge of map
	x=WIDTH-1;
	for(y=0; y<HEIGHT; y++)
	{
		if(cell[y][x].element == 'I')
		{
			ptr = (struct spawner*) malloc( sizeof(struct spawner));
			ptr->x = E_EDGE ;
			ptr->y = y;
			ptr->direction = 'W';
			ptr->timer = get_timer('W');
			ptr->next = head;
			head = ptr;
		}
	}

	// scan bottom edge of map
	y=0;
	for(x=0; x<WIDTH; x++)
	{
		if(cell[y][x].element == 'I')
		{
			ptr = (struct spawner*) malloc( sizeof(struct spawner));
			ptr->x = x ;
			ptr->y = S_EDGE;
			ptr->direction = 'N';
			ptr->timer = get_timer('N');
			ptr->next = head;
			head = ptr;
		}
	}

	// scan top edge of map
	y=HEIGHT-1;
	for(x=0; x<WIDTH; x++)
	{
		if(cell[y][x].element == 'I')
		{
			ptr = malloc( sizeof(struct spawner));
			ptr->x = x ;
			ptr->y = N_EDGE;
			ptr->direction = 'S';
			ptr->timer = get_timer('S');
			ptr->next = head;
			head = ptr;
		}
	}
	// return head of linked-list of spawners
	return head;
}
***/

//--------------------------------------------------------------------------------------
// updates spawn timer based on direction of spawner
// range of spawn times set in header file 'traffic.h'
int get_timer( char dir )
{
	if( dir == 'N')
	{
		return( N_SPAWN_MIN + (N_SPAWN_MAX - N_SPAWN_MIN)*(uniform()) );
	}
	else if( dir == 'S')
	{
		return( S_SPAWN_MIN + (S_SPAWN_MAX - S_SPAWN_MIN)*(uniform()) );
	}
	else if( dir == 'W')
	{
		return( W_SPAWN_MIN + (W_SPAWN_MAX - W_SPAWN_MIN)*(uniform()) );
	}
	else if( dir == 'E')
	{
		return( E_SPAWN_MIN + (E_SPAWN_MAX - E_SPAWN_MIN)*(uniform()) );
	}
	else
	{
		printf("Error get_timer() - Invalid direction '%c'\n",dir);
		return 0;
	}
}

//--------------------------------------------------------------------------------------
// Function call returns an upper bound for the maximum number of cars that can fit on the roads.
// Evaluates perfect squares until greater than or equal to the number of intersections/lights.
/***
int get_max_cars(void)
{
	int i=0,sq=0;

	// loop continues until sq >= NUM_LIGHTS
	while( sq < NUM_LIGHTS )
	{
		i++;
		sq = i*i;
	}
	return ( (WIDTH+HEIGHT)*i*2 );
}
***/

//----------------------------------------------------------------------------------------
int fprint_vel(int n1, int n2,int time, car* cars, cell **a,FILE* f)
{

	// check for valid inputs
	if (n1 < 1 || n2 < 1)
	{
		printf("Error - fprint_vel(): rows and columns of matrix must be positive\n");
		return 1;
	}
	else if ( a == NULL)
	{
		printf("Error - fprint_vel(): arg 'a[][]' is NULL\n");
		return 1;
	}
	else if ( f == NULL)
	{
		printf("Error - fprint_vel(): arg 'file' is NULL\n");
		return 1;
	}

	// declare variables
	int i,j;

	// print loop
	fprintf(f,"TIME (%d)\n",time);
	for( i=0; i<n1; i++)
	{
		for(j=0; j<n2; j++)
		{
 			if( a[i][j].map_elem == EMPTY || a[i][j].map_elem == MIDDLE_LANE ) // checks if not-road
 			{
 				fprintf(f," %c",EMPTY); // print blank
 			}
 			else if( a[i][j].car_id == -1) // if road without car
 			{
 				fprintf(f," %c",a[i][j].map_elem); // print road element
 			}
 			else if( a[i][j].car_id == -2) // if road has ghost car
 			{
 				fprintf(f," %c",'&'); // print road element
 			}
 			else // if road with car
 			{
 				fprintf(f," %d",cars[a[i][j].car_id].v_old); // print ID*****
 			}
		}
		fprintf(f,"\n");
	}
	fprintf(f,"\n");

	return 0; // no errors found
}

void fprint_density( int num_cars, int max_cars, int sim_time,FILE* f)
{
	float density = (float)num_cars / (float)max_cars;

	fprintf(f,"%d	%f\n",sim_time,density);
}
