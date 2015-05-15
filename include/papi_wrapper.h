#ifndef _PAPI_WRAPPER_H
#define _PAPI_WRAPPER_H

#include "papi.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

/* Alter this to be the max number of events available for your machine. */
#define MAX_NUM_EVENTS 30

/* For the events that everyone should proceed with no matter what. */
#define DEFAULT_NUM_EVENTS 5

#define EMPTY_ERROR_CODE 0

using namespace std;

static int DEFAULT_EVENTS[DEFAULT_NUM_EVENTS] = {
    PAPI_TOT_CYC, 
    PAPI_TOT_INS,
    PAPI_L1_DCM, 
    PAPI_L2_DCA, 
    PAPI_L2_DCH
};

/*  Will hold the codes for the EVENT names. */
vector<int> EVENTS(MAX_NUM_EVENTS);

struct papiEvents {
  int eventSet;
  long long values[MAX_NUM_EVENTS];
};

/* TODO: Good signal handler. */
inline void error_return(const int ret_val) {
    /* List of Error Codes */
//     0   PAPI_OK     No error    
//     -1  PAPI_EINVAL     Invalid argument    
//     -2  PAPI_ENOMEM     Insufficient memory     
//     -3  PAPI_ESYS   A system or C library call failed, please check errno   
//     -4  PAPI_ESBSTR     Substrate returned an error, usually the result of an unimplemented feature     
//     -5  PAPI_ECLOST     Access to the counters was lost or interrupted  
//     -6  PAPI_EBUG   Internal error, please send mail to the developers  
//     -7  PAPI_ENOEVNT    Hardware event does not exist   
//     -8  PAPI_ECNFLCT    Hardware event exists, but cannot be counted due to counter resource limitations    
//     -9  PAPI_ENOTRUN    No events or event sets are currently not counting  
//     -10     PAPI_EISRUN     Event Set is currently running  
//     -11     PAPI_ENOEVST    No such event set available     
//     -12     PAPI_ENOTPRESET     Event is not a valid preset     
//     -13     PAPI_ENOCNTR    Hardware does not support performance counters  
//     -14     PAPI_EMISC  ‘Unknown error’ code    
//     -15     PAPI_EPERM  You lack the necessary permissions 

    exit(ret_val);
}

// Invoke from master process
inline void initialize_papi() {
    const int retVal = PAPI_library_init(PAPI_VER_CURRENT);
    
    if (retVal != PAPI_VER_CURRENT) {
        std::cerr << "Failure in initializing PAPI Library!\n" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (PAPI_set_debug(PAPI_VERB_ECONT) != PAPI_OK) {
        exit(EXIT_FAILURE);
    }

    /* Set the EVENTS all to be error codes of 0. This is useful for when I remove events later on. */
    for (unsigned int i = 0; i < EVENTS.size(); ++i) {
        EVENTS[i] = EMPTY_ERROR_CODE;
    }
}

/* Starts the event set with the DEFAULT EVENTS if the user does not know what they want. */
inline papiEvents* start_events() {
    papiEvents * events = (papiEvents*) calloc(1, sizeof(papiEvents));

    // Must be initialized to PAPI_NULL before calling PAPI_create_event
    events->eventSet = PAPI_NULL;

    int ret_val;

    // Create the event set, add the events, and start them.
    if ( (ret_val=PAPI_create_eventset(&events->eventSet)) != PAPI_OK ) {
        error_return(ret_val);
    }

    std::copy(std::begin(DEFAULT_EVENTS), std::end(DEFAULT_EVENTS), std::begin(EVENTS));

    if ( (ret_val=PAPI_add_events(events->eventSet, DEFAULT_EVENTS, DEFAULT_NUM_EVENTS)) != PAPI_OK ) {
        error_return(ret_val);
    }

    if ( (ret_val=PAPI_start(events->eventSet)) != PAPI_OK ) {
        error_return(ret_val);
    }

    return events;
}

/* Call before the section to start collecting counters. */
/* Overloaded for if the user wants to add their own events. */
inline papiEvents* start_events(std::vector<std::string> & eventNames) {
    papiEvents * events = (papiEvents*) calloc(1, sizeof(papiEvents));

    // Must be initialized to PAPI_NULL before calling PAPI_create_event
    events->eventSet = PAPI_NULL;
    
    int ret_val;

    // Create the event set to put events into.
    if ( (ret_val=PAPI_create_eventset(&events->eventSet)) != PAPI_OK ) {
        cout << "Failed to create the event set." << endl;
        error_return(ret_val);
    }

    unsigned int index = 0;
    int event_code = 0;
    for(std::string & eventName : eventNames) {
        /* Find the event code for the specified event and its info. */
        /* A bit silly that .c_str() returns a char const * and we need a char* */
        PAPI_event_name_to_code(&*eventName.begin(), &event_code);
        
        PAPI_event_info_t info;
        ret_val = PAPI_get_event_info(event_code, &info);
        if (ret_val != PAPI_OK) {
            cout << "Failed to get event info." << endl;
            error_return(ret_val);
        }

        /* Now use the eventName -> Code translation to query the event. */
        ret_val = PAPI_query_event(event_code);
        if (ret_val != PAPI_OK) {
            std::cout << "Event " << eventName << " is not available on your machine or the event name is spelled wrong.\n" << std::endl;
            
            /* Remove the item from our event vector in O(1) time. */
            std::swap(eventNames[index], eventNames.back());
            eventNames.pop_back();
            continue;
        }

        // Otherwise it's a valid event, so let's add it to the set and start it
        if ((ret_val = PAPI_add_event(events->eventSet, event_code)) != PAPI_OK) {
            cout << "Failed to add the event!" << endl;
            error_return(ret_val);
        }
        ++index;
    }

    /* Start all the events in the event set. */
    if ( (ret_val=PAPI_start(events->eventSet)) != PAPI_OK ) {
        cout << "Failed to start the counters!" << endl;
        error_return(ret_val);
    }

    return events;
}

// Call to finish collecting counters
inline void stop_events(papiEvents * events, const unsigned int num_events) {
    int ret_val;

    if ( (ret_val=PAPI_stop(events->eventSet, events->values)) != PAPI_OK ) {
        cout << "Failed to stop the events." << endl;
        error_return(ret_val);
    }

    int event_code;
    for (unsigned int i = 0; i < EVENTS.size(); ++i) {
        if (EVENTS[i] == EMPTY_ERROR_CODE) {
            break;
        }

        ret_val = PAPI_remove_event(events->eventSet, EVENTS[i]);
        if (ret_val != PAPI_OK) {
            cout << "Failed to remove the event with event code: " << event_code << endl;
            error_return(ret_val);
        }
    }

    /* In case there are loose events that need to get cleaned up, i.e. makes the event set completely empty. */
    if ((ret_val = PAPI_cleanup_eventset(events->eventSet)) != PAPI_OK) {
        cout << "Problem removing extra loose events." << endl;
        error_return(ret_val);
    }

    // Free all PAPI memory and data structures, Event set must be empty.
    if ( (ret_val=PAPI_destroy_eventset(&events->eventSet)) != PAPI_OK ) {
        cout << "Failed to destroy the event set." << endl;
        error_return(ret_val);
    }
}

// Call to shutdown PAPI
inline void shutdown_papi() {
    PAPI_shutdown();
}

#endif
