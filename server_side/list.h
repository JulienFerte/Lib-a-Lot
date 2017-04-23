#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct list_aux {
	struct list_aux* previous;
	void* content;
	struct list_aux* next;
} list_aux;

list_aux* list_aux_new( void );
void list_aux_destroy( list_aux* );

typedef struct {
	list_aux* head;
	list_aux* tail;
} list;

list* list_new( void );

list* list_pushhead( list*, void* );
list* list_pushtail( list*, void* );

list* list_insert_after( list*, list_aux*, void* );
list* list_insert_before( list*, list_aux*, void* );

void* list_readhead( list* );
void* list_readtail( list* );

list* list_pophead( list*, void** );
list* list_poptail( list*, void** );

void list_remove( list*, list_aux* );

bool list_is_empty( const list* );

//*** To avoid collision with /usr/include/mysql/my_list.h which declares already a function named "list_free"
void list_destroy( list* );

#endif
