/**
 * Kevin Zhang (zhangk6)
 * Homework 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* function list */
void help(char*);
void *walloc(char**, int*);
int  match(char*, char*);

int main(int argc, char **argv) {
	if(argc == 1) {
		help(argv[0]);
	}

	else if(argc != 3) {
		printf("Error: Wrong number of arguments.\n");
	}

	else {
		FILE *inf;	 // input file
		char **buf;  // char pointer array
		int sz = 20; // current size of array
		
		/* open file */
		if((inf = fopen(argv[1], "r")) == NULL) {
			printf("Error: File %s unable to be opened for reading.", argv[1]);
			return 0;
		}

		/* initialize character buffer */
		buf = calloc(20, sizeof(char*));
		printf("Allocated initial array of 20 character pointers.\n");
		
				
		/* read file */
		int idx = 0;
		char *w;
		while(fscanf(inf, "%ms", &w) == 1) {
			buf[idx] = strdup(w);
			idx++;

			/* check array size */
			if(idx == sz) {
				/* reallocate memory */
				buf = walloc(buf, &sz);

				/* failure case */
				if(buf == NULL) {
					printf("Error: Reallocating memory failed (likely input file too large).\n");
					return 0;
				}
			}

			free(w);
		}

		/* analyze array */
		int i;
		for(i = 0; i < idx; ++i) {
			if(match(buf[i], argv[2])) printf("%s\n", buf[i]);
		}

		/* finished: close file and free remaining blocks */
		fclose(inf);
		i = 0;
		for(i = 0; i < idx; ++i) free(buf[i]);
		free(buf);
	}

	return 0;
}

// prints usage
void help(char args[]) {
	printf("Usage: %s [FILE] [PREFIX]\n", args);
}

// word alloc. allocates a new buffer size by
// doubling the previous buffer size.
void *walloc(char** buffer, int* size) {
	/* get new size */
	*size *= 2;

	/* realloc with new size */ 
	buffer = realloc(buffer, *size * sizeof(char*));

	/* allocation successful */
	if(buffer != NULL) printf("Reallocated array of %d character pointers.\n", *size);

	return buffer;
}

// matches the beginning of [word] with [match].
// returns true if [match] is at the beginning of
// [word] and false otherwise.
int match(char *word, char *match) {
	int i;
	for(i = 0; i < strlen(match); ++i) {
		if(word[i] != match[i]) return 0;
	}
	return 1;
}
