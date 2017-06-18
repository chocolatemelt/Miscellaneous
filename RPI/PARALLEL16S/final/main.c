#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <mpi.h>

/*******************************
 * GLOBALS
 ******************************/

int g_xres; // total resolution width
int g_yres; // total resolution height

float g_xmin; // min x world coordinate
float g_ymin; // min y world coordinate
float g_xmax; // max x world coordinate
float g_ymax; // max y world coordinate

char *g_out; // output filepath

int g_threads; // number of threads per rank
int g_rankrows; // number of mpi rank rows

char* g_world; // world array
int g_my_xres; // "my" resolution width
int g_my_yres; // "my" resolution height

int mpi_myrank; // my mpi rank
int mpi_commsize; // total mpi ranks

const int PIXEL = 12; // width of a pixel (RRR GGG BBB )

/*******************************
 * FUNCTIONS
 ******************************/

void create_world();
void destroy_world();
void mandelbrot();

float square(float);
void colorize(int, int);

/*******************************
 * main()
 ******************************/

int main(int argc, char *argv[]) {
	// check arguments
	if(argc != 10) {
		printf("ERROR: Not enough arguments.\n");
		return EXIT_FAILURE;
	}

	// start MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_myrank);

	// initialize globals
	g_xres = atoi(argv[1]);
	g_yres = atoi(argv[2]);
	g_xmin = atof(argv[3]);
	g_ymin = atof(argv[4]);
	g_xmax = atof(argv[5]);
	g_ymax = atof(argv[6]);
	g_out = argv[7];
	g_threads = atoi(argv[8]);
	g_rankrows = atoi(argv[9]);

	// initialize the ppm file
	// extremely small task so we assign rank 0 to do it, could be any though
	if(mpi_myrank == 0) {
		FILE *fp = fopen(g_out, "wb");
		// PPM format:
		// header
		// width
		// height
		// max color value (> 0, < 65536)
		// actual image data
		fprintf(fp, "P3 %d %d 255\n", g_xres, g_yres);
		fclose(fp);
	}

	// determine width x height
	g_my_xres = g_xres / mpi_commsize / g_rankrows;
	g_my_yres = g_yres / g_rankrows;

	// create our world
	create_world();

	// wait for catch up
	MPI_Barrier(MPI_COMM_WORLD);

	// make our mandelbrot portion
	mandelbrot();

	// output this rank's data to the ppm
	int initial = 3 + strlen(argv[1]) + 1 + strlen(argv[2]) + 5;
	MPI_Offset offset = (initial * sizeof(char)) +
		(mpi_myrank * g_my_xres * g_my_yres * PIXEL * sizeof(char));
	MPI_File file;
	MPI_Status iostatus;
	MPI_Datatype localarray;

	/* create a type describing our piece of the array */
	int globalsizes[2] = {initial + g_xres * g_yres * PIXEL};
	int localsizes [2] = {g_my_xres * g_my_yres * PIXEL};
	int starts[2]      = {initial + mpi_myrank * g_my_xres * g_my_yres * PIXEL};
	int order          = MPI_ORDER_C;
	MPI_Type_create_subarray(1, globalsizes, localsizes, starts, order, MPI_CHAR, &localarray);
	MPI_Type_commit(&localarray);

	// open the file, and set the view
	MPI_File_open(MPI_COMM_WORLD, g_out,
			MPI_MODE_CREATE|MPI_MODE_WRONLY,
			MPI_INFO_NULL, &file);
	MPI_File_set_view(file, 0,  MPI_CHAR, localarray, "native", MPI_INFO_NULL);

	// write to file at specified offset
	MPI_File_write_at(file, offset, g_world, g_my_xres * g_my_yres * PIXEL, MPI_CHAR, &iostatus);
	MPI_File_close(&file);

	// END - barrier and exit
	destroy_world();
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return EXIT_SUCCESS;
}

/*******************************
 * mandelbrot()
 ******************************/

void mandelbrot() {
	// iterators
	int i, j;

	// world coordinate
	// i.e. our local section of the greater universe, calculated with iterators included
	int iX, iY;

	// plane coordinate
	// i.e. the actual coordinates (hence floats instead of ints)
	float pX, pY;

	// choose pixel widths and heights
	float px_width = (g_xmin - g_xmax) / g_xres;
	float px_height = (g_ymin - g_ymax) / g_yres;

	// z = zX + zY * itr
	// zX2 = zX ^ 2
	// zY2 = zY ^ 2
	float zX, zY;
	float zX2, zY2;

	// color
	// monochrome, so R/G/B values are identical
	int color;

	// escape value
	unsigned int max = 255;

	// current iteration
	int itr;

	// cirle radius
	const float radius = 2;
	float r2 = square(radius);

	// begin computation
	for(i = 0; i < g_my_yres; ++i) {
		// variables to calculate starting points (iX, iY at 0)
		int pos_x = (mpi_myrank % (mpi_commsize / g_rankrows)) - 1;
		if(pos_x == -1) pos_x = mpi_commsize / g_rankrows - 1;
		int pos_y = mpi_myrank / g_rankrows;
		int start_x = pos_x * g_my_xres;
		int start_y = pos_y * g_my_yres;

		iY = start_y + i;
		pY = g_ymin + i * px_height;

		// main antenna
		if(fabs(pY) < px_height / 2) pY = 0.0;

		for(j = 0; j < g_my_xres; ++j) {
			iX = start_x + j;
			pX = g_xmin + i * px_width;

			// initial value of orbit is at critical point z = 0
			zX = 0.0;
			zY = 0.0;
			zX2 = square(zX);
			zY2 = square(zX);

			for(itr = 0; itr < max && ((zX2 + zY2) < r2); itr++) {
				zY = 2 * zX * zY + pY;
				zX = zX2 - zY2 + pX;
				zX2 = square(zX);
				zY2 = square(zY);
			}

			// color pixels
			color = 255 - itr;

			// store pixel
			colorize((j * g_my_xres + i) * PIXEL, color);
		}
	}
}

/*******************************
 * create_world()
 ******************************/

void create_world() {
	g_world = (char *)malloc(g_my_xres * g_my_yres * PIXEL * sizeof(char));
}

/*******************************
 * destroy_world()
 ******************************/

void destroy_world() {
	free(g_world);
}

/*******************************
 * square()
 ******************************/

float square(float input) {
	return input * input;
}

/*******************************
 * color()
 ******************************/

void colorize(int index, int color) {
	// get the digits
	int hundreds, tens, ones;
	hundreds = color % 1000 / 100;
	tens = color % 100 / 10;
	ones = color % 10;

	// print
	int i;
	for(i = 0; i < 3; ++i) {
		g_world[index + i * 4 + 0] = '0' + hundreds;
		g_world[index + i * 4 + 1] = '0' + tens;
		g_world[index + i * 4 + 2] = '0' + ones;
		g_world[index + i * 4 + 3] = ' ';
	}
}
