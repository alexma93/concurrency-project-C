

void leggi_messaggio(void) {
	list_t *list = list_init();
	pthread_mutex_t mutex;
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
void prova(reader_t *reader) {
	msg_t *m = NULL;
	while (m!=POISON_PILL)
		m = read_msg(reader);
	pthread_mutex_lock(reader->listMutex);
	removeElement(reader->list,reader); //TODO: anche il dispatcher li toglie dalla lista...
	pthread_mutex_unlock(reader->listMutex);
	printf("\nfinito\n");
	//reader_destroy(reader);
}

//i reader non devono fare sleep
void leggi_messaggio_lista_bloccata(void) {
	list_t * list = list_init();
	accepter_t *accepter = accepter_init(2,list);

	//creo un reader
	send_request(accepter->buffer,0);
	msg_t * request = get_bloccante(accepter->buffer);
	reader_t *reader = add_reader(request,accepter);

	msg_t *message = msg_init_string("MESSAGGIO");
	put_bloccante(reader->buffer,message);
	put_bloccante(reader->buffer,POISON_PILL);

	pthread_mutex_lock(&(accepter->listMutex));
	pthread_t readerThread;
	pthread_create(&readerThread,NULL,prova,reader);
	sleep(1);
	CU_ASSERT_EQUAL(size(list),1);
	pthread_mutex_unlock(&(accepter->listMutex));
	pthread_join(readerThread,NULL);
	CU_ASSERT_EQUAL(size(list),0);

}
