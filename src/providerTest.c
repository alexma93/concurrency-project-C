

//il provider invia 2 messaggi nel buffer
void invio_2_messaggi(void) {
	provider_t *provider = provider_init(3);
	msg_t *msg1 = msg_init_string("MESSAGGIO 1");
	msg_t *msg2 = msg_init_string("MESSAGGIO 2");
	send_msg(provider,msg1);
	send_msg(provider,msg2);
	send_poison_pill(provider);
	CU_ASSERT_EQUAL(provider->buffer->occupied,3);
	CU_ASSERT_EQUAL(provider->buffer->cells[2].content,NULL); //c'e' una poison pill

	provider_destroy(provider);
	msg1->msg_destroy(msg1);
	msg2->msg_destroy(msg2);
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

