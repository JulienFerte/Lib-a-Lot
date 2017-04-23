#include "thread_context.h"

thread_context* thread_context_new() {
	thread_context* result = (thread_context*) malloc( sizeof( thread_context ) );

	result->configuration = NULL;
	result->task_fifo_mutex = NULL;
	result->task_fifo_emptiness_semaphore = NULL;
	result->task_fifo = NULL;
	result->running = NULL;
	result->flag = NULL;
	result->title = NULL;
	result->text = NULL;
	pthread_mutex_init( &(result->text_mutex), NULL );

	return result;
}
void thread_context_free( thread_context* context ) {
	pthread_mutex_destroy( &(context->text_mutex) );

	if( context->title != NULL ) {
		free( context->title );
	}

	list_aux* current = NULL;
	for( current = context->text->head; current != NULL; current = current->next ) {
		free( (char*) (current->content) );
	}
	list_destroy( context->text );

	free( context );
}
