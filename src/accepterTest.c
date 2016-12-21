#include "accepter.h"
#include "list/list.h"
#include "CUnit/Basic.h"

void accepter_add_reader(void) {
	list_t * list = list_init();
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(2,list,&listMutex);

	send_request(accepter->buffer,1);

	msg_t * request = get_bloccante(accepter->buffer);
	reader_t *reader = add_reader(request,accepter);

	CU_ASSERT_EQUAL(size(list),1);
	CU_ASSERT_EQUAL(reader->list,list);
	CU_ASSERT_EQUAL(reader->proc_time,1);

	list_destroy(list);
	pthread_mutex_destroy(accepter->listMutex);
	accepter_destroy(accepter);
}

//l'accepter e' in esecuzione e gli arriva una richiesta
void accepter_concurrent_add_reader(void) {
	pthread_mutex_t listMutex =PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(2,list_init(),&listMutex);
	pthread_t *accThread;
	pthread_create(&accThread,NULL,accepter_run,accepter);

	send_request(accepter->buffer,2);
	send_poison_pill(accepter);
	send_request(accepter->buffer,2); //richiesta che non verra' evasa

	pthread_join(accThread,NULL);
	CU_ASSERT_EQUAL(size(accepter->readerList),1);

	list_destroy(accepter->readerList);
	pthread_mutex_destroy(accepter->listMutex);
	accepter_destroy(accepter);
}

struct request_args{accepter_t *accepter;int time;};

msg_t *send_request_thread(void *argp) {
	struct request_args *args = argp;
	return send_request(args->accepter->buffer,args->time);
}

// arrivano piu' richieste contemporaneamente a un accepter in esecuzione
void accepter_multiple_requests(void) {
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(5,list_init(),&listMutex);
	list_t *list = accepter->readerList;
	pthread_t *accThread, request1, request2, request3;
	struct request_args arg;
	arg.accepter = accepter;
	arg.time = 1;
	pthread_mutex_lock(&(accepter->buffer->mutex)); //cosi' e' piu' probabile che nessuno parta prima
	pthread_create(&accThread,NULL,accepter_run,accepter);
	pthread_create(&request1,NULL,send_request_thread,&arg);
	pthread_create(&request2,NULL,send_request_thread,&arg);
	pthread_create(&request3,NULL,send_request_thread,&arg);
	pthread_mutex_unlock(&(accepter->buffer->mutex));

	pthread_join(request1,NULL);
	pthread_join(request2,NULL);
	pthread_join(request3,NULL);
	send_poison_pill(accepter);
	pthread_join(accThread,NULL);

	CU_ASSERT_EQUAL(size(list),3);


	pthread_mutex_destroy(accepter->listMutex);
	list_destroy(list);
	accepter_destroy(accepter);
}





