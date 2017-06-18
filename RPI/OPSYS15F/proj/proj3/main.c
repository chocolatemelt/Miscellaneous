#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "q.h"
#include "process.h"
#include "vmemory.h"

/* functions */
char *trim(char *);
int done(int *, int);
void list_proc(priority_queue);
void event_call(char *, int, char, char, priority_queue);
void preempt_call(int, char, char, priority_queue);
void s_srt(priority_queue, v_memory, int, int, float *, int *, int *, int *, int);
void s_rr(priority_queue, v_memory, int, int, float *, int *, int *, int *, int, int);
void simout(char *, char *, FILE *, float *, float *, float *, int *, int, int, int);

int main(int argc, char **argv) {
	/* SIMULATION VARIABLES */
	int n = 0;      // processes
	int cs_t = 13;  // context switch time (in ms)
	FILE *inf;      // input file (processes.txt)
	FILE *outf;   // output file (simout.txt)

	// recording variables
	float a_cpu_t = 0.0;  // average cpu burst time
	float a_turn_t = 0.0; // average turnaround time
	float a_wait_t = 0.0; // average wait time
	int total_cs = 0;     // total context switches
	int total_burst = 0;  // total cpu bursts
	int total_mem = 256;  // total virtual memory
	int total_t = 0;      // total time
	int total_d = 0;      // total defrag time
	int t_slice = 80;     // round robin time slice
	int t_memmove = 10;   // defragmentation memmove time

	if(argc != 1) {
		printf("There are no arguments associated with this simulation. Input is read from processes.txt.\n");
		return 1;
	}

	// priority queues and process
	priority_queue srtf = pq_new(n), srtn = pq_new(n), srtb = pq_new(n), rrf = pq_new(n), rrn = pq_new(n), rrb = pq_new(n);
	process *p1, *p2, *p3, *p4, *p5, *p6;

	/* read file */
	if((inf = fopen("processes.txt", "r")) == NULL) {
		perror("open() failed");
		return EXIT_FAILURE;
	}

	if((outf = fopen("simout.txt", "w")) == NULL) {
		perror("fopen() failed");
		return EXIT_FAILURE;
	}

	// get current line
	char *line = NULL;
	size_t nbytes = 0;

	while(getline(&line, &nbytes, inf) != -1) {
		// trim the string first
		char *newline;
		newline = trim(line);

		if(*newline != 0) {
			// get values
			char pn;
			int at, bt, bn, it, mem;
			sscanf(newline, "%c|%i|%i|%i|%i|%i", &pn, &at, &bt, &bn, &it, &mem);

			// allocate and init
			p1 = malloc(sizeof(process));
			p2 = malloc(sizeof(process));
			p3 = malloc(sizeof(process));
			p4 = malloc(sizeof(process));
			p5 = malloc(sizeof(process));
			p6 = malloc(sizeof(process));
			*p1 = proc_new(pn, at, bt, bn, it, 0, mem);
			*p2 = *p1;
			*p3 = *p1;
			*p4 = *p1;
			*p5 = *p1;
			*p6 = *p1;

			// calculate burst statistics
			a_cpu_t += bt * bn;
			total_burst += bn;

			// push to queues
			++n;
			pq_push(srtf, p1, at);
			pq_push(srtn, p2, at);
			pq_push(srtb, p3, at);
			pq_push(rrf, p2, at);
			pq_push(rrn, p5, at);
			pq_push(rrb, p6, at);
		}
	}

	// free memory
	free(line);

	// create virtual memory
	v_memory vm_ff = vm_new(total_mem, 'f');
	v_memory vm_nf = vm_new(total_mem, 'n');
	v_memory vm_bf = vm_new(total_mem, 'b');

	/* ===begin simulations=== */
	a_cpu_t /= total_burst;
	s_srt(srtf, vm_ff, n, cs_t, &a_wait_t, &total_cs, &total_t, &total_d, t_memmove);
	simout("SRT", "First-Fit", outf, &a_cpu_t, &a_turn_t, &a_wait_t, &total_cs, total_burst, total_t, total_d);
	printf("\n\n");
	s_srt(srtn, vm_nf, n, cs_t, &a_wait_t, &total_cs, &total_t, &total_d, t_memmove);
	simout("SRT", "First-Fit", outf, &a_cpu_t, &a_turn_t, &a_wait_t, &total_cs, total_burst, total_t, total_d);
	printf("\n\n");
	s_srt(srtb, vm_bf, n, cs_t, &a_wait_t, &total_cs, &total_t, &total_d, t_memmove);
	simout("SRT", "First-Fit", outf, &a_cpu_t, &a_turn_t, &a_wait_t, &total_cs, total_burst, total_t, total_d);
	printf("\n\n");
	s_rr(rrf, vm_ff, n, cs_t, &a_wait_t, &total_cs, &total_t, &total_d, t_memmove, t_slice);
	simout("RR", "First-Fit", outf, &a_cpu_t, &a_turn_t, &a_wait_t, &total_cs, total_burst, total_t, total_d);
	printf("\n\n");
	s_rr(rrn, vm_nf, n, cs_t, &a_wait_t, &total_cs, &total_t, &total_d, t_memmove, t_slice);
	simout("RR", "First-Fit", outf, &a_cpu_t, &a_turn_t, &a_wait_t, &total_cs, total_burst, total_t, total_d);
	printf("\n\n");
	s_rr(rrb, vm_bf, n, cs_t, &a_wait_t, &total_cs, &total_t, &total_d, t_memmove, t_slice);
	simout("RR", "First-Fit", outf, &a_cpu_t, &a_turn_t, &a_wait_t, &total_cs, total_burst, total_t, total_d);

	// close streams
	fclose(inf);
	fclose(outf);

	// free memory and exit gracefully
	vm_free(vm_ff);
	vm_free(vm_nf);
	vm_free(vm_bf);
	pq_free(srtf);
	pq_free(srtn);
	pq_free(srtb);
	pq_free(rrf);
	pq_free(rrn);
	pq_free(rrb);
	free(srtf);
	free(srtn);
	free(srtb);
	free(rrf);
	free(rrn);
	free(rrb);
	return EXIT_SUCCESS;
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
 * checks if times are still running
 */
int done(int *t, int sz) {
	int i;
	for(i = 0; i < sz; ++i) {
		if(t[i] > 0) return 0;
	}
	return 1;
}

/**
 * lists processes in the form [Q ... ]
 * dirty function
 */
void list_proc(priority_queue pq) {
	printf("[Q");
	int i, sz = pq_size(pq);
	process *procs[sz];
	int pris[sz];
	for(i = 0; i < sz; ++i) {
		procs[i] = pq_pop(pq, &pris[i]);
		printf(" %c", procs[i]->num);
	}
	for(i = 0; i < sz; ++i)
		pq_push(pq, procs[i], pris[i]);
	printf("]\n");
}

/**
 * outputs events
 * uses [call] to determine the event to output
 */
void event_call(char* sim, int time, char proc, char call, priority_queue pq) {
	printf("time %ims: ", time);
	switch(call) {
		case '0':
			printf("Simulator started for %s ", sim);
			break;
		case '1':
			printf("Process '%c' started using the CPU ", proc);
			break;
		case '2':
			printf("Process '%c' completed its CPU burst ", proc);
			break;
		case '3':
			printf("Process '%c' performing I/O ", proc);
			break;
		case '4':
			printf("Process '%c' completed I/O ", proc);
			break;
		case '5':
			printf("Process '%c' terminated ", proc);
			break;
		case '6':
			printf("Process '%c' added to system ", proc);
			break;
		case '7':
			printf("Simulator for %s ended ", sim);
			break;
		case '8':
			printf("Process '%c' added to system ", proc);
			break;
		case '9':
			printf("Simulated Memory:\n");
			return;
		case 'a':
			printf("Process '%c' unable to be added; lack of memory\n", proc);
			return;
		case 'b':
			printf("Starting defragmentation (suspending all processes)\n");
			return;
		case 'c':
			printf("Completed defragmentation (moved %d memory units)\n", (int)proc / 10);
			return;
		case 'd':
			printf("Process '%c' deallocated\n", proc);
			return;
		default:
			return;
	}
	list_proc(pq);
}

/**
 * special call for preempts
 */
void preempt_call(int time, char proc1, char proc2, priority_queue pq) {
	printf("time %ims: ", time);
	// time slice preempt
	if(proc2 == '0') {
		printf("Process '%c' preempted due to time slice expiration ", proc1);
	}
	// burst time preempt
	else {
		printf("Process '%c' preempted by Process '%c' ", proc1, proc2);
	}
	list_proc(pq);
}

/**
 * srt simulation
 */
void s_srt(priority_queue q, v_memory vm, int num, int cs_t, float *a_wait_t, int *total_cs, int *total_t, int *total_d, int t_memmove) {
	int cur_t = 0;             // current time
	int cur_cs_t = cs_t + 1;   // current context switch time (-1 when not switching)
	int in_use = 0;            // boolean for processor usage
	int procs_t[num];          // processor countdowns
	process *procs[num];       // processes in use
	process *tmp;              // process to pop from waiting queue
	int active;                // active processor in use
	char *algo;                // fitting algorithm
	priority_queue pq;         // processes queue

	// zero arrays
	int i;
	for(i = 0; i < num; ++i) {
		procs_t[i] = 0;
		procs[i] = 0;
	}

	// initialize pq
	pq = pq_new(0);

	// print vm algorithm
	switch(vm.algo) {
		case 'f':
			algo = "First-Fit";
			break;
		case 'n':
			algo = "Next-Fit";
			break;
		case 'b':
			algo = "Best-Fit";
			break;
		default:
			break;
	}
	printf("time 0ms: Simulator started for SRT and %s\n", algo);

	while(1) {
		// check process arrival times
		tmp = NULL;
		while(tmp == NULL) {
			tmp = pq_peek(q, NULL);
			if(tmp && tmp->a_t == cur_t) {
				int rc;

				// add process to system
				tmp = pq_pop(q, NULL);
				pq_push(pq, tmp, tmp->b_t);
				rc = vm_add(&vm, *tmp);
				if(rc != 1) {
					event_call("", cur_t, tmp->num, 'a', NULL);
					event_call("", cur_t, 0, 'b', NULL);
					event_call("", cur_t, 0, '9', NULL);
					vm_print(vm);
					event_call("", cur_t, 0, 'c', NULL);
					event_call("", cur_t, 0, '9', NULL);
					vm_print(vm);
					vm_add(&vm, *tmp);
				}

				// print
				event_call("", cur_t, tmp->num, '8', pq);
				event_call("", cur_t, 0, '9', NULL);
				vm_print(vm);
				tmp = NULL;
			} else {
				tmp = NULL;
				break;
			}
		}

		// switching context
		if(cur_cs_t > 0) --cur_cs_t;

		// switched context
		if(cur_cs_t == 0) {
			--cur_cs_t;

			// check if open process available and run it if possible
			if((pq_size(pq) != 0) && !in_use) {
				int procs_u = 0;
				while(procs[procs_u] != 0) ++procs_u; // get next open index
				in_use = 1;
				procs[procs_u] = pq_pop(pq, NULL); // pop the next open process
				change_status(procs[procs_u], 1);  // set status to using cpu
				procs_t[procs_u] = procs[procs_u]->b_t + 1; // start timer
				if(procs[procs_u]->cur_t > 0) procs_t[procs_u] = procs[procs_u]->cur_t;
				active = procs_u;
				--procs[procs_u]->b_n;
				event_call("", cur_t, procs[procs_u]->num, '1', pq);
			}
		}

		// process timers
		for(i = 0; i < num; ++i) {
			if(procs[i] != 0) {
				// check timers
				if(procs_t[i] > 0) {
					--procs_t[i];

					// handle timers
					if(procs_t[i] == 0) {

						// completed cpu burst
						if(procs[i]->status == 1) {
							change_status(procs[i], 2);
							procs_t[i] = procs[i]->io_t;
							if(procs[i]->cur_t > 0) procs[i]->cur_t = 0; // reset cur_t
							in_use = 0;
							cur_cs_t = cs_t;
							++*total_cs;

							// if process does not require i/o
							if(procs_t[i] == 0) {
								if(procs[i]->b_n <= 0) {
									vm_del(&vm, procs[i]->num);
									event_call("", cur_t, procs[i]->num, '5', pq);
									event_call("", cur_t, procs[i]->num, 'd', NULL);
									event_call("", cur_t, procs[i]->num, '9', NULL);
									vm_print(vm);
									change_status(procs[i], 3); // terminate
								}
								else {
									change_status(procs[i], 0);
									pq_push(pq, procs[i], procs[i]->b_t);
									event_call("", cur_t, procs[i]->num, '2', pq);
									procs[i] = 0;
								}
							}
							else {
								if(procs[i]->b_n != 0) {
									event_call("", cur_t, procs[i]->num, '2', pq);
									event_call("", cur_t, procs[i]->num, '3', pq);
								}
								// terminate process
								else {
									procs_t[i] = 0;
									change_status(procs[i], 3);
									vm_del(&vm, procs[i]->num);
									event_call("", cur_t, procs[i]->num, '5', pq);
									event_call("", cur_t, procs[i]->num, 'd', NULL);
									event_call("", cur_t, procs[i]->num, '9', NULL);
									vm_print(vm);
								}
							}
						}

						// completed i/o
						else if(procs[i]->status == 2) {
							change_status(procs[i], 0);
							if(!in_use) cur_cs_t = cs_t;

							// finished burst
							if(procs[i]->b_n <= 0) {
								// terminate on last burst
								if(pq_size(pq) != 0) {
									vm_del(&vm, procs[i]->num);
									event_call("", cur_t, procs[i]->num, '5', pq);
									event_call("", cur_t, procs[i]->num, 'd', NULL);
									event_call("", cur_t, procs[i]->num, '9', NULL);
									vm_print(vm);
								}
								change_status(procs[i], 3);
							}
							else {
								// preemption check
								if(procs[active] != 0 && i != active && procs[i]->b_t < procs_t[active]) {
									event_call("", cur_t, procs[i]->num, '4', pq);
									procs[active]->cur_t = procs_t[active]; // store the current processing time
									++procs[active]->b_n; // restore burst number since it hasnt finished
									change_status(procs[active], 0); // reset status
									pq_push(pq, procs[active], procs_t[active]); // push back into queue
									preempt_call(cur_t, procs[active]->num, procs[i]->num, pq);

									// push to the top of the queue and set variables
									change_status(procs[i], 0);
									pq_push(pq, procs[i], -1);
									in_use = 0;
									cur_cs_t = cs_t;
									++*total_cs;

									// void existing processors
									procs_t[active] = 0;
									procs_t[i] = 0;
									procs[active] = 0;
									procs[i] = 0;
									active = i;
								} else {
									pq_push(pq, procs[i], procs[i]->b_t);
									event_call("", cur_t, procs[i]->num, '4', pq);
									procs[i] = 0;
								}
							}
						}
					}
				}
			}
		}

		// get statistics from all processes
		for(i = 1; i <= pq_size(pq); ++i) {
			if(((process*)(pq->buf[i].data))->status == 0) {
				++*a_wait_t;
			}
		}

		if(pq_size(pq) == 0 && !in_use && done(procs_t, sizeof(procs_t) / sizeof(int))) {
			*total_t = cur_t;
			event_call("SRT", cur_t, 0, '7', pq);
			return;
		}

		++cur_t;
	}
}

/**
 * rr simulation
 */
void s_rr(priority_queue q, v_memory vm, int num, int cs_t, float *a_wait_t, int *total_cs, int *total_t, int *total_d, int t_memmove, int t_slice) {
	int cur_t = 0;             // current time
	int cur_cs_t = cs_t + 1;   // current context switch time (-1 when not switching)
	int cur_s_t = 0;           // current time-slice time
	int in_use = 0;            // boolean for processor usage
	int procs_t[num];          // processor countdowns
	process *procs[num];       // processes in use
	process *tmp;              // process to pop from waiting queue
	int active;                // active processor in use
	int pri = 0;               // rr priority (fcfs)
	char *algo;                // fitting algorithm
	priority_queue pq;         // processes queue

	// zero arrays
	int i;
	for(i = 0; i < num; ++i) {
		procs_t[i] = 0;
		procs[i] = 0;
	}

	// initialize stuff
	pq = pq_new(0);

	// print vm algorithm
	switch(vm.algo) {
		case 'f':
			algo = "First-Fit";
			break;
		case 'n':
			algo = "Next-Fit";
			break;
		case 'b':
			algo = "Best-Fit";
			break;
		default:
			break;
	}
	printf("time 0ms: Simulator started for RR (t_slice %d) and %s\n", t_slice, algo);

	while(1) {
		// check process arrival times
		tmp = NULL;
		while(tmp == NULL) {
			tmp = pq_peek(q, NULL);
			if(tmp && tmp->a_t == cur_t) {
				int rc;

				// add process to system
				tmp = pq_pop(q, NULL);
				pq_push(pq, tmp, ++pri);
				rc = vm_add(&vm, *tmp);
				if(rc != 1) { // (defrag)
					event_call("", cur_t, tmp->num, 'a', NULL);
					event_call("", cur_t, 0, 'b', NULL);
					event_call("", cur_t, 0, '9', NULL);
					vm_print(vm);
					*total_d += vm_defrag(&vm, 10);
					event_call("", cur_t, 0, 'c', NULL);
					event_call("", cur_t, 0, '9', NULL);
					vm_print(vm);
					vm_add(&vm, *tmp);
				}

				// print
				event_call("", cur_t, tmp->num, '8', pq);
				event_call("", cur_t, 0, '9', NULL);
				vm_print(vm);
				tmp = NULL;
			} else {
				tmp = NULL;
				break;
			}
		}

		// switching context
		if(cur_cs_t > 0) --cur_cs_t;

		// switched context
		if(cur_cs_t == 0) {
			--cur_cs_t;

			// check if open process available and run it if possible
			if((pq_size(pq) != 0) && !in_use) {
				int procs_u = 0;
				while(procs[procs_u] != 0) ++procs_u; // get next open index
				in_use = 1;
				procs[procs_u] = pq_pop(pq, NULL); // pop the next open process
				change_status(procs[procs_u], 1);  // set status to using cpu
				procs_t[procs_u] = procs[procs_u]->b_t + 1; // start process timer
				cur_s_t = t_slice + 1; // start t_slice timer
				if(procs[procs_u]->cur_t > 0) procs_t[procs_u] = procs[procs_u]->cur_t;
				active = procs_u;
				--procs[procs_u]->b_n;
				event_call("", cur_t, procs[procs_u]->num, '1', pq);
			}
		}

		// process timers
		for(i = 0; i < num; ++i) {
			if(procs[i] != 0) {
				// check timers
				if(procs_t[i] > 0) {
					--procs_t[i];
					if(cur_s_t > 0) --cur_s_t;

					// handle timers
					if(procs_t[i] == 0) {

						// completed cpu burst
						if(procs[i]->status == 1) {
							change_status(procs[i], 2);
							procs_t[i] = procs[i]->io_t;
							if(procs[i]->cur_t > 0) procs[i]->cur_t = 0; // reset cur_t
							in_use = 0;
							cur_cs_t = cs_t;
							++*total_cs;

							// if process does not require i/o
							if(procs_t[i] == 0) {
								if(procs[i]->b_n <= 0) {
									vm_del(&vm, procs[i]->num);
									event_call("", cur_t, procs[i]->num, '5', pq);
									event_call("", cur_t, procs[i]->num, 'd', NULL);
									event_call("", cur_t, procs[i]->num, '9', NULL);
									vm_print(vm);
									change_status(procs[i], 3); // terminate
								}
								else {
									change_status(procs[i], 0);
									pq_push(pq, procs[i], ++pri);
									event_call("", cur_t, procs[i]->num, '2', pq);
									procs[i] = 0;
								}
							}
							else {
								if(procs[i]->b_n != 0) {
									event_call("", cur_t, procs[i]->num, '2', pq);
									event_call("", cur_t, procs[i]->num, '3', pq);
								}
								// terminate process
								else {
									procs_t[i] = 0;
									change_status(procs[i], 3);
									vm_del(&vm, procs[i]->num);
									event_call("", cur_t, procs[i]->num, '5', pq);
									event_call("", cur_t, procs[i]->num, 'd', NULL);
									event_call("", cur_t, procs[i]->num, '9', NULL);
									vm_print(vm);
								}
							}
						}

						// completed i/o
						else if(procs[i]->status == 2) {
							change_status(procs[i], 0);
							if(!in_use) cur_cs_t = cs_t;

							// finished burst
							if(procs[i]->b_n <= 0) {
								// terminate on last burst
								if(pq_size(pq) != 0) {
									vm_del(&vm, procs[i]->num);
									event_call("", cur_t, procs[i]->num, '5', pq);
									event_call("", cur_t, procs[i]->num, 'd', NULL);
									event_call("", cur_t, procs[i]->num, '9', NULL);
									vm_print(vm);
								}
								change_status(procs[i], 3);
							}
							else {
								pq_push(pq, procs[i], ++pri);
								event_call("", cur_t, procs[i]->num, '4', pq);
								procs[i] = 0;
							}
						}
					}

					// check time slice expiration
					else if(cur_s_t == 0 && procs[i]->status == 1) {
						if(pq_size(pq) != 0) {
							procs[active]->cur_t = procs_t[active]; // store the current processing time
							++procs[active]->b_n; // restore burst number since it hasnt finished
							change_status(procs[active], 0); // reset status
							pq_push(pq, procs[active], ++pri); // push back into queue
							preempt_call(cur_t, procs[active]->num, '0', pq); // print
							cur_cs_t = cs_t; // switch contexts
							++*total_cs;

							// void existing processors
							procs_t[active] = 0;
							procs[active] = 0;
							in_use = 0;
							cur_s_t = -1;
						}
					}
				}
			}
		}

		// get statistics from all processes
		for(i = 1; i <= pq_size(pq); ++i) {
			if(((process*)(pq->buf[i].data))->status == 0) {
				++*a_wait_t;
			}
		}

		if(pq_size(pq) == 0 && !in_use && done(procs_t, sizeof(procs_t) / sizeof(int))) {
			*total_t = cur_t;
			event_call("RR", cur_t, 0, '7', pq);
			return;
		}

		++cur_t;
	}
}

/**
 * prints simout.txt information from given values, then zeroes them for the next simout call
 */
void simout(char *sim, char *vm, FILE *outf, float *a_cpu_t, float *a_turn_t, float *a_wait_t, int *total_cs, int num_burst, int total_t, int total_d) {
	float percent_d = (float)total_d / (float)total_t;
	percent_d *= 100.0;
	*a_wait_t /= num_burst;
	*a_turn_t = *a_cpu_t + *a_wait_t + (*total_cs * 13 / num_burst);
	fprintf(outf, "Algorithm %s and %s\n", sim, vm);
	fprintf(outf, "-- average CPU burst time: %.2f ms\n", *a_cpu_t);
	fprintf(outf, "-- average wait time: %.2f ms\n", *a_wait_t);
	fprintf(outf, "-- average turnaround time: %.2f ms\n", *a_turn_t);
	fprintf(outf, "-- total number of context switches: %d\n", *total_cs);
	fprintf(outf, "-- total defrag time: %d\n", total_d);
	fprintf(outf, "-- percent time defragging: %.2f%%\n", percent_d);
	*a_wait_t = 0;
	*a_turn_t = 0;
	*total_cs = 0;
}
