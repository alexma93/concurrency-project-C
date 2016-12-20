
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
}

/* il reader legge un messaggio e la poison pill,
 * e si rimuove dalla lista dei reader dopo che la lista diventa libera */
//TODO: i reader non devono fare sleep, problema chiesto al prof
void leggi_messaggio_lista_bloccata(void) {
	list_t * list = list_init();
	accepter_t *accepter = accepter_init(2,list);

	//creo un reader
	send_request(accepter->buffer,2);
	msg_t * request = get_bloccante(accepter->buffer);
	reader_t *reader = add_reader(request,accepter);

	//carico un messaggio sul reader
	msg_t *message = msg_init_string("MESSAGGIO");
	put_bloccante(reader->buffer,message);
	put_bloccante(reader->buffer,POISON_PILL);

	pthread_t readerThread;
	pthread_mutex_lock(&(accepter->listMutex));
	pthread_create(&readerThread,NULL,reader_run,reader);
	sleep(1);

	CU_ASSERT_EQUAL(size(list),1);

	pthread_mutex_unlock(&(accepter->listMutex));
	pthread_join(readerThread,NULL);

	CU_ASSERT_EQUAL(size(list),0);

}






