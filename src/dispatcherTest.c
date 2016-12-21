#include "dispatcher.h"
#include "provider.h"
#include "accepter.h"
#include "CUnit/Basic.h"


void dispatcher_e_provider_buffer_grande(void) {
	provider_t *provider = provider_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++)
		messages[i] = msg_init_string("MESSAGGIO");

	pthread_mutex_t mutex; //mutex della lista dei reader,inutile tanto non ci sono accepter e reader
	pthread_mutex_init(&mutex,NULL);

	//lancio il dispatcher
	pthread_t dispatcher;
	dispatch_args dispArgs;
	dispArgs.buffer = provider->buffer;
	dispArgs.listReader = list_init();
	dispArgs.listMutex = &mutex;
	pthread_create(&dispatcher,NULL,dispatch_run_thread,&dispArgs);

	send_sequence(provider,messages,3);
	pthread_join(dispatcher,NULL);

	CU_ASSERT_EQUAL(provider->buffer->occupied,0);

	provider_destroy(provider);
	for(int i=0;i<3;i++)
			messages[i]->msg_destroy(messages[i]);
	pthread_mutex_destroy(&mutex);
}

void dispatcher_e_provider_buffer_piccolo(void) {
	provider_t *provider = provider_init(3);
	msg_t *messages[4];
	for(int i=0;i<4;i++)
		messages[i] = msg_init_string("MESSAGGIO");

	pthread_mutex_t mutex; //un mutex a caso, tanto non ci sono accepter e reader
	pthread_mutex_init(&(mutex),NULL);

	//lancio il dispatcher
	pthread_t dispatcher;
	dispatch_args dispArgs;
	dispArgs.buffer = provider->buffer;
	dispArgs.listReader = list_init();
	dispArgs.listMutex = &mutex;
	pthread_create(&dispatcher,NULL,dispatch_run_thread,&dispArgs);

	send_sequence(provider,messages,4);
	pthread_join(dispatcher,NULL);

	CU_ASSERT_EQUAL(provider->buffer->occupied,0);

	provider_destroy(provider);
	for(int i=0;i<4;i++)
			messages[i]->msg_destroy(messages[i]);
	pthread_mutex_destroy(&mutex);
}

//versione semplificata(per i test): assumo che i buffer hanno dimensioni enormi quindi nessun reader sara' lento
void send_to_reader_simple(reader_t *reader,msg_t *m) {
	put_non_bloccante(reader->buffer,m);
}
//versione semplificata per i test
void dispatch_run_simple(buffer_t *buffer,list_t *listReader) {
	iterator_t *iterator;
	msg_t *msg = get_bloccante(buffer);
	reader_t *reader;
	while (msg!=POISON_PILL) {
	    iterator = iterator_init(listReader);
	    while(hasNext(iterator)) {
	        reader = (reader_t*)next(iterator);
	        send_to_reader_simple(reader,msg);
	    }
	    iterator_destroy(iterator);
	    msg = get_bloccante(buffer);
	}
	//invio la poison_pill a tutti
	iterator = iterator_init(listReader);
	while(hasNext(iterator)) {
		reader = (reader_t*)next(iterator);
		put_bloccante(reader->buffer,POISON_PILL);
	}
	iterator_destroy(iterator);
}



// testo un dispatcher semplificato, che considera i reader con buffer infiniti
void dispatch_messaggi_buffer_grandi_semplice(void) {
	DIM_BUFFER_READER = 5;
	//metto 3 messaggi in un buffer(che rappresenta il provider) piu' la poison pill
	buffer_t *provBuff = buffer_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++) {
		messages[i] = msg_init_string("MESSAGGIO");
		put_bloccante(provBuff,messages[i]);
	}
	put_bloccante(provBuff,POISON_PILL);

	/*creo 3 reader e li lancio in esecuzione direttamente
	 * utilizzando un accepter che presuppongo gia' testato */
	list_t *list = list_init();
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(5,list,&listMutex);
	for(int i=0;i<3;i++)
		add_reader(send_request(accepter->buffer,1),accepter);

	dispatch_run_simple(provBuff,list);

	iterator_t *iterator = iterator_init(list);
	while(hasNext(iterator)) {
		reader_t *reader = (reader_t*)next(iterator);
		CU_ASSERT(reader->buffer->occupied >=3); //al piu' hanno letto un messaggio
	}
	iterator_destroy(iterator);
	CU_ASSERT_EQUAL(provBuff->occupied,0);

	buffer_destroy(provBuff);
	for(int i=0;i<3;i++)
			messages[i]->msg_destroy(messages[i]);
	pthread_mutex_destroy(&listMutex);
}

// uguale al test precedente, solo con la vera funzione dispatch_run
void dispatch_messaggi_buffer_grandi(void) {
	DIM_BUFFER_READER = 5;
	//metto 3 messaggi in un buffer(che rappresenta il provider) piu' la poison pill
	buffer_t *provBuff = buffer_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++) {
		messages[i] = msg_init_string("MESSAGGIO");
		put_bloccante(provBuff,messages[i]);
	}
	put_bloccante(provBuff,POISON_PILL);

	/*creo 3 reader e li lancio in esecuzione direttamente
	 * utilizzando un accepter che presuppongo gia' testato */
	list_t *list = list_init();
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(5,list,&listMutex);
	for(int i=0;i<3;i++)
		add_reader(send_request(accepter->buffer,1),accepter);

	dispatch_run(provBuff,list,&listMutex);

	iterator_t *iterator = iterator_init(list);
	while(hasNext(iterator)) {
		reader_t *reader = (reader_t*)next(iterator);
		CU_ASSERT(reader->buffer->occupied >=3); //al piu' hanno letto un messaggio
	}
	iterator_destroy(iterator);
	CU_ASSERT_EQUAL(provBuff->occupied,0);

	buffer_destroy(provBuff);
	for(int i=0;i<3;i++)
			messages[i]->msg_destroy(messages[i]);
	pthread_mutex_destroy(&listMutex);
}

// eseguo il dispatcher su 1 reader con buffer ridotto
void dispatcher_un_reader_buffer_piccolo(void) {
	pthread_t * readerThread[3];
	DIM_BUFFER_READER = 2;
	//metto 3 messaggi nel buffer del provider + la poison pill
	buffer_t *provBuff = buffer_init(5);
	msg_t *messages[4];
	for(int i=0;i<4;i++) {
		messages[i] = msg_init_string("MESSAGGIO");
		put_bloccante(provBuff,messages[i]);
	}
	put_bloccante(provBuff,POISON_PILL);

	//creo 3 reader e li eseguo
	list_t *list = list_init();
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(5,list,&listMutex);
	add_reader(send_request(accepter->buffer,1),accepter); // reader gia' in esecuzione

	dispatch_run(provBuff,list,&listMutex);
	CU_ASSERT_EQUAL(size(list),0); // ho rimosso il reader dalla lista perche' lento

	buffer_destroy(provBuff);
	for(int i=0;i<3;i++)
		messages[i]->msg_destroy(messages[i]);
	pthread_mutex_destroy(&listMutex);
	accepter_destroy(accepter);
}

// eseguo il dispatcher su 3 reader con buffer ridotto
void dispatch_messaggi_buffer_piccoli(void) {
	pthread_t * readerThread[3];
	DIM_BUFFER_READER = 2;
	//metto 3 messaggi nel buffer del provider + la poison pill
	buffer_t *provBuff = buffer_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++) {
		messages[i] = msg_init_string("MESSAGGIO");
		put_bloccante(provBuff,messages[i]);
	}
	put_bloccante(provBuff,POISON_PILL);

	//creo 3 reader e li eseguo
	list_t *list = list_init();
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(5,list,&listMutex);
	for(int i=0;i<3;i++)
		add_reader(send_request(accepter->buffer,1),accepter); //3 reader gia' in esecuzione

	dispatch_run(provBuff,list,&listMutex);
	CU_ASSERT_EQUAL(size(list),0);

	buffer_destroy(provBuff);
	for(int i=0;i<3;i++)
			messages[i]->msg_destroy(messages[i]);
	pthread_mutex_destroy(&listMutex);
	accepter_destroy(accepter);
}

// il dispatcher riceve una poison pill e la manda a 3 reader
void dispatch_poison_pill(void) {
	pthread_t * readerThread[3];
	//metto nel buffer del provider la poison pill
	buffer_t *provBuff = buffer_init(3);
	put_bloccante(provBuff,POISON_PILL);

	//creo 3 reader e li eseguo
	list_t *list = list_init();
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(5,list,&listMutex);
	for(int i=0;i<3;i++)
		add_reader(send_request(accepter->buffer,1),accepter); //3 reader gia' in esecuzione

	dispatch_run(provBuff,list,&listMutex);

	CU_ASSERT_EQUAL(size(list),3);
	sleep(2); //non ho i thread dei reader per fare join. Ma i reader "dovrebbero" terminare
	CU_ASSERT_EQUAL(size(list),0);

	buffer_destroy(provBuff);
	pthread_mutex_destroy(&listMutex);
	accepter_destroy(accepter);
}

/* testo il comportamento del dispatcher interagendo con le altre
 * componenti se si aggiunge un altro reader */
void dispatch_aggiunto_un_altro_reader(void) {
	DIM_BUFFER_READER = 5;
	//metto 2 messaggi nel buffer del provider
	provider_t *provider = provider_init(5);
	msg_t *msg =msg_init_string("MESSAGGIO");
	msg_t *msg2 =msg_init_string("MESSAGGIO");
	send_msg(provider,msg);
	send_msg(provider,msg2);

	//creo 1 reader lento e lo eseguo
	list_t *list = list_init();
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(5,list,&listMutex);
	reader_t *reader1 = add_reader(send_request(accepter->buffer,5),accepter);

	//lancio il dispatcher
	pthread_t *dispatcher;
	dispatch_args dispArgs;
	dispArgs.buffer = provider->buffer;
	dispArgs.listReader = list;
	dispArgs.listMutex = &listMutex;
	pthread_create(&dispatcher,NULL,dispatch_run_thread,&dispArgs);

	//dopo aver inviato i messaggi aggiungo un altro reader
	sleep(1);
	reader_t *reader2 = add_reader(send_request(accepter->buffer,5),accepter);

	//aggiungo la poison pill nel provider così che il dispatcher la invii automaticamente
	send_poison_pill(provider);

	sleep(1);
	CU_ASSERT_EQUAL(reader1->buffer->occupied,2); //il primo ha letto al piu 1 messaggio e ne ha 2 nel buffer
	CU_ASSERT_EQUAL(reader2->buffer->occupied,0); //il secondo sta processando l'unico messaggio ricevuto

	provider_destroy(provider);
	msg->msg_destroy(msg);
	msg2->msg_destroy(msg2);
	pthread_mutex_destroy(&listMutex);
	accepter_destroy(accepter);
}




