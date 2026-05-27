#ifndef PHYSICS_H
#define PHYSICS_H

#include "state.h"

double norm(double x, double y);

double moon_x(double t);
double moon_y(double t);
double moon_vx(double t);
double moon_vy(double t);

State derivatives(State s, double t);

#endif