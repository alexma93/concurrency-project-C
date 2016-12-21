#ifndef PROVIDER_H_
#define PROVIDER_H_


#include "buffer/buffer.h"
#include "poison_pill.h"

//si poteva evitare la struct, ma pensavo che avrei aggiunto altre caratteristiche al provider oltre al buffer
typedef struct provider_t {
	buffer_t *buffer;
} provider_t;

//usata come argomento di pthread_create
typedef struct {
	provider_t *provider;
	msg_t **messages;
	int quantity;
} provider_args;


msg_t *send_msg(provider_t *provider, msg_t *msg);
msg_t *send_poison_pill(provider_t *provider);
provider_t *provider_init(int dimBuffer);
void provider_destroy(provider_t *provider);
void provider_run(provider_t *provider, msg_t **messages,int quantity);
void provider_run_thread(void *argp);

#endif
