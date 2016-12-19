
void dispatcher_e_provider_buffer_grande(void) {
	provider_t *provider = provider_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++)
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

	send_sequence(provider,messages,3);
	pthread_join(dispatcher,NULL);
	CU_ASSERT_EQUAL(provider->buffer->occupied,0);
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
}

void dispatch_messaggi_buffer_grandi_semplice(void) {
	DIM_BUFFER_READER = 5;
	//metto 3 messaggi nel buffer
	provider_t *provider = provider_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++)
		messages[i] = msg_init_string("MESSAGGIO");
	send_sequence(provider,messages,3);

	//creo 3 reader
	list_t *list = list_init();
	accepter_t *accepter = accepter_init(5,list);
	for(int i=0;i<3;i++)
		add_reader(send_request(accepter->buffer,1),accepter); //3 reader

	dispatch_run_simple(provider->buffer,list);

	iterator_t *iterator = iterator_init(list);
	while(hasNext(iterator)) {
		reader_t *reader = (reader_t*)next(iterator);
		CU_ASSERT_EQUAL(reader->buffer->occupied,3);
	}
}

void dispatch_messaggi_buffer_grandi(void) {
	DIM_BUFFER_READER = 5; //TODO: AAAA
	//metto 3 messaggi nel buffer del provider
	provider_t *provider = provider_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++)
		messages[i] = msg_init_string("MESSAGGIO");
	send_sequence(provider,messages,3);

	//creo 3 reader e li eseguo
	list_t *list = list_init();
	accepter_t *accepter = accepter_init(5,list);
	for(int i=0;i<3;i++)
		add_reader(send_request(accepter->buffer,i),accepter); //3 reader

	dispatch_run(provider->buffer,list,&(accepter->listMutex));
	iterator_t *iterator = iterator_init(list);
	while(hasNext(iterator)) {
		reader_t *reader = (reader_t*)next(iterator);
		CU_ASSERT_EQUAL(reader->buffer->occupied,4);
	}
}



void dispatch_messaggi_buffer_piccoli(void) { //devo capire come testarlo per bene
	pthread_t * readerThread[3];
	DIM_BUFFER_READER = 1; //TODO: AAAA
	//metto 3 messaggi nel buffer del provider
	provider_t *provider = provider_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++)
		messages[i] = msg_init_string("MESSAGGIO");
	send_sequence(provider,messages,3);

	//creo 3 reader e li eseguo
	list_t *list = list_init();
	accepter_t *accepter = accepter_init(5,list);
	for(int i=0;i<3;i++)
		add_reader(send_request(accepter->buffer,1),accepter); //3 reader gia' in esecuzione

	dispatch_run(provider->buffer,list,&(accepter->listMutex));
	CU_ASSERT_EQUAL(size(list),0);
}

