# process-mem.c
prints argument count as well as first 3 arguments (really first 2 arguments since ```argv[0]``` is always program name)

prints sizes of some primitives

dynamically allocates space for some strings

note: ```malloc``` just allocates space. ```calloc``` allocates space and initializes it. ```malloc``` is fine for strings (we terminate at ```\0``` anyway, right?) but ```calloc``` is better for arrays in case we access something strange.

not implemented but of course we need to call ```free()``` or we get memory leaks.

note returns - ```EXIT_FAILURE``` and ```EXIT_SUCCESS```

# additional notes
there used to be a file called ```main-with-debug.c```. it's gone now.

processes line up in a queue for the CPU to handle. the next process in line is handled by the CPU, aka CPU burst.

after that the process can either 1) terminate or 2) perform/wait for I/O.

you allocate memory on the heap. everything else goes on the stack.

heap = malloc, calloc, new (in c++/java), free

stack = pretty much everything else (variables, constants, etc)
