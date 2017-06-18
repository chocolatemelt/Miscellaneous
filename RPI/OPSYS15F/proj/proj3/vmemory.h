#ifndef V_MEMORY_H
#define V_MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include "process.h"

typedef struct {
	int n_mem_units; // total memory units
	int n_sect;      // total sectors
	int n_free;      // total free memory units
	int p_last;      // pointer index to last allocated unit
	int *alloc;      // allocated memory units
	char *mem;       // memory units associated with above
	char algo;       // placement algorithm to be used
} v_memory;

// create new virtual memory with memory units n
v_memory vm_new(int n, char a) {
	v_memory ret;
	ret.n_mem_units = n;
	ret.n_sect = 1;
	ret.p_last = 0;
	ret.n_free = n;
	ret.alloc = (int *)malloc(n * sizeof(int));
	ret.mem = (char *)malloc(n * sizeof(char));
	int i;
	for(i = 0; i < n; ++i) {
		ret.alloc[i] = n;
		ret.mem[i] = '.'; // indicates free memory
	}
	ret.algo = a;
	return ret;
}

// consolidate free space
// doesn't affect the vm itself
// run after deleting memory
void consolidate(v_memory *vm) {
	int i;
	for(i = 0; i < vm->n_sect - 1; ++i) {
		// look for empty sectors
		if(vm->mem[i] == '.') {
			// check if next sector is empty
			if(vm->mem[i + 1] == '.') {
				// allocate free space
				vm->alloc[i] += vm->alloc[i + 1];
				// shift array by 1
				int j;
				for(j = i + 1; j < vm->n_sect - 1; ++j) {
					vm->alloc[j] = vm->alloc[j + 1];
					vm->mem[j] = vm->mem[j + 1];
				}
				// decrement sectors
				--vm->n_sect;
			}
		}
	}
}

// allocate memory to vm
void alloc(v_memory *vm, int idx, process p) {
	// not a perfect fit; have to split
	if(vm->alloc[idx] > p.mem) {
		int i;
		// shift array by 1
		for(i = vm->n_sect - 1; i > idx; --i) {
			vm->alloc[i] = vm->alloc[i - 1];
			vm->mem[i] = vm->mem[i - 1];
		}
		// allot extra free space sector
		vm->alloc[idx + 1] = vm->alloc[idx] - p.mem;
		vm->mem[idx] = '.';
	}
	vm->alloc[idx] = p.mem;
	vm->mem[idx] = p.num;
	++vm->n_sect;
	vm->p_last = idx;
}

// add process to vm
// returns 0 on failure and 1 on success
int vm_add(v_memory *vm, process p) {
	int i, idx, min;
	switch(vm->algo) {
		// first fit: 0-end, look for any allowable space
		case 'f':
			for(i = 0; i <= vm->n_sect; ++i) {
				// check if free memory
				if(vm->mem[i] == '.') {
					// check if enough space
					if(vm->alloc[i] >= p.mem) {
						// allocate memory
						alloc(vm, i, p);
						return 1;
					}
				}
			}
			break;
		// next fit: last-end, look for any allowable space
		case 'n':
			// check starting from last allocated unit
			for(i = vm->p_last; i <= vm->n_sect; ++i) {
				// check if free memory
				if(vm->mem[i] == '.') {
					// check if enough space
					if(vm->alloc[i] >= p.mem) {
						// allocate memory
						alloc(vm, i, p);
						return 1;
					}
				}
			}
			// check from the beginning to last allocated unit if nothing found
			for(i = 0; i < vm->p_last; ++i) {
				// check if free memory
				if(vm->mem[i] == '.') {
					// check if enough space
					if(vm->alloc[i] >= p.mem) {
						// allocate memory
						alloc(vm, i, p);
						return 1;
					}
				}
			}
			break;
		// best fit: 0-end, look for smallest allowable space
		case 'b':
			min = vm->n_mem_units + 1;
			idx = -1;
			// check from beginning to end minimizing space
			for(i = 0; i <= vm->n_sect; ++i) {
				// check if free memory
				if(vm->mem[i] == '.') {
					// check if enough space
					if(vm->alloc[i] >= p.mem) {
						// minimize
						if(vm->alloc[i] < min) {
							min = vm->alloc[i];
							idx = i;
						}
					}
				}
			}
			if(idx != -1) {
				// allocate memory
				alloc(vm, idx, p);
				return 1;
			}
			break;
		default:
			printf("ERROR: Not an algorithm. No memory allocated.\n");
			break;
	}
	return 0;
}

// remove process from vm (free memory)
void vm_del(v_memory *vm, char p) {
	// find index of process
	int i;
	for(i = 0; i < vm->n_sect; ++i) {
		if(vm->mem[i] == p) {
			vm->mem[i] = '.';
			break;
		}
	}
	// consolidate potential free space
	consolidate(vm);
}

// defragment given vm
// returns time taken to defragment
int vm_defrag(v_memory *vm, int t_memmove) {
	int t_taken = 0;
	int i;
	// loop until end
	for(i = 0; i < vm->n_sect; ++i) {
		// look for empty sectors
		if(vm->mem[i] == '.') {
			// skip to next non-empty sector
			int j;
			for(j	= i; j < vm->n_sect; ++j) {
				if(vm->mem[j] != '.') break;
			}

			// swap sector i with sector j if j isn't empty
			if(vm->mem[j] != '.') {
				int tmp1 = vm->alloc[i];
				char tmp2 = vm->mem[i];
				vm->alloc[i] = vm->alloc[j];
				vm->mem[i] = vm->mem[j];
				vm->alloc[j] = tmp1;
				vm->mem[j] = tmp2;
				t_taken += t_memmove * vm->alloc[i];
			}
		}
	}
	// consolidate free memory in the end
	for(i = 0; i < vm->n_sect; ++i) {
		if(vm->mem[i] == '.') vm->alloc[i] = vm->n_mem_units;
	}
	return t_taken;
}

// print virtual memory
void vm_print(v_memory vm) {
	int i, j = 0;
	int idx = 0; // current memory unit index
	int cnt = vm.alloc[0]; // current count
	printf("================================\n");
	for(i = 0; i < vm.n_mem_units; ++i) {
		printf("%c", vm.mem[idx]);
		++j;
		--cnt;
		if(j == 32) {
			printf("\n");
			j = 0;
		}
		if(cnt == 0) {
			cnt = vm.alloc[++idx];
		}
	}
	printf("================================\n");
}

// reset the vm
void vm_reset(v_memory *vm) {
	vm->n_sect = 1;
	vm->p_last = 0;
	vm->n_free = vm->n_mem_units;
	int i;
	for(i = 0; i < vm->n_mem_units; ++i) {
		vm->alloc[i] = vm->n_mem_units;
		vm->mem[i] = '.'; // indicates free memory
	}
}

// free malloc'd data in the vm
void vm_free(v_memory vm) {
	free(vm.alloc);
	free(vm.mem);
}

#endif
