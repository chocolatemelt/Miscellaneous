/*
 * Author: Kevin Zhang
 * Section: 01
 * Title: nibzip.c
 * Comments:
 * I included <stdint.h> to guarantee an 8 bit integer for cross-platform independence.
 * This also lets me guarantee bitwise operators within 8 bits.
 * I also note that DOS platforms use \r\n as new line and such would not be abe to
 * compress correctly ('\r' not being a supported character).
 * My compression algorithm stores every 2 characters as a byte. In the case that there
 * is an uneven amount of characters, the last 4 bits of the last byte is entered as 
 * '1111' or 15, a nibble not used in the character table.
 * My uncompression algorithm reverts this process and ends when it hits either the
 * EOF or the '1111' sentinel value. However, if it encounters this value before the null
 * byte (EOF), it returns 1, signaling an error in the uncompression process.
 */
#include <stdio.h> // general I/O functions
#include <stdlib.h> // certain overloaded functions (malloc in particular)
#include <stdint.h> // guarantee of unsigned 8 bit integer
#define MAXBUF 500000000 // 500MB maximum filesize for buffer 

/* function list */
void help(char*);
int mktable(char*);
uint8_t getnib(char);
char getch(uint8_t);
char* readfile(FILE*);
void compress(char*, FILE*);
int uncompress(char*, FILE*);

/* struct for character distribution table */
struct tblchar {
    char c;
    int count;
};

int main(int argc, char **argv) {
    /* Preliminary checks and argument handling */
    if(argc == 1) { // if user only inputs the program name
        help(argv[0]);
    }

    else if(strcmp(argv[1], "-h") == 0 && argc != 2
         || strcmp(argv[1], "-a") == 0 && argc != 3
         || strcmp(argv[1], "-c") == 0 && argc != 4
         || strcmp(argv[1], "-d") == 0 && argc != 4) {
        printf("Error: Wrong number of arguments. See -h for usage information.\n");
        return 0;
    }

    else {
        if(strcmp(argv[1], "-h") == 0) {
            help(argv[0]);
        } // help

        else if(strcmp(argv[1], "-a") == 0
             || strcmp(argv[1], "-c") == 0) {
            FILE *inf;
            char *buffer;
            /* check input filestream availability */
            if((inf = fopen(argv[2],"r")) == NULL) {
                printf("Error: File %s could not be opened for reading.\n", argv[2]);
                return 0;
            }
            /* read file */
            buffer = readfile(inf);

            if(strcmp(argv[1], "-a") == 0) {
                mktable(buffer);
            } // analyze

            else if(strcmp(argv[1], "-c") == 0) {
                FILE *outf;
                /* check output filestream availability */
                if((outf = fopen(argv[3],"wb")) == NULL) {
                    printf("Error: File %s could not be created for writing.\n", argv[3]);
                    return 0;
                }

                if(strcmp(argv[1], "-c") == 0) {
                    if(mktable(buffer) == 0) {
                        compress(buffer, outf);
                        /* get filesize */
                        fseek(outf, 0L, SEEK_END);
                        int sz = ftell(outf);
                        printf("\nOutput File: %s\n\twas compressed successfully. New size is %d bytes.\n", argv[3], sz);
                    }
                    else {
                        printf("\nOutput File: %s\n\twas not compressed.\n", argv[3]);
                    }
                }

                fclose(outf); // close the output filestream
            } // compress
            
            /* free memory since we're done analyzing / compressing */
            free(buffer);
            fclose(inf);
        } // -a, -c flags (all use nonbinary input stream)

        else if(strcmp(argv[1], "-d") == 0) {
            FILE *inf;
            char *buffer;
            /* check input filestream availability */
            if((inf = fopen(argv[2],"rb")) == NULL) {
                printf("Error: File %s could not be opened for reading.\n", argv[2]);
                return 0;
            }
            /* read file */
            buffer = readfile(inf);

            FILE *outf;
            /* check output filestream availability */
            if((outf = fopen(argv[3],"w")) == NULL) {
                printf("Error: File %s could not be opened for writing.\n", argv[3]);
                return 0;
            }

            if(uncompress(buffer, outf) == 0) {
                fclose(outf);
                outf = fopen(argv[3],"r"); // now we open it as read
                char *obuffer = readfile(outf);
                mktable(obuffer);
                /* free allocated memory since we don't need it anymore */
                free(buffer);
                free(obuffer);
                /* get filesize */
                fseek(outf, 0L, SEEK_END);
                int sz = ftell(outf);
                printf("\nOutput File: %s\n\twas uncompressed successfully. File size is %d bytes.\n", argv[3], sz);
            }
            else {
                printf("\nOutput File: %s\n\twas not uncompressed. Errors were found.\n", argv[3]);
            }

            /* close filestreams */
            fclose(inf);
            fclose(outf);
        } // uncompress (uses binary input stream)

        else {
            printf("Error: flag %s does not exist. See -h for usage information.\n", argv[1]);
            return 0;
        } // error handling

    }
    return 0;
}

/*
 * help(char*)
 * Prints out the help and usage information to the standard output.
 */
void help(char args[])  {
    printf("Usage: %s [-acdh] [INFILE] [OUTFILE]\n", args);
    printf("Flags:\n");
    printf("-h: Prints help and usage information.\n");
    printf("-a: Analyzes and prints out a character distribution table computed from the input file.\n");
    printf("-c: Compresses the input file into the output file and prints out a character distribution table.\n");
    printf("-d: Decompresses the input file, writes the unpacked bytes to the output file and prints out the character distribution table.\n");
}

/*
 * readfile{FILE*)
 * Reads the file into memory and returns the char pointer.
 * Uses malloc() to dynamically allocate space to the char array. This assumes the size
 * of a char is a byte; obviously this is not guaranteed, but most modern systems consider a
 * char to be a byte in size.
 */
char* readfile(FILE *inf) {
    char *buffer = NULL; // define the buffer
    buffer = malloc(sizeof(char) * (MAXBUF + 1)); // dynamically allocate the buffer; + 1
                                                  // to support null terminator
    size_t len = fread(buffer, sizeof(char), MAXBUF, inf); // read file into buffer
    buffer[++len] = '\0'; // null terminate
    return buffer;
}

/*
 * mktable(char*)
 * Prints out the character distribution table of the given array. Returns the number of unsupported
 * characters. The character 'x' signifies unsupported characters. Uses the struct tblchar defined
 * at the top - essentially, it holds a character and the count of that character.
 */
int mktable(char *buffer) {
    struct tblchar table[16];
    char characters[] = "0123456789.-, \nx"; // char table
    
    /* populate table's character contents */
    int x;
    for(x = 0; x < 16; x++) {
        table[x].c = characters[x];
        table[x].count = 0;
    }

    int i = 0;
    printf("Character   Count\n");
    /* Populate the counts table */
    while(buffer[i] != '\0') { // basically, until we hit the null byte '\0' - C specific and added by readfile()
        switch(buffer[i]) {
            case '0':
                table[0].count++;
                break;
            case '1':
                table[1].count++;
                break;
            case '2':
                table[2].count++;
                break;
            case '3':
                table[3].count++;
                break;
            case '4':
                table[4].count++;
                break;
            case '5':
                table[5].count++;
                break;
            case '6':
                table[6].count++;
                break;
            case '7':
                table[7].count++;
                break;
            case '8':
                table[8].count++;
                break;
            case '9':
                table[9].count++;
                break;
            case ',':
                table[10].count++;
                break;
            case '-':
                table[11].count++;
                break;
            case '.':
                table[12].count++;
                break;
            case ' ':
                table[13].count++;
                break;
            case '\n':
                table[14].count++;
                break;
            default:
                table[15].count++;
        }
        i++;
    }
    /* Output character distribution table */
    i = 0; // just resetting the original counter
    for(i; i < 14; i++) {
        printf("%c %15d\n", table[i].c, table[i].count);
    }
    printf("%s %14d\n", "\\n", table[14].count);
    printf("%s %10d\n", "others", table[15].count);
    if(table[15].count > 0) {
        printf("Error: unknown character(s) encountered in analysis.\n");
    }
    return table[15].count;
}

/*
 * compress(char*, FILE*)
 * Compresses the character array into the output file. Converts each character in the array
 * to the appropriate 4 bit binary (see getnib()) and pairs them up into bytes before
 * writing to file. In the case of an odd number of characters, the last nibble will
 * be written as '1111' or 15. This will be handled by the uncompression function as
 * 'x', signaling an early EOF before the null bye '\0'.
 */
void compress(char *buffer, FILE *outf) {
    uint8_t b = 0; // byte to be written to file
    int i = 0;
    while(buffer[i] != '\0') {
        b |= (getnib((buffer[i])) << 4); // shift c into the first 4 bits
        /* place the next character to complete the byte; if it reaches the end of file it will
         * place a 15 (the only bit not recognized as a character in a nibzip file) */
        if(buffer[++i] != '\0') {
            b |= getnib(buffer[i]); // place c into the last 4 bits, completing the byte
            i++; // increment again if it's not end of file
        }
        else {
            b |= (uint8_t) 15; // place 15 into the last 4 bits, completing the byte and thus the file
            // no increment here since we have reached the end of the file
        }
        /* write to file and reset b) */
        fputc(b, outf);
        b = 0;
    }
}

/*
 * getnib(char)
 * Returns the nibblet of the char as an 8 bit int, padded with 0s. It is assumed that the character
 * being passed is a supported character; unsupported characters are returned as 15.
 */
uint8_t getnib(char c) {
     switch(c) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            return (uint8_t) c - '0'; // this gets the integer equivalent of the char
        case ',':
            return (uint8_t) 10;
        case '-':
            return (uint8_t) 11;
        case '.':
            return (uint8_t) 12;
        case ' ':
            return (uint8_t) 13;
        case '\n':
            return (uint8_t) 14;
        default:
            return (uint8_t) 15; // will signify an EOF when decompressed
    }
}

/*
 * getch(uint8_t)
 * Returns the char of an 8 bit int. It is assumed that the int being passed is one of the supported
 * characters, i.e. the first 4 bits are 0. Unsupported characters are returned as 'x' to be handled
 * accordingly.
 */
char getch(uint8_t i) {
    switch(i) {
        case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
            return (char)((int) '0') + i; // this gets the char equivalent of the integer
        case 10:
            return ',';
        case 11:
            return '-';
        case 12:
            return '.';
        case 13:
            return ' ';
        case 14:
            return '\n';
        case 15:
        default:
            return 'x'; // this is the EOF signal in the case that there is an odd number of nibbles
    }
}

/*
 * uncompress(char*, FILE*)
 * Uncompresses the input file into the output file by splitting bytes into nibbles. Returns
 * 0 if successful and 1 if unsuccessful. As long as input file being uncompressed was correctly compressed
 * via the compress function and doesn't contain unsupported characters, uncompress() should always return 0.
 */
int uncompress(char *buffer, FILE *outf) {
    int i = 0;
    while(buffer[i] != '\0') {
        /* get the character representations of each nibble */
        uint8_t first = (uint8_t) buffer[i] >> 4; // shifts by 4 to get the original integer representation
        uint8_t second = (uint8_t) buffer[i] & (uint8_t) 15; // masks the first half (0000 1111)
        char first_c = getch(first);
        char second_c = getch(second);
        fputc(first_c, outf);
        if(second_c != 'x') { // i.e. extra nibble at the end
            fputc(second_c, outf);
        }
        i++;
        if(second_c == 'x' && buffer[i] != '\0') { // unsupported conversion before end of file?
            return 1;
        }
    }
    return 0;
}
