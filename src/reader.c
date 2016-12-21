#include "reader.h"

reader_t *reader_init(int time,list_t *list,pthread_mutex_t *mutex) {
	reader_t *r = (reader_t *) malloc(sizeof(reader_t));
	r->buffer = buffer_init(DIM_BUFFER_READER);
	r->proc_time = time;
	pthread_mutex_lock(mutex);
	r->list = list;
	pthread_mutex_unlock(mutex);
	r->listMutex = mutex;
	return r;
}

void reader_destroy(reader_t *reader) {
	buffer_destroy(reader->buffer);
	free(reader);
}

msg_t *read_msg(reader_t *reader) {
	msg_t * m = get_bloccante(reader->buffer);
	sleep(reader->proc_time);
	return m;
}

void reader_run(reader_t *reader) {
	msg_t *m = NULL;
	while (m!=POISON_PILL) {
		if (m!=NULL)
			m->msg_destroy(m);
		m = read_msg(reader);
	}
	pthread_mutex_lock(reader->listMutex);
	removeElement(reader->list,reader);
	pthread_mutex_unlock(reader->listMutex);
	reader_destroy(reader);
}






