#include <stdio.h>
#include <stdlib.h>
//includo i .c poiche' gli ultimi giorni ho avuto problemi col linker
#include "buffer/msg.c"
#include "buffer/buffer.c"
#include "poison_pill.c"
#include "list/list.c"
#include "provider.c"
#include "accepter.c"
#include "reader.c"
#include "dispatcher.c"

/*il main rappresenta una prova di esecuzione che vuole
 * evidenziare i comportamenti tra le varie componenti*/
int main(void) {
	int accepterBufferSize = 4;
	int providerBufferSize = 5;
	int numMessages = 3;
	DIM_BUFFER_READER = 5; //definita in reader.h

	//creo l'array di messaggi
	msg_t *providerMessages[numMessages];
	providerMessages[0] = msg_init_string("MESSAGGIO 1");
	providerMessages[1] = msg_init_string("MESSAGGIO 2");
	providerMessages[2] = msg_init_string("MESSAGGIO 3");

	//creo accepter e provider
	printf("\nCREO UN ACCEPTER CON BUFFER DI DIMENSIONE: %d\n",accepterBufferSize);
	list_t *readerList = list_init();
	pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
	accepter_t *accepter = accepter_init(accepterBufferSize,readerList,&listMutex);

	printf("\nCREO UN PROVIDER CON BUFFER DI DIMENSIONE: %d\n",providerBufferSize);
	provider_t *provider = provider_init(providerBufferSize);

	//lancio accepter e provider
	pthread_t accThread,provThread,dispatcher;
	printf("\nLANCIO L'ACCEPTER\n");
	pthread_create(&accThread,NULL,accepter_run,accepter);

	//invio le richieste per i reader
	printf("\nINVIO ALL'ACCEPTER 3 RICHIESTE DI READER \nCON VELOCITA' DIVERSE(2 3 10 secondi/messaggio)\n");
	send_request(accepter->buffer,2);
	send_request(accepter->buffer,3);
	send_request(accepter->buffer,10);
	sleep(1);

	printf("\nLANCIO IL PROVIDER CON 3 MESSAGGI + LA POISON PILL\n");
	provider_args provArgs;
	provArgs.provider = provider;
	provArgs.messages = providerMessages;
	provArgs.quantity = numMessages;
	pthread_create(&provThread,NULL,provider_run_thread,&provArgs);
	sleep(1);
	printf("\nPROVIDER BUFFER: %d ; valore atteso = 4\n",provider->buffer->occupied);

	//lancio il dispatcher
	printf("\nLANCIO IL DISPATCHER\n");
	dispatch_args dispArgs;
	dispArgs.buffer = provider->buffer;
	dispArgs.listReader = accepter->readerList;
	dispArgs.listMutex = &listMutex;
	pthread_create(&dispatcher,NULL,dispatch_run_thread,&dispArgs);
	sleep(1);

	// stanno processando il primo messaggio
	printf("\nI READER STANNO PROCESSANDO IL PRIMO MESSAGGIO");
	printf("\nREADER LIST: %d ; valore atteso = 3\n",size(readerList));
	iterator_t *it = iterator_init(readerList);
	reader_t *reader;
	int i=1;
	while(hasNext(it)) {
		reader = (reader_t*)next(it);
		printf("\t%d) READER BUFFER: %d ; valore atteso = 3\n",i,reader->buffer->occupied);
		i++;
	}
	iterator_destroy(it);

	printf("\n IL CONTENUTO DEL BUFFER DEL READER PIU LENTO(3) (null=poison pill):\n");
	for(int i=reader->buffer->extraction;i<reader->buffer->insertion;i++)
		printf("\tcontent: %s\n",reader->buffer->cells[i].content);

	//terminato il provider, termino l'accepter
	pthread_join(provThread,NULL);
	printf("\nIL PROVIDER E' TERMINATO\n");

	printf("\nACCEPTER BUFFER: %d ; valore atteso = 0",accepter->buffer->occupied);
	printf("\nINVIO LA POISON PILL ALL'ACCEPTER\n");
	send_accepter_poison_pill(accepter);

	pthread_join(accThread,NULL);

	printf("\nASPETTO CHE I PRIMI 2 READER LEGGANO I 3 MESSAGGI E LA POISON PILL\n");
	printf("\tREADER LIST: %d ; valore atteso = 3\n",size(readerList));
	printf("\ndopo 8 secondi, finisce il primo: ");
	printf("\n");
	sleep(8);
	printf("\tREADER LIST: %d ; valore atteso = 2\n",size(readerList));
	printf("\ndopo 12 secondi, finisce il secondo:  ");
	printf("\n");
	sleep(4);
	printf("\tREADER LIST: %d ; valore atteso = 1\n",size(readerList));

	printf("\nMOSTRO CHE IL DISPATCHER ELIMINA I READER LENTI:");
	printf("\nCREO UN ALTRO PROVIDER CHE INVIA 4 MESSAGGI AL DISPATCHER,\n");
	provider_t *provider2 = provider_init(providerBufferSize);
	msg_t *providerMessages2[4];
	providerMessages2[0] = msg_init_string("MESSAGGIO 1");
	providerMessages2[1] = msg_init_string("MESSAGGIO 2");
	providerMessages2[2] = msg_init_string("MESSAGGIO 3");
	providerMessages2[3] = msg_init_string("MESSAGGIO 3");
	pthread_t *provThread2;
	provider_args provArgs2;
	provArgs2.provider = provider2;
	provArgs2.messages = providerMessages2;
	provArgs2.quantity = 4;
	pthread_create(&provThread2,NULL,provider_run_thread,&provArgs2);

	printf("IL BUFFER DEL TERZO READER SI RIEMPIRA' E IL DISPATCHER LO ELIMINA PERCHE' TROPPO LENTO\n");
	dispatch_run(provider2->buffer,readerList,&listMutex);

	printf("\nREADER LIST: %d ; valore atteso = 0\n",size(readerList));

	pthread_join(provThread2,NULL);


	provider_destroy(provider);
	provider_destroy(provider2);
	accepter_destroy(accepter);
	for(int i=0;i<3;i++)
		providerMessages[i]->msg_destroy(providerMessages[i]);
	for(int i=0;i<4;i++)
		providerMessages2[i]->msg_destroy(providerMessages2[i]);
	pthread_mutex_destroy(&listMutex);
	list_destroy(readerList);
}







