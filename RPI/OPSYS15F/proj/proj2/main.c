#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "q.h"
#include "process.h"

/* functions */
char *trim(char *);
int done(int *);
void list_proc(priority_queue);
void event_call(char *, int, int, char, priority_queue);
void preempt_call(int, int, int, priority_queue);
void s_fcfs(priority_queue, int, int, float *, int *);
void s_srt(priority_queue, int, int, float *, int *);
void s_pwa(priority_queue, int, int, float *, int *);
void simout(char *, FILE *, float *, float *, float *, int *, int);

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
	
	if(argc != 1) {
		printf("There are no arguments associated with this simulation. Input is read from processes.txt.\n");
		return 1;
	}

	// priority queues and process
	priority_queue fcfs = pq_new(n), srt = pq_new(n), pwa = pq_new(n);
	process *p1, *p2, *p3;

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
			int pn, bt, bn, it, pri;
			sscanf(newline, "%i|%i|%i|%i|%i", &pn, &bt, &bn, &it, &pri);

			// allocate and init
			p1 = malloc(sizeof(process));
			p2 = malloc(sizeof(process));
			p3 = malloc(sizeof(process));
			*p1 = proc_new(pn, bt, bn, it, pri);
			*p2 = *p1;
			*p3 = *p1;

			// calculate burst statistics
			a_cpu_t += bt * bn;
			total_burst += bn;

			// push to queues
			++n;
			pq_push(fcfs, p1, n);
			pq_push(srt, p2, bt);
			pq_push(pwa, p3, pri);
		}
	}

	// free memory
	free(line);

	/* ===begin simulations=== */
	a_cpu_t /= total_burst;
	s_fcfs(fcfs, n, cs_t, &a_wait_t, &total_cs);
	simout("FCFS", outf, &a_cpu_t, &a_turn_t, &a_wait_t, &total_cs, total_burst);
	printf("\n\n");
	s_srt(srt, n, cs_t, &a_wait_t, &total_cs);
	simout("SRT", outf, &a_cpu_t, &a_turn_t, &a_wait_t, &total_cs, total_burst);
	printf("\n\n");
	s_pwa(pwa, n, cs_t, &a_wait_t, &total_cs);
	simout("PWA", outf, &a_cpu_t, &a_turn_t, &a_wait_t, &total_cs, total_burst);

	// close streams
	fclose(inf);
	fclose(outf);
	
	// free memory and exit gracefully
	pq_free(fcfs);
	pq_free(srt);
	pq_free(pwa);
	free(fcfs);
	free(srt);
	free(pwa);
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
int done(int *t) {
	int i;
	int sz = sizeof(t) / sizeof(int);
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
		printf(" %d", procs[i]->num);
	}
	for(i = 0; i < sz; ++i)
		pq_push(pq, procs[i], pris[i]);
	printf("]\n");
}

/**
 * outputs events
 * uses [call] to determine the event to output
 */
void event_call(char* sim, int time, int proc, char call, priority_queue pq) {
	printf("time %ims: ", time);
	switch(call) {
		case '0':
			printf("Simulator started for %s ", sim);
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
			printf("Simulator for %s ended ", sim);
	}
	list_proc(pq);
}

/**
 * special call for preempts
 */
void preempt_call(int time, int proc1, int proc2, priority_queue pq) {
	printf("time %ims: ", time);
	printf("P%i preempted by P%i ", proc1, proc2);
	list_proc(pq);
}

/**
 * fcfs simulation
 */
void s_fcfs(priority_queue pq, int num, int cs_t, float *a_wait_t, int *total_cs) {
	int cur_t = 0;             // current time
	int cur_cs_t = cs_t + 1;   // current context switch time (-1 when not switching)
	int in_use = 0;            // boolean for processor usage
	int procs_t[num];          // processor countdowns
	process *procs[num];       // processes in use

	// zero arrays
	int i;
	for(i = 0; i < num; ++i) {
		procs_t[i] = 0;
		procs[i] = 0;
	}

	event_call("FCFS", cur_t, 0, '0', pq);

	while(1) {
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
							in_use = 0;
							cur_cs_t = cs_t;
							++*total_cs;

							// if process does not require i/o
							if(procs_t[i] == 0) {
								if(procs[i]->b_n <= 0) {
									event_call("", cur_t, procs[i]->num, '5', pq);
									change_status(procs[i], 3); // terminate
								}
								else {
									change_status(procs[i], 0);
									pq_push(pq, procs[i], pq_pri(pq) + 1);
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
									event_call("", cur_t, procs[i]->num, '5', pq);
								}
							}
						}

						// completed i/o
						else if(procs[i]->status == 2) {
							change_status(procs[i], 0);
							if(!in_use) cur_cs_t = cs_t;

							// finished burst
							if(procs[i]->b_n <= 0) {
								if(pq_size(pq) != 0) event_call("", cur_t, procs[i]->num, '5', pq);
								change_status(procs[i], 3);
							}
							else {
								pq_push(pq, procs[i], pq_pri(pq) + 1);
								event_call("", cur_t, procs[i]->num, '4', pq);
								procs[i] = 0;
							}
						}
					}
				}
			}
		}

		if(pq_size(pq) == 0 && !in_use && done(procs_t)) {
			event_call("FCFS", cur_t, 0, '6', pq);
			return;
		}

		// get statistics from all processes
		for(i = 1; i <= pq_size(pq); ++i) {
			++*a_wait_t;
		}

		++cur_t;
	}
}

/**
 * srt simulation
 */
void s_srt(priority_queue pq, int num, int cs_t, float *a_wait_t, int *total_cs) {
	int cur_t = 0;             // current time
	int cur_cs_t = cs_t + 1;   // current context switch time (-1 when not switching)
	int in_use = 0;            // boolean for processor usage
	int procs_t[num];          // processor countdowns
	process *procs[num];       // processes in use
	int active;                // active processor in use

	// zero arrays
	int i;
	for(i = 0; i < num; ++i) {
		procs_t[i] = 0;
		procs[i] = 0;
	}

	event_call("SRT", cur_t, 0, '0', pq);

	while(1) {
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
									event_call("", cur_t, procs[i]->num, '5', pq);
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
									event_call("", cur_t, procs[i]->num, '5', pq);
								}
							}
						}

						// completed i/o
						else if(procs[i]->status == 2) {
							change_status(procs[i], 0);
							if(!in_use) cur_cs_t = cs_t;

							// finished burst
							if(procs[i]->b_n <= 0) {
								if(pq_size(pq) != 0) event_call("", cur_t, procs[i]->num, '5', pq);
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

		if(pq_size(pq) == 0 && !in_use && done(procs_t)) {
			event_call("SRT", cur_t, 0, '6', pq);
			return;
		}

		// get statistics from all processes
		for(i = 1; i <= pq_size(pq); ++i) {
			if(((process*)(pq->buf[i].data))->status == 0) {
				++*a_wait_t;
			}
		}

		++cur_t;
	}
}

/**
 * pwa simulation
 */
void s_pwa(priority_queue pq, int num, int cs_t, float *a_wait_t, int *total_cs) {
	int cur_t = 0;             // current time
	int cur_cs_t = cs_t + 1;   // current context switch time (-1 when not switching)
	int in_use = 0;            // boolean for processor usage
	int procs_t[num];          // processor countdowns
	process *procs[num];       // processes in use
	int active;                // active processor in use

	// zero arrays
	int i;
	for(i = 0; i < num; ++i) {
		procs_t[i] = 0;
		procs[i] = 0;
	}
	pq_fcfs(pq);

	event_call("PWA", cur_t, 0, '0', pq);

	while(1) {
		// age processes
		for(i = 1; i <= pq_size(pq); ++i) {
			int cw = ((process*)(pq->buf[i].data))->cur_w;
			int bt = ((process*)(pq->buf[i].data))->b_t;
			if(cw > (3 * bt)) {
				((process*)(pq->buf[i].data))->cur_w = 0;
				--((process*)(pq->buf[i].data))->pri;
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
				procs[procs_u]->cur_w = 0; // reset wait time
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
									event_call("", cur_t, procs[i]->num, '5', pq);
									change_status(procs[i], 3); // terminate
								}
								else {
									change_status(procs[i], 0);
									pq_push(pq, procs[i], procs[i]->pri);
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
									event_call("", cur_t, procs[i]->num, '5', pq);
								}
							}
						}

						// completed i/o
						else if(procs[i]->status == 2) {
							change_status(procs[i], 0);
							if(!in_use) cur_cs_t = cs_t;

							// finished burst
							if(procs[i]->b_n <= 0) {
								if(pq_size(pq) != 0) event_call("", cur_t, procs[i]->num, '5', pq);
								change_status(procs[i], 3);
							}
							else {
								// preemption check
								if(procs[active] != 0 && i != active && procs[i]->pri < procs[active]->pri) {
									event_call("", cur_t, procs[i]->num, '4', pq);
									procs[active]->cur_t = procs_t[active]; // store the current processing time
									++procs[active]->b_n; // restore burst number since it hasnt finished
									change_status(procs[active], 0); // reset status
									pq_push(pq, procs[active], procs[active]->pri); // push back into queue
									preempt_call(cur_t, procs[active]->num, procs[i]->num, pq);

									// push to the top of the queue and set variables
									change_status(procs[i], 0);
									pq_push(pq, procs[i], -1);
									pq_fcfs(pq);
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
									pq_push(pq, procs[i], procs[i]->pri);
									pq_fcfs(pq);
									event_call("", cur_t, procs[i]->num, '4', pq);
									procs[i] = 0;
								}
							}
						}
					}
				}
			}
		}

		if(pq_size(pq) == 0 && !in_use && done(procs_t)) {
			event_call("PWA", cur_t, 0, '6', pq);
			return;
		}

		//increase waiting time for processors in queue
		for(i = 1; i <= pq_size(pq); ++i) {
			++((process*)(pq->buf[i].data))->cur_w;
		}

		// get statistics from all processes
		for(i = 1; i <= pq_size(pq); ++i) {
			if(((process*)(pq->buf[i].data))->status == 0) {
				++*a_wait_t;
			}
		}
		++cur_t;
	}
}

/**
 * prints simout.txt information from given values, then zeroes them for the next simout call
 */
void simout(char *sim, FILE *outf, float *a_cpu_t, float *a_turn_t, float *a_wait_t, int *total_cs, int num_burst) {
	*a_wait_t /= num_burst;
	*a_turn_t = *a_cpu_t + *a_wait_t + (*total_cs * 13 / num_burst);
	fprintf(outf, "Algorithm %s\n", sim);
	fprintf(outf, "-- average CPU burst time: %.2f ms\n", *a_cpu_t);
	fprintf(outf, "-- average wait time: %.2f ms\n", *a_wait_t);
	fprintf(outf, "-- average turnaround time: %.2f ms\n", *a_turn_t);
	fprintf(outf, "-- total number of context switches: %d\n", *total_cs);
	*a_wait_t = 0;
	*a_turn_t = 0;
	*total_cs = 0;
}