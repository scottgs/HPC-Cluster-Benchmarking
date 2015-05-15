#include "papi_wrapper.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * Tests to make sure that if a user does not pass any events in, our event set is still
 * default events we choose.
 */
void TEST_DEFAULT_EVENTS() {
	initialize_papi();

	papiEvents * events;
	events = start_events();

	stop_events(events, DEFAULT_NUM_EVENTS);
	cout << "Passed adding and removing the default events." << endl;

	shutdown_papi();
}

void TEST_ADDING_USER_DEFINED_EVENTS() {
	initialize_papi();

	papiEvents * events;
	vector<string> event_names = {
	    "PAPI_L2_DCA",  // L2 Data Cache Access
	    "PAPI_L2_DCH"   // L2 Data Cache Hits
	};
	
	events = start_events(event_names);
	
	stop_events(events, event_names.size());
	cout << "Passed adding user defined events and removing them." << endl;

	shutdown_papi();
}

void run_all_tests() {
	TEST_DEFAULT_EVENTS();
	TEST_ADDING_USER_DEFINED_EVENTS();
}

int main() {
	run_all_tests();
	return EXIT_SUCCESS;
}