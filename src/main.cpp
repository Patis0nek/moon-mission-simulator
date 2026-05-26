#include "constants.h"
#include "integrator.h"
#include "physics.h"
#include "state.h"
#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

int main()
{
    ofstream file("results/trajectory.txt");

    double final_moon_phase = 0.4;
    double alpha = -1;
    double speed = 10930.0;
    double gamma = 0.375;

    double r0 = re + 200000.0;
    double parking_period = 2.0 * pi * sqrt(pow(r0, 3.0) / (G * me));
    double parking_omega = 2.0 * pi / parking_period;

    moon_phase = final_moon_phase - moon_omega * parking_period;

    double closest_moon = 1.0e30;
    double closest_moon_time = 0.0;
    double max_altitude = 0.0;
    double return_time = 0.0;
    double return_velocity = 0.0;
    bool returned = false;

    for (double t = 0.0; t <= parking_period; t += dt) {
        double angle = alpha + parking_omega * t;

        State p;

        p.x = r0 * cos(angle);
        p.y = r0 * sin(angle);
        p.vx = -sqrt(G * me / r0) * sin(angle);
        p.vy = sqrt(G * me / r0) * cos(angle);
        p.m = mf;

        double r = norm(p.x, p.y);
        double altitude = r - re;
        double v = norm(p.vx, p.vy);

        double xm = moon_x(t);
        double ym = moon_y(t);

        double dxm = p.x - xm;
        double dym = p.y - ym;
        double distance_moon = norm(dxm, dym) - rm;

        file << t << " " << p.x << " " << p.y << " " << p.vx << " " << p.vy << " " << p.m << " " << altitude << " " << v << " " << xm << " " << ym << " " << distance_moon << endl;
    }

    double burn_time = parking_period;

    State s;

    double ux = cos(alpha);
    double uy = sin(alpha);

    double tx = -sin(alpha);
    double ty = cos(alpha);

    s.x = r0 * ux;
    s.y = r0 * uy;

    s.vx = speed * (cos(gamma) * tx + sin(gamma) * ux);
    s.vy = speed * (cos(gamma) * ty + sin(gamma) * uy);

    s.m = mf;

    for (double tau = 0.0; tau <= tmax; tau += dt) {
        double t = burn_time + tau;

        double r = norm(s.x, s.y);
        double altitude = r - re;
        double v = norm(s.vx, s.vy);

        double xm = moon_x(t);
        double ym = moon_y(t);

        double dxm = s.x - xm;
        double dym = s.y - ym;
        double distance_moon = norm(dxm, dym) - rm;

        file << t << " " << s.x << " " << s.y << " " << s.vx << " " << s.vy << " " << s.m << " " << altitude << " " << v << " " << xm << " " << ym << " " << distance_moon << endl;

        if (distance_moon < closest_moon) {
            closest_moon = distance_moon;
            closest_moon_time = t;
        }

        if (altitude > max_altitude) {
            max_altitude = altitude;
        }

        bool after_moon = t > closest_moon_time + 24.0 * 3600.0 && closest_moon < 100000.0 * 1000.0;

        if (after_moon && altitude < 200.0 * 1000.0 && returned == false) {
            return_time = t;
            return_velocity = v;
            returned = true;
            break;
        }

        s = rk4(s, t);
    }

    file.close();

    ofstream summary("results/summary.txt");

    summary << "Moon Mission Simulator - Mission Summary" << endl;
    summary << endl;
    summary << "model = parking orbit + impulsive TLI + Earth-Moon coast" << endl;
    summary << endl;
    summary << "final moon_phase at TLI = " << final_moon_phase << " rad" << endl;
    summary << "corrected moon_phase at simulation start = " << moon_phase << " rad" << endl;
    summary << "parking orbit altitude = " << (r0 - re) / 1000.0 << " km" << endl;
    summary << "parking orbit period = " << parking_period / 60.0 << " min" << endl;
    summary << endl;
    summary << "alpha = " << alpha << " rad" << endl;
    summary << "TLI speed = " << speed << " m/s" << endl;
    summary << "gamma = " << gamma << " rad" << endl;
    summary << endl;
    summary << "closest Moon approach = " << closest_moon / 1000.0 << " km" << endl;
    summary << "time of closest Moon approach = " << closest_moon_time / 3600.0 << " h" << endl;
    summary << "maximum Earth altitude = " << max_altitude / 1000.0 << " km" << endl;

    if (returned) {
        summary << "return time = " << return_time / 3600.0 << " h" << endl;
        summary << "return velocity = " << return_velocity / 1000.0 << " km/s" << endl;
    } else {
        summary << "return time = not reached" << endl;
    }

    summary.close();

    return 0;
}