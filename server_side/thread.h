#ifndef THREAD_H
#define THREAD_H

#include "h_header.h"

#define THREAD_NOT_LAUNCHED		0
#define THREAD_RUNNING			1
#define THREAD_WAITING_JOIN		2

//*** Setting the flag of termination
void tells_finished( void* );

#endif	//	THREAD_H
