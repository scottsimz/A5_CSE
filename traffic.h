/*
 * traffic.h
 *
 *  Created on: Nov 24, 2017
 *      Author: ScottSims
 */

#ifndef TRAFFIC_H_
#define TRAFFIC_H_

#define V_MAX 5 // maximum speed
#define NUM_LIGHTS 1 // used to determine MAX_CARS
#define WIDTH 100 // map width
#define HEIGHT 100 // map height

#define S_SPAWN_MIN 1
#define S_SPAWN_MAX 10
#define N_SPAWN_MIN 1
#define N_SPAWN_MAX 10
#define E_SPAWN_MIN 1
#define E_SPAWN_MAX 10
#define W_SPAWN_MIN 1
#define W_SPAWN_MAX 10

extern int MAX_CARS; // declared here but defined & initialized in simulate.c

// stores variables of motion for each car
struct car{
	int active; // true=1, false=0
	int speed; // speed of car
	int x; // x-position of car
	int y; // y-position of car
	char direction; // will be either 'N','S','E','W'
};

// spawns cars into the map from the edges
struct spawner{
	struct spawner *next;
	char direction;
	int timer;
	int x;
	int y;
};

// cell of map
struct map{
	char element;
	int car;
};
//--------------------------------------------------
// FUNCTIONS TO BE SHARED
struct car kinematics(struct car car_old);

//--------------------------------------------------
#endif /* TRAFFIC_H_ */
