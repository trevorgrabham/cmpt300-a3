#include "scheduler.h"
#include "sched_impl.h"
#include <stdlib.h>

/* Fill in your scheduler implementation code below: */

void del(list_elem_t* li){free(li);}

static void init_thread_info(thread_info_t *info, sched_queue_t *queue)
{
	/*...Code goes here...*/
	info->queue = queue;
	pthread_mutex_init(&(info->mutex),NULL);
}

static void destroy_thread_info(thread_info_t *info)
{
	/*...Code goes here...*/
}

static void enter_sched_queue(thread_info_t *info){
	list_elem_t* data = (list_elem_t*)malloc(sizeof(list_elem_t*));
	data->datum = info;
	data->next = data->prev = NULL;
	while(info->queue->num_threads >= info->queue->MAX_THREADS){
		// spin wait
	}
	pthread_mutex_lock(&(info->queue->mutex));
	if(info->queue->num_threads < info->queue->MAX_THREADS){
		list_insert_tail(info->queue->queue, data);
		info->queue->num_threads++;
		pthread_mutex_unlock(&(info->queue->mutex));
} else{
		pthread_mutex_unlock(&(info->queue->mutex));
		enter_sched_queue(info);
		return;
	}
}

static void leave_sched_queue(thread_info_t *info){
	pthread_mutex_lock(&(info->queue->mutex));
	list_elem_t* curr = list_get_head(info->queue->queue);
	while(curr && curr->datum != info){
		curr = curr->next;
	}
	list_remove_elem(info->queue->queue, curr);
	info->queue->num_threads--;
	pthread_mutex_unlock(&(info->queue->mutex));
}

static void wait_for_cpu(thread_info_t *info){
	pthread_mutex_lock(&(info->mutex));
	info->wait = 1;
	pthread_mutex_unlock(&(info->mutex));
	while(info->wait){
		// spin wait
	}
}

static void release_cpu(thread_info_t *info){
	pthread_mutex_lock(&(info->queue->mutex));
	info->queue->in_use = 0;
	pthread_mutex_unlock(&(info->queue->mutex));
}
/*...More functions go here...*/

static void init_sched_queue(sched_queue_t *queue, int queue_size)
{
	/*...Code goes here...*/
	queue->MAX_THREADS = queue_size;
	pthread_mutex_init(&(queue->mutex),NULL);
	queue->num_threads = 0;
	queue->queue = (list_t*)malloc(sizeof(list_t*));
	list_init(queue->queue);
	// queue and numthreads
}

static void destroy_sched_queue(sched_queue_t *queue)
{
	/*...Code goes here...*/
	list_foreach(queue->queue, &del);
	free(queue->queue);
}

static void wake_up_worker(thread_info_t *info){
	pthread_mutex_lock(&(info->mutex));
	info->wait = 0;
	pthread_mutex_unlock(&(info->mutex));
}
static void wait_for_worker(sched_queue_t *queue){
	pthread_mutex_lock(&(queue->mutex));
	queue->in_use = 1;
	pthread_mutex_unlock(&(queue->mutex));
	while(queue->in_use){
		// spin wait
	}
}
static thread_info_t * next_worker_fifo(sched_queue_t *queue){
	 return list_get_tail(queue->queue)->datum;
}
static thread_info_t * next_worker_rr(sched_queue_t *queue){
	list_elem_t* ret = list_get_head(queue->queue);
	list_remove_elem(queue->queue,ret);
	list_insert_tail(queue->queue,ret);
	return ret->datum;
}
static void wait_for_queue(sched_queue_t *queue){
	while(queue->num_threads <= 0){
		// busy wait
	}
}

/*...More functions go here...*/

/* You need to statically initialize these structures: */
sched_impl_t sched_fifo = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu },
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker, next_worker_fifo, wait_for_queue} },
sched_rr = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu },
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker, next_worker_rr, wait_for_queue}};
