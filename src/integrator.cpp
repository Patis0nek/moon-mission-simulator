#include "integrator.h"
#include "constants.h"
#include "physics.h"

State add(State a, State b, double c)
{
    State s;

    s.x = a.x + c * b.x;
    s.y = a.y + c * b.y;
    s.vx = a.vx + c * b.vx;
    s.vy = a.vy + c * b.vy;
    s.m = a.m + c * b.m;

    return s;
}

State rk4(State s, double t)
{
    State k1 = derivatives(s, t);
    State k2 = derivatives(add(s, k1, dt / 2.0), t + dt / 2.0);
    State k3 = derivatives(add(s, k2, dt / 2.0), t + dt / 2.0);
    State k4 = derivatives(add(s, k3, dt), t + dt);

    State next;

    next.x = s.x + dt * (k1.x + 2.0 * k2.x + 2.0 * k3.x + k4.x) / 6.0;
    next.y = s.y + dt * (k1.y + 2.0 * k2.y + 2.0 * k3.y + k4.y) / 6.0;
    next.vx = s.vx + dt * (k1.vx + 2.0 * k2.vx + 2.0 * k3.vx + k4.vx) / 6.0;
    next.vy = s.vy + dt * (k1.vy + 2.0 * k2.vy + 2.0 * k3.vy + k4.vy) / 6.0;
    next.m = s.m + dt * (k1.m + 2.0 * k2.m + 2.0 * k3.m + k4.m) / 6.0;

    if (next.m < mf) {
        next.m = mf;
    }

    return next;
}