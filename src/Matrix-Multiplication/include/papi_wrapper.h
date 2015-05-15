#ifndef _PAPI_WRAPPER_H
#define _PAPI_WRAPPER_H

#include "papi.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_EVENTS 5

typedef struct papiEvents {
  int eventSet;
  long long values[NUM_EVENTS];
} papiEvents;

// Ref: http://www.cs.utexas.edu/~pingali/CSE392/2011sp/hw2/
#define ERROR_RETURN(returnVal) { fprintf(stderr, "Error %d %s:line %d: \n", returnVal,__FILE__,__LINE__);  exit(returnVal); }

// Invoke from master process
void initializePapi();

// Call before the section to start collecting counters
papiEvents* startEvents();

// Call to finish collecting counters
void stopEvents(papiEvents * events);

// Call to shutdown PAPI
void shutdown();

#endif