#include "msg.h"

/* 3 funzioni prese dalle slide per messaggi contenenti stringhe */
msg_t* msg_copy_string(msg_t* msg) {
    return msg->msg_init( msg->content );
}

void msg_destroy_string(msg_t* msg) {
    free(msg->content); // free copia privata
    free(msg); // free struct
}

msg_t* msg_init_string(void* content) {
//viene creata una copia "privata" della stringa
    msg_t* new_msg = (msg_t*)malloc( sizeof(msg_t) );
    char* string = (char*)content;
    char* new_content = (char*)malloc(strlen(string)+1); // +1 per \0 finale
    strcpy(new_content, string);
    new_msg->content = new_content;
    new_msg->msg_init = msg_init_string;
    new_msg->msg_destroy = msg_destroy_string;
    new_msg->msg_copy = msg_copy_string;
    return new_msg;
}

msg_t* msg_copy_int(msg_t* msg) {
    return msg->msg_init( msg->content );
}

void msg_destroy_int(msg_t* msg) {
    free(msg); // free struct
}

msg_t* msg_init_int(void* content) {
//viene creata una copia "privata" della stringa
    msg_t* new_msg = (msg_t*)malloc( sizeof(msg_t) );
    int integer = (int)content;
    new_msg->content = integer;
    new_msg->msg_init = msg_init_int;
    new_msg->msg_destroy = msg_destroy_int;
    new_msg->msg_copy = msg_copy_int;
    return new_msg;
}
