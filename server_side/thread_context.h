#ifndef THREAD_CONTEXT_H
#define THREAD_CONTEXT_H

#include "h_header.h"

#include "list.h"

struct configuration_struct_tag;

typedef struct {
//*** The global configuration
	struct configuration_struct_tag* configuration;

//*** About the tasks
	pthread_mutex_t* task_fifo_mutex;
	sem_t* task_fifo_emptiness_semaphore;
	list* task_fifo;

//*** The following boolean is true iff Lib-a-Lot is running
	bool* running;

//*** Flag telling the state
	int* flag;

//*** List the lines to show
	char* title;
	list* text;
	pthread_mutex_t text_mutex;
} thread_context;

thread_context* thread_context_new( void );
void thread_context_free( thread_context* );

#endif
