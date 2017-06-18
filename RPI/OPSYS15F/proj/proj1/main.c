#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* process struct */
struct process {
	int num;      // proc-num
	int b_t;      // burst-time
	int b_n;      // num-burst
	int io_t;     // io-time
	int enq;      // enqueue number, -1 = not in queue
};

/* functions */
char *trim(char *);
void event_call(int, int, char, struct process *);
void pop(struct process *);
void push(struct process *, int);
int  nextp(struct process *);
int done(struct process *);

int main(int argc, char **argv) {
	/* SIMULATION VARIABLES */
	int n = 0;      // processes
	int cs_t = 13;  // context switch time (in ms)
	int cur_t;      // current time (in ms)
	FILE* inf;      // input file (processes.txt)
	
	if(argc != 1) {
		printf("There are no arguments associated with this simulation. Input is read from processes.txt.\n");
		return 1;
	}

	// processes array, beginning with 10
	// realloc when necessary
	int capacity = 10;
	struct process *processes = malloc(capacity * sizeof(struct process));

	/* read file */
	if((inf = fopen("processes.txt", "r")) != NULL) {
		// get current line
		char *line = NULL;
		size_t nbytes = 0;

		while(getline(&line, &nbytes, inf) != -1) {
			// preemptive check for capacity
			if(n == capacity) {
				capacity += 10;
				processes = realloc(processes, capacity * sizeof(struct process));
			}
			
			// trim the string first
			char *newline;
			newline = trim(line);

			if(*newline != 0) {
				// get values
				int pn, bt, bn, it;
				sscanf(newline, "%i|%i|%i|%i", &pn, &bt, &bn, &it);

				// store values
				processes[n].num  = pn;
				processes[n].b_t  = bt;
				processes[n].b_n  = bn;
				processes[n].io_t = it;
				processes[n].enq  = n;

				++n;
			}
		}

		// free memory
		free(line);

		/* ===begin simulation=== */
		
		/**
		 * status is a numerical indicator of the state
		 * the current process is in:
		 * 0 - not running / terminated
		 * 1 - using cpu
		 * 2 - performing i/o
		 */
		
		// initialize timekeeping
		cur_t = 0;
		int cur_cs_t = cs_t; // current context switch time
		int proc_t[sizeof(processes)] = { -1 }; // individual prcess times
		int proc_s[sizeof(processes)] = { 0 };  // individual process status
		int i; // for loop iterator
		int in_use = 0; // defines whether a processor is in use or not

		// copy processes to queue
		struct process queue[sizeof(processes)];
		memcpy(queue, processes, sizeof(struct process) * sizeof(processes));

		event_call(cur_t, 0, '0', queue);

		// start timer!
		while(cur_t != -1) {
			++cur_t;
			
			// switching context
			if(cur_cs_t > 0) --cur_cs_t;

			// switched context
			if(cur_cs_t == 0) {
				--cur_cs_t;
				
				// check if open process available and run it if possible
				int idx = nextp(queue);
				if(idx != -1 && !in_use) {
					in_use = 1;
					int process = queue[idx].num;
					proc_s[idx] = 1;  // set status to using cpu
					proc_t[idx] = queue[idx].b_t + 1; // start timer
					--queue[idx].b_n; // decrement burst number
					pop(queue);     // pop the highest process
					event_call(cur_t, process, '1', queue);
				}
			}
			
			// process timers
		 	for(i = 0; i < sizeof(processes); ++i) {

		 		// check if theres a timer associated with that process
				if(proc_t[i] > 0) {
					--proc_t[i];
					
					// handle events
					if(proc_t[i] == 0) {	

						// completed cpu burst
						if(proc_s[i] == 1) {
							proc_s[i] = 2;
							proc_t[i] = queue[i].io_t;
							in_use = 0;
							cur_cs_t = cs_t; // switch contexts if possible

							// if process does not require i/o
							if(proc_t[i] == 0) {
								if(queue[i].b_n <= 0) {
									event_call(cur_t, queue[i].num, '5', queue);
									if(done(queue)) {
										event_call(cur_t, 0, '6', queue);
										cur_t = -1;
									}
								}
								else {
									push(queue, i);
									event_call(cur_t, queue[i].num, '2', queue);
								}
								proc_s[i] = 0;
							}
							else {
								if(queue[i].b_n != 0) {
									event_call(cur_t, queue[i].num, '2', queue);
									event_call(cur_t, queue[i].num, '3', queue);
								}
								// terminate process
								else {
									proc_t[i] = 0;
									proc_s[i] = 0;
									event_call(cur_t, queue[i].num, '5', queue);
									if(done(queue)) {
										event_call(cur_t, 0, '6', queue);
										cur_t = -1;
									}
								}
							}
						}
						
						// completed i/o
						else if(proc_s[i] == 2) {
							proc_s[i] = 0;
							if(!in_use) cur_cs_t = cs_t;

							// finished burst
							if(queue[i].b_n <= 0) {
								if(done(queue)) {
									event_call(cur_t, 0, '6', queue);
									cur_t = -1;
								}
								else event_call(cur_t, queue[i].num, '5', queue);
							}
							else {
								push(queue, i);
								event_call(cur_t, queue[i].num, '4', queue);
							}
						}
					}
				}
			}
		}
	}

	else {
		printf("Error: Couldn't open processes.txt for reading.\n");
	}
	
	// free memory and exit gracefully
	free(processes);
	return 0;
}

/**
 * removes whitespace and comments
 */
char *trim(char *str) {
	// trim leading space
	while(isspace((unsigned char)*str)) str++;
	
	// return if nothing left
	if(*str == 0) return str;

	// null terminate if comment found
	int i;
	for(i = 0; i < strlen(str); ++i) {
		if(str[i] == '#') {
			str[i] = '\0';
			break;
		}
	}

	return str;
}

/**
 * outputs events
 * uses [call] to determine the event to output
 * does NOT handle timekeeping
 */
void event_call(int time, int proc, char call, struct process *queue) {
	printf("time %ims: ", time);
	switch(call) {
		case '0':
			printf("Simulator started ");
			break;
		case '1':
			printf("P%i started using the CPU ", proc);
			break;
		case '2':
			printf("P%i completed its CPU burst ", proc);
			break;
		case '3':
			printf("P%i performing I/O ", proc);
			break;
		case '4':
			printf("P%i completed I/O ", proc);
			break;
		case '5':
			printf("P%i terminated ", proc);
			break;
		default:
			printf("Simulator ended\n");
			return;
	}
	printf("[Q");
	int i;
	int q = 0;
	while(q <= sizeof(queue)) {
		for(i = 0; i < sizeof(queue); ++i) {
			if(queue[i].enq == q) printf(" %i", queue[i].num);
		}
		++q;
	}
	printf("]\n");
}

/**
 * pops the newest processor
 */
void pop(struct process *queue) {
	int i;
	for(i = 0; i < sizeof(queue); ++i) {
		if(queue[i].enq != -1) --queue[i].enq;
	}
}

/**
 * adds a processor to the queue that isn't already in the queue
 */
void push(struct process *queue, int proc_num) {
	if(queue[proc_num].enq < 0) {
		int enq = -1;
		int i;
		for(i = 0; i < sizeof(queue); ++i) {
			if(queue[i].enq > enq) enq = queue[i].enq;
		}
		queue[proc_num].enq = enq + 1;
	}
}

/**
 * finds the next processor in line and gets its index
 * returns -1 if nothing is enqueued
 */
int nextp(struct process *queue) {
	int i;
	for(i = 0; i < sizeof(queue); ++i) {
		if(queue[i].enq == 0) return i;
	}
	return -1;
}

/**
 * checks if simulation is finished by checking num-burst
 */
int done(struct process *queue) {
	int i;
	for(i = 0; i < sizeof(queue); ++i) {
		if(queue[i].b_n > 0) return 0;
	}
	return 1;
}