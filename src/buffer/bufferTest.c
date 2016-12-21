#include "CUnit/Basic.h"
#include <stdio.h>
#include "buffer.h"



/************* Test case functions ****************/


// struttura per passare gli argomenti alla put in thread_create
struct put_args {buffer_t *buffer;msg_t *msg;};

// (P=1; C=0; N=1) Produzione di un solo messaggio in un buffer vuoto
void produzione_buffer_vuoto(void) {
	buffer_t *buffer_vuoto = buffer_init(1);
	msg_t* new_msg = msg_init_string("UN MESSAGGIO");
	put_non_bloccante(buffer_vuoto,new_msg);

	CU_ASSERT_STRING_EQUAL(buffer_vuoto->cells[0].content,"UN MESSAGGIO");
	CU_ASSERT_EQUAL(buffer_vuoto->occupied,1);

	buffer_destroy(buffer_vuoto);
	new_msg->msg_destroy(new_msg);
}

/*(P=0; C=1; N=1) Consumazione di un solo messaggio da un buffer pieno */
void consumazione_buffer_pieno(void) {
	buffer_t *buffer = buffer_init(1);
	msg_t* new_msg;
	put_non_bloccante(buffer,msg_init_string("UN MESSAGGIO"));

	new_msg = get_non_bloccante(buffer);

	CU_ASSERT_STRING_EQUAL(new_msg->content,"UN MESSAGGIO");
	CU_ASSERT_EQUAL(buffer->occupied,0);

	buffer_destroy(buffer);
	new_msg->msg_destroy(new_msg);
}


void produzione_non_bloccante_buffer_pieno(void) {
	buffer_t *buffer_pieno = buffer_init(1);
	msg_t *expected_msg = msg_init_string("MESSAGGIO 1");
	put_non_bloccante(buffer_pieno,expected_msg);
	msg_t *new_msg =msg_init_string("MESSAGGIO 2");
	msg_t *result = put_non_bloccante(buffer_pieno,&new_msg);

	CU_ASSERT_EQUAL(result,BUFFER_ERROR);
	CU_ASSERT_STRING_EQUAL(buffer_pieno->cells[0].content,expected_msg->content);

	buffer_destroy(buffer_pieno);
	expected_msg->msg_destroy(expected_msg);
	new_msg->msg_destroy(new_msg);
}

// chiama una put, e se non si blocca crea un effetto collaterale
void put_si_blocca(void *argp) {
	struct put_args *args = argp;
	buffer_t *buffer = args->buffer;
	msg_t *msg = args->msg;
	put_bloccante(buffer,msg);
	msg->content = "EFFETTO COLLATERALE"; //se si blocca non si verifica
}

void produzione_bloccante_buffer_pieno(void) {
	buffer_t *buffer_pieno = buffer_init(1);
	pthread_t producer;
	struct put_args args;
	msg_t *expected_msg = msg_init_string("MESSAGGIO 1");
	put_non_bloccante(buffer_pieno,expected_msg);

	msg_t *new_msg = msg_init_string("MESSAGGIO 2");
	args.buffer = buffer_pieno; args.msg = new_msg;
	pthread_create(&producer,NULL,put_si_blocca,&args);

	CU_ASSERT_STRING_EQUAL(buffer_pieno->cells[0].content,expected_msg->content);
	//verifico che non e' avvenuto l'effetto collaterale
	CU_ASSERT_STRING_EQUAL(new_msg->content,"MESSAGGIO 2");

	get_bloccante(buffer_pieno); //libero il thread bloccato con una get
	pthread_join(producer,NULL);

	buffer_destroy(buffer_pieno);
	expected_msg->msg_destroy(expected_msg);
}


void consumazione_bloccante_buffer_vuoto(void) {
	buffer_t *buffer = buffer_init(1);
	pthread_t consumer;
	msg_t *consumed;
	pthread_create(&consumer,NULL,get_bloccante,buffer);

	msg_t *go_msg = msg_init_string("GO_MESSAGE!");
	put_bloccante(buffer,go_msg);

	pthread_join(consumer,&consumed);
	CU_ASSERT_STRING_EQUAL(consumed->content,"GO_MESSAGE!");
	CU_ASSERT_EQUAL(buffer->occupied,0);

	buffer_destroy(buffer);
	consumed->msg_destroy(consumed);
	go_msg->msg_destroy(go_msg);
}


//prende gli argomenti dalla pthread_create e li assegna alla put_bloccante
void *put_bloccante_thread(void *argp) {
	struct put_args *args = argp;
	buffer_t *buffer = args->buffer;
	msg_t *msg = args->msg;
	return put_bloccante(buffer,msg);
}

//prende gli argomenti dalla pthread_create e li assegna alla put_non_bloccante
void *put_non_bloccante_thread(void *argp) {
	struct put_args *args = argp;
	buffer_t *buffer = args->buffer;
	msg_t *msg = args->msg;
	return put_non_bloccante(buffer,msg);
}

/*(P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio
	da un buffer unitario pieno; prima il consumatore */
void consumazione_e_produzione(void) {
	buffer_t *buffer = buffer_init(1);
	msg_t *first_msg = msg_init_string("PRIMO MESSAGGIO");
	put_bloccante(buffer,first_msg);

	pthread_t consumer,producer;
	msg_t *consumed;
	msg_t *new_message = msg_init_string("MESSAGGIO PRODOTTO");
	pthread_create(&consumer,NULL,get_non_bloccante,buffer);

	struct put_args *args;
	args->buffer = buffer; args->msg = new_message;
	pthread_create(&producer,NULL,put_bloccante_thread,args);

	pthread_join(consumer,&consumed);
	pthread_join(producer,NULL);

	CU_ASSERT_STRING_EQUAL(consumed->content,"PRIMO MESSAGGIO");
	CU_ASSERT_STRING_EQUAL(buffer->cells[0].content,"MESSAGGIO PRODOTTO");

	buffer_destroy(buffer);
	new_message->msg_destroy(new_message);
	consumed->msg_destroy(consumed);
	first_msg->msg_destroy(first_msg);

}


/*(P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio
	da un buffer unitario pieno; prima il produttore */
void produzione_e_consumazione(void) {
	buffer_t *buffer = buffer_init(1);
	msg_t *first_msg = msg_init_string("PRIMO MESSAGGIO");
	put_bloccante(buffer,first_msg);

	pthread_t consumer,producer;
	msg_t *consumed;
	msg_t *new_message = msg_init_string("MESSAGGIO PRODOTTO");

	struct put_args *args;
	args->buffer = buffer; args->msg = new_message;
	pthread_create(&producer,NULL,put_bloccante_thread,args);
	pthread_create(&consumer,NULL,get_bloccante,buffer);

	pthread_join(consumer,&consumed);
	pthread_join(producer,NULL);

	CU_ASSERT_STRING_EQUAL(consumed->content,"PRIMO MESSAGGIO");
	CU_ASSERT_STRING_EQUAL(buffer->cells[0].content,"MESSAGGIO PRODOTTO");

	buffer_destroy(buffer);
	new_message->msg_destroy(new_message);
	consumed->msg_destroy(consumed);
	first_msg->msg_destroy(first_msg);
}

/* (P>1; C=0; N=1) Produzione concorrente di molteplici messaggi in un
buffer unitario vuoto */
void produzioni_buffer_unitario_vuoto(void) {
	buffer_t *buffer = buffer_init(1);
	msg_t *new_msg,*new_msg1,*new_msg2;
	struct put_args args,args1,args2;
	pthread_t producer,producer1,producer2;
	//metto il mutex così un thread non può già partire se non ho ancora creato gli altri
	pthread_mutex_lock(&(buffer->mutex));

	new_msg = msg_init_string("MESSAGGIO: 1");
	args.buffer = buffer; args.msg = new_msg;
	pthread_create(&producer,NULL,put_bloccante_thread,&args);

	new_msg1 = msg_init_string("MESSAGGIO: 2");
	args1.buffer = buffer; args1.msg = new_msg1;
	pthread_create(&producer1,NULL,put_bloccante_thread,&args1);

	new_msg2 = msg_init_string("MESSAGGIO: 3");
	args2.buffer = buffer; args2.msg = new_msg2;
	pthread_create(&producer2,NULL,put_bloccante_thread,&args2);

	while(buffer->occupied==0)
		pthread_cond_wait(&(buffer->notEmpty),&(buffer->mutex));

	CU_ASSERT_EQUAL(buffer->occupied,1);

	pthread_mutex_unlock(&(buffer->mutex));
	//libero i due thread rimasti in attesa
	get_bloccante(buffer);
	get_bloccante(buffer);
	pthread_join(producer,NULL);
	pthread_join(producer1,NULL);
	pthread_join(producer2,NULL);

	new_msg->msg_destroy(new_msg);
	new_msg1->msg_destroy(new_msg1);
	new_msg2->msg_destroy(new_msg2);
	buffer_destroy(buffer);

}


/* (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un
buffer vuoto; il buffer si satura in corso */
void produzioni_buffer_si_satura(void) {
	buffer_t *buffer = buffer_init(2);
	msg_t *result,*result1,*result2;
	msg_t *new_msg,*new_msg1,*new_msg2;
	struct put_args args,args1,args2;
	pthread_t producer,producer1,producer2;

	pthread_mutex_lock(&(buffer->mutex));
	new_msg = msg_init_string("MESSAGGIO: 1");
	args.buffer = buffer; args.msg = new_msg;
	pthread_create(&producer,NULL,put_non_bloccante_thread,&args);

	new_msg1 = msg_init_string("MESSAGGIO: 2");
	args1.buffer = buffer; args1.msg = new_msg1;
	pthread_create(&producer1,NULL,put_non_bloccante_thread,&args1);

	new_msg2 = msg_init_string("MESSAGGIO: 3");
	args2.buffer = buffer; args2.msg = new_msg2;
	pthread_create(&producer2,NULL,put_non_bloccante_thread,&args2);
	pthread_mutex_unlock(&(buffer->mutex));

	pthread_join(producer,&result);
	pthread_join(producer1,&result1);
	pthread_join(producer2,&result2);

	//verifico che i messaggi nel buffer sono tra i 3 inseriti(non ci sono state interferenze)
	int contains = 0;
	if(buffer_contains_string(buffer,new_msg)) contains++;
	if(buffer_contains_string(buffer,new_msg1)) contains++;
	if(buffer_contains_string(buffer,new_msg2)) contains++;

	CU_ASSERT_EQUAL(contains,2);
	CU_ASSERT(result==BUFFER_ERROR || result1==BUFFER_ERROR || result2==BUFFER_ERROR);
	CU_ASSERT_EQUAL(buffer->occupied,2);

	buffer_destroy(buffer);
	new_msg->msg_destroy(new_msg);
	new_msg1->msg_destroy(new_msg1);
	new_msg2->msg_destroy(new_msg2);

}

/* (P=0; C>1; N>1) Consumazione concorrente di molteplici messaggi da un buffer pieno. non si svuota */
void consumazioni_buffer_non_unitario(void) {
	buffer_t *buffer = buffer_init(5);
	pthread_t consumer,consumer1,consumer2;
	for(int i=0;i<5;i++)
		put_non_bloccante(buffer,msg_init_string("MESSAGGIO PRESENTE"));
	pthread_mutex_lock(&(buffer->mutex));
	pthread_create(&consumer,NULL,get_bloccante,buffer);
	pthread_create(&consumer1,NULL,get_bloccante,buffer);
	pthread_create(&consumer2,NULL,get_bloccante,buffer);
	pthread_mutex_unlock(&(buffer->mutex));

	pthread_join(consumer,NULL);
	pthread_join(consumer1,NULL);
	pthread_join(consumer2,NULL);

	CU_ASSERT_EQUAL(buffer->occupied,2);

	buffer_destroy(buffer);
}

/* (P>1; C>1; N=1) Consumazioni e produzioni concorrenti di molteplici
messaggi in un buffer unitario */
void prod_e_cons_buffer_unitario(void) {
	buffer_t *buffer = buffer_init(1);
	msg_t *result,*result2;
	pthread_t consumer,consumer2,producer,producer2;
	struct put_args args,args2;

	pthread_mutex_unlock(&(buffer->mutex));
	pthread_create(&consumer,NULL,get_bloccante,buffer);
	pthread_create(&consumer2,NULL,get_bloccante,buffer);

	args.buffer = buffer;
	args.msg = msg_init_string("MESSAGGIO: 1");
	pthread_create(&producer,NULL,put_bloccante_thread,&args);

	args2.buffer = buffer;
	args2.msg = msg_init_string("MESSAGGIO: 2");
	pthread_create(&producer2,NULL,put_bloccante_thread,&args2);
	pthread_mutex_unlock(&(buffer->mutex));

	pthread_join(consumer,&result);
	pthread_join(consumer2,&result2);
	pthread_join(producer,NULL);
	pthread_join(producer2,NULL);

	CU_ASSERT_EQUAL(buffer->occupied,0);
	CU_ASSERT(strcmp(result->content,"MESSAGGIO: 1") || strcmp(result2->content,"MESSAGGIO: 1"));
	CU_ASSERT(strcmp(result->content,"MESSAGGIO: 2") || strcmp(result2->content,"MESSAGGIO: 2"));

	buffer_destroy(buffer);
	result->msg_destroy(result);
	result2->msg_destroy(result2);
}

/* (P>1; C>1; N>1) Consumazioni e produzioni concorrenti di molteplici
messaggi in un buffer */
void prod_e_cons_buffer_non_unitario(void) {
	buffer_t *buffer = buffer_init(4);
	msg_t *result,*result2;
	pthread_t consumer,consumer2,producer,producer2,producer3;
	struct put_args args,args2,args3;
	// 3 messaggi iniziali nel buffer
	for(int i=0;i<3;i++)
		put_non_bloccante(buffer,msg_init_string("MESSAGGIO PRESENTE"));

	pthread_mutex_lock(&(buffer->mutex));//così i thread sono tutti bloccati assieme

	args.buffer = buffer;
	args.msg = msg_init_string("MESSAGGIO: 1");
	pthread_create(&producer,NULL,put_bloccante_thread,&args);

	args2.buffer = buffer;
	args2.msg = msg_init_string("MESSAGGIO: 2");
	pthread_create(&producer2,NULL,put_bloccante_thread,&args2);

	args3.buffer = buffer;
	args3.msg = msg_init_string("MESSAGGIO: 3");
	pthread_create(&producer3,NULL,put_bloccante_thread,&args3);

	pthread_create(&consumer,NULL,get_bloccante,buffer);
	pthread_create(&consumer2,NULL,get_bloccante,buffer);

	pthread_mutex_unlock(&(buffer->mutex));

	pthread_join(consumer,&result);
	pthread_join(consumer2,&result2);
	pthread_join(producer,NULL);
	pthread_join(producer2,NULL);
	pthread_join(producer3,NULL);

	CU_ASSERT_EQUAL(buffer->occupied,4);
	CU_ASSERT_STRING_EQUAL(result->content,"MESSAGGIO PRESENTE");
	CU_ASSERT_STRING_EQUAL(result2->content,"MESSAGGIO PRESENTE");

	buffer_destroy(buffer);

	result->msg_destroy(result);
	result2->msg_destroy(result2);
}

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

int main ( void )
{
   CU_pSuite basicSuite = NULL;
   CU_pSuite advancedSuite = NULL;

   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add a suite to the registry */
   basicSuite = CU_add_suite( "un produttore e un consumatore test suite", init_suite, clean_suite );
   advancedSuite = CU_add_suite("piu produttori e piu consumatori test suite",init_suite,clean_suite);
   if ( NULL == basicSuite || NULL==advancedSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(basicSuite, "produzione_buffer_vuoto", produzione_buffer_vuoto))
		|| (NULL == CU_add_test(basicSuite, "consumazione_buffer_pieno", consumazione_buffer_pieno))
		|| (NULL == CU_add_test(basicSuite, "produzione_non_bloccante_buffer_pieno", produzione_non_bloccante_buffer_pieno))
		|| (NULL == CU_add_test(basicSuite, "produzione_bloccante_buffer_pieno", produzione_bloccante_buffer_pieno))
		|| (NULL == CU_add_test(basicSuite, "consumazione_bloccante_buffer_vuoto",consumazione_bloccante_buffer_vuoto))
		|| (NULL == CU_add_test(basicSuite, "consumazione_e_produzione", consumazione_e_produzione))
		|| (NULL == CU_add_test(basicSuite, "produzione_e_consumazione", produzione_e_consumazione))
		|| (NULL == CU_add_test(advancedSuite, "produzioni_buffer_unitario_vuoto", produzioni_buffer_unitario_vuoto))
		|| (NULL == CU_add_test(advancedSuite, "produzioni_buffer_si_satura", produzioni_buffer_si_satura))
		|| (NULL == CU_add_test(advancedSuite, "consumazioni_buffer_non_unitario", consumazioni_buffer_non_unitario))
		|| (NULL == CU_add_test(advancedSuite, "prod_e_cons_buffer_unitario", prod_e_cons_buffer_unitario))
		|| (NULL == CU_add_test(advancedSuite, "prod_e_cons_buffer_non_unitario", prod_e_cons_buffer_non_unitario)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   printf("\n");
   CU_basic_show_failures(CU_get_failure_list());
   printf("\n\n");

   return CU_get_error();
}
