
void dispatcher_e_provider_buffer_grande(void) {
	provider_t *provider = provider_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++)
		messages[i] = msg_init_string("MESSAGGIO");

	pthread_mutex_t mutex; //mutex della lista dei reader,inutile tanto non ci sono accepter e reader
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

// testo un dispatcher semplificato, che considera i reader con buffer infiniti
void dispatch_messaggi_buffer_grandi_semplice(void) {
	DIM_BUFFER_READER = 5;
	//metto 3 messaggi nel provider_buffer piu' la poison pill
	provider_t *provider = provider_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++)
		messages[i] = msg_init_string("MESSAGGIO");
	send_sequence(provider,messages,3);

	//creo 3 reader e li lancio in esecuzione direttamente
	list_t *list = list_init();
	accepter_t *accepter = accepter_init(5,list);
	for(int i=0;i<3;i++)
		add_reader(send_request(accepter->buffer,1),accepter);

	dispatch_run_simple(provider->buffer,list);
	iterator_t *iterator = iterator_init(list);
	while(hasNext(iterator)) {
		reader_t *reader = (reader_t*)next(iterator);
		CU_ASSERT(reader->buffer->occupied >=3); //al piu' hanno letto un messaggio
	}
	CU_ASSERT_EQUAL(provider->buffer->occupied,0);
}

// uguale al test precedente, solo con la vera funzione dispatch_run
void dispatch_messaggi_buffer_grandi(void) {
	DIM_BUFFER_READER = 5;
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
		add_reader(send_request(accepter->buffer,1),accepter); //3 reader

	dispatch_run(provider->buffer,list,&(accepter->listMutex));
	iterator_t *iterator = iterator_init(list);
	while(hasNext(iterator)) {
		reader_t *reader = (reader_t*)next(iterator);
		CU_ASSERT(reader->buffer->occupied >=3); //al piu' hanno letto un messaggio
	}
	CU_ASSERT_EQUAL(provider->buffer->occupied,0);
}

// eseguo il dispatcher su 1 reader con buffer ridotto
void dispatcher_un_reader_buffer_piccolo(void) {
	pthread_t * readerThread[3];
	DIM_BUFFER_READER = 2;
	//metto 3 messaggi nel buffer del provider + la poison pill
	provider_t *provider = provider_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++)
		messages[i] = msg_init_string("MESSAGGIO");
	send_sequence(provider,messages,3);

	//creo 3 reader e li eseguo
	list_t *list = list_init();
	accepter_t *accepter = accepter_init(5,list);
	add_reader(send_request(accepter->buffer,1),accepter); // reader gia' in esecuzione

	dispatch_run(provider->buffer,list,&(accepter->listMutex));
	CU_ASSERT_EQUAL(size(list),0);
}

// eseguo il dispatcher su 3 reader con buffer ridotto
void dispatch_messaggi_buffer_piccoli(void) {
	pthread_t * readerThread[3];
	DIM_BUFFER_READER = 2;
	//metto 3 messaggi nel buffer del provider + la poison pill
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

// il dispatcher riceve una poison pill e la manda a 3 reader
void dispatch_poison_pill(void) {
	pthread_t * readerThread[3];
	//metto 3 messaggi nel buffer del provider + la poison pill
	provider_t *provider = provider_init(1);
	send_poison_pill(provider);

	//creo 3 reader e li eseguo
	list_t *list = list_init();
	accepter_t *accepter = accepter_init(5,list);
	for(int i=0;i<3;i++)
		add_reader(send_request(accepter->buffer,1),accepter); //3 reader gia' in esecuzione

	dispatch_run(provider->buffer,list,&(accepter->listMutex));
	CU_ASSERT_EQUAL(size(list),3);
	sleep(2); //non ho i thread dei reader per fare join. Ma i reader "dovrebbero" terminare
	CU_ASSERT_EQUAL(size(list),0);
}






