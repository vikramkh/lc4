/*
 * CIS 240 HW 10: LC4 Simulator
 * milestone_file.c 
 */
#include "milestone_file.h"

/*
 * Call this function to output the ASCII file: milestone.txt
 * Return a zero if successful and a non-zero error code if you encounter a
 * problem.
 * Params: pointer to current machine state.
 */
int generate_milestone_file (lc4_state* state) {
	FILE* fw = fopen("milestone.txt", "w");
	if (!fw) 
	{
		//Print error and free all memory
		printf("FILE ERROR: milestone.txt does not exist\n");
		free(state->memory.memory_array);
		free(state);
		exit(2);
	}
	//Check User Code Memory for Code and print to file
	for (int i = -1; i < 8193; i++) {
		if (i == -1)
		{
			fprintf(fw, "CODE:\n");
		}
		else if (state->memory.memory_array[i] != 0)
		{
			fprintf(fw, "address: %05d contents: 0x%04X\n", 
				i, state->memory.memory_array[i]);
		}
	}

	//Check OS Code Memory for Code and print to file
	for (int i = 32768; i < 40960; i++) {
		if (state->memory.memory_array[i] != 0)
		{
			fprintf(fw, "address: %05d contents: 0x%04X\n", 
				i, state->memory.memory_array[i]);
		}
	}

	//Check User Data Memory for Data and print to file
	for (int i = 8192; i < 32768; i++) {
		if (i == 8192)
		{
			fprintf(fw, "DATA:\n");
		}
		else if (state->memory.memory_array[i] != 0)
		{
			fprintf(fw, "address: %05d contents: 0x%04X\n", 
				i, state->memory.memory_array[i]);
		}
	}

	//Check OS Data Memory for Data and print to file
	for (int i = 40960; i < 65535; i++) {
		if (state->memory.memory_array[i] != 0)
		{
			fprintf(fw, "address: %05d contents: 0x%04X\n", 
				i, state->memory.memory_array[i]);
		}
	}
	//Close the file
	fclose(fw);
	return 0;
}

