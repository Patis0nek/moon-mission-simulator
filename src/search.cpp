#include "constants.h"
#include "integrator.h"
#include "physics.h"
#include "state.h"
#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

struct Result {
    double closest_moon;
    double closest_moon_time;
    double closest_earth_after_moon;
    double closest_earth_time;
    double return_velocity;
    bool returned;
};

Result simulate(double vx0, double vy0)
{
    State s;

    s.x = re + 200000.0;
    s.y = 0.0;
    s.vx = vx0;
    s.vy = vy0;
    s.m = mf;

    Result result;

    result.closest_moon = 1.0e30;
    result.closest_moon_time = 0.0;
    result.closest_earth_after_moon = 1.0e30;
    result.closest_earth_time = 0.0;
    result.return_velocity = 0.0;
    result.returned = false;

    for (double t = 0.0; t <= tmax; t += dt) {
        double r = norm(s.x, s.y);
        double altitude = r - re;
        double v = norm(s.vx, s.vy);

        double xm = moon_x(t);
        double ym = moon_y(t);

        double dxm = s.x - xm;
        double dym = s.y - ym;
        double distance_moon = norm(dxm, dym) - rm;

        if (distance_moon < result.closest_moon) {
            result.closest_moon = distance_moon;
            result.closest_moon_time = t;
        }

        bool after_moon = t > result.closest_moon_time + 24.0 * 3600.0 && result.closest_moon < 80000.0 * 1000.0;

        if (after_moon && altitude < result.closest_earth_after_moon) {
            result.closest_earth_after_moon = altitude;
            result.closest_earth_time = t;
            result.return_velocity = v;
        }

        if (after_moon && altitude < 200.0 * 1000.0 && altitude > 0.0) {
            result.returned = true;
            result.closest_earth_after_moon = altitude;
            result.closest_earth_time = t;
            result.return_velocity = v;
            break;
        }

        if (after_moon && altitude < 0.0) {
            result.returned = true;
            result.closest_earth_after_moon = altitude;
            result.closest_earth_time = t;
            result.return_velocity = v;
            break;
        }

        s = rk4(s, t);
    }

    return result;
}

int main()
{
    ofstream file("results/search_best.txt");

    double target_moon = 6550.0 * 1000.0;
    double target_return = 100.0 * 1000.0;

    double best_phase = 0.0;
    double best_vx = 0.0;
    double best_vy = 0.0;
    double best_score = 1.0e30;

    Result best_result;

    for (double phase = 0.85; phase <= 1.15; phase += 0.005) {
        moon_phase = phase;

        for (double vx0 = 2100.0; vx0 <= 2350.0; vx0 += 10.0) {
            for (double vy0 = 10650.0; vy0 <= 10750.0; vy0 += 5.0) {
                Result result = simulate(vx0, vy0);

                double moon_score = fabs(result.closest_moon - target_moon);
                double return_score = fabs(result.closest_earth_after_moon - target_return);

                double score = moon_score + 2.0 * return_score;

                if (result.closest_moon < 0.0) {
                    score += 1.0e9;
                }

                if (result.closest_moon > 50000.0 * 1000.0) {
                    score += 1.0e9;
                }

                if (result.returned == false) {
                    score += 1.0e9;
                }

                file << phase << " " << vx0 << " " << vy0 << " " << result.closest_moon << " " << result.closest_moon_time << " " << result.closest_earth_after_moon << " " << result.closest_earth_time
                     << " " << result.returned << " " << score << endl;

                if (score < best_score) {
                    best_score = score;
                    best_phase = phase;
                    best_vx = vx0;
                    best_vy = vy0;
                    best_result = result;

                    cout << "phase = " << best_phase << ", vx = " << best_vx << ", vy = " << best_vy << ", Moon = " << best_result.closest_moon / 1000.0
                         << " km, Earth return = " << best_result.closest_earth_after_moon / 1000.0 << " km, returned = " << best_result.returned << endl;
                }
            }
        }
    }

    file.close();

    cout << endl;
    cout << "best moon_phase = " << best_phase << endl;
    cout << "best vx = " << best_vx << endl;
    cout << "best vy = " << best_vy << endl;
    cout << "closest Moon distance = " << best_result.closest_moon / 1000.0 << " km" << endl;
    cout << "time of Moon flyby = " << best_result.closest_moon_time / 3600.0 << " h" << endl;
    cout << "closest Earth return altitude = " << best_result.closest_earth_after_moon / 1000.0 << " km" << endl;
    cout << "time of Earth return = " << best_result.closest_earth_time / 3600.0 << " h" << endl;
    cout << "return velocity = " << best_result.return_velocity / 1000.0 << " km/s" << endl;
    cout << "returned = " << best_result.returned << endl;

    return 0;
}