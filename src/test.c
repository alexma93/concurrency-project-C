#include "CUnit/Basic.h"
#include <stdio.h>
#include <stdlib.h>
#include "buffer/buffer.c"
#include "poison_pill.c"
#include "list/list.c"
#include "provider.c"
#include "accepter.c"
#include "reader.c"
#include "dispatcher.c"
#include "readerTest.c"
#include "providerTest.c"
#include "accepterTest.c"
#include "dispatcherTest.c"


/************* Test case functions ****************/


/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

int main ( void )
{
   CU_pSuite providerSuite = NULL;
   CU_pSuite readerSuite = NULL;
   CU_pSuite accepterSuite = NULL;
   CU_pSuite dispatcherSuite = NULL;

   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add a suite to the registry */
   providerSuite = CU_add_suite( "Provider test suite", init_suite, clean_suite );
   readerSuite = CU_add_suite( "Reader test suite", init_suite, clean_suite );
   accepterSuite = CU_add_suite( "Accepter test suite", init_suite, clean_suite );
   dispatcherSuite = CU_add_suite( "Dispatcher test suite", init_suite, clean_suite );

   if ( NULL == providerSuite || NULL==readerSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(providerSuite, "invio_1_messaggio", invio_1_messaggio))
		|| (NULL == CU_add_test(providerSuite, "invio_sequenza_messaggi", invio_sequenza_messaggi))
		|| (NULL == CU_add_test(readerSuite, "leggi_messaggio", leggi_messaggio))
		|| (NULL == CU_add_test(readerSuite, "leggi_poison_pill", leggi_poison_pill))
		//|| (NULL == CU_add_test(readerSuite, "leggi_messaggio_lista_bloccata", leggi_messaggio_lista_bloccata))
		|| (NULL == CU_add_test(accepterSuite, "accepter_add_reader", accepter_add_reader))
		|| (NULL == CU_add_test(accepterSuite, "accepter_concurrent_add_reader", accepter_concurrent_add_reader))
		|| (NULL == CU_add_test(accepterSuite, "accepter_multiple_requests", accepter_multiple_requests))
		|| (NULL == CU_add_test(dispatcherSuite, "dispatcher_e_provider_buffer_grande", dispatcher_e_provider_buffer_grande))
		|| (NULL == CU_add_test(dispatcherSuite, "dispatcher_e_provider_buffer_piccolo", dispatcher_e_provider_buffer_piccolo))
		|| (NULL == CU_add_test(dispatcherSuite, "dispatch_messaggi_buffer_grandi_semplice", dispatch_messaggi_buffer_grandi_semplice))
		|| (NULL == CU_add_test(dispatcherSuite, "dispatch_messaggi_buffer_grandi", dispatch_messaggi_buffer_grandi))
		|| (NULL == CU_add_test(dispatcherSuite, "dispatcher_un_reader_buffer_piccolo", dispatcher_un_reader_buffer_piccolo))
		|| (NULL == CU_add_test(dispatcherSuite, "dispatch_messaggi_buffer_piccoli", dispatch_messaggi_buffer_piccoli))
		|| (NULL == CU_add_test(dispatcherSuite, "dispatch_poison_pill", dispatch_poison_pill))

		)
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   printf("\n");
   CU_basic_show_failures(CU_get_failure_list());
   printf("\n\n");

   return CU_get_error();
}
