#ifndef CONTROL_LOOP
#define CONTROL_LOOP

#include "controller.h"

extern controller_input controller_in;

void* controllerLoop(void*);

#endif