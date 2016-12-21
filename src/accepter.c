#include "accepter.h"

accepter_t *accepter_init(int bufferSize,list_t *list,pthread_mutex_t *mutex) {
	accepter_t *accepter = (accepter_t *) malloc(sizeof(accepter_t));
	accepter->buffer = buffer_init(bufferSize);
	pthread_mutex_lock(mutex);
	accepter->readerList = list;
	pthread_mutex_unlock(mutex);
	accepter->listMutex = mutex;
	return accepter;
}

void accepter_destroy(accepter_t *accepter){
	buffer_destroy(accepter->buffer);
	free(accepter);
}

// aggiungo un reader alla lista dei reader e lo lancio in esecuzione
reader_t *add_reader(msg_t *request,accepter_t *accepter) {
	pthread_t readThread;
	int procTime = request->content;
	request->msg_destroy(request);
	reader_t *reader = reader_init(procTime,accepter->readerList,accepter->listMutex);
	pthread_mutex_lock(accepter->listMutex);
	addElement(accepter->readerList,reader);
	pthread_mutex_unlock(accepter->listMutex);
	pthread_create(&readThread,NULL,reader_run,reader);
	return reader;
}

// funzione principale di un accepter
void accepter_run(accepter_t *accepter) {
	buffer_t *accepterBuffer = accepter->buffer;
	msg_t *request = get_bloccante(accepterBuffer);
	while (request!=POISON_PILL) {
		add_reader(request,accepter);
		request = get_bloccante(accepterBuffer);
	}

	request->msg_destroy(request);
}

// invio una richiesta nel buffer dell'accepter
msg_t *send_request(buffer_t *bufferAccepter,int procTime) {
	msg_t *msg = msg_init_int(procTime);
	put_bloccante(bufferAccepter,msg);
	return msg;
}

msg_t *send_accepter_poison_pill(accepter_t *accepter) {
	put_bloccante(accepter->buffer,POISON_PILL);
	return POISON_PILL;
}







