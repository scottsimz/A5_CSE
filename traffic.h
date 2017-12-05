/*
 * traffic.h
 *
 *  Created on: Nov 29, 2017
 *      Author: ScottSims
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifndef TRAFFIC_H_
#define TRAFFIC_H_
//--------------------------------------------------
#define DURATION 1000 // duration of simulation
#define TIME_STABLE 200 // (default 200) set when to start recording data for 'fraction of time stopped' and 'avg map-time per car'.
//--------------------------------------------------
#define V_MAX 5 // maximum speed
#define PROB_SLOW 0.2 // probability car speeds decrease randomly
#define DENSITY 0.2 // initial density of cars on map
//--------------------------------------------------
#define S_SPAWN_MEAN 5//(10*DURATION)
#define N_SPAWN_MEAN 5//(10*DURATION)
#define E_SPAWN_MEAN 5 //(10*DURATION)
#define W_SPAWN_MEAN 5//(10*DURATION)
//---------------------------------------------------
#define LENGTH 60 //length of a road segment
#define NUM_LIGHTS_HOR 1 //number of horizontal blocks
#define NUM_LIGHTS_VERT 1 //number of vertical blocks
//---------------------------------------------------
#define GRID_HEIGHT ( NUM_LIGHTS_VERT * (2*LENGTH + 3) ) //no of rows
#define GRID_WIDTH ( NUM_LIGHTS_HOR * (2*LENGTH + 3) ) //no of columns
//---------------------------------------------------
#define S_EDGE (GRID_HEIGHT-1-V_MAX)
#define W_EDGE (0+V_MAX)
#define N_EDGE (0+V_MAX)
#define E_EDGE (GRID_WIDTH-1-V_MAX)
//---------------------------------------------------
#define NUM_SPAWNERS ( ( NUM_LIGHTS_VERT + NUM_LIGHTS_HOR ) * 2 ) // declares array of cars_new7ghh
#define NUM_LIGHTS ( NUM_LIGHTS_VERT * NUM_LIGHTS_HOR ) // number of lights/intersections
//---------------------------------------------------
#define GREEN 1
#define YELLOW 0
#define RED -1
#define TIME_GREEN 60
#define TIME_YELLOW 3
#define TIME_RED (TIME_GREEN - TIME_YELLOW)
#define TIME_SHIFT 0
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
#define MAX(a,b) ( ((a)>(b)) ? (a):(b) )
#define MIN(a,b) ( ((a)<(b)) ? (a):(b) )
//---------------------------------------------------
// car structure stores variables of motion
typedef struct car {
	char direction; //Car direction
	int id; //Car ID
	int x_old;  //old x position
	int y_old;  //old y position
	int v_old;  //old velocity
	int x_new;  //new x position
	int y_new;  //new y position
	int v_new;  //new velocity
	int stop_time; //time the car was stopped
	int spawn_time;
} car;

// spawner structure for cars entering the map from the edges
typedef struct spawner{
	char direction;
	int timer;
	int x;
	int y;
}spawner;

// cell structure of map
typedef struct cell{
	char map_elem;
	int car_id;
}cell;

// traffic light structure
typedef struct trafficLight{
	int id;
	int x;
	int y;
	int timer;
	int time_red;
	int time_green;
	int time_yellow;
	int northSouthLight; //the color of the North and South facing lights
	//eastWestLight = -1*northSouthLight
} trafficLight;

//--------------------------------------------------
// VARIABLES TO BE SHARED


//--------------------------------------------------
// FUNCTIONS TO BE SHARED
void random_fill(cell** grid,int gridHeight,int gridWidth,double p, car* activeCarList);
cell** init_grid(trafficLight* traffic_lights_list, int* max_cars);
void free_grid(int n1, int n2, cell **a);
int update_lights(trafficLight* lights);
void print_elements(int n1, int n2, cell **a);
void print_car_ids(int n1, int n2, cell **a);
void ghost(cell ** grid, int midRow, int midCol, trafficLight *color);
int emptycellcount(cell **grid, int gridWidth, int gridHeight, int i, int j, char direction);
car* update_car(struct car *c, cell** grid, int empty, int ncars, int i, struct car *activeCarList);
//int update_cars(int max_cars, car* cars, cell** grid);
double uniform(void);
void car_mvmt_plot(cell** grid, char direction, int index, int gridWidth, int gridHeight,car* cars, FILE* outputFile);
//--------------------------------------------------
#endif /* TRAFFIC_H_ */
