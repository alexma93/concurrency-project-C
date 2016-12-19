#include <stdio.h>
#include <stdlib.h>
#include "provider.c"
#include "accepter.c"
#include "dispatcher.c"
#include "reader.c"
#include <pthread.h>


int main(void) {
	int accepterBufferSize = 4;
	int providerBufferSize = 5;
	int numMessages = 3;

	//creo l'array di messaggi
	msg_t *providerMessages[numMessages];
	for(int i=0;i<numMessages;i++)
		providerMessages[i] = msg_init_string("MESSAGGIO");

	//creo accepter e provider
	list_t *readerList = list_init();
	accepter_t *accepter = accepter_init(accepterBufferSize,readerList);
	provider_t *provider = provider_init(providerBufferSize);

	//lancio accepter e provider
	pthread_t accThread,provThread,dispatcher;
	provider_args provArgs;
	provArgs.provider = provider;
	provArgs.messages = providerMessages;
	provArgs.quantity = numMessages;

	pthread_create(&provThread,NULL,provider_run_thread,&provArgs);
	pthread_create(&accThread,NULL,accepter_run,accepter);
	printf("\n1\n");

	//invio le richieste per i reader
	send_request(accepter->buffer,1);
	send_request(accepter->buffer,2);
	send_request(accepter->buffer,3);

	sleep(1);
	//terminato il provider, termino l'accepter
	pthread_join(provThread,NULL);
	printf("\nPROVIDER BUFFER: %d 4\n",provider->buffer->occupied);
	printf("\nREADER LIST: %d 3\n",size(readerList));
	printf("\nACCEPTER BUFFER: %d 0\n",accepter->buffer->occupied);
	send_accepter_poison_pill(accepter);

	//lancio il dispatcher
	dispatch_args dispArgs;
	dispArgs.buffer = provider->buffer;
	dispArgs.listReader = accepter->readerList;
	dispArgs.listMutex = &(accepter->listMutex);
	pthread_create(&dispatcher,NULL,dispatch_run_thread,&dispArgs);
	sleep(1);
	printf("\nPROVIDER BUFFER: %d 0\n",provider->buffer->occupied);
	printf("\nREADER BUFFER: %d 4\n",((reader_t*)next(iterator_init(readerList)))->buffer->occupied);
	printf("\nREADER LIST: %d 3\n",size(readerList));

	//terminato l'accepter termino i reader
	pthread_join(accThread,NULL);
	iterator_t *iterator = iterator_init(readerList);
	reader_t *r;
	while(hasNext(iterator)) {
		r = (reader_t*)next(iterator);
		put_bloccante(r->buffer,POISON_PILL);
		removeElement(readerList,r);
	}
	printf("\nREADER LIST: %d 0\n",size(readerList));


}







