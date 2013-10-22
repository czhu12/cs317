/*
 * eth_switch2.c
 * Author: 
 */

#include <stdint.h>
#include "eth_switch2.h"

/* Creates a new empty bounded queue and returns it. */
bounded_queue create_bounded_queue(void) {
  bounded_queue bq = malloc(sizeof(struct bounded_queue));
	bq->front = 0;
	bq->back = 0;
	bq->empty = 1;
	return bq;
}

/* Frees all dynamically allocated space and releases all open
   resources created for the implementation of this queue.
 */
void destroy_bounded_queue(bounded_queue queue) {
	free(queue);
}

/* If the queue is not empty, dequeues an element from the queue and
   returns it. If the queue is empty, returns 0.
 */
uint16_t dequeue_bounded(bounded_queue queue) {
	if(queue_is_empty(queue)) return 0;

	uint16_t frameid = queue->queue[queue->front];

	queue->front++;
	queue->front = queue->front % BUFFER_SIZE;

	if(queue->front == queue->back){
		queue->empty = 1;	
	}

	return frameid;
		
}

/* If the queue is not full, enqueues the provided element into the
   queue and returns non-zero. If the queue is full, the queue is left
   unmodified and the function returns zero.
 */
int enqueue_bounded(bounded_queue queue, uint16_t frameid) {
	if(queue_is_full(queue)) return 0;

	if(queue->empty == 0 && queue->front == queue->back){
		return 0;
	}
	queue->empty = 0;

	queue->queue[queue->back] = frameid;
	queue->back++; 
	queue->back = queue->back % BUFFER_SIZE;

	return 1;
}

/* Returns non-zero if the queue is empty (has no elements), or zero
   otherwise. Note that you are not required to use this function in
   your algorithms, however you may use it as a helper function to
   improve the readability of your code.
 */
int queue_is_empty(bounded_queue queue) {
	if(queue->empty){
		return 1;
	}
	return 0;
}

/* Returns non-zero if the queue is full (has reached the limit number
   of elements), or zero otherwise. Note that you are not required to
   use this function in your algorithms, however you may use it as a
   helper function to improve the readability of your code. Also note
   that this function is not used in the provided main function, so
   the correct implementation of this function will only affect the
   result of your tests if you decide to use it in your
   dequeue/enqueue functions.
 */
int queue_is_full(bounded_queue queue) {
	if (!queue->empty && queue->front == queue->back){
		return 1;
	}
	return 0;
}
