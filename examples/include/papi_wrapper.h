#ifndef _PAPI_WRAPPER_H
#define _PAPI_WRAPPER_H

#include "papi.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

/* Alter this to be the max number of events you'd like. */
#define NUM_EVENTS 5

using namespace std;

static int EVENTS[NUM_EVENTS] = {
    PAPI_TOT_CYC, 
    PAPI_TOT_INS,
    PAPI_L1_DCM, 
    PAPI_L2_DCA, 
    PAPI_L2_DCH
};

struct papiEvents {
  int eventSet;
  long long values[NUM_EVENTS];
};

// Ref: http://www.cs.utexas.edu/~pingali/CSE392/2011sp/hw2/
inline void ERROR_RETURN(const int returnVal) {
    fprintf(stderr, "Error %d %s:line %d: \n", returnVal,__FILE__,__LINE__);    
    exit(returnVal);
}

// Invoke from master process
inline void initializePapi() {
    int retVal = PAPI_library_init(PAPI_VER_CURRENT);
    if (retVal != PAPI_VER_CURRENT) {
        std::cerr << "Failure in initializing PAPI Library!\n" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (PAPI_set_debug(PAPI_VERB_ECONT) != PAPI_OK) {
        exit(EXIT_FAILURE);
    }
}

// Call before the section to start collecting counters
inline papiEvents* startEvents(std::vector<std::string> & eventNames) {
    int retVal;
    papiEvents * events = (papiEvents*) calloc(1, sizeof(papiEvents));

    // Must be initialized to PAPI_NULL before calling PAPI_create_event
    events->eventSet = PAPI_NULL;

    int eventCode = 0;
    for(std::string & eventName : eventNames) {
        /* Find the event code for the specified event and its info. */
        /* A bit silly that .c_str() returns a char const * and we need a char* */
        PAPI_event_name_to_code(&*eventName.begin(), &eventCode);
        
        PAPI_event_info_t info;
        retVal = PAPI_get_event_info(eventCode, &info);
        if (retVal != PAPI_OK) {
            ERROR_RETURN(retVal);
        }

        /* Now use the eventName -> Code translation to query the event. */
        retVal = PAPI_query_event(eventCode);
        if (retVal != PAPI_OK) {
            std::cout << "Event " << eventName << " is not available or the event name is spelled wrong.\n" << std::endl;
        }
    }

    // Create the event set, add the events, and start them.
    if ( (retVal=PAPI_create_eventset(&events->eventSet)) != PAPI_OK ) {
        ERROR_RETURN(retVal);
    }

    if ( (retVal=PAPI_add_events(events->eventSet, EVENTS, NUM_EVENTS)) != PAPI_OK ) {
        ERROR_RETURN(retVal);
    }

    if ( (retVal=PAPI_start(events->eventSet)) != PAPI_OK ) {
        ERROR_RETURN(retVal);
    }

    return events;
}

// Call to finish collecting counters
inline void stopEvents(papiEvents * events) {
    int retVal;

    if ( (retVal=PAPI_stop(events->eventSet, events->values)) != PAPI_OK ) {
        ERROR_RETURN(retVal);
    }

    if ( (retVal=PAPI_remove_events(events->eventSet, EVENTS, NUM_EVENTS))!=PAPI_OK ) {
        ERROR_RETURN(retVal);
    }

    // Free all PAPI memory and data structures, Event set must be empty.
    if ( (retVal=PAPI_destroy_eventset(&events->eventSet)) != PAPI_OK ) {
        ERROR_RETURN(retVal);
    }
}

// Call to shutdown PAPI
inline void shutdownPapi() {
    PAPI_shutdown();
}

#endif