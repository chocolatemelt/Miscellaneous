#ifndef Q_H
#define Q_H

#include <stdlib.h>
#include <stdio.h>

typedef struct { void * data; int pri; } q_elem_t;
typedef struct { q_elem_t *buf; int n, alloc; } priority_queue_t, *priority_queue;
 
#define pq_purge(q) (q)->n = 1
#define pq_size(q) ((q)->n - 1)

priority_queue pq_new(int size)
{
  if (size < 4) size = 4;
 
  priority_queue q = malloc(sizeof(priority_queue_t));
  q->buf = malloc(sizeof(q_elem_t) * size);
  q->alloc = size;
  q->n = 1;
 
  return q;
}

void pq_push(priority_queue q, void *data, int pri)
{
  q_elem_t *b;
  int n, m;

  /* look for tie */
  int i, tie = 0;
  for(i = 1; i <= pq_size(q); ++i) {
    if(q->buf[i].pri == pri) tie = 1;
  }

  if(tie == 1)
    ++pri;
 
  if (q->n >= q->alloc) {
    q->alloc *= 2;
    b = q->buf = realloc(q->buf, sizeof(q_elem_t) * q->alloc);
  } else
    b = q->buf;
 
  n = q->n++;
  /* append at end, then up heap */
  while ((m = n / 2) && pri < b[m].pri) {
    b[n] = b[m];
    n = m;
  }
  b[n].data = data;
  b[n].pri = (tie == 1) ? --pri : pri;
}
 
/* remove top item. returns 0 if empty. *pri can be null. */
void *pq_pop(priority_queue q, int *pri)
{
  void *out;
  if (q->n == 1) return 0;
 
  q_elem_t *b = q->buf;
 
  out = b[1].data;
  if (pri) *pri = b[1].pri;
 
  /* pull last item to top, then down heap. */
  --q->n;
 
  int n = 1, m;
  while ((m = n * 2) < q->n) {
    if (m + 1 < q->n && b[m].pri > b[m + 1].pri) m++;
 
    if (b[q->n].pri <= b[m].pri) break;
    b[n] = b[m];
    n = m;
  }
 
  b[n] = b[q->n];
  if (q->n < q->alloc / 2 && q->n >= 16)
    q->buf = realloc(q->buf, (q->alloc /= 2) * sizeof(b[0]));
 
  return out;
}

/* get the top element without removing it from queue */
void *pq_peek(priority_queue q, int *pri)
{
  if (q->n == 1) return 0;
  if (pri) *pri = q->buf[1].pri;
  return q->buf[1].data;
}

/* get the highest priority */
int pq_pri(priority_queue q) {
  int pri = 0, sz = pq_size(q), i;
  void *temp[sz];
  int pris[sz];
  for(i = 0; i < sz; ++i) {
    temp[i] = pq_pop(q, &pris[i]);
    if(pris[i] > pri) pri = pris[i];
  }
  for(i = 0; i < sz; ++i)
    pq_push(q, temp[i], pris[i]);
  return pri;
}

/* free dynamically allocated data */
void pq_free(priority_queue q)
{
	int i;
	for(i = 1; i <= pq_size(q); ++i) free(q->buf[i].data);
	free(q->buf);
	pq_purge(q);
}

/* flip */
void pq_fcfs(priority_queue q) {
  int sz = pq_size(q);
  void *popped[sz];
  int pris[sz];
  int i;
  for(i = 1; i <= sz; ++i) {
    popped[i-1] = pq_pop(q, &pris[i-1]);
  }
  for(i = sz; i > 0; --i) {
    pq_push(q, popped[i-1], pris[i-1]);
  }
}

#endif