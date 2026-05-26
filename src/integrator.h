#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "state.h"

State add(State a, State b, double c);
State rk4(State s, double t);

#endif