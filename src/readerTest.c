#include "CUnit/Basic.h"
#include "list/list.h"
#include <pthread.h>
#include "reader.h"
#include "accepter.h"

// il reader legge un messaggio dal buffer
void leggi_messaggio(void) {
	list_t *list = list_init();
	pthread_mutex_t mutex; //il mutex della lista dei reader
	pthread_mutex_init(&mutex,NULL);
	reader_t *reader = reader_init(0,list,&mutex);
	msg_t *message = msg_init_string("MESSAGGIO");
	put_bloccante(reader->buffer,message);
	msg_t *result = read_msg(reader);

	CU_ASSERT_STRING_EQUAL(result->content,message->content);

	list_destroy(list);
	reader_destroy(reader);
	message->msg_destroy(message);
	result->msg_destroy(result);
}

void leggi_poison_pill(void) {
	list_t *list = list_init();
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex,NULL);
	reader_t *reader = reader_init(0,list,&mutex);
	msg_t *message = POISON_PILL;
	put_bloccante(reader->buffer,message);
	reader_run(reader);

	CU_ASSERT_EQUAL(size(list),0);

	list_destroy(list);
	message->msg_destroy(message);
}

/* il reader legge un messaggio e la poison pill,
 * e si rimuove dalla lista dei reader dopo che la lista diventa libera */
void leggi_messaggio_lista_bloccata(void) {
	list_t * list = list_init();
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;

	//creo un reader e lo eseguo
	pthread_t readThread;
	reader_t *reader = reader_init(1,list,&listMutex);
	addElement(list,reader);

	//blocco la lista
	pthread_mutex_lock(&listMutex);

	pthread_create(&readThread,NULL,reader_run,reader);

	//carico un messaggio sul reader
	msg_t *message = msg_init_string("MESSAGGIO");
	put_bloccante(reader->buffer,message);
	put_bloccante(reader->buffer,POISON_PILL);

	sleep(3);
	CU_ASSERT_EQUAL(size(list),1); //il reader non si e' rimosso dalla lista
	pthread_mutex_unlock(&listMutex);

	pthread_join(readThread,NULL); //aspetto che finisce il reader
	CU_ASSERT_EQUAL(size(list),0);

	list_destroy(list);

}






