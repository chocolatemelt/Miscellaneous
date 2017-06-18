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
// New Includes for Assignment 2
#include<clcg4.h>
#include<mpi.h>


/***************************************************************************/
/* Defines *****************************************************************/
/***************************************************************************/

#define ALIVE 1
#define DEAD  0
#define GHOST_BOT 1
#define GHOST_TOP 2

/***************************************************************************/
/* Global Vars *************************************************************/
/***************************************************************************/

unsigned int **g_GOL_CELL=NULL;

unsigned int g_x_cell_size=0;
unsigned int g_y_cell_size=0;

unsigned int g_num_ticks=0;

int g_rowsize=0;

double g_thresh_hold=0.0;

/***************************************************************************/
/* Function Decs ***********************************************************/
/***************************************************************************/

void compute_one_tick(unsigned int **, int);
void output_final_cell_state(unsigned int **, int);
int adjacent(unsigned int **, int, int);
int xwrap(int, int);
int ywrap(int, int);
void cleanup(unsigned int **);

/***************************************************************************/
/* Function: Main **********************************************************/
/***************************************************************************/

int main(int argc, char *argv[])
{
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

	int i, j;
	int mpi_myrank;
	int mpi_commsize;
	double starttime = 0.0, endtime = 0.0;

	// Example MPI startup and using CLCG4 RNG
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_myrank);

	// Init 16,384 RNG streams - each rank has an independent stream
	InitDefault();

	// Note, use the mpi_myrank to select which RNG stream to use.
	printf("Rank %d of %d has been started and a first Random Value of %lf\n",
			mpi_myrank, mpi_commsize, GenVal(mpi_myrank));

	MPI_Barrier(MPI_COMM_WORLD);

	// MPI timer
	if(mpi_myrank == 0) {
		starttime = MPI_Wtime();
	}

	// initialize myrank's chunk of the universe
	// also allocate space for ghost rows
	g_rowsize = g_y_cell_size / mpi_commsize;
	unsigned int **my_universe = calloc(g_rowsize + 2, sizeof(unsigned int *));
	for(i = 0; i < g_rowsize + 2; ++i) {
		// allocate rows
		my_universe[i] = calloc(g_x_cell_size, sizeof(unsigned int));
		for(j = 0; j < g_x_cell_size; ++j) {
			if(i != 0 && i != g_rowsize + 1) {
				// initialize non-ghost rows
				my_universe[i][j] = (unsigned int)(GenVal(mpi_myrank) * 100.0) % 2;
			}
		}
	}

	// block before processing
	MPI_Barrier(MPI_COMM_WORLD);

	// process world in ticks
	int k;
	for(k = 0; k < g_num_ticks; ++k) {
		// exchange row data through MPI_Isend / MPI_Irecv

		// initialize buffers
		unsigned int *sendbuf_top = NULL, *sendbuf_bot = NULL;
		unsigned int *recvbuf_top = NULL, *recvbuf_bot = NULL;
		int bufsize = g_x_cell_size;

		// allocate and copy sendbufs
		sendbuf_top = calloc(bufsize, sizeof(unsigned int));
		sendbuf_bot = calloc(bufsize, sizeof(unsigned int));
		memcpy(sendbuf_top, my_universe[1], bufsize * sizeof(unsigned int));
		memcpy(sendbuf_bot, my_universe[g_rowsize], bufsize * sizeof(unsigned int));

		// allocate recvbufs
		recvbuf_top = calloc(bufsize, sizeof(unsigned int));
		recvbuf_bot = calloc(bufsize, sizeof(unsigned int));

		// mpi data
		MPI_Request requests[4];
		MPI_Status  statuses[4];
		int mpi_top, mpi_bot;
		mpi_top = mpi_myrank - 1;
		if(mpi_top < 0) mpi_top = mpi_commsize - 1;
		mpi_bot = mpi_myrank + 1;
		if(mpi_bot >= mpi_commsize) mpi_bot = 0;

		// send top and bottom row
		MPI_Isend(sendbuf_top, bufsize, MPI_UNSIGNED, mpi_top, GHOST_TOP, MPI_COMM_WORLD, &requests[0]);
		MPI_Isend(sendbuf_bot, bufsize, MPI_UNSIGNED, mpi_bot, GHOST_BOT, MPI_COMM_WORLD, &requests[1]);

		// receive top and bottom rows
		MPI_Irecv(recvbuf_top, bufsize, MPI_UNSIGNED, mpi_top, GHOST_BOT, MPI_COMM_WORLD, &requests[2]);
		MPI_Irecv(recvbuf_bot, bufsize, MPI_UNSIGNED, mpi_bot, GHOST_TOP, MPI_COMM_WORLD, &requests[3]);

		// wait on MPI messages
		MPI_Waitall(4, requests, statuses);

		// update ghost rows with new buffers
		if(!my_universe[0]) free(my_universe[0]);
		if(!my_universe[g_rowsize+1]) free(my_universe[g_rowsize+1]);
		my_universe[0] = recvbuf_top;
		my_universe[g_rowsize+1] = recvbuf_bot;

		// cleanup sent rows
		free(sendbuf_top);
		free(sendbuf_bot);

		// update universe
		compute_one_tick(my_universe, mpi_myrank);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	output_final_cell_state(my_universe, mpi_myrank);

	// cleanup universe and ghost rows
	cleanup(my_universe);

	// end timers
	if(mpi_myrank == 0) {
		endtime = MPI_Wtime();
		printf("That took %f seconds\n", endtime-starttime);
	}
	// END -Perform a barrier and then leave MPI
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return EXIT_SUCCESS;
}

/***************************************************************************/
/* Function: compute_one_tick **********************************************/
/***************************************************************************/

void compute_one_tick(unsigned int **universe, int mpi_myrank)
{
	// initialize and allocate new universe
	int i, j;
	unsigned int **new_universe;
	new_universe = calloc(g_rowsize + 2, sizeof(unsigned int *));
	for(i = 0; i < g_rowsize + 2; ++i) {
		// allocate rows
		new_universe[i] = calloc(g_x_cell_size, sizeof(unsigned int));
		for(j = 0; j < g_x_cell_size; ++j) {
			if(i != 0 && i != g_rowsize + 1) {
				// initialize non-ghost rows
				new_universe[i][j] = 0;
			}
		}
	}

	// calculate randomization through rank rng
	for(i = 1; i < g_rowsize + 1; ++i) {
		// rng is within threshold, recompute the entire row
		if(GenVal(mpi_myrank) < g_thresh_hold) {
			for(j = 0; j < g_x_cell_size; ++j)
				new_universe[i][j] = (int)(GenVal(mpi_myrank) * 100.0) % 2;
		}

		// rng is not within threshold, execute game of life rules
		else {
			for(j = 0; j < g_x_cell_size; ++j) {
				int a = adjacent(universe, j, i); // i is the row
				if(a == 2) new_universe[i][j] = universe[i][j];
				if(a == 3) new_universe[i][j] = 1;
				if(a < 2 || a > 3) new_universe[i][j] = 0;
			}
		}
	}

	// copy values over
	for(i = 1; i < g_rowsize + 1; ++i) {
		for(j = 0; j < g_x_cell_size; ++j) {
			universe[i][j] = new_universe[i][j];
		}
	}

	// clean up
	cleanup(new_universe);
}


/***************************************************************************/
/* Function: output_final_cell_state ***************************************/
/***************************************************************************/

void output_final_cell_state(unsigned int **universe, int mpi_myrank)
{
	// master (rank 0) receives rows from workers (all other ranks)
	if(mpi_myrank == 0) {
		// print own rows (guaranteed first set)
		int i, j, k;
		for(i = 1; i < g_rowsize + 1; ++i) {
			for(j = 0; j < g_x_cell_size; ++j) {
				printf("%d ", universe[i][j]);
			}
			printf("\n");
		}

		// receive rows sequentially by rank
		int ranks = g_y_cell_size / g_rowsize;
		for(i = 1; i < ranks; ++i) {
			// initialize buffers and mpi stuff
			MPI_Status status;
			unsigned int *recvbuf = NULL;
			int bufsize = g_x_cell_size;

			// allocate receiving buf
			recvbuf = calloc(bufsize, sizeof(unsigned int));

			// receive rows and print them
			for(j = 0; j < g_rowsize; ++j) {
				MPI_Recv(recvbuf, bufsize, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &status);
				for(k = 0; k < g_x_cell_size; ++k)
					printf("%d ", recvbuf[k]);
				printf("\n");
			}

			// cleanup buffer
			free(recvbuf);
		}
	}

	// workers just send their rows
	else {
		int i;
		for(i = 1; i < g_rowsize + 1; ++i) {
			// initialize buffers
			unsigned int *sendbuf = NULL;
			int bufsize = g_x_cell_size;

			// allocate and copy sendbufs
			sendbuf = calloc(bufsize, sizeof(unsigned int));
			memcpy(sendbuf, universe[i], bufsize * sizeof(unsigned int));

			// send row
			MPI_Send(sendbuf, bufsize, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);

			// cleanup buffer
			free(sendbuf);
		}
	}
}

/***************************************************************************/
/* Function: adjacent ******************************************************/
/***************************************************************************/

int adjacent(unsigned int **universe, int x, int y) {
	int count, i, j;
	count = 0;

	// check area around cell
	// due to ghost rows we can just check above no matter which cell we're looking at
	for(i = -1; i <= 1; ++i)
		for(j = -1; j <= 1; ++j)
			if(i || j)
				// if nonzero, then calculate wraparound and count
				if(universe[ywrap(y, j)][xwrap(x, i)]) ++count;

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
	// thanks to ghost rows we can simply check above and below
	return i + a;
}

/***************************************************************************/
/* Function: cleanup *******************************************************/
/***************************************************************************/

void cleanup(unsigned int **universe)
{
	int i;
	for(i = 0; i < g_rowsize; ++i) {
		free(universe[i]);
	}
	free(universe);
}
