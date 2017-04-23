#include "c_header.h"

void* get_in_addr( struct sockaddr* sa )
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void* socket_manager(void* parameter) {
//*** Treats the parameter
	thread_context* context = (thread_context*) parameter;

//*** Writing stuff for output
	char buffer[256];

//*** Declaring variables
	fd_set client_master;

	int client_listener;

	int socket_id = -1;
	int new_socket_id = -1;
	int socket_id_max = -1;

//*** The amount of time before select stops blocking, allowing for Lib-a-Lot to terminate this present thread
	struct timeval select_waits;
//*** Every second select allows the main loop to test if running is set to false
	select_waits.tv_sec = 1;
	select_waits.tv_usec = 0;

//*** Storing server address
	struct sockaddr_in server_address;

//*** Storing client remote address
	socklen_t client_length;
	struct sockaddr_in client_address;

//*** Treating packets
	size_t packet_content_size = 256;
	char packet_content[ packet_content_size ];
	int packet_size;

	int error_number;

//*** Initialize socket descriptor sets
	FD_ZERO( &client_master );

//*** The flag for the sockets
	unsigned int socket_flags = 0;
	if( context->configuration->engine_reuse_address ) {
		socket_flags |= SO_REUSEADDR;
	}
	if( context->configuration->engine_reuse_port ) {
		socket_flags |= SO_REUSEPORT;
	}

//*** Getting the port from which receiving the sockets
	uint16_t engine_port = (uint16_t) atoi( context->configuration->engine_port );

//*** Initialize listener
	client_listener = socket( AF_INET, SOCK_STREAM, 0 );
	if( client_listener < 0 ) {
		error_number = errno;
		write_log( context->configuration, LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: the socket manager can't open the client listener, error message: \"%s\"", __FUNCTION__, __FILE__, __LINE__, strerror( error_number ) );
		return NULL;
	}
	setsockopt( client_listener, SOL_SOCKET, socket_flags, &(int){ 1 }, sizeof(int) );

//*** Bind the client listener
	bzero( (char *) &server_address, sizeof( server_address ) );
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons( engine_port );

	if( bind( client_listener, (struct sockaddr *) &server_address, sizeof( server_address ) ) < 0 ) {
		error_number = errno;
		write_log( context->configuration, LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: the socket manager can't bind the client listener, error message: \"%s\"", __FUNCTION__, __FILE__, __LINE__, strerror( error_number ) );
		return NULL;
	}
	write_log( context->configuration, LOG_LIBALOT_EVENT, "Socket manager has successfully bound the client listener to %s:%u", inet_ntoa( server_address.sin_addr ), ntohs( (unsigned int) server_address.sin_port ) );

	fcntl( client_listener, F_SETFL, O_NONBLOCK );

	if( listen( client_listener, FD_SETSIZE ) == -1 ) {
		error_number = errno;
		write_log( context->configuration, LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: the socket manager can't make the client listener listen, error message: \"%s\"", __FUNCTION__, __FILE__, __LINE__, strerror( error_number ) );
		return NULL;
	}

//*** Add the listener to the client master
	FD_SET( client_listener, &client_master );
	socket_id_max = client_listener;

//*** Main loop
	while( *(context->running) ) {

//*** Copy the client master, because select modifies client_master_temporary
		fd_set client_master_temporary;
		FD_ZERO( &client_master_temporary );
		client_master_temporary = client_master;

		select_waits.tv_sec = 1;
		select_waits.tv_usec = 0;

		if( select( socket_id_max + 1, &client_master_temporary, NULL, NULL, &select_waits ) == -1 ) {
			error_number = errno;
			write_log( context->configuration, LOG_LIBALOT_ERROR, "Error when calling %s, in %s at line %i: the socket manager can't select, error message: \"%s\"", __FUNCTION__, __FILE__, __LINE__, strerror( error_number ) );
//There should be some cleaning
			return NULL;
		}

//*** Go through the existing connections
//***	If the listener is active, it means there is a pending connection
//***	If another connection is active, it means there is a packet waiting for being treated
		for( socket_id = 0; socket_id <= socket_id_max; ++socket_id ) {
			if( FD_ISSET( socket_id, &client_master_temporary ) ) {

//*** Something is happening somewhere
				if( socket_id == client_listener ) {

//*** A new connection is awaiting
					client_length = sizeof( struct sockaddr_in );
					memset( &client_address, '\0', client_length );
					new_socket_id = accept( client_listener, (struct sockaddr*) &client_address, &client_length );

					if( new_socket_id == -1 ) {
						error_number = errno;
						write_log( context->configuration, LOG_LIBALOT_ERROR, "Error accepting the new socket from the %d listener %d, error n° %d, message: \"%s\"", socket_id, client_listener, error_number, strerror( error_number ) );
					}
					else {

//*** Add the new connection to the client master
						fcntl( new_socket_id, F_SETFL, O_NONBLOCK );
						FD_SET( new_socket_id, &client_master );
						if( new_socket_id > socket_id_max ) {
							socket_id_max = new_socket_id;
						}
						write_log( context->configuration, LOG_LIBALOT_EVENT, "Accepted a new connection from %s:%u on the socket %d", inet_ntoa( client_address.sin_addr ), (unsigned int) client_address.sin_port, new_socket_id );
					}
				}
				else {

//*** The data which are awaiting for being treated are bytes from a packet
					memset( packet_content, '\0', packet_content_size );
					packet_size = read( socket_id, packet_content, packet_content_size - 1 );
					if( packet_size < 0 ) {
						error_number = errno;
						write_log( context->configuration, LOG_LIBALOT_ERROR, "Error getting the data of the packet sent by the socket %d, error message: \"%s\"", socket_id, strerror( error_number ) );
					}
					else if( packet_size == 0 ) {
//						write_log( context->configuration, LOG_LIBALOT_EVENT, "The socket %d hung up", socket_id );
					}
					else {

//*** Treat the packet
						write_log( context->configuration, LOG_LIBALOT_EVENT, "Parsing the packet of size %i \"%s\" received from the socket %d", packet_size, packet_content, socket_id );

						char* packet = malloc( sizeof( char ) * ( packet_size + 1 ) );
						strncpy( packet, (char*) packet_content, packet_size );
						packet[ packet_size ] = '\0';
						char* parse_packet = packet;

						unsigned long tool_id = strtoul( parse_packet, &parse_packet, 10 );
						unsigned long sequence_id = strtoul( parse_packet, &parse_packet, 10 );
						unsigned long mutation_id = strtoul( parse_packet, &parse_packet, 10 );
						unsigned long structure_id = strtoul( parse_packet, &parse_packet, 10 );

						write_log( context->configuration, LOG_LIBALOT_EVENT, "Parsed the received packet into: tool ID = %lu, sequence ID = %lu, mutation ID = %lu, structure ID = %lu", tool_id, sequence_id, mutation_id, structure_id );

//*** Queue the new task
						new_task( context, tool_id, sequence_id, mutation_id, structure_id );
						write_log( context->configuration, LOG_LIBALOT_EVENT, "Queued the new task (%lu, %lu, %lu, %lu)", tool_id, sequence_id, mutation_id, structure_id );

//*** Free strings
						free( packet );
//*** End of the treatment of the packet

//*** Close the socket and remove it from the client master
						close( socket_id );
						FD_CLR( socket_id, &client_master );
						write_log( context->configuration, LOG_LIBALOT_EVENT, "Closed the socket %u", socket_id );
					}
				}
			}
		}

//*** Empty context->text
		pthread_mutex_lock( &(context->text_mutex) );

		list_aux* current_text = context->text->head;
		for( ; current_text != NULL; current_text = current_text->next ) {
			free( (char*) (current_text->content) );
		}
		list_destroy( context->text );
		context->text = list_new();

		pthread_mutex_unlock( &(context->text_mutex) );

//*** Fill context->text
		struct sockaddr_in address;
		socklen_t address_len;

		for( socket_id = 0; socket_id <= socket_id_max; ++socket_id ) {
			if( FD_ISSET( socket_id, &client_master ) ) {
				address_len = sizeof( struct sockaddr_in );
				if( getpeername( socket_id, (struct sockaddr*) &address, &address_len ) == 0 ) {
					snprintf( buffer, 255, "%s:%u", inet_ntoa( address.sin_addr ), (unsigned int) address.sin_port );
					add_text( context, buffer );
				}
			}
		}
	}

	close( client_listener );
	FD_CLR( client_listener, &client_master );

	return NULL;
}
