#ifndef MSG_H_
#define MSG_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct msg {
	void *content;
	void (*msg_destroy)(struct msg *);
	struct msg *(*msg_init)(void *);
	struct msg *(*msg_copy)(struct msg *);
} msg_t;


/* 3 funzioni prese dalla consegna per messaggio di stringhe */
msg_t* msg_copy_string(msg_t* msg);

void msg_destroy_string(msg_t* msg);

msg_t* msg_init_string(void* content);
msg_t* msg_copy_int(msg_t* msg);

void msg_destroy_int(msg_t* msg);

msg_t* msg_init_int(void* content);

#endif

