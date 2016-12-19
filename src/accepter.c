#include "accepter.h"

accepter_t *accepter_init(int bufferSize,list_t *list) {
	accepter_t *accepter = (accepter_t *) malloc(sizeof(accepter_t));
	accepter->buffer = buffer_init(bufferSize);
	accepter->readerList = list;
	pthread_mutex_init(&(accepter->listMutex),NULL);
	return accepter;
}

void accepter_destroy(accepter_t *accepter){
	buffer_destroy(accepter->buffer);
	free(accepter); //la lista non la distruggo
}

reader_t *add_reader(msg_t *request,accepter_t *accepter) {
	pthread_t readThread;
	int procTime = request->content;
	request->msg_destroy(request);

	pthread_mutex_lock(&(accepter->listMutex));
	reader_t *reader = reader_init(procTime,accepter->readerList,&(accepter->listMutex));
	addElement(accepter->readerList,reader);
	pthread_mutex_unlock(&(accepter->listMutex));
	pthread_create(&readThread,NULL,reader_run,reader);
	return reader;
}

void accepter_run(accepter_t *accepter) {
	buffer_t *accepterBuffer = accepter->buffer;
	msg_t *request = get_bloccante(accepterBuffer);
	while (request!=POISON_PILL) {
		add_reader(request,accepter);
		request = get_bloccante(accepterBuffer);
	}

	request->msg_destroy(request);
	accepter_destroy(accepter);
}

msg_t *send_request(buffer_t *bufferAccepter,int procTime) {
	msg_t *msg = msg_init_int(procTime);
	put_bloccante(bufferAccepter,msg);
	return msg;
}

msg_t *send_accepter_poison_pill(accepter_t *accepter) {
	put_bloccante(accepter->buffer,POISON_PILL);
	return POISON_PILL;
}







