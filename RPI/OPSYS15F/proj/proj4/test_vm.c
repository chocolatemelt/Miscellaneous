#include "vmemory.h"

int main() {
	v_memory tmp = vm_new(128, 'f');
	printf("initialize virtual memory tmp with free space %d\n", vm_getmem(tmp));
	vm_print(tmp);
	printf("add 32 blocks\n");
	vm_add(&tmp, 32, 1);
	printf("remaining free blocks: %d\n", vm_getmem(tmp));
	vm_print(tmp);
	printf("add random blocks\n");
	vm_add(&tmp, 16, 1);
	vm_add(&tmp, 32, 1);
	vm_add(&tmp, 16, 1);
	vm_add(&tmp, 32, 1);
	printf("remaining free blocks: %d\n", vm_getmem(tmp));
	vm_print(tmp);
	printf("remove B and D\n");
	vm_del(&tmp, 'B');
	vm_del(&tmp, 'D');
	printf("remaining free blocks: %d\n", vm_getmem(tmp));
	vm_print(tmp);
	printf("moment of truth: add 32 blocks\n");
	int i = vm_add(&tmp, 17, 1);
	printf("clusters: %d\n", i);
	printf("remaining free blocks: %d\n", vm_getmem(tmp));
	vm_print(tmp);
	vm_free(tmp);
	return 0;
}
