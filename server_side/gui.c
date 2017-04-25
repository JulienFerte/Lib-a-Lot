#include "c_header.h"

gui_stuff* gui_stuff_new() {
	gui_stuff* result = (gui_stuff*) malloc( sizeof( gui_stuff ) );

//*** Get the correct coordinates
	gui_stuff_update( result );

//*** List the options available to the admin
	result->admin_menu_choices_size = 4;

	result->admin_menu_user_data = (int*) malloc( sizeof( int ) * ( result->admin_menu_choices_size ) );
	result->admin_menu_choices = (ITEM**) malloc( sizeof( ITEM* ) * ( result->admin_menu_choices_size + 1 ) );

	result->admin_menu_user_data[0] = ADMIN_CLEAR_LOGS;
	result->admin_menu_choices[0] = new_item( "Clear the logs", "" );
	set_item_userptr( result->admin_menu_choices[0], (void*) ( result->admin_menu_user_data ) );

	result->admin_menu_user_data[2] = ADMIN_RELOAD_CONFIGURATION;
	result->admin_menu_choices[2] = new_item( "Reload configuration", "" );
	set_item_userptr( result->admin_menu_choices[2], (void*) ( result->admin_menu_user_data + 2 ) );

	result->admin_menu_user_data[ result->admin_menu_choices_size-1 ] = ADMIN_EXIT;
	result->admin_menu_choices[result->admin_menu_choices_size-1] = new_item( "Exit Lib-a-Lot", "" );
	set_item_userptr( result->admin_menu_choices[result->admin_menu_choices_size-1], (void*) ( result->admin_menu_user_data + result->admin_menu_choices_size - 1 ) );

	result->admin_menu_choices[result->admin_menu_choices_size] = NULL;


	result->admin_menu = new_menu( result->admin_menu_choices );

	return result;
}
void gui_stuff_update( gui_stuff* gui ) {
	gui->width = COLS;
	gui->height = LINES;

//*** Compute the dimensions necessary for the rectangles
	gui->half_height = (int) ( gui->height / 2 );
	gui->one_quarter_width = (int) ( ( gui->width ) / 4 );
	gui->half_width = (int) ( gui->width / 2 );
	gui->three_quarter_width = (int) ( ( 3 * gui->width ) / 4 );

//*** Compute the rectangles for the subwindows
	gui->admin_rectangle.row = 0;
	gui->admin_rectangle.column = 25;
	gui->admin_rectangle.height = gui->half_height - 1;
	gui->admin_rectangle.width = gui->half_width - 25 - 1;

	gui->socket_rectangle.row = 0;
	gui->socket_rectangle.column = gui->half_width + 1;
	gui->socket_rectangle.height = gui->height;
	gui->socket_rectangle.width = gui->one_quarter_width - 2;

	gui->task_rectangle.row = 0;
	gui->task_rectangle.column = gui->three_quarter_width + 1;
	gui->task_rectangle.height = gui->height;
	gui->task_rectangle.width = gui->one_quarter_width - 1;
}
void gui_stuff_free( gui_stuff* gui ) {
	unpost_menu( gui->admin_menu );
	int i = 0;
	free( gui->admin_menu_user_data );
	for( ; i < gui->admin_menu_choices_size; ++i ) {
		free_item( gui->admin_menu_choices[ i ] );
	}
	free( gui->admin_menu );
}

void fill_blanks( int row, int column, int max_column ) {
	for( ; column <= max_column; ++column ) {
		mvaddch( row, column, ' ' );
	}
}
void show_text( rectangle* area, thread_context* context ) {
	int row = area->row;
	int column = area->column;

	int max_rows = area->row + area->height;
	int max_columns = area->column + area->width;

	pthread_mutex_lock( &(context->text_mutex) );

	if( context->title != NULL ) {
		char* title = context->title;
		while( *title ) {
			if( *title == '\n' ) {
				fill_blanks( row, column, max_columns );
				column = area->column;
				++row;
			}
			else if( *title == '\t' ) {
				mvaddch( row, column, *title );
				column = 4 * ( ( column / 4 ) + 1);
			}
			else {
				mvaddch( row, column, *title );
				++column;
			}
			if( column > max_columns ) {
				column = area->column;
				++row;
			}
			++title;
		}
		column = area->column;
		++row;
	}

	char* lines = NULL;
	list_aux* current_text = context->text->head;
	while( current_text != NULL && row <= max_rows ) {
		lines = (char*) ( current_text->content );
		while( *lines && row <= max_rows ) {
			if( *lines == '\n' ) {
				fill_blanks( row, column, max_columns );
				column = area->column;
				++row;
			}
			else if( *lines == '\t' ) {
				mvaddch( row, column, *lines );
				column = 4 * ( ( column / 4 ) + 1);
			}
			else {
				mvaddch( row, column, *lines );
				++column;
			}
			if( column > max_columns ) {
				column = area->column;
				++row;
			}
			++lines;
		}
		current_text = current_text->next;
		if( row <= max_rows ) {
			fill_blanks( row, column, max_columns );
		}
		column = area->column;
		++row;
	}

	if( current_text != NULL ) {
		context->text->tail = current_text;

		current_text = current_text->next;
		list_aux* next = NULL;
		while( current_text != NULL ) {
			free( (char*) ( current_text->content ) );
			next = current_text->next;
			list_aux_destroy( current_text );
			current_text = next;
		}

		context->text->tail->next = NULL;
	}

	column = area->column;
	for( ; row <= max_rows; ++row ) {
		fill_blanks( row, column, max_columns );
	}

	pthread_mutex_unlock( &(context->text_mutex) );
}

void draw( gui_stuff* gui, thread_context* admin_context, thread_context* socket_context, thread_context* task_context ) {
//*** Display the menu
	post_menu( gui->admin_menu );

//*** Draw the lines separating the fields
	mvhline_set( gui->half_height, 0, WACS_HLINE, gui->half_width );
	mvvline_set( 0, gui->half_width, WACS_VLINE, gui->height );
	mvvline_set( 0, gui->three_quarter_width, WACS_VLINE, gui->height );
	mvadd_wch( gui->half_height, gui->half_width, WACS_RTEE );

//*** Show the text from the amdin manager
	show_text( &(gui->admin_rectangle), admin_context );

//*** Show the text from the socket manager thread
	show_text( &(gui->socket_rectangle), socket_context );

//*** Show the text from the task manager thread
	show_text( &(gui->task_rectangle), task_context );
}

list* add_text( thread_context* context, const char* text ) {
	size_t text_length = strlen( text );
	char* copy = malloc( sizeof( char ) * ( text_length + 1 ) );
	strcpy( copy, text );
	copy[ text_length ] = '\0';

	pthread_mutex_lock( &(context->text_mutex) );
	list* result = list_pushhead( context->text, copy );
	pthread_mutex_unlock( &(context->text_mutex) );

	return result;
}
list* add_text_end( thread_context* context, const char* text ) {
	size_t text_length = strlen( text );
	char* copy = malloc( sizeof( char ) * ( text_length + 1 ) );
	strcpy( copy, text );
	copy[ text_length ] = '\0';

	pthread_mutex_lock( &(context->text_mutex) );
	list* result = list_pushtail( context->text, copy );
	pthread_mutex_unlock( &(context->text_mutex) );

	return result;
}
list* insert_text_before( thread_context* context, list_aux* iterator, const char* text ) {
	size_t text_length = strlen( text );
	char* copy = malloc( sizeof( char ) * ( text_length + 1 ) );
	strcpy( copy, text );
	copy[ text_length ] = '\0';

	pthread_mutex_lock( &(context->text_mutex) );
	list* result = list_insert_before( context->text, iterator, copy );
	pthread_mutex_unlock( &(context->text_mutex) );

	return result;
}
list* insert_text_after( thread_context* context, list_aux* iterator, const char* text ) {
	size_t text_length = strlen( text );
	char* copy = malloc( sizeof( char ) * ( text_length + 1 ) );
	strcpy( copy, text );
	copy[ text_length ] = '\0';

	pthread_mutex_lock( &(context->text_mutex) );
	list* result = list_insert_after( context->text, iterator, copy );
	pthread_mutex_unlock( &(context->text_mutex) );

	return result;
}
