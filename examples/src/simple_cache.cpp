#include "papi_wrapper.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

#define NUM_ITERATIONS 10000000

void do_some_work(const int n) {
    volatile double a = 0.5, b = 2.2;
    double c = 0.11;

    for(int i = 0; i < n; ++i) {
        c += a * b;
    }
}

int main() {
    // Set up PAPI library environment
    initializePapi();

	papiEvents * events;
	vector<string> eventNames = {
    	"PAPI_TOT_CYC", // Total cycles
	    "PAPI_TOT_INS", // Total inserts into the cache
	    "PAPI_L2_DCA",  // L2 Data Cache Access
	    "PAPI_L2_DCH"   // L2 Data Cache Hits
        // Can add more events here...
    };
    events = startEvents(eventNames);

    do_some_work(NUM_ITERATIONS);

    // Stop the events and remove them from the EventSet.
    stopEvents(events);

    std::cout << "Total cycles is: " << events->values[0] << std::endl;
    std::cout << events->values[3] << " L2 cache misses and " << events->values[1] << " loads.\n" << std::endl;
    
    // Shutdown PAPI environment.
    shutdownPapi();

	return EXIT_SUCCESS;
}
