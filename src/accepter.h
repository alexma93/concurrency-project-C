#ifndef ACCEPTER_H_
#define ACCEPTER_H_

#include "reader.h"

typedef struct accepter_t {
	buffer_t *buffer;
	list_t *readerList;
	pthread_mutex_t listMutex;
} accepter_t;

accepter_t *accepter_init(int bufferSize,list_t *list);
void accepter_destroy(accepter_t *accepter);
reader_t *add_reader(msg_t *request,accepter_t *accepter);
void accepter_run(accepter_t *accepter);
msg_t *send_request(buffer_t *bufferAccepter,int procTime);
msg_t *send_accepter_poison_pill(accepter_t *accepter);

#endif







