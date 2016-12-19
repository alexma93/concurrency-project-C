
#include "dispatcher.h"
#include "buffer/buffer.h"
#include "reader.h"

void remove_slow_readers(list_t *listReader,list_t *slowReaders) {
	iterator_t *iterator = iterator_init(slowReaders);
	reader_t *reader;
	while(hasNext(iterator)) {
		reader = (reader_t*)next(iterator);
		put_bloccante(reader->buffer,POISON_PILL);
		removeElement(listReader,reader);
	}
}

int send_to_reader(reader_t *reader,msg_t *m) {
	msg_t *msg = put_non_bloccante(reader->buffer,m);
	if(msg==BUFFER_ERROR)
		return 0;
	return 1;
}


void dispatch_run(buffer_t *buffer,list_t *listReader,pthread_mutex_t *listMutex) {
	iterator_t *iterator;
	msg_t *msg = get_bloccante(buffer);;
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
	    remove_slow_readers(listReader,slowReaders);//TODO: rimuove due volte i lettori lenti
	    pthread_mutex_unlock(listMutex);
	    msg = get_bloccante(buffer);
	}
	//invio la poison_pill a tutti
	iterator = iterator_init(listReader);
	while(hasNext(iterator)) {
		reader = (reader_t*)next(iterator);
		put_bloccante(reader->buffer,POISON_PILL);
	}

}

dispatch_run_thread(void *argp) {
	dispatch_args *args = argp;
	dispatch_run(args->buffer,args->listReader,args->listMutex);
}

//i buffer hanno dimensioni enormi
void dispatch_run_simple(buffer_t *buffer,list_t *listReader) {
	iterator_t *iterator;
	msg_t *msg = get_bloccante(buffer);;
	reader_t *reader;
	while (msg!=POISON_PILL) {
	    iterator = iterator_init(listReader);
	    while(hasNext(iterator)) {
	        reader = (reader_t*)next(iterator);
	        send_to_reader_simple(reader,msg);
	    }
	    msg = get_bloccante(buffer);
	}
}


void send_to_reader_simple(reader_t *reader,msg_t *m) {
	msg_t *msg = put_non_bloccante(reader->buffer,m);
}




