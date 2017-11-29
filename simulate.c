#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "traffic.h"			// Use shared definitions
//----------------------------------
// FUNCTION PROTOTYPES
spawner* init_spawners(cell** grid);
trafficLight* init_lights(int green,int yellow, int red, int shift);
int get_bounds( int y, int x );
void update_spawners(int max_cars,spawner* spawners,car* cars,cell** grid);
int get_timer( char dir );
void init_cars(int max_cars, car* cars);
void fprint_vel(int n1, int n2, car* cars, cell **a,FILE* f);
	//void spawn( struct spawner *ptr );
	//int get_max_cars(void);
//----------------------------------

// MAIN SIM-LOOP
int main(int argc, char *argv[])
{
	// check invalid number of arguments
	if (argc != 2)
	{
		printf("Error main() - Expected one argument, an integer for the simulation duration\n");
		return 1;
	}
	// store argument
	int sim_time = strtol(argv[1],NULL,10);
	// check invalid argument range
	if (sim_time < 1)
	{
		printf("Error main() - Duration must be a positive integer but sim_time = %d\n",sim_time);
		return 1;
	}

	// open file for writing
	FILE *output = fopen( "traffic_results.txt","w");

	// declared variables
	int x,y,i,max_cars=0;
	srand( 1 ); // seeds the rand() function

	// struct car* cars = (struct car*) malloc(max_cars*sizeof(struct car));
	// struct car* cars_new = (struct car*) malloc(max_cars*sizeof(struct car));

	//-------------------------------------------------------------
	// INITIALIZATIONS

	// (1) Initialize Lights
	trafficLight* lights = init_lights(12,3,6,0); // time_green,time_yellow,time_red,shift

	// (2) Initialize Map
	cell** grid = init_grid(lights,&max_cars);

	// (3) Initialize Cars on map
	// two separate car arrays, one of them "new"
	car cars[max_cars]; // declares array of cars
	car cars_new[max_cars]; // declares array of cars_new
	init_cars(max_cars,cars); // initializes all cars in list to inactive
	random_fill(grid,0.3,cars); // initializes map with cars, modifying both the car-list and grid-matrix

	// (4) Initialize Ghost Cars
	init_ghosts();

	// (5) Initialize Spawners
	spawner* spawners = init_spawners(grid);

	//-------------------------------------------------------------
	// SIM-LOOP

	int time=0;

	while(time < sim_time)
	{
		time ++;

	// (1) Update Lights
		update_lights(lights);

	// (2) Update Ghost Cars
		update_ghosts();

	// (3) Update Cars
		for(i=0; i<max_cars; i++)
		{
			if( cars[i].id > -1) // check if car is active on map
			{
				cars_new[i]=kinematics( cars[i] ); // stores updated motion into new car array
			}
		}

	// (4) Update Spawners
		update_spawners(max_cars,spawners,cars,grid);

	// (5) Update Map and copy cars for next iteration
		for(i=0; i<max_cars; i++)
		{
			if( cars[i].id > -1) // check if car is active on map
			{
				grid[cars[i].y][cars[i].x].car_id = -1; // remove old car from map
				y = cars_new[i].y ; // store new car positions
				x = cars_new[i].x ;
				if( get_bounds(y,x) == 1) // true=1 if car inside boundaries
				{
					grid[y][x].car_id = i ; // add index of car to new map location
					cars[i] = cars_new[i] ; // copy new car values for next loop iteration
				}
				else // if car outside boundaries (along edges of map)
				{
					cars[i].id = -1; // remove car from old array
					cars_new[i].id = -1; // remove car from new array
				}
			}
		}

	// (6) Print Velocities of cars on map
		fprint_vel(GRID_HEIGHT,GRID_WIDTH,cars,grid,output);

	}
	//----------------------------------------------------------------
	//FREE ALLOCATED MEMORY
	free_grid(GRID_HEIGHT,GRID_WIDTH,grid);
	free(lights);
	free(spawners);

	//----------------------------------------------------------------
	//EXIT
	fclose(output);
	return 0;
}

//--------------------------------------------------------------------------------------
// Function call determines if location is inside edge boundaries of map.
// When a car is out of bounds, get_bounds() returns 0 and the car should be removed.
int get_bounds( int y, int x )
{
	// check inputs
	if(y<0){
		printf("Error get_bounds() - input 'y' cannot be a negative integer");
	}
	if(x<0){
		printf("Error get_bounds() - input 'x' cannot be a negative integer");
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
	car car_default;
	car_default.id = -1;
	car_default.speed = V_MAX;
	car_default.x = 0;
	car_default.y = 0;
	car_default.direction = ' ';

	for(int i=0; i<max_cars; i++)
	{
		cars[i]=car_default;
	}

}

//--------------------------------------------------------------------------------------
// Function call counts-down the timer of each spawner. When a timer reaches zero,
// a new car is "spawned" at specific location moving in a specific direction determined
// by the spawn-point. The timer is reset to a randomly generated duration.

void update_spawners(int max_cars,spawner* spawners,car* cars,cell** grid)
{
	for( int j=0; j<NUM_SPAWNERS; j++ )
	{
		spawners[j].timer --; // decrement spawn timer
		int x = spawners[j].x ; // stores x-position of spawn point
		int y = spawners[j].y ; // stores y-position of spawn point

		// checks if timer is zero AND if spawn-point is clear of cars
		if( (spawners[j].timer <= 0) && (grid[y][x].car_id == -1) )
		{
			for(int i=0; i<max_cars; i++) // search through index of car-array
			{
				if(cars[i].id == -1) // if index is inactive
				{
					cars[i].id = i; // current car-array index is activated
					cars[i].speed = V_MAX; // initial speed set to speed limit
					cars[i].x = x; // spawned at location of spawn-point
					cars[i].y = y;
					cars[i].direction = spawners[j].direction; // spawned in direction of spawn-point
					grid[y][x].car_id = i; // map is updated with car's location
					break; // exit for-loop
				}
			}
			// Resets timer. Currently set to a random number from 1-10.
			spawners[j].timer = get_timer(spawners[j].direction) ;
		}
	}
}

/***
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
	int x,y,i=0;
	spawner* spawners = (spawner*) malloc( NUM_SPAWNERS*sizeof(spawner) );

	// scan left edge of map
	x=0;
	for(y=0; y<GRID_HEIGHT; y++)
	{
		if(grid[y][x].element == 'I')
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
		if(grid[y][x].element == 'I')
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
		if(grid[y][x].element == 'I')
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
		if(grid[y][x].element == 'I')
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

/***
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
		return( N_SPAWN_MIN + (N_SPAWN_MAX - N_SPAWN_MIN)*(rand()/RAND_MAX) );
	}
	else if( dir == 'S')
	{
		return( S_SPAWN_MIN + (S_SPAWN_MAX - S_SPAWN_MIN)*(rand()/RAND_MAX) );
	}
	else if( dir == 'W')
	{
		return( W_SPAWN_MIN + (W_SPAWN_MAX - W_SPAWN_MIN)*(rand()/RAND_MAX) );
	}
	else if( dir == 'E')
	{
		return( E_SPAWN_MIN + (E_SPAWN_MAX - E_SPAWN_MIN)*(rand()/RAND_MAX) );
	}
	else
	{
		printf("Error get_timer() - Invalid direction %c set\n",dir);
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
void fprint_vel(int n1, int n2, car* cars, cell **a,FILE* f)
{

	// check for valid inputs
	if (n1 < 1 || n2 < 1)
	{
		printf("Error - fprint_vel(): rows and columns of matrix must be positive\n");
		return;
	}
	else if ( a == NULL)
	{
		printf("Error - fprint_vel(): matrix argument is NULL\n");
		return;
	}
	else if ( f == NULL)
	{
		printf("Error - fprint_vel(): file argument is NULL\n");
		return;
	}

	int i,j;

	for( i=n1; i>=0; i--)
	{
		for(j=0; j<n2; j++)
		{
 			if( a[i][j].element == EMPTY || a[i][j].element == MIDDLE_LANE ) // checks if not-road
 			{
 				fprintf(f," %c",EMPTY); // print blank
 			}
 			else if( a[i][j].car_id == -1) // if road without car
 			{
 				fprintf(f," %c",a[i][j].element); // print road element
 			}
 			else // if road with car
 			{
 				fprintf(f," %d",cars[a[i][j].car_id].speed); // print speed
 			}
		}
		fprintf(f,"\n");
	}
	fprintf(f,"\n");
}
