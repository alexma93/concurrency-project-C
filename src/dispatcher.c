
#include "dispatcher.h"

/* rimuove tutti i reader lenti (quelli che avevano il buffer pieno) dalla lista.
 * Il reader riceve una poison_pill e si distruggera' da solo.
 * tentera' anche di rimuoversi dalla lista, nonostante in questo caso gia' non ci sia (non crea problemi).
 */
void remove_slow_readers(list_t *listReader,list_t *slowReaders) {
	iterator_t *iterator = iterator_init(slowReaders);
	reader_t *reader;
	while(hasNext(iterator)) {
		reader = (reader_t*)next(iterator);
		put_bloccante(reader->buffer,POISON_PILL);
		removeElement(listReader,reader);
	}
	iterator_destroy(iterator);
	list_destroy(slowReaders);
}

// invia un messaggio a un reader, ritorna 1 se ci e' riuscito, altrimenti 0
int send_to_reader(reader_t *reader,msg_t *m) {
	msg_t *msg = put_non_bloccante(reader->buffer,m);
	if(msg==BUFFER_ERROR)
		return 0;
	return 1;
}

// funzione di esecuzione principale
void dispatch_run(buffer_t *buffer,list_t *listReader,pthread_mutex_t *listMutex) {
	iterator_t *iterator;
	msg_t *msg = get_bloccante(buffer);
	reader_t *reader;
	while (msg!=POISON_PILL) {
		list_t *slowReaders = list_init();
		pthread_mutex_lock(listMutex);

		iterator = iterator_init(listReader);
	    while(hasNext(iterator)) {
	        reader = (reader_t*)next(iterator);
	        if(!send_to_reader(reader,msg))
	        	addElement(slowReaders,reader);
	    }
	    iterator_destroy(iterator);
	    remove_slow_readers(listReader,slowReaders);

	    pthread_mutex_unlock(listMutex);
	    msg = get_bloccante(buffer);
	}
	//invio la poison_pill a tutti
	pthread_mutex_lock(listMutex);
	iterator = iterator_init(listReader);
	while(hasNext(iterator)) {
		reader = (reader_t*)next(iterator);
		put_bloccante(reader->buffer,POISON_PILL);
	}
	iterator_destroy(iterator);
	pthread_mutex_unlock(listMutex);
}

void dispatch_run_thread(void *argp) {
	dispatch_args *args = argp;
	dispatch_run(args->buffer,args->listReader,args->listMutex);
}





