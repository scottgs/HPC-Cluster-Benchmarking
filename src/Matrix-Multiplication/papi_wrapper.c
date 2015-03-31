#include "papi_wrapper.h"

// #define _DEBUG

static int EVENTS[NUM_EVENTS] = {PAPI_TOT_CYC, PAPI_TOT_INS, PAPI_L1_DCM, PAPI_L2_DCA, PAPI_L2_DCH}; // PAPI_L2_DCM}, PAPI_L2_DCR, PAPI_L2_DCW};

void initializePapi() {    
    int retVal = PAPI_library_init(PAPI_VER_CURRENT);
    if (retVal != PAPI_VER_CURRENT) {
        printf("Failure in initializing PAPI Library!");
        exit(EXIT_FAILURE);
    }

    if (PAPI_set_debug(PAPI_VERB_ECONT) != PAPI_OK) {
        exit(EXIT_FAILURE);
    }
}

papiEvents* startEvents() {
    int retVal;
    papiEvents * events = calloc(1, sizeof(papiEvents));

    // Must be initialized to PAPI_NULL before calling PAPI_create_event
    events->eventSet = PAPI_NULL;

    retVal = PAPI_query_event(PAPI_L2_DCM);
    if (retVal != PAPI_OK) {
        printf("PAPI_L2_DCM not available\n");
    }

    retVal = PAPI_query_event(PAPI_LD_INS);
    if (retVal != PAPI_OK) {
        printf("PAPI_LD_INS not available\n");
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

void stopEvents(papiEvents * events) {
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

void shutdown() {
    PAPI_shutdown();
} 