#include "provider.h"


msg_t *send_msg(provider_t *provider, msg_t *msg) {
	return put_bloccante(provider->buffer,msg);
}

msg_t *send_poison_pill(provider_t *provider) {
	put_bloccante(provider->buffer,POISON_PILL);
	return POISON_PILL;
}

provider_t *provider_init(int dimBuffer) {
	provider_t *p = (provider_t *) malloc(sizeof(provider_t));
	p->buffer = buffer_init(dimBuffer);
	return p;
}

void provider_destroy(provider_t *provider) {
	buffer_destroy(provider->buffer);
	free(provider);
}

// invia una sequenza di messaggi
void send_sequence(provider_t *provider, msg_t **messages,int quantity) {
	for (int i=0;i<quantity;i++)
		send_msg(provider,messages[i]);
	send_poison_pill(provider);
}

void provider_run(provider_t *provider, msg_t **messages,int quantity) {
	send_sequence(provider,messages,quantity);
}

// funzione di supporto usata per la creazione di un thread di un provider
void provider_run_thread(void *argp) {
	provider_args *args = argp;
	provider_run(args->provider,args->messages,args->quantity);
}










