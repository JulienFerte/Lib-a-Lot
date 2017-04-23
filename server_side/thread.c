#include "thread.h"

void tells_finished( void* argument ) {
	int* flag = (int*) argument;
	*flag = THREAD_WAITING_JOIN;
}
