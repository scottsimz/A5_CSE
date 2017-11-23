# A5_CSE
Traffic Simulation for assignment 5 of CSE 6010

# Sections

## Environment Map
The env_map file can build a grid using the following inputs:
1. L = This is the length of the road segment required. Right now, for 1 intersection, you get roads of length L. For more intersections, the road sections at the edges have length L while road sections not at the edges have length 2L (when two intersections are combined, the distance between them ends up being 2L)
2. n_hor = Number of intersection blocks we want to have horizontally along the grid
3. n_vert = Number of intersection blocks we want to have vertically along the grid.

Currently, the map can only make n_hor x n_vert grids which are fully populated. This means that if we have set 3 horizontal intersection blocks and 2 vertical intersection block, we will get a total of 6 intersections.

Work Remaining:
1. Need to add right hand turn lanes
2. If we want to handle odd number of intersections, I will need to make adjustments.

### Extra Files
#### References
This folder consists of any extra files that can be used for references. Important files here include:
1. waitList.c : Sample code that can be used to implement lists in our simulation. I adapted this using Karl's code. Functionality includes: creating a new list with MAX_SIZE, adding elements to the end of the list (and reallocing the size if max_size is crossed), removing elements, freeing memory for list.
