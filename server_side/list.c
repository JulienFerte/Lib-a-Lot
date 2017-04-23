#include "list.h"

#include "log_record.h"

#include <stdio.h>
#include <stdlib.h>

list_aux* list_aux_new( void ) {
	list_aux* result = malloc( sizeof( list_aux ) );

	if( result != NULL ) {
		result->previous = NULL;
		result->content = NULL;
		result->next = NULL;
	}
	else {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't allocate memory for the list_aux", __FUNCTION__, __FILE__, __LINE__ );
	}

	return result;
}

void list_aux_destroy( list_aux* thing ) {
	free( thing );
}

list* list_new( void ) {
	list* result = malloc( sizeof( list ) );

	if( result != NULL ) {
		result->head = NULL;
		result->tail = NULL;
	}
	else {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: I can't allocate memory for the list", __FUNCTION__, __FILE__, __LINE__ );
	}

	return result;
}

list* list_pushhead( list* thing, void* element ) {
	list_aux* new_head = list_aux_new();
	new_head->content = element;

	if( thing->head != NULL ) {
		thing->head->previous = new_head;
		new_head->next = thing->head;
	}
	else {
		thing->tail = new_head;
	}

	thing->head = new_head;

	return thing;
}
list* list_pushtail( list* thing, void* element ) {
	list_aux* new_tail = list_aux_new();
	new_tail->content = element;

	if( thing->tail != NULL ) {
		thing->tail->next = new_tail;
		new_tail->previous = thing->tail;
	}
	else {
		thing->head = new_tail;
	}

	thing->tail = new_tail;

	return thing;
}

list* list_insert_after( list* container, list_aux* iterator, void* element ) {
	if( container->tail == iterator ) {
		list_pushtail( container, element );
	}
	else {
		list_aux* new_iterator = list_aux_new();
		new_iterator->content = element;

		new_iterator->next = iterator->next;
		new_iterator->next->previous = new_iterator;
		iterator->next = new_iterator;
		new_iterator->previous = iterator;
	}
	return container;
}
list* list_insert_before( list* container, list_aux* iterator, void* element ) {
	if( container->head == iterator ) {
		list_pushhead( container, element );
	}
	else {
		list_aux* new_iterator = list_aux_new();
		new_iterator->content = element;

		new_iterator->next = iterator;
		new_iterator->previous = iterator->previous;
		new_iterator->previous->next = new_iterator;
		iterator->previous = new_iterator;
	}
	return container;
}

void* list_readhead( list* thing ) {
	if( thing->head != NULL ) {
		return thing->head->content;
	}
	else {
		return NULL;
	}
}
void* list_readtail( list* thing ) {
	if( thing->tail != NULL ) {
		return thing->tail->content;
	}
	else {
		return NULL;
	}
}

list* list_pophead( list* thing, void** container ) {
	if( thing->head != NULL ) {
		if( container != NULL ) {
			*container = thing->head->content;
		}
		if( thing->head == thing->tail ) {
			thing->tail = NULL;
			list_aux_destroy( thing->head );
			thing->head = NULL;
		}
		else {
			list_aux* new_head = thing->head->next;
			list_aux_destroy( thing->head );
			thing->head = new_head;
		}
	}
	else {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: there is nothing to pop", __FUNCTION__, __FILE__, __LINE__ );
	}
	return thing;
}
list* list_poptail( list* thing, void** container ) {
	if( thing->tail != NULL ) {
		if( container != NULL ) {
			*container = thing->tail->content;
		}
		if( thing->head == thing->tail ) {
			thing->head = NULL;
			list_aux_destroy( thing->tail );
			thing->tail = NULL;
		}
		else {
			list_aux* new_tail = thing->tail->previous;
			list_aux_destroy( thing->tail );
			thing->tail = new_tail;
		}
	}
	else {
//		write_log( LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: there is nothing to pop", __FUNCTION__, __FILE__, __LINE__ );
	}
	return thing;
}

void list_remove( list* thing, list_aux* element ) {
	if( element == thing->head ) {
		list_pophead( thing, NULL );
	}
	else if( element == thing->tail ) {
		list_poptail( thing, NULL );
	}
	else {
		list_aux* previous = element->previous;
		list_aux* next = element->next;

		previous->next = next;
		next->previous = previous;

		list_aux_destroy( element );
	}
}

bool list_is_empty( const list* thing ) {
	return (thing->head == NULL);
}

void list_destroy( list* thing ) {
	list_aux* current = thing->head;
	list_aux* next = NULL;
	while( current != NULL ) {
		next = current->next;
		list_aux_destroy( current );
		current = next;
	}
	free( thing );
}
