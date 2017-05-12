// trace.c file contains main function

#include "milestone_file.h"

int main(int argc, char const *argv[]) {
	if (argc < 4) 
	{
		printf("Error: Too Few Files\n");
		return 1;
	}

	//Declare lc4_state and initialize all values to 0

	lc4_state ls;
	ctrl control;

	ls.memory.memory_array = malloc(sizeof(short unsigned int) * 65536);

	//Allocate memory for memory array
	if (ls.memory.memory_array == NULL)
	{
		//Print error and free malloc'ed memory
		printf("Memory Allocation Failed");

        return 1;
	}

	reset_lc4(&ls, &control);


	int i = 1;

	//If -m flag, offset every argument
	if (strcmp(argv[1], "-m") == 0)  
	{
		i++;
	}
	//Get Breakpoint from input
	unsigned int breakpoint = 0;
	sscanf(argv[i], "%x", &breakpoint);
	i++;
	//Get File From input
	FILE* fp = fopen(argv[i], "w");
	set_fp(fp);
	while(i < argc)
	{
		char* filename = (char*) argv[i];
		read_object_file(filename, &ls);
		i++;
	}

	if (strcmp(argv[1], "-m") == 0) 
	{
		generate_milestone_file (&ls);
	}
	//While PC is not breakpoint, update lc4_state
	while(ls.PC != breakpoint) {
		if (ls.PC == 0x00FF)
		{
			break;
		}
		if (update_lc4_state(&ls, &control) != 0) {
			break;
		}
	}
	//Free memory and close file
	free(ls.memory.memory_array);
	fclose(fp);
    return 0;
}
