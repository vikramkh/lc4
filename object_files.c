/*
 * CIS 240 HW 10: LC4 Simulator
 * object_files.c
 */

#include "object_files.h"
/*
 * Read an object file and modify the machine state accordingly.
 * Return a zero if successful and a non-zero error code if you encounter a
 * problem. For the Milestone you can simply set machine_state to 0.
 * Params: pointer to filename, pointer to current machine state.
 * If you have to deal with endianness, call flip_endian here.
 */
int read_object_file (char *filename, lc4_state* state) {
	FILE* objfile = fopen(filename, "r");
	if (!objfile) 
	{
		printf("FILE ERROR: %s does not exist\n", filename);
		free(state->memory.memory_array);
		free(state);
		exit(2);
	}
	//Parse through objfile
	while(!feof(objfile)) 
	{
		unsigned short int type = 0;
		type = read_instruction(objfile);
		unsigned short int data;
		unsigned short int length;
		unsigned short int address;
		//If Code, get address, and store in state memory
		if (type == 0xCADE) {
			address = read_instruction(objfile);
			length = read_instruction(objfile);
			for (int i = 0; i < length; i++)
			{
				data = read_instruction(objfile);
				state->memory.memory_array[address + i] = data;
			}
		}
		//If Data, get address, and store in state memory
		if (type == 0xDADA) {
			address = read_instruction(objfile);
			length = read_instruction(objfile);
			for (int i = 0; i < length; i++)
			{
				data = read_instruction(objfile);
				state->memory.memory_array[address + i] = data;
			}
		}
		//If Symbol parse through to ensure format
		if (type == 0xC3B7) {
			address = read_instruction(objfile);
			length = read_instruction(objfile);
			for (int i = 0; i < length; i++)
			{
				char symbol = fgetc(objfile);
			}
		}
		//If FileName parse through to ensure format
		if (type == 0xF17E) {
			address = read_instruction(objfile);
			length = read_instruction(objfile);
			for (int i = 0; i < length; i++)
			{
				char file = fgetc(objfile);
			}
		}
		//If LineNumber parse through to ensure format
		if (type == 0x715E) {
			address = read_instruction(objfile);
			unsigned short int line = read_instruction(objfile);
			unsigned short int index = read_instruction(objfile);
		}
	}
	//Close the file
	fclose(objfile);
	return 0;

}



/*
* Read in a 16-bit instruction from the current object file and flip its endianness.
* Return the correct endianness value of the instruction.
* Param: pointer to FILE.
*/
int read_instruction(FILE* fp) {
	unsigned short int data = 0;
	fread(&data, 2, 1, fp);
	data = flip_endian(data);
	return data;
}



/*
 * Check that the given file exists.
 * Params: pointer to file, pointer to filename.
 */
void check_file (char *filename, FILE *file) {
	if (!file) 
	{
		printf("FILE ERROR: %s does not exist\n", filename);
		exit(2);
	}
}



/*
 * Changes a 4-byte word's endian. ex: xDECA -> xCADE
 * This is an *optional* function.
 * If you do run into issues with endianness, implement flip_endian.
 * Params: int to convert.
 */
short int flip_endian (short int word) {
	unsigned short int first = word & 0x00ff;
	first = first << 8;
	unsigned short int second = (word >> 8) & 0x00ff;
	word = first + second;
	return word;
}




