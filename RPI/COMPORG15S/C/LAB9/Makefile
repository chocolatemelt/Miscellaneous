# Generic Makefile for Lab 9

# List of files
C_SRCS		= main.c trig.c
C_OBJS		= main.o trig.o
C_HEADERS	= trig.h pi.h
OBJS = $(C_OBJS)

# Desired name of the executable
EXE		= lab9

# Compiler and loader commands and flags
CC		= gcc
CC_FLAGS	= -g -Wall -c
LD_FLAGS	= -g -Wall

# PATH
FILENAME	= lab9.exe

# The first, and hence default, target is the executable
$(EXE): $(OBJS)
	@echo "Final linking of all object modules."
	$(CC) $(LD_FLAGS) $(OBJS) -o $(EXE)

# Force recompilation of C objects if headers change
${C_OBJS}: ${C_HEADERS}

# Compile .c files to .o files
%.o: %.c
	@echo "Compiling each C source file separately"
	$(CC) $(CC_FLAGS) $<

# Clean up the directory
clean:
	@echo "Cleaning project directory."
	rm -f *.o *~ $(EXE) 

# Example of Running Commands in Make
envinfo:
	@echo "Getting environment information:"
	date
	uname -a
	pwd
test:
	@echo "Testing if file exists."
ifneq ($(wildcard $(FILENAME)),)
	@echo "Test OK."
else
	@echo "Test failed. Remaking source."
	make
endif
	@echo "Executing lab9:"
	./$(EXE)