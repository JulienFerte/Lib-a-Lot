#include "c_header.h"

void* task_manager( void* parameter ) {
//*** Treats the parameter
	thread_context* context = (thread_context*) parameter;

//*** Main loop variables
	task* current_task = NULL;
	unsigned long current_module = -1;

	bool is_empty;
	char buffer[256];
	task* task_show = NULL;

	while( *(context->running) ) {

//*** Treat the fifo
		pthread_mutex_lock( context->task_fifo_mutex );
		is_empty = list_is_empty( context->task_fifo );
		pthread_mutex_unlock( context->task_fifo_mutex );
		if( ! is_empty ) {

//*** Empty context->text
			pthread_mutex_lock( &(context->text_mutex) );

			list_aux* current_text = context->text->head;
			for( ; current_text != NULL; current_text = current_text->next ) {
				free( (char*) (current_text->content) );
			}
			list_destroy( context->text );
			context->text = list_new();

			pthread_mutex_unlock( &(context->text_mutex) );

//*** Locking the variable fifo and modifying it
			pthread_mutex_lock( context->task_fifo_mutex );
			list_pophead( context->task_fifo, (void**) &current_task );

//*** Synchronizes the output by filling context->text
			if( context->title != NULL ) {
				free( context->title );
				context->title = NULL;
			}
			context->title = (char*) malloc( sizeof( char ) * 256 );
			snprintf( context->title, 255, "Executing %s %lu %lu %lu", context->configuration->module_array[ current_task->module_number ]->name, current_task->sequence_id, current_task->mutation_id, current_task->structure_id );

			list_aux* current_show = context->task_fifo->head;
			for( ; current_show != NULL; current_show = current_show->next ) {
				task_show = (task*) (current_show->content);
				snprintf( buffer, 255, "%s %lu %lu %lu", context->configuration->module_array[ task_show->module_number ]->name, task_show->sequence_id, task_show->mutation_id, task_show->structure_id );
				add_text_end( context, buffer );
			}

			pthread_mutex_unlock( context->task_fifo_mutex );

//*** Treating the top task
			if( current_task != NULL && 0 <= current_task->module_number && current_task->module_number < context->configuration->module_count ) {
				module* mod = context->configuration->module_array[ current_task->module_number ];

//*** Prepare the module context
				module_context module_context = { .configuration = context->configuration,
													.module_id = (unsigned int) (current_task->module_number) };

//*** Testing for the need to actually launch the module
				bool need = (*(mod->need))( &module_context, current_task->sequence_id, current_task->mutation_id, current_task->structure_id );
				write_log( context->configuration, LOG_LIBALOT_EVENT, "Need to launch %s on sequence id %lu, mutation id %lu, structure id %lu? %s", mod->name, current_task->sequence_id, current_task->mutation_id, current_task->structure_id, ( need ? "true" : "false" ) );

//*** Launch the module if needed
				if( need ) {
					write_log( context->configuration, LOG_LIBALOT_EVENT, "Launching %s on sequence id %lu, mutation id %lu, structure id %lu", mod->name, current_task->sequence_id, current_task->mutation_id, current_task->structure_id );
					(*(mod->exec))( &module_context, current_task->sequence_id, current_task->mutation_id, current_task->structure_id );
				}

				free( current_task );
			}
		}
		else {
			if( context->title != NULL ) {
				free( context->title );
				context->title = NULL;
			}

//*** The engine waits until a new entry is pushed in the fifo
			sem_wait( context->task_fifo_emptiness_semaphore );
		}
	}

	return NULL;
}
