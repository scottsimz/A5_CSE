//Sample code that can be used to implement lists. 
// I adapted this using Karl's code: I added the functionality to reallocate space
// and the remove_first_event function


// define WaitList Data Structure
typedef struct WaitList {
    EventData *elements;     // pointer to array of elements
    int MAX_SIZE;      // maximum size of list
    int counter;       // number of elements already in the list
} WaitList;


//Initialize a WaitList object
WaitList* waitlist;



WaitList* create_list(int size) {
    // Allocate memory for the structure
    WaitList *W = malloc( sizeof (WaitList) );
    if (W == NULL) {
        printf("Could not create waitlist. Out of Memory!\n");
        return NULL;
    }

    // Allocate space for the array in the WaitList struct
    // Note that this space is different from that already 
    // allocated to the structure.
    W->elements = malloc( sizeof(EventData) * size);
    if (W->elements == NULL) {
        printf("Could not create waitlist. Out of Memory!\n");
        return NULL;
    }

    W->counter = 0;     // 0 elements initially in the list
    W->MAX_SIZE = size; // maximum size the list can hold

    return W;
}

int add_to_end(WaitList* aList,EventData* element){
    if (aList == NULL){
        printf("Could not add to waitlist. No wait list found!");
        return 1;
    }       

    // check if list is full
    if (aList->counter == aList->MAX_SIZE){
        aList->elements = realloc(aList->elements,sizeof(struct EventData)*(aList->MAX_SIZE)*2);
       	aList->MAX_SIZE = aList->MAX_SIZE * 2;
    }    

    aList->elements[aList->counter] = *element;


    aList->counter++;



    return 0;
}


int remove_first_event(WaitList* aList){

    if(aList == NULL){
        printf("Could not remove from waitlist. No wait list found!");
        return 1;
    } else if (aList->counter == 0) {
        return 0; //don't do anything if counter is already 0
    }

	//shift values of aList to the left
	// printf("Removing first event: aList->counter = %d\n",aList->counter);
	for(int i = 0; i < aList->counter - 1; i++){
        aList->elements[i] = aList->elements[i+1];
    }

    aList->counter = aList->counter - 1; //reduce the counter size by 1
  	// printf("Removed first event: aList->counter = %d\n",aList->counter);

}


int free_list(WaitList *aList) {
    // printf("Free List Called\n");

    // NULL pointer can't be freed
    if (aList == NULL){
		return 1;
    }

    // printf("alist is not null\n");

    // first free the dynamically allocated array
    if (aList->elements != NULL) 
        free(aList->elements);
	
	// printf("Now let's free alist\n");

    // now free the pointer to the struct
    free(aList);    
    return 0;
}