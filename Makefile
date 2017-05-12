all: trace 

lc4.o: lc4.c lc4.h
	clang -g -c lc4.c

object_files.o: object_files.c object_files.h lc4.h
	clang -g -c object_files.c

milestone_file.o: milestone_file.c milestone_file.h lc4.h object_files.h
	clang -g -c milestone_file.c

trace: trace.c lc4.o object_files.o milestone_file.o
	clang -g -o trace trace.c lc4.o object_files.o milestone_file.o 

clean: 
	rm -rf *.o
	
clobber: clean        
	rm -rf trace

