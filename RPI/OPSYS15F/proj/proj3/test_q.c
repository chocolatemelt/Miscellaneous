#include <stdio.h>
#include <stdlib.h>
#include "q.h"
#include "process.h"

int main() 
{
  /* PRIORITY QUEUE TESTING */
  int i, p;
  char *c, *tasks[] ={
    "Clear drains", "Feed cat", "Make tea", "Solve RC tasks", "Tax return" };
  int pri[] = { 3, 4, 5, 1, 2 };
 
  /* make two queues */
  priority_queue q = pq_new(0), q2 = pq_new(0);
 
  /* push all 5 tasks into q */
  for (i = 0; i < 5; i++)
    pq_push(q, tasks[i], pri[i]);

	/* get the highest priority */
	printf("highest priority: %d\n", pq_pri(q));
	char f[] = "test";
	pq_push(q, f, pq_pri(q) + 1);
	printf("highest priority: %d\n", pq_pri(q));
 
  /* pop them and print one by one */
  while ((c = pq_pop(q, &p)))
    printf("%d: %s\n", p, c);
 
  /* put a million random tasks in each queue */
  for (i = 0; i < 1 << 5; i++) {
    p = rand() / ( RAND_MAX / 5 );
    pq_push(q, tasks[p], pri[p]);
 
    p = rand() / ( RAND_MAX / 5 );
    pq_push(q2, tasks[p], pri[p]);
  }
 
  printf("\nq has %d items, q2 has %d items\n", pq_size(q), pq_size(q2));
 
  /* pop q until it's empty */
  for (i = 0; (c = pq_pop(q, 0)); i++);
  printf("Popped %d items out of q\n", i);

  /* PROCESS TESTING 
  process x = proc_new(1, 100, 100, 100);
  progress(&x);
  printf("process x's status is now: %d\n", x.status);
  progress(&x);
  printf("process x's status is now: %d\n", x.status);
  progress(&x);
  printf("process x's status is now: %d\n", x.status);
  change_status(&x, 3);
  printf("process x's status is now: %d\n", x.status);

  printf("process x total burst: %d\n", x.total_cpu);
	*/
 
  return 0;
}
