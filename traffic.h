/*
 * traffic.h
 *
 *  Created on: Nov 29, 2017
 *      Author: ScottSims
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef TRAFFIC_H_
#define TRAFFIC_H_
//--------------------------------------------------
#define V_MAX 5 // maximum speed
//--------------------------------------------------
#define S_SPAWN_MIN 1
#define S_SPAWN_MAX 6
#define N_SPAWN_MIN 1
#define N_SPAWN_MAX 6
#define E_SPAWN_MIN 1
#define E_SPAWN_MAX 6
#define W_SPAWN_MIN 1
#define W_SPAWN_MAX 6
//---------------------------------------------------
#define LENGTH 3 //length of a road segment
#define NUM_LIGHTS_HOR 1 //number of horizontal blocks
#define NUM_LIGHTS_VERT 1 //number of vertical blocks
//---------------------------------------------------
#define GRID_HEIGHT ( NUM_LIGHTS_VERT * (2*LENGTH + 3) ) //no of rows
#define GRID_WIDTH ( NUM_LIGHTS_HOR * (2*LENGTH + 3) ) //no of columns
//---------------------------------------------------
#define S_EDGE (0+V_MAX)
#define W_EDGE (0+V_MAX)
#define N_EDGE (GRID_HEIGHT-1-V_MAX)
#define E_EDGE (GRID_WIDTH-1-V_MAX)
//---------------------------------------------------
#define NUM_SPAWNERS ( ( NUM_LIGHTS_VERT + NUM_LIGHTS_HOR ) * 2 ) // declares array of cars_new7ghh
#define NUM_LIGHTS ( NUM_LIGHTS_VERT * NUM_LIGHTS_HOR ) // number of lights/intersections
//---------------------------------------------------
#define GREEN 1
#define YELLOW 0
#define RED -1
//---------------------------------------------------
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
//---------------------------------------------------
// car structure stores variables of motion
typedef struct car{
	//int active; // true=1, false=0
	int id; // index in car array
	int speed; // speed of car
	int x; // x-position of car
	int y; // y-position of car
	char direction; // will be either 'N','S','E','W'
}car;

// spawner structure for cars entering the map from the edges
typedef struct spawner{
	char direction;
	int timer;
	int x;
	int y;
}spawner;

// cell structure of map
typedef struct cell{
	char element;
	int car_id;
}cell;

// traffic light structure
typedef struct trafficLight{
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

//--------------------------------------------------
// VARIABLES TO BE SHARED


//--------------------------------------------------
// FUNCTIONS TO BE SHARED
void random_fill(cell** grid,double p, car* carlist);
cell** init_grid(trafficLight* traffic_lights_list, int* max_cars);
void free_grid(int n1, int n2, cell **a);
void update_lights(trafficLight* lights);

//--------------------------------------------------
#endif /* TRAFFIC_H_ */
