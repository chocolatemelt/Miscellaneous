#ifndef PROCESS_H
#define PROCESS_H

#include <assert.h>

typedef struct {
	int num;      // proc-num
	int b_t;      // burst-time
	int b_n;      // num-burst
	int io_t;     // io-time
	int pri;      // priority (0-5)
	int cur_t;    // current-time (used when on hold)
	int cur_w;    // current-wait-time (pwa only)
	int total_cpu;
	int total_turn;
	int total_wait;
	int status;
} process;

/**
 * status is a numerical indicator of the state
 * the current process is in:
 * 0 - ready to use cpu
 * 1 - using cpu
 * 2 - blocked / performing i/o
 * 3 - terminated (special status)
 */

/**
 * creates a new process
 */
process proc_new(int n, int bt, int bn, int io, int pri) {
	process p;
	p.num = n;
	p.b_t = bt;
	p.b_n = bn;
	p.io_t = io;
	p.pri = pri;
	p.cur_t = 0;
	p.cur_w = 0;
	p.total_cpu = p.b_t * p.b_n;
	p.total_turn = 0;
	p.total_wait = 0;
	p.status = 0;
	return p;
}

/**
 * changes the status to a specific number
 * terminates processes
 */
void change_status(process *p, int s) {
	assert(s >= 0);
	assert(s <= 3);
	p->status = s;
}

#endif