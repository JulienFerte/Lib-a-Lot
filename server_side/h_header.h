#ifndef H_HEADERS_H
#define H_HEADERS_H

//*** I/O-ing with MySQL
#include <mysql/my_global.h>
#include <mysql/my_sys.h>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>

//*** Threading
#include <pthread.h>
#include <semaphore.h>

//*** Usual libraries
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//*** Treating XML documents, the config file of Lib-a-Lot is an XML document
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

//*** Socket connection and file transfert
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <curl/curl.h>

//*** Dynamic loading of the libraries
#include <dlfcn.h>

//*** Error catching
#include <errno.h>

//*** Time functions, for logging and socket management
#include <time.h>
#include <sys/time.h>

//*** Terminal size and drawing
#include <locale.h>
#include <ncursesw/ncurses.h>
#include <menu.h>

#endif
