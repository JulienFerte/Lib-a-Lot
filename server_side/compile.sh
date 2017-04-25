#!/bin/bash

gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c configuration.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c daemon.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c gui.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c list.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c log_record.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c main.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c module_context.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c mysql_connection_common.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c sized_string.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c socket_manager.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c string.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c task.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c task_manager.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c thread.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c thread_context.c &&

gcc -g -O0 -o LibALot.run \
		configuration.o daemon.o gui.o list.o log_record.o main.o module_context.o mysql_connection_common.o sized_string.o socket_manager.o string.o task.o task_manager.o thread.o thread_context.o \
		-lfl `curl-config --libs` `mysql_config --libs` `xml2-config --libs` -ldl -lmenu -lncursesw -lstdc++ &&

rm *.o &&

echo "Has successfully compiled LibALot.run"
