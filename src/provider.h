#ifndef PROVIDER_H_
#define PROVIDER_H_


#include "buffer/buffer.c"

typedef struct provider_t {
	buffer_t *buffer;
	//devo aggiungerci altro
} provider_t;

typedef struct {
	provider_t *provider;
	msg_t **messages;
	int quantity;
} provider_args;

//il provider deve inviare una sequenza finita
msg_t *send_msg(provider_t *provider, msg_t *msg);
msg_t *send_poison_pill(provider_t *provider);
provider_t *provider_init(int dimBuffer);
void provider_destroy(provider_t *provider);
void send_sequence(provider_t *provider, msg_t **messages,int quantity);
void provider_run(provider_t *provider, msg_t **messages,int quantity);
void provider_run_thread(void *argp);

#endif
