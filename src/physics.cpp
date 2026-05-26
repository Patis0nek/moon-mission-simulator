#include "physics.h"
#include "constants.h"
#include <cmath>

double moon_phase = 0.9;

double norm(double x, double y)
{
    return sqrt(x * x + y * y);
}

double moon_x(double t)
{
    return moon_distance * cos(moon_omega * t + moon_phase);
}

double moon_y(double t)
{
    return moon_distance * sin(moon_omega * t + moon_phase);
}

double moon_vx(double t)
{
    return -moon_distance * moon_omega * sin(moon_omega * t + moon_phase);
}

double moon_vy(double t)
{
    return moon_distance * moon_omega * cos(moon_omega * t + moon_phase);
}

State derivatives(State s, double t)
{
    State ds;

    double r = norm(s.x, s.y);

    double xm = moon_x(t);
    double ym = moon_y(t);

    double dxm = s.x - xm;
    double dym = s.y - ym;
    double rmoon = norm(dxm, dym);

    double axe = -G * me * s.x / pow(r, 3.0);
    double aye = -G * me * s.y / pow(r, 3.0);

    double axm = -G * mm * dxm / pow(rmoon, 3.0);
    double aym = -G * mm * dym / pow(rmoon, 3.0);

    ds.x = s.vx;
    ds.y = s.vy;
    ds.vx = axe + axm;
    ds.vy = aye + aym;
    ds.m = 0.0;

    return ds;
}