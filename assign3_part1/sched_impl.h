#ifndef	__SCHED_IMPL__H__
#define	__SCHED_IMPL__H__

#include "list.h"
#include <pthread.h>


struct sched_queue {
	/*...Fill this in...*/
  list_t* queue;
  pthread_mutex_t mutex;
  int num_threads;
  int MAX_THREADS;
  int in_use;
};

struct thread_info {
	/*...Fill this in...*/
  int wait;
  struct sched_queue* queue;
  list_elem_t* data;
  pthread_mutex_t mutex;
};

#endif /* __SCHED_IMPL__H__ */
