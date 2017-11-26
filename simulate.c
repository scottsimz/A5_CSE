#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "traffic.h"			// Use shared definitions

#define S_EDGE (0+V_MAX)
#define W_EDGE (0+V_MAX)
#define N_EDGE (HEIGHT-1-V_MAX)
#define E_EDGE (WIDTH-1-V_MAX)
//----------------------------------
// FUNCTION PROTOTYPES
struct spawner* init_spawn(void);
int get_bounds( int y, int x );
void spawn( struct spawner *ptr );
int get_timer( char dir );
int get_max_cars(void);
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

	// declared variables
	int x,y,i,time=0;
	struct spawner* spawn_head; // head of linked-list of spawners
	srand( 1 ); // seeds the rand() function

	// array of struct car
	int MAX_CARS = get_max_cars(); // external variable defined in header traffic.h
	struct car cars[MAX_CARS];
	struct car cars_new[MAX_CARS];
	// struct car* cars = (struct car*) malloc(MAX_CARS*sizeof(struct car));
	// struct car* cars_new = (struct car*) malloc(MAX_CARS*sizeof(struct car));

	//-------------------------------------------------------------
	// INITIALIZATIONS

	// Initialize Map
	init_grid();
	// Initialize Cars on map
	// two separate car arrays, one of them "new"
	init_cars();
	// Initialize Ghost Cars
	init_ghost();
	// Initialize Spawners
	spawn_head = init_spawn();

	//-------------------------------------------------------------
	// SIM-LOOP

	while(time < sim_time)
	{
		time ++;

	// (1) Update Lights
		lights();

	// (2) Update Ghost Cars
		ghost();

	// (3) Update Spawners
		spawn(spawn_head);

	// (4) Update Cars
		for(i=0; i<MAX_CARS; i++)
		{
			if( cars[i].active==1) // check if car is active on map
			{
				cars_new[i]=kinematics( cars[i] ); // store updated motion into new car array
			}
		}

	// (5) Update Map and copy cars for next iteration
		for(i=0; i<MAX_CARS; i++)
		{
			if( cars[i].active==1) // check if car is active on map
			{
				cell[cars[i].y][cars[i].x].car = -1; // remove old car from map
				y = cars_new[i].y ; // store new car positions
				x = cars_new[i].x ;
				if( get_bounds(y,x) == 1) // true=1 if car inside boundaries
				{
					cell[y][x].car = i ; // add index of car to new map location
					cars[i] = cars_new[i] ; // copy new car values for next loop iteration
				}
				else // if car outside boundaries (along edges of map)
				{
					cars[i].active = 0; // remove car from old array
					cars_new[i].active = 0; // remove car from new array
				}
			}
		}

	}
	//----------------------------------------------------------------
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
// Function call counts-down the timer of each spawn-point. When a timer reaches zero,
// a new car is "spawned" at specific location moving in a specific direction determined
// by the spawn-point. The timer is reset to a randomly generated duration.
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

//--------------------------------------------------------------------------------------
// Initializes a linked-list of spawners and returns a head pointer for the linked-list.
struct spawner* init_spawn(void)
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

