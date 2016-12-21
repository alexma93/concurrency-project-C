#ifndef READER_H_
#define READER_H_

#include "buffer/buffer.h"
#include "list/list.h"
#include "poison_pill.h"

//dimensione nota e uguale per tutti del buffer. La cambio per alcuni test e nel main.
int DIM_BUFFER_READER = 5;

typedef struct reader_t {
	buffer_t *buffer;
	int proc_time; //tempo di processamento
	list_t *list;
	pthread_mutex_t *listMutex;
} reader_t;

reader_t *reader_init(int time,list_t *list,pthread_mutex_t *mutex);
void reader_destroy(reader_t *reader);
msg_t *read_msg(reader_t *reader);
void reader_run(reader_t *reader);

#endif
