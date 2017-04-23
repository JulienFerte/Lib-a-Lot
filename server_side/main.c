#include "c_header.h"

int main(int argc, char** argv ) {
//*** The global configuration
	configuration_struct configuration;

//*** About the tasks
	pthread_mutex_t task_fifo_mutex;
	sem_t task_fifo_emptiness_semaphore;
	list* task_fifo;

//*** The following boolean is true iff Lib-a-Lot is running
	bool running;

//*** Capturing errors
	int error_number;

//*** Initialize the ncurses library
	initscr();
	cbreak();
	keypad( stdscr, true );
	noecho();
	curs_set( 0 );
	halfdelay( 1 );

//*** Set the locale for the wide characters, utf-8
	setlocale( LC_ALL, "C" );

//*** Prepare the data for the GUI
	gui_stuff* gui = gui_stuff_new();

//*** Initiate the mutexes, the fifo_emptiness_semaphore and the lists
	pthread_mutex_init( &task_fifo_mutex, NULL );
	sem_init( &task_fifo_emptiness_semaphore, 0, 1 );

	task_fifo = list_new();

//*** Loading the configuration file .LibALot.conf
	configuration_load( &configuration );

//*** Global initialization of libcurl
	curl_global_init( CURL_GLOBAL_DEFAULT );

//*** Global loop variable
	running = true;

//*** The admin context
	int admin_thread_flag = THREAD_RUNNING;
	thread_context context_admin = {	.configuration = &configuration,
										.task_fifo_mutex = &task_fifo_mutex,
										.task_fifo_emptiness_semaphore = &task_fifo_emptiness_semaphore,
										.task_fifo = task_fifo,
										.running = &running,
										.flag = &admin_thread_flag,
										.title = NULL,
										.text = list_new() };
	pthread_mutex_init( &(context_admin.text_mutex), NULL );
	pthread_t admin_thread_id = pthread_self();

//*** Create and runs a new thread for the task manager
	int task_manager_thread_flag = THREAD_NOT_LAUNCHED;
	thread_context context_task = {	.configuration = &configuration,
									.task_fifo_mutex = &task_fifo_mutex,
									.task_fifo_emptiness_semaphore = &task_fifo_emptiness_semaphore,
									.task_fifo = task_fifo,
									.running = &running,
									.flag = &task_manager_thread_flag,
									.title = NULL,
									.text = list_new() };
	pthread_mutex_init( &(context_task.text_mutex), NULL );
	pthread_t task_manager_thread_id;
	if( pthread_create( &task_manager_thread_id, NULL, task_manager, &context_task ) ) {
		write_log( &configuration, LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: can't launch the thread of the task manager", __FUNCTION__, __FILE__, __LINE__ );
		return 1;
	}

//*** Create and runs a new thread for the socket manager
	int socket_manager_thread_flag = THREAD_NOT_LAUNCHED;
	thread_context context_socket = { 	.configuration = &configuration,
										.task_fifo_mutex = &task_fifo_mutex,
										.task_fifo_emptiness_semaphore = &task_fifo_emptiness_semaphore,
										.task_fifo = task_fifo,
										.running = &running,
										.flag = &socket_manager_thread_flag,
										.title = NULL,
										.text = list_new() };
	pthread_mutex_init( &(context_socket.text_mutex), NULL );
	pthread_t socket_manager_thread_id;
	if( pthread_create( &socket_manager_thread_id, NULL, socket_manager, &context_socket ) ) {
		write_log( &configuration, LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: can't launch the thread of the socket manager", __FUNCTION__, __FILE__, __LINE__ );
		return 1;
	}

//*** Prepare everything for launching the thread importing the SIFTS database
	int put_sifts_into_database_thread_flag = THREAD_NOT_LAUNCHED;
	thread_context context_update_sifts = {	.configuration = &configuration,
											.task_fifo_mutex = &task_fifo_mutex,
											.task_fifo_emptiness_semaphore = &task_fifo_emptiness_semaphore,
											.task_fifo = task_fifo, .running = &running,
											.flag = &put_sifts_into_database_thread_flag,
											.title = NULL,
											.text = list_new() };
	pthread_mutex_init( &(context_update_sifts.text_mutex), NULL );
	pthread_t put_sifts_into_database_thread_id;

//*** Prepare everything for launching the thread clearing the log files
	int clear_log_thread_flag = THREAD_NOT_LAUNCHED;
	thread_context context_clear_log = {	.configuration = &configuration,
											.task_fifo_mutex = &task_fifo_mutex,
											.task_fifo_emptiness_semaphore = &task_fifo_emptiness_semaphore,
											.task_fifo = task_fifo, .running = &running,
											.flag = &clear_log_thread_flag,
											.title = NULL,
											.text = list_new() };
	pthread_mutex_init( &(context_clear_log.text_mutex), NULL );
	pthread_t clear_log_thread_id;

//*** Managing Lib-a-Lot
	int command;
	int choice = ADMIN_NONE;

	while( running ) {

		command = getch();

		switch( command ) {
			case KEY_DOWN:;
				menu_driver( gui->admin_menu, REQ_DOWN_ITEM );
				break;
			case KEY_UP:;
				menu_driver( gui->admin_menu, REQ_UP_ITEM );
				break;
//*** The neter key has been pressed
			case 10:;
				choice = *( (int*) ( item_userptr( current_item( gui->admin_menu ) ) ) );
				switch( choice ) {
//*** For exiting Lib-a-Lot
					case ADMIN_EXIT:;
						running = false;
						write_log( &configuration, LOG_ADMIN_EVENT, "Lib-a-Lot exiting..." );
						add_text( &context_admin, "Lib-a-Lot exiting..." );
						break;
//*** For clearing all the log files
					case ADMIN_CLEAR_LOGS:;
						write_log( &configuration, LOG_ADMIN_EVENT, "Clearing logs..." );
						add_text( &context_admin, "Clearing logs..." );
						clear_logs( &context_clear_log );
						write_log( &configuration, LOG_ADMIN_EVENT, "Logs cleared" );
						add_text( &context_admin, "Logs cleared" );
						break;
//*** For updating the database
					case ADMIN_IMPORT_SIFTS:;
						switch( put_sifts_into_database_thread_flag ) {
							case THREAD_RUNNING:;
								add_text( &context_admin, "The thread putting SIFTS into the database is still running" );
								break;
							case THREAD_WAITING_JOIN:;
								if( pthread_join( put_sifts_into_database_thread_id, NULL ) ) {
									error_number = errno;
									write_log( &configuration, LOG_ADMIN_ERROR, "Error when calling %s, in %s at line %i: can't join the thread putting SIFTS into the database, error message: \"%s\"",
																				__FUNCTION__, __FILE__, __LINE__, strerror( error_number ) );
								}
								else {
									add_text( &context_admin, "Successfully joined the thread putting SIFTS into the database" );
								}
							case THREAD_NOT_LAUNCHED:;
								if( pthread_create( &put_sifts_into_database_thread_id, NULL, put_sifts_into_database, &context_update_sifts ) ) {
									error_number = errno;
									write_log( &configuration, LOG_ADMIN_ERROR, "Error when calling %s, in %s at line %i: can't launch the thread putting SIFTS into the database, error message: \"%s\"",
																				__FUNCTION__, __FILE__, __LINE__, strerror( error_number ) );
								}
								else {
									put_sifts_into_database_thread_flag = THREAD_RUNNING;
									write_log( &configuration, LOG_ADMIN_EVENT, "Launched the thread putting the SIFTS database into the local database" );
									add_text( &context_admin, "Importing SIFTS" );
								}
								break;
						}
						break;
//*** For reloading the configuration file
					case ADMIN_RELOAD_CONFIGURATION:;
						configuration_reload( &configuration );
						write_log( &configuration, LOG_ADMIN_EVENT, "Reloaded the configuration" );
						add_text( &context_admin, "Reloaded the configuration" );
						break;
				}
				break;
		}

//*** Drawing and refreshing
		draw( gui, &context_admin, &context_update_sifts, &context_socket, &context_task );
		refresh();
	}


//*** Second half of the main programm
//*** Waits until the thread running the socket manager is finished
	if( pthread_join( socket_manager_thread_id, NULL ) ) {
		error_number = errno;
		write_log( &configuration, LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: can't join the socket manager thread, error message: \"%s\"", __FUNCTION__, __FILE__, __LINE__, strerror( error_number ) );
		return 1;
	}

//*** Forces the unlocking of the task_manager
	sem_post( &task_fifo_emptiness_semaphore );

//*** Waits until the thread running the task manager is finished
	if( pthread_join( task_manager_thread_id, NULL ) ) {
		error_number = errno;
		write_log( &configuration, LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: can't join the task manager thread, error message: \"%s\"", __FUNCTION__, __FILE__, __LINE__, strerror( error_number ) );
		return 1;
	}

//*** Terminate the thread importing SIFTS if it was launched
	switch( put_sifts_into_database_thread_flag ) {
		case THREAD_RUNNING:;
			write_log( &configuration, LOG_LIBALOT_EVENT, "The thread putting SIFTS into the database is still running, killing it" );
			add_text( &context_admin, "The thread putting SIFTS into the database is still running, killing it\n" );
			pthread_kill( put_sifts_into_database_thread_id, 0 );
			write_log( &configuration, LOG_LIBALOT_EVENT, "Killed the thread putting SIFTS into the database" );
			add_text( &context_admin, "Killed the thread putting SIFTS into the database\n" );
			break;
		case THREAD_WAITING_JOIN:;
			if( pthread_join( put_sifts_into_database_thread_id, NULL ) ) {
				error_number = errno;
				write_log( &configuration, LOG_ADMIN_ERROR, "Error when calling %s, in %s at line %i: can't join the thread putting SIFTS into the database, error message: \"%s\"", __FUNCTION__, __FILE__, __LINE__, strerror( error_number ) );
			}
			break;
		case THREAD_NOT_LAUNCHED:;
			break;
	}

//*** Unloading the configuration file .LibALot.conf
	configuration_unload( &configuration );

//*** Clean CURL lib
	curl_global_cleanup();

//*** Clean the thread resources
	pthread_mutex_destroy( &task_fifo_mutex );
	sem_destroy( &task_fifo_emptiness_semaphore );

	pthread_mutex_destroy( &(context_admin.text_mutex) );
	pthread_mutex_destroy( &(context_task.text_mutex) );
	pthread_mutex_destroy( &(context_socket.text_mutex) );
	pthread_mutex_destroy( &(context_update_sifts.text_mutex) );
	pthread_mutex_destroy( &(context_clear_log.text_mutex) );

	list_destroy( task_fifo );

//*** End the use of ncurses
	gui_stuff_free( gui );
	endwin();

    return 0;
}
