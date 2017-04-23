#ifndef GUI_H
#define GUI_H

#include "h_header.h"

#include "list.h"
#include "thread_context.h"

#define ADMIN_NONE					0
#define ADMIN_EXIT					1
#define ADMIN_CLEAR_LOGS			2
#define ADMIN_IMPORT_SIFTS			3
#define ADMIN_RELOAD_CONFIGURATION	4

typedef struct {
	int row;
	int column;
	int width;
	int height;
} rectangle;

typedef struct {
	int height;
	int width;

	int half_height;
	int one_quarter_width;
	int half_width;
	int three_quarter_width;

	rectangle admin_rectangle;
	rectangle import_sifts_rectangle;
	rectangle socket_rectangle;
	rectangle task_rectangle;

	int admin_menu_choices_size;
	int* admin_menu_user_data;
	ITEM** admin_menu_choices;
	MENU* admin_menu;
} gui_stuff;

gui_stuff* gui_stuff_new( void );
void gui_stuff_update( gui_stuff* );
void gui_stuff_free( gui_stuff* );

void fill_blanks( int, int, int );
void show_text( rectangle*, thread_context* );

void draw( gui_stuff*, thread_context*, thread_context*, thread_context*, thread_context* );

list* add_text( thread_context*, const char* );
list* add_text_end( thread_context*, const char* );
list* insert_text_before( thread_context*, list_aux*, const char* );
list* insert_text_after( thread_context*, list_aux*, const char* );

#endif
