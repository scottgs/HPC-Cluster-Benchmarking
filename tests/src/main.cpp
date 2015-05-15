#include "papi_wrapper.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define for_each_item(item, list) \
    for(T * item = list->head; item != NULL; item = item->next)

/**
 * Tests to make sure that if a user does not pass any events in, our event set is still
 * default events we choose.
 * @return true if pass, false otherwise
 */
bool TEST_DEFAULT_EVENTS() {
	papiEvents * events;
	events = start_events();

	cout << events->eventSet << endl;
}

bool TEST_ADDING_USER_DEFINED_EVENTS() {
	papiEvents * events;
	vector<string> event_names = {
		"PAPI_TOT_CYC", // Total cycles
	    "PAPI_TOT_INS", // Total inserts into the cache
	    "PAPI_L2_DCA",  // L2 Data Cache Access
	    "PAPI_L2_DCH"   // L2 Data Cache Hits
	};
	
	events = start_events(event_names);

	/* Check to make sure the right number of events got added. */
	// cout << events->eventSet << endl;
	
	stop_events(events);
}

void run_all_tests() {
	// TEST_DEFAULT_EVENTS();
	TEST_ADDING_USER_DEFINED_EVENTS();
}

int main() {
	initialize_papi();

	run_all_tests();

	shutdown_papi();

	return EXIT_SUCCESS;
}