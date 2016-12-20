
/*qui e' testato il provider singolarmente,
 * in dispatcherTest assieme al dispatcher */

//il provider invia 1 messaggio nel buffer e la poison pill
void invio_1_messaggio(void) {
	provider_t *provider = provider_init(3);
	msg_t *msg1 = msg_init_string("MESSAGGIO 1");
	send_msg(provider,msg1);
	send_poison_pill(provider);

	CU_ASSERT_EQUAL(provider->buffer->occupied,2);
	CU_ASSERT_STRING_EQUAL(provider->buffer->cells[0].content,"MESSAGGIO 1");
	CU_ASSERT_EQUAL(provider->buffer->cells[2].content,NULL); //c'e' una poison pill

	provider_destroy(provider);
	msg1->msg_destroy(msg1);
}

//il provider invia una sequenza di messaggi
void invio_sequenza_messaggi(void) {
	provider_t *provider = provider_init(5);
	msg_t *messages[3];
	for(int i=0;i<3;i++)
		messages[i] = msg_init_string("MESSAGGIO");
	send_sequence(provider,messages,3);
	CU_ASSERT_EQUAL(provider->buffer->occupied,4);
	CU_ASSERT_STRING_EQUAL(provider->buffer->cells[1].content,"MESSAGGIO");
}

