#ifndef TASK_H
#define TASK_H

#include "h_header.h"

#include "thread_context.h"

typedef struct {
	unsigned long module_number;
	unsigned long sequence_id;
	unsigned long mutation_id;
	unsigned long structure_id;
} task;

task* task_new( configuration_struct*, unsigned long, unsigned long, unsigned long, unsigned long );
void task_free( task* );

void new_task( thread_context*, unsigned long, unsigned long, unsigned long, unsigned long );

#endif
