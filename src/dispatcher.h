#ifndef DISPATCHER_H_
#define DISPATCHER_H_


#include "buffer/buffer.h"
#include "reader.h"

//struttura per assegnare i parametri al thread del dispatcher
typedef struct {
	buffer_t *buffer;
	list_t *listReader;
	pthread_mutex_t *listMutex;
} dispatch_args;

void remove_slow_readers(list_t *listReader,list_t *slowReaders);
int send_to_reader(reader_t *reader,msg_t *m);
void dispatch_run(buffer_t *buffer,list_t *listReader,pthread_mutex_t *listMutex);
void dispatch_run_thread(void *argp);

#endif
