#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>

const double pi = acos(-1.0);

const double G = 6.67430 * pow(10.0, -11.0);

const double me = 5.972 * pow(10.0, 24.0);
const double re = 6371000.0;

const double mm = 7.34767309 * pow(10.0, 22.0);
const double rm = 1737400.0;
const double moon_distance = 384400000.0;
const double moon_period = 27.321661 * 24.0 * 3600.0;
const double moon_omega = 2.0 * pi / moon_period;
extern double moon_phase;

const double m0 = 500000.0;
const double mf = 120000.0;

const double dt = 10.0;
const double tmax = 16.0 * 24.0 * 3600.0;

#endif