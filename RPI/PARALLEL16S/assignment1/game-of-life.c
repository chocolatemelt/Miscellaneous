/***************************************************************************/
/* Template for Asssignment 1 **********************************************/
/* Your Name Here             **********************************************/
/***************************************************************************/

/***************************************************************************/
/* Includes ****************************************************************/
/***************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<math.h>

/***************************************************************************/
/* Defines *****************************************************************/
/***************************************************************************/

#define ALIVE 1
#define DEAD  0

/***************************************************************************/
/* Global Vars *************************************************************/
/***************************************************************************/

unsigned int **g_GOL_CELL=NULL;

unsigned int g_x_cell_size=0;
unsigned int g_y_cell_size=0;

unsigned int g_num_ticks=0;

double g_thresh_hold=0.0;

/***************************************************************************/
/* Function Decs ***********************************************************/
/***************************************************************************/

void allocate_and_init_cells();
void compute_one_tick();
void output_final_cell_state();
int adjacent(unsigned int **, int, int);
int xwrap(int, int);
int ywrap(int, int);
void cleanup();

/***************************************************************************/
/* Function: Main **********************************************************/
/***************************************************************************/

int main(int argc, char *argv[])
{
  int i = 0;

  // insert your code to capture inputs from argc and argv
  // Argument 1 is size of X and Y cell dimensions
  // Argument 2 is the number of ticks
  // Argument 3 is the thresh hold percent 0%, 25%, 50%, 75% and 90%.
  // Convert strings to integer values using "atoi()"

	if(argc != 4) {
		printf("ERROR: Not enough arguments.\n");
		return EXIT_FAILURE;
	}

	// assign command line arguments to global vars
	g_x_cell_size = atoi(argv[1]);
	g_y_cell_size = g_x_cell_size;
	g_num_ticks = atoi(argv[2]);
	int temp_threshold = atoi(argv[3]);
	g_thresh_hold = temp_threshold / 100.0;

	// check for errors and proceed if pass
	if(g_x_cell_size <=0 || g_y_cell_size <= 0 || g_num_ticks <= 0
			|| g_thresh_hold < 0 || g_thresh_hold > 1) {
		printf("ERROR: Invalid argument.\n");
		return EXIT_FAILURE;
	}

	allocate_and_init_cells();

	for(i = 0; i < g_num_ticks; i++) {
		compute_one_tick();
	}
	
    output_final_cell_state();

	cleanup();

  return EXIT_SUCCESS;
}

/***************************************************************************/
/* Function: allocate_and_init_cells ***************************************/
/***************************************************************************/

void allocate_and_init_cells()
{
  // use "calloc" to allocate space for your cell matrix
  // use "drand48" to init the state of each grid cell once allocated.
	int i, j;
	g_GOL_CELL = calloc(g_x_cell_size, sizeof(unsigned int *));
	for(i = 0; i < g_x_cell_size; ++i) {
		// initialize rows
		g_GOL_CELL[i] = calloc(g_y_cell_size, sizeof(unsigned int));
		for(j = 0; j < g_y_cell_size; ++j) {
			// initialize elements
			g_GOL_CELL[i][j] = (int)(drand48() * 100.0) % 2; // random 0 or 1
		}
	}
}

/***************************************************************************/
/* Function: compute_one_tick **********************************************/
/***************************************************************************/

void compute_one_tick()
{
  // iterate over X (outside loop) and Y (inside loop) dimensions of the g_GOL_CELL
  // Use drand48() for uniform distribution. It is already included in stdlib.h

	// check rng threshold, and recompute each cell if smaller
	if(drand48() < g_thresh_hold) {
		cleanup();
		allocate_and_init_cells();
		return;
	}

	// otherwise apply rules to each cell
	// copy board into new board using memcpy
	int i, j, a;
	int **tmp_cells = malloc(g_x_cell_size * sizeof(unsigned int *));
	for(i = 0; i < g_x_cell_size; ++i)
		memcpy(&tmp_cells[i], &g_GOL_CELL[i], sizeof(g_GOL_CELL[0]));

	// compute
	for(i = 0; i < g_x_cell_size; ++i) {
		for(j = 0; j < g_y_cell_size; ++j) {
			a = adjacent(g_GOL_CELL, i, j);
			if(a == 2) tmp_cells[i][j] = g_GOL_CELL[i][j]; // nothing happens
			if(a == 3) tmp_cells[i][j] = 1; // birth or nothing happens
			if(a < 2) tmp_cells[i][j] = 0; // underpopulation
			if(a > 3) tmp_cells[i][j] = 0; // overpopulation
		}
	}
	// copy over to new board
	for(i = 0; i < g_x_cell_size; ++i)
		memcpy(&g_GOL_CELL[i], &tmp_cells[i], sizeof(tmp_cells[0]));

	// cleanup
	free(tmp_cells);
}


/***************************************************************************/
/* Function: output_final_cell_state ***************************************/
/***************************************************************************/

void output_final_cell_state()
{
  // print out in grid form 16 value per row of the g_GOL_CELL
  // This data will be used to create your graphs
	int i, j;
	for(i = 0; i < g_x_cell_size; ++i) {
		for(j = 0; j < g_y_cell_size; ++j) {
			printf("%d ", g_GOL_CELL[i][j]);
		}
		printf("\n");
	}
}

/***************************************************************************/
/* Function: adjacent ******************************************************/
/***************************************************************************/

int adjacent(unsigned int **board, int x, int y) {
	int count, i, j;
	count = 0;

	// check area around cell
	for(i = -1; i <= 1; ++i)
		for(j = -1; j <= 1; ++j)
			if(i || j)
				// if nonzero, then calculate wraparound and count
				if(board[xwrap(x, i)][ywrap(y, j)]) ++count;

	return count;
}

/***************************************************************************/
/* Function: xwrap *********************************************************/
/***************************************************************************/

int xwrap(int i, int a) {
	// simply wrap around the width
	i += a;
	while(i < 0) i += g_x_cell_size;
	while(i >= g_x_cell_size) i -= g_x_cell_size;
	return i;
}

/***************************************************************************/
/* Function: ywrap *********************************************************/
/***************************************************************************/

int ywrap(int i, int a) {
	// simply wrap around the height
	i += a;
	while(i < 0) i += g_y_cell_size;
	while(i >= g_x_cell_size) i -= g_y_cell_size;
	return i;
}

/***************************************************************************/
/* Function: cleanup *******************************************************/
/***************************************************************************/

void cleanup()
{
	int i;
	for(i = 0; i < g_x_cell_size; ++i) {
		free(g_GOL_CELL[i]);
	}
	free(g_GOL_CELL);
}
