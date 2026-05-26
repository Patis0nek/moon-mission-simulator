#include "constants.h"
#include "integrator.h"
#include "physics.h"
#include "state.h"
#include <fstream>
#include <iostream>

using namespace std;

int main()
{
    ofstream file("results/trajectory.txt");
    moon_phase = 0.97;

    State s;

    s.x = re + 200000.0;
    s.y = 0.0;
    s.vx = 2250.0;
    s.vy = 10695.0;
    s.m = mf;

    for (double t = 0.0; t <= tmax; t += dt) {
        double r = norm(s.x, s.y);
        double altitude = r - re;
        double v = norm(s.vx, s.vy);

        double xm = moon_x(t);
        double ym = moon_y(t);

        double dxm = s.x - xm;
        double dym = s.y - ym;
        double distance_moon = norm(dxm, dym) - rm;

        file << t << " " << s.x << " " << s.y << " " << s.vx << " " << s.vy << " " << s.m << " " << altitude << " " << v << " " << xm << " " << ym << " " << distance_moon << endl;

        if (altitude < 0.0 && t > 2.0 * 24.0 * 3600.0) {
            break;
        }

        s = rk4(s, t);
    }

    file.close();

    return 0;
}