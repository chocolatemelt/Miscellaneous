/* shared.h */

#define BUFFER_SIZE 8

#define SHM_SHARED_KEY 6789

struct shared_data
{
  int buffer[BUFFER_SIZE];   /* circular array */
  int in;   /* index of next available producer array slot */
  int out;  /* index of next array slot to consume from */
  int count; /* number of elements in the buffer */
};

