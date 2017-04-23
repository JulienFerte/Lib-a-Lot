#!/bin/bash

cd ./proteomics/

gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c chain.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c chain_sequence.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c fasta.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c file_manager.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c mutation.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c needleman_wunsch.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c point_mutation.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c protein.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c proteome_management.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c sequence.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c structure.c &&
gcc -g -O0 `curl-config --cflags` `mysql_config --cflags` `xml2-config --cflags` -c type_fasta.c &&

cd ../

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
		./proteomics/chain.o ./proteomics/chain_sequence.o ./proteomics/fasta.o ./proteomics/file_manager.o ./proteomics/mutation.o ./proteomics/needleman_wunsch.o ./proteomics/point_mutation.o ./proteomics/protein.o \
		./proteomics/proteome_management.o ./proteomics/sequence.o ./proteomics/structure.o ./proteomics/type_fasta.o \
		-lfl `curl-config --libs` `mysql_config --libs` `xml2-config --libs` -ldl -lmenu -lncursesw -Lproteomics/lib/ -lmmcifchains -lmmtfparser -Lproteomics/lib/cifparse-obj-v7.105-prod-src/lib/ -lmmcif -lstdc++ &&

rm *.o ./proteomics/*.o &&

echo "Has successfully compiled LibALot.run"
