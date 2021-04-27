#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "node.h"

Queue* createQueue() {
	Queue* queue = (Queue*)malloc(sizeof(Queue));

	//initialize the Queue struct
	queue->first = NULL;
	queue->last = NULL;
	queue->count = 0;

	return queue;
}

void enqueue(Queue** queue, void* new)
{
	Node* node = (Node*)malloc(sizeof(Node));
	
	//create a new node using the data passed in
	node->data = new;
	node->next = NULL;

	//if the queue is empty, then use this newly constructed node as the first
	if ((*queue)->first == NULL) {
		(*queue)->first = node;
		(*queue)->last = node;
	} else { //otherwise add the new node to the end of the queue
		(*queue)->last->next = node;
		(*queue)->last = node;
	}

	//increment the total number of nodes in queue by 1
	(*queue)->count += 1;
}

void* dequeue(Queue* queue) 
{
	//if the queue is empty, then return NULL pointer
	if (queue->first == NULL)
		return NULL;

	Node* node = queue->first;
	void* data = node->data;

	//if the queue only contains one node, then empty out the queue after dequeue
	if (queue->first == queue->last)
		queue->last = NULL;

	//remove the first node from queue
	queue->first = node->next;

	//decrease the total number of nodes in queue by 1
	queue->count -= 1;

	free(node);
	return data;
}

void destroyNode(Queue* queue)
{
	Node* cur = queue->first;
	Node* tmp;

	//go through the entire queue and free up all memory allocated for data in node
	while(cur) {
		tmp = cur->next;
		if(cur->data)
			free(cur->data);
		cur = tmp;
	}
}

void destroyQueue(Queue* queue) 
{
	Node* cur = queue->first;
	Node* tmp;

	//go through the entire queue and free up all memory allocated for Node struct
	while(cur) {
		tmp = cur->next;
		free(cur);
		cur = tmp;
	}

	//destroy this Queue struct
	free(queue);
}
