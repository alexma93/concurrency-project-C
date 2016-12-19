/*
 * Autore: Valter Crescenzi e gli studenti del 
 * Corso di Programmazione Concorrente
 *  
 * listDomainAssertion.c
 *
 * asserzioni di dominio sulle liste
*/
#include <CUnit/CUnit.h>
#include "../list.c"

void ASSERT_SINGLETONLIST(list_t *l, void *e);
void ASSERT_LIST_EQUAL2(list_t *l, void *e1, void *e2);

