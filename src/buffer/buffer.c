#include "buffer.h"



buffer_t* buffer_init(unsigned int maxsize){
	buffer_t *buff = (buffer_t *) malloc(sizeof(buffer_t));
	buff->extraction = 0;
	buff->insertion = 0;
	buff->size = maxsize;
	buff->occupied = 0;
	buff->cells = (msg_t *) malloc(sizeof(msg_t)*maxsize);
	pthread_mutex_init(&(buff->mutex),NULL);
	pthread_cond_init(&(buff->notFull),NULL);
	pthread_cond_init(&(buff->notEmpty),NULL);
	return buff;
}


/* L'ho commentato perche' da' errori nei test come se eliminassi due
 * volte un puntatore(double free or corruption). */
void buffer_destroy(buffer_t* buffer){
	/*for(int i=0; i< buffer->size;i++) {
	 	// da' segmentation fault perche' se una cella non ha un messaggio non puo'
	 	// accedere a msg_destroy. ma comunque il problema non e' questo.
		buffer->cells[i].msg_destroy(&(buffer->cells[i]));
	}*/
	free(buffer);
}

// inserimento bloccante: sospende se pieno, quindi
// effettua l’inserimento non appena si libera dello spazio.
// restituisce il messaggio inserito; N.B.: msg!=null
msg_t* put_bloccante(buffer_t* buffer, msg_t* msg){
	pthread_mutex_lock(&(buffer->mutex));
	while(buffer->occupied >= buffer->size)//finche' e' pieno
		pthread_cond_wait(&(buffer->notFull),&(buffer->mutex));
	int i = buffer-> insertion;
	buffer->cells[i%buffer->size] = *msg;
	buffer->insertion = (i+1) % buffer->size;
	buffer->occupied++;

	pthread_cond_signal(&(buffer->notEmpty));
	pthread_mutex_unlock(&(buffer->mutex));
	return msg;
}

// inserimento non bloccante: restituisce BUFFER_ERROR se pieno,
// altrimenti effettua l’inserimento e restituisce il messaggio
// inserito; N.B.: msg!=null
msg_t* put_non_bloccante(buffer_t* buffer, msg_t* msg){
	pthread_mutex_lock(&(buffer->mutex));
	if(buffer->occupied >= buffer->size) {
		pthread_mutex_unlock(&(buffer->mutex));
		return BUFFER_ERROR;
	}
	int i = buffer-> insertion;
	buffer->cells[i%buffer->size] = *msg;
	buffer->insertion = (i+1) % buffer->size;
	buffer->occupied++;
	pthread_cond_signal(&(buffer->notEmpty));
	pthread_mutex_unlock(&(buffer->mutex));
	return msg;

}

// estrazione bloccante: sospende se vuoto, quindi
// restituisce il valore estratto non appena disponibile
msg_t* get_bloccante(buffer_t* buffer){
	pthread_mutex_lock(&(buffer->mutex));
	while(buffer->occupied==0)
		pthread_cond_wait(&(buffer->notEmpty),&(buffer->mutex));
	int e = buffer-> extraction;
	msg_t get_msg = buffer->cells[e%buffer->size];
	msg_t *msg = get_msg.msg_copy(&get_msg);
	// lascio il messaggio nel buffer, e quando sara' lo sovrascrivo

	buffer->extraction = (e+1) % buffer->size;
	buffer->occupied--;

	pthread_cond_signal(&(buffer->notFull));
	pthread_mutex_unlock(&(buffer->mutex));
	return msg;
}

// estrazione non bloccante: restituisce BUFFER_ERROR se vuoto
// ed il valore estratto in caso contrario
msg_t* get_non_bloccante(buffer_t* buffer){
	pthread_mutex_lock(&(buffer->mutex));
	if(buffer->occupied==0) {
		pthread_mutex_unlock(&(buffer->mutex));
		return BUFFER_ERROR;
	}
	int e = buffer-> extraction;
	msg_t get_msg = buffer->cells[e%buffer->size];
	msg_t *msg = get_msg.msg_copy(&get_msg);

	buffer->extraction = (e+1) % buffer->size;
	buffer->occupied--;

	pthread_cond_signal(&(buffer->notFull));
	pthread_mutex_unlock(&(buffer->mutex));
	return msg;
}

int buffer_contains_string(buffer_t *buffer,msg_t *msg) {
	for(int i=0;i<buffer->size;i++)
		if(!strcmp(buffer->cells[i].content,msg->content))
			return 1;
	return 0;
}


