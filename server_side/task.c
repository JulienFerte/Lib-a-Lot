#include "c_header.h"

//*** The functions creating and deleting a task
task* task_new( configuration_struct* configuration, unsigned long module_number, unsigned long sequence_id, unsigned long mutation_id, unsigned long structure_id ) {
	task* new_task = NULL;

	if( new_task = malloc( sizeof( task ) ) ) {
		new_task->module_number = module_number;
		new_task->sequence_id = sequence_id;
		new_task->mutation_id = mutation_id;
		new_task->structure_id = structure_id;
	}
	else {
		write_log( configuration, LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't allocate memory for the new task", __FUNCTION__, __FILE__, __LINE__ );
	}

	return new_task;
}
void task_free( task* target ) {
	free( target );
}

void new_task( thread_context* context, unsigned long tool_id, unsigned long sequence_id, unsigned long mutation_id, unsigned long structure_id ) {
//*** Put the task on the fifo
	pthread_mutex_lock( context->task_fifo_mutex );
	list_pushtail( context->task_fifo, task_new( context->configuration, tool_id, sequence_id, mutation_id, structure_id ) );
	sem_post( context->task_fifo_emptiness_semaphore );
	pthread_mutex_unlock( context->task_fifo_mutex );
}
