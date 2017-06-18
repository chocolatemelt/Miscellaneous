#include "vmemory.h"
#include "process.h"

int main() {
	v_memory tmp = vm_new(256, 'b');
	process a = proc_new('A', 0, 0, 0, 0, 0, 8);
	process b = proc_new('B', 0, 0, 0, 0, 0, 10);
	process c = proc_new('C', 0, 0, 0, 0, 0, 8);
	process d = proc_new('D', 0, 0, 0, 0, 0, 8);
	process e = proc_new('E', 0, 0, 0, 0, 0, 8);
	process f = proc_new('F', 0, 0, 0, 0, 0, 8);
	printf("initialize virtual memory tmp\n");
	vm_print(tmp);
	printf("add processes to tmp\n");
	vm_add(&tmp, a);
	vm_add(&tmp, b);
	vm_add(&tmp, c);
	vm_add(&tmp, d);
	vm_add(&tmp, e);
	vm_print(tmp);
	printf("delete processes B and D\n");
	vm_del(&tmp, 'B');
	vm_del(&tmp, 'D');
	vm_print(tmp);
	printf("add process F\n");
	vm_add(&tmp, f);
	vm_print(tmp);
	printf("defrag tmp\n");
	int x = vm_defrag(&tmp, 10);
	vm_print(tmp);
	printf("%d units moved\n", x/10);
	printf("reset vm\n");
	vm_reset(&tmp);
	vm_print(tmp);
	vm_free(tmp);
	return 0;
}
