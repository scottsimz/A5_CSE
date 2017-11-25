

//Overall car update function
car update_car(car c, cell ** grid, int active_car_counter, car* active_car_list);

//Counts number of cells in front of a car that are free - how many spaces can it move.
int emptycellcount(car c, cell ** grid, char direction);

//Update velocity of cars
int update_velocity(struct car c, cell** grid);

//Allocate memory for car structure
void malloc_carstruct(car c);

//Generate ghost car --this needs some work
car ghost(car c, cell ** grid, int midRow, int midCol);

double p = 0.2;
int vmax = 5;
int density = 0.4;
int num_samples = 200;
int car_count = 0;
int numfront = 0;

car update_car(struct car c, cell** grid, int active_car_counter, car* active_car_list){
	for(int i = 0; i < active_car_counter; i++){
		char direction = grid[c.x_old][c.y_old].map_elem;

		int x = c.x_old;
		int y = c.y_old;

		//if this is an east road and there is a car in a cell
		if((direction == grid[x][y].map_elem == 'E') && grid[x][y].car.id != 0){

			//see traffic lights
			//					if(TRAFFIC == RED){
			//						car gc = ghost(c,grid,midRow,midCol);
			//					}

			//Calculate number of cells in front of car
			numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			c.v_new = update_velocity(c,grid);

			//current positions become old
			c.x_old = c.x_new;
			c.y_old = c.y_new;

			//new positions become current positions
			c.x_new = c.x_old+c.v_new;
			//y direction does not change with east bound traffic
			c.y_new = y;

			//Old spot has car ID of 0 (no car)
			grid[c.x_old][c.y_old].car.id = 0;
			grid[c.x_new][c.y_new].car.id = 1;

		}//end if east


		if((direction == grid[x][y].map_elem == 'W') && grid[x][y].car.id != 0){

			//see traffic lights
			//					if(TRAFFIC == RED){
			//						car gc = ghost(c,grid,midRow,midCol);
			//					}

			//Calculate number of cells in front of car
			numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			c.v_new = update_velocity(c,grid);

			//current positions become old
			c.x_old = c.x_new;
			c.y_old = c.y_new;

			//new positions become current positions
			c.x_new = c.x_old-c.v_new;
			//y direction does not change with west bound traffic
			c.y_new = y;

			//Old spot has car ID of 0 (no car)
			grid[c.x_old][c.y_old].car.id = 0;
			grid[c.x_new][c.y_new].car.id = 1;

		}//end if west

		if((direction == grid[x][y].map_elem == 'N') && grid[x][y].car.id != 0){


			//see traffic lights
			//					if(TRAFFIC == RED){
			//						car gc = ghost(c,grid,midRow,midCol);
			//					}

			//Calculate number of cells in front of car
			numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			c.v_new = update_velocity(c,grid);

			//current positions become old
			c.x_old = c.x_new;
			c.y_old = c.y_new;

			//new positions become current positions
			c.y_new = c.y_old-c.v_new;
			//x direction does not change with north bound traffic
			c.x_new = x;

			//Old spot has car ID of 0 (no car)
			grid[c.x_old][c.y_old].car.id = 0;
			grid[c.x_new][c.y_new].car.id = 1;

		}//end if north


		if(direction == grid[x][y].map_elem == 'S' && grid[x][y].car.id != 0){


			//see traffic lights
			//if(TRAFFIC == RED){
			//car gc = ghost(c,grid,midRow,midCol);
			//}

			//Calculate number of cells in front of car
			numfront = emptycellcount(c,grid,direction);

			//update velocity first to see how far it wants to go.
			c.v_new = update_velocity(c,grid);

			//current positions become old
			c.x_old = c.x_new;
			c.y_old = c.y_new;

			//new positions
			c.y_new = c.y_old-c.v_new;
			//x direction does not change with south bound traffic
			c.x_new = x;

			//Old spot has car ID of 0 (no car)
			grid[c.x_old][c.y_old].car.id = 0;
			grid[c.x_new][c.y_new].car.id = 1;


		}//end if south


	}//end for i

	return c;

}//end update car east function

//UPDATE VELOCITIES
int update_velocity(struct car c, cell** grid){
	double rn = (double)rand() / RAND_MAX;

	//Set current value velocity as "old velocity"
	c.v_old = c.v_new;

	//Create a temporary integer for the current velocity
	int v_temp = c.v_old;

	//If the current velocity is less than v max, increment
	if(v_temp < vmax){
		v_temp ++;
	}
	//If the new vtemp velocity is greater than number of spaces in front
	//V temp is the number in front minus 1.
	if(v_temp > numfront){
		v_temp = numfront - 1;
	}

	//Randomization step. If random number less than prob, decrement vtemp
	if(rn < p){
		v_temp --;
	}

	//The new velocity is v_temp
	c.v_new = v_temp;
	return c.v_new;
}


//FIND OUT HOW MANY SPACES ARE IN FRONT
int emptycellcount(struct car c, cell** grid, char direction){
	int emptyspaces = 0;
	if(grid[c.x_old][c.y_old].map_elem == EAST){

		printf("East position%d ",c.x_old);

		int j = c.y_old;
		for(int i = 0; i < c.v_new; i++){
			int emptyspaces = 0;
			if(grid[c.x_old + 1 + i][j].car.id == 0){
				emptyspaces++;
			}
			else{
				break;
			}//end else
		}//end for i
	}//end if east

	else if(grid[c.x_old][c.y_old].map_elem == WEST){
		int j = c.y_old;
		for(int i = 0; i < c.v_new; i++){
			if(grid[c.x_old-1-i][j].car.id == 0){
				emptyspaces++;
			}
			else{
				break;
			}//end else
		}//end for i
		//return emptyspaces;
	}//end if west

	else if(grid[c.x_old][c.y_old].map_elem == SOUTH){
		int i = c.x_old;
		for(int j = 0; j < c.v_new; j++){
			if(grid[i][c.y_old+j+1].car.id == 0){
				emptyspaces++;
			}
			else{
				break;
			}//end else
			return emptyspaces;
		}//end for j
	}//end if south


	else if(grid[c.x_old][c.y_old].map_elem == NORTH){
		int i = c.x_old;
		for(int j = 0; j < c.v_new; j++){
			if(grid[i][c.y_old-1-j].car.id == 0){
				emptyspaces++;
			}
			else{
				break;
			}//end else
		}//end for j
	}//end if north
	return emptyspaces;
}//end function



//GENERATE GHOST CAR FOR RED LIGHTS
car ghost(car ghost_car, cell ** grid, int midRow, int midCol){

	malloc_carstruct(ghost_car);

	int row_east = midRow - 1;
	int col_east = midCol - 1;

	int row_west = midRow + 1;
	int col_west = midCol + 1;

	ghost_car.id = 2; //Certain ID for ghost cars?
	ghost_car.x_old = 0;
	ghost_car.y_old = 0;
	ghost_car.v_old = 0; //Velocity is 0
	ghost_car.v_new = 0;
	ghost_car.y_new = 0;
	ghost_car.y_new = 0;
	grid[row_east][col_east].car = ghost_car;
	grid[row_west][row_west].car = ghost_car;
	return ghost_car;
}


//ALLOCATE MEMORY FOR CAR STRUCTURE?
void malloc_carstruct(car c){
	//Allocate memory for car structure
	car* car_m;
	car_m = (struct car*)malloc(sizeof(car));
	if(car_m == NULL){
		printf("Memory Allocation Failed");
	}
}

