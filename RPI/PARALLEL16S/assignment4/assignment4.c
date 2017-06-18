#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<math.h>
#include<clcg4.h>
#include<mpi.h>
#include<hwi/include/bqc/A2_inlines.h>

/***************************************************************************/
/* Global Variables ********************************************************/
/***************************************************************************/

int g_matrix_size;
int g_row_size;
int g_threads;
int g_file_ranks;
char *g_out;

int mpi_myrank;
int mpi_commsize;

unsigned int *my_matrix;
unsigned int *my_transpose;

pthread_mutex_t mutexsum;

/***************************************************************************/
/* Function Decs ***********************************************************/
/***************************************************************************/

void *mtx_sum(void *);

/***************************************************************************/
/* Function: Main **********************************************************/
/***************************************************************************/

int main(int argc, char *argv[]) {
	if(argc != 5) {
		printf("ERROR: Not enough arguments.\n");
		return EXIT_FAILURE;
	}

	// Example MPI startup and using CLCG4 RNG
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_myrank);

	// assign cli args to globals
	g_matrix_size = atoi(argv[1]);
	g_threads = atoi(argv[2]);
	g_file_ranks = atoi(argv[3]);
	g_out = argv[4];
	g_row_size = g_matrix_size / mpi_commsize;

	// Init 16,384 RNG streams - each rank has an independent stream
	InitDefault();

	MPI_Barrier(MPI_COMM_WORLD);

	// my_matrix stores a local slice of the 128gb matrix
	// my_transpose stores the transpose of that slice
	my_matrix = calloc(g_row_size * g_matrix_size, sizeof(unsigned int));
	my_transpose = calloc(g_row_size * g_matrix_size, sizeof(unsigned int));

	// initialize and randomize matrix thru mpi ranks
	// each rank holds some number of rows
	// held in a 1d array to make mpi sending easier
	unsigned int i, j, k, l;
	for(i = 0; i < g_row_size; ++i) {
		for(j = 0; j < g_matrix_size; ++j) {
			my_matrix[i * g_matrix_size + j] = (unsigned int)(GenVal(mpi_myrank) * 100.0) + 1;
		}
	}

	// populate transpose with own values
	unsigned int start_idx = mpi_myrank * g_row_size;
	for(i = 0; i < g_row_size; ++i) {
		for(j = start_idx; j < g_row_size * g_matrix_size; j = j + g_matrix_size) {
			// calculation for the matrix
			k = (j - start_idx) / g_matrix_size + start_idx;
			my_transpose[i + j] = my_matrix[i * g_matrix_size + k];
		}
	}

	// initialize and allocate buffers
	unsigned int bufsize = g_row_size * g_row_size;
	unsigned int *sendbuf = calloc(bufsize, sizeof(unsigned int *));
	unsigned int **recvbuf = calloc(mpi_commsize-1, sizeof(unsigned int *));
	for(i = 0; i < mpi_commsize-1; ++i) {
		recvbuf[i] = calloc(bufsize, sizeof(unsigned int));
	}

	// mpi stuff
	unsigned int num_reqs = 2 * (mpi_commsize - 1);
	unsigned int cur_req = 0;
	MPI_Request *requests = (MPI_Request *)malloc(num_reqs * sizeof(MPI_Request));
	MPI_Status *statuses = (MPI_Status *)malloc(num_reqs * sizeof(MPI_Status));

	// send to all other ranks
	for(i = 0; i < mpi_commsize; ++i) {
		if(i != mpi_myrank) {
			// store relevant data for the receiving rank
			int cnt = 0;
			int tx = 0;
			start_idx = i * g_row_size;
			for(j = 0; j < g_row_size; ++j) {
				for(k = start_idx; k < g_row_size * g_matrix_size; k = k + g_matrix_size) {
					// calculation for the matrix (a little messy, could be optimized)
					l = (k - start_idx) / g_matrix_size + start_idx;
					if(cnt >= bufsize) {
						// handles the overflow, after which we offset it (new column)
						cnt = ++tx;
					}
					sendbuf[cnt] = my_matrix[j * g_matrix_size + l];
					cnt += g_row_size;
				}
			}
			MPI_Isend(sendbuf, bufsize, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &requests[cur_req++]);
		}
	}

	// recv from all other rows
	// handling a little messy since irecv is nonblocking
	int cnt = 0;
	for(i = 0; i < mpi_commsize; ++i) {
		if(i != mpi_myrank) {
			MPI_Irecv(recvbuf[cnt++], bufsize, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, &requests[cur_req++]);
		}
	}

	// wait on MPI messages
	MPI_Waitall(num_reqs, requests, statuses);

	// store relevant values
	k = 0;
	l = 0;
	for(i = 0; i < g_row_size; ++i) {
		for(j = 0; j < g_matrix_size; ++j) {
			if(my_transpose[i * g_matrix_size + j] == 0) {
				my_transpose[i * g_matrix_size + j] = recvbuf[k][l++];
				if(l >= bufsize) {
					k++;
					l = 0;
				}
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	unsigned long long t1 = 0, t2 = 0;
	if(mpi_myrank == 0) {
		t1 = GetTimeBase();
	}

	// split into pthreads
	pthread_t *call_thd;
	call_thd = (pthread_t *)malloc(g_threads * sizeof(pthread_t));
	void *status;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&mutexsum, NULL);

	long x;
	for(x = 0; x < g_threads; ++x) {
		pthread_create(&call_thd[i], &attr, mtx_sum, (void *)x);
	}

	// wait on threads
	for(x = 0; x < g_threads; ++x) {
		pthread_join(call_thd[i], &status);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(mpi_myrank == 0) {
		t2 = GetTimeBase();
		float tmp = (t2-t1) / 1600000;
		printf("Elapsed compute time: %f\n", tmp);
	}

	// I/O
	if(mpi_myrank == 0) {
		t1 = GetTimeBase();
	}
	MPI_Offset offset = (mpi_myrank % g_file_ranks) * g_row_size * g_matrix_size * sizeof(unsigned int);
	MPI_File file;
	MPI_Status iostatus;
	MPI_Datatype localarray;

	/* create a type describing our piece of the array */
	int globalsizes[2] = {g_matrix_size, g_matrix_size};
	int localsizes [2] = {g_row_size, g_matrix_size};
	int starts[2]      = {mpi_myrank * g_row_size, 0};
	int order          = MPI_ORDER_C;
	MPI_Type_create_subarray(2, globalsizes, localsizes, starts, order, MPI_UNSIGNED, &localarray);
	MPI_Type_commit(&localarray);

	// open the file, and set the view
	MPI_File_open(MPI_COMM_WORLD, g_out,
			MPI_MODE_CREATE|MPI_MODE_WRONLY,
			MPI_INFO_NULL, &file);
	MPI_File_set_view(file, 0,  MPI_UNSIGNED, localarray, "native", MPI_INFO_NULL);

	// write to file at specified offset
	MPI_File_write_at(file, offset * mpi_myrank, my_matrix, g_row_size * g_matrix_size, MPI_UNSIGNED, &iostatus);
	MPI_File_close(&file);
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(mpi_myrank == 0) {
		t2 = GetTimeBase();
		float tmp = (t2-t1) / 1600000;
		printf("Elapsed IO time: %f\n", tmp);
	}

	// cleanup routine

	MPI_Type_free(&localarray);

	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutexsum);
	free(call_thd);

	free(my_matrix);
	free(my_transpose);

	for(i = 0; i < mpi_commsize-1; ++i) free(recvbuf[i]);
	free(recvbuf);
	free(sendbuf);
	free(requests);
	free(statuses);

	// END -Perform a barrier and then leave MPI
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return EXIT_SUCCESS;
}

/***************************************************************************/
/* Function: matrix sum ****************************************************/
/***************************************************************************/

void *mtx_sum(void *arg) {
	long me = (long)arg;
	// get thread size and starting index
	unsigned int thd_size = (g_row_size * g_matrix_size) / g_threads;
	unsigned int start = me * thd_size;

	// sum
	int i;
		pthread_mutex_lock(&mutexsum);
	for(i = start; i < start + thd_size; ++i) {
		my_matrix[i] += my_transpose[i];
	}
		pthread_mutex_unlock(&mutexsum);

	pthread_exit((void *)0);
}
