#ifndef V_MEMORY_H
#define V_MEMORY_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int n_mem_units; // total memory units
	int n_sect;      // total sectors
	int p_last;      // pointer index to last allocated unit
	int *alloc;      // allocated memory units
	char *mem;       // memory units associated with above
	char id;         // current file identifier
	char algo;       // placement algorithm to be used
} v_memory;

// query available space
int vm_getmem(v_memory vm) {
	int i, count = 0;
	for(i = 0; i < vm.n_sect; ++i) {
		if(vm.mem[i] == '.') {
			count += vm.alloc[i];
		}
	}
	return count;
}

// create new virtual memory with memory units n
v_memory vm_new(int n, char a) {
	v_memory ret;
	ret.n_mem_units = n;
	ret.n_sect = 1;
	ret.p_last = 0;
	ret.alloc = (int *)malloc(n * sizeof(int));
	ret.mem = (char *)malloc(n * sizeof(char));
	int i;
	for(i = 0; i < n; ++i) {
		ret.alloc[i] = n;
		ret.mem[i] = '.'; // indicates free memory
	}
	ret.id = 'A'; // just the current file identifier
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
void alloc(v_memory *vm, int idx, int blocks) {
	// not a perfect fit; have to split
	if(vm->alloc[idx] > blocks) {
		int i;
		// shift array by 1
		for(i = vm->n_sect; i > idx; --i) {
			vm->alloc[i] = vm->alloc[i - 1];
			vm->mem[i] = vm->mem[i - 1];
		}
		// allot extra free space sector
		vm->alloc[idx + 1] = vm->alloc[idx] - blocks;
		vm->mem[idx] = '.';
		++vm->n_sect;
	}
	vm->alloc[idx] = blocks;
	vm->mem[idx] = vm->id++;
	if((int)vm->id > 90) vm->id = 'A'; // reset after Z
	vm->p_last = idx;
}

// add file to vm
// returns 0 on failure and 1 on success
int vm_add(v_memory *vm, int bytes, int block_size) {
	int i;
	int blocks = (bytes + block_size - 1) / block_size; // division round up
	switch(vm->algo) {
		// first fit: 0-end, look for any allowable space
		// modified to use clusters for project 4
		case 'f':
			// check total free memory
			if(vm_getmem(*vm) >= blocks) {
				for(i = 0; i <= vm->n_sect; ++i) {
					// check if free memory
					if(vm->mem[i] == '.') {
						// check space
						if(vm->alloc[i] >= blocks) {
							// allocate memory
							alloc(vm, i, blocks);
							return 1;
						}
						else {
							// too little space, split up as clusters
							// neat recursive stuff~

							// calculate fit
							int cbytes, cblocks;
							cblocks = vm->alloc[i];
							blocks -= vm->alloc[i];
							cbytes = bytes - (cblocks * block_size);

							// allocate memory
							alloc(vm, i, cblocks);
							if(vm->id == 'A') vm->id = 'Z';
							else --vm->id;

							// recurse
							return 1 + vm_add(vm, cbytes, block_size);
						}
					}
				}
			}
			break;
		default:
			printf("ERROR: Not an algorithm. No memory allocated.\n");
			break;
	}
	return 0;
}

// remove file from vm (free memory)
int vm_del(v_memory *vm, char p) {
	// find index of process
	int i, count = 0;
	for(i = 0; i < vm->n_sect; ++i) {
		if(vm->mem[i] == p) {
			vm->mem[i] = '.';
			count += vm->alloc[i];
		}
	}
	// consolidate potential free space
	consolidate(vm);
	return count;
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
	int i;
	for(i = 0; i < vm->n_mem_units; ++i) {
		vm->alloc[i] = vm->n_mem_units;
		vm->mem[i] = '.'; // indicates free memory
	}
}

// free malloc'd data in the vm
void vm_free(v_memory *vm) {
	free(vm->alloc);
	free(vm->mem);
}

#endif
