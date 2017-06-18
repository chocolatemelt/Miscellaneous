/*
 * Author: Kevin Zhang
 * Section: 01
 * Title: bonus.c
 * Comments:
 * I included a size member to the struct to keep track of the size of the array 'lines', which
 * holds 100 integers for the maximum possible number of lines per identifier according to
 * the pdf.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* struct for identifier reference table */
struct tblref {
    char identifier[11];
    int defined;
    int lines[100];
    int size;
};

/* function list */
void help(char*);
int analyze(FILE*, struct tblref*);
void print_file(FILE*, FILE*);
void print_table(FILE*, struct tblref*, int);
int whitespace(char);
int find(char*, struct tblref*, int);

int main(int argc, char **argv) {
    /* Preliminary checks and argument handling */
    if(argc == 1) { // if user only inputs the program name
        help(argv[0]);
    }
    /* Actual execution */
    else if(argc == 3) {
    	FILE *inf, *outf;
    	struct tblref references[100]; // array holding all references
    	int tblsize; // size of the above array

        /* check input filestream availability */
        if((inf = fopen(argv[1],"r")) == NULL) {
            printf("Error: File %s could not be opened for reading.\n", argv[1]);
            return 0;
        }

        /* check output filestream availability */
        if((outf = fopen(argv[2],"w")) == NULL) {
            printf("Error: File %s could not be created for writing.\n", argv[3]);
            return 0;
        }

        /* read file */
        tblsize = analyze(inf, references);

        /* print file */
        print_file(inf, outf);

        /* print table */
        print_table(outf, references, tblsize);

        /* close filestreams */
        fclose(inf);
        fclose(outf);
    }
    /* Errors */
    else {
    	printf("Error: Wrong number of arguments. Run without arguments for usage information.\n");
    }
    return 0;
}

/*
 * help(char*)
 * Prints out the help and usage information to the standard output.
 */
void help(char args[])  {
    printf("Usage: %s [INFILE] [OUTFILE]\n", args);
    printf("Summary:\n");
    printf("Analyzes a MIPS program and prints out a reference table of identifiers.\n");
}

/*
 * analyze(FILE*, struct tblref*)
 * Reads in the file and updates the given reference table array.
 */
int analyze(FILE *inf, struct tblref *references) {
	rewind(inf); // just in case
	char *str = malloc(81 * sizeof(char)); // including the null character
	char *token;
	int line = 1; // line number
	int tblsize = 0; // table size
	int index; // index of the table
	while(fgets(str, 81, inf)) {
		token = strtok(str, " \t");
		while(token != NULL) {

			/* if it's a comment, we can exit the loop */
			/* same for quotes */
			if(token[0] == '#' || token[0] == '\'' || token[0] == '\"') break;

			/* if it ends in a newline, get rid of it */
			if(token[strlen(token) - 1] == '\n') token[strlen(token) - 1] = '\0';

			/* if it has a ':', it's a label */
			if(token[strlen(token) - 1] == ':') {
				token[strlen(token) - 1] = '\0';
				strcpy(references[tblsize].identifier, token);
				references[tblsize].defined = line;
				references[tblsize].size = 0;
				tblsize++;
			}

			/* otherwise, we can check if it's used */
			else {
				index = find(token, references, tblsize);
				if(index != -1) {
					references[index].lines[references[index].size++] = line;
				}
			}

			/* get next token */
			token = strtok(NULL, " \t");

		}
		line++;
	}
	return tblsize;
}

/*
 * print_file(FILE*, FILE*)
 * Prints the input file.
 */
void print_file(FILE* inf, FILE* outf){
	rewind(inf);
	char c;
	int newline = 1; // new line boolean
	int line = 1; // line number
	while((c = fgetc(inf)) != EOF) {

		if(newline) {
			if(c != '\n') {
				fprintf(outf, "%d\t", line++);
				newline = 0;
			}
		} // print line number

		if(c == '\n') newline = 1;
		fprintf(outf, "%c", c);

	}
}

/*
 * print_table(FILE*, struct tblref*, int)
 * Prints the reference table array.
 */
void print_table(FILE *outf, struct tblref* references, int tblsize) {
	fprintf(outf, "\nIdentifier Reference Table\n\n");
	fprintf(outf, "\tIdentifier Definition-Line Use Line(s)\n");
	int i, j;
	for(i = 0; i < tblsize; ++i) {
		fprintf(outf, "\t%-11s", references[i].identifier);
		fprintf(outf, "%-16d", references[i].defined);
		for(j = 0; j < references[i].size; ++j) {
			fprintf(outf, "%-4d", references[i].lines[j]);
		}
		fprintf(outf, "\n");
	}
}

/*
 * int whitespace(char)
 * Small helper function that determines whether something is whitespace.
 */
 int whitespace(char c) {
 	if(c == EOF || c == '\r' || c == '\t' || c == '\n' || c == ' ') return 1;
 	return 0;
 }

 /*
  * int find(char*, struct tblref*, int)
  * Small helper function that finds the string in the table and returns its index.
  * Returns -1 if not found.
  */
int find(char *str, struct tblref *references, int tblsize) {
	int i;
	for(i = 0; i < tblsize; ++i) {
		if(strcmp(references[i].identifier, str) == 0) return i;
	}
	return -1;
}