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
    double closest_relx;
    double closest_rely;
    double alpha;
    double speed;
    double gamma;
    bool returned;
};

Result simulate(double alpha, double speed, double gamma)
{
    State s;

    double r0 = re + 200000.0;

    double ux = cos(alpha);
    double uy = sin(alpha);

    double tx = -sin(alpha);
    double ty = cos(alpha);

    s.x = r0 * ux;
    s.y = r0 * uy;

    s.vx = speed * (cos(gamma) * tx + sin(gamma) * ux);
    s.vy = speed * (cos(gamma) * ty + sin(gamma) * uy);

    s.m = mf;

    Result result;

    result.closest_moon = 1.0e30;
    result.closest_moon_time = 0.0;
    result.closest_earth_after_moon = 1.0e30;
    result.closest_earth_time = 0.0;
    result.return_velocity = 0.0;
    result.closest_relx = 0.0;
    result.closest_rely = 0.0;
    result.alpha = alpha;
    result.speed = speed;
    result.gamma = gamma;
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

            double theta = atan2(ym, xm);

            double xr = s.x * cos(-theta) - s.y * sin(-theta);
            double yr = s.x * sin(-theta) + s.y * cos(-theta);

            result.closest_relx = xr - moon_distance;
            result.closest_rely = yr;
        }

        bool after_moon = t > result.closest_moon_time + 24.0 * 3600.0 && result.closest_moon < 100000.0 * 1000.0;

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
    double target_relx = rm + target_moon;
    double target_rely = 0.0;

    double best_phase = 0.0;
    double best_score = 1.0e30;

    Result best_result;

    for (double phase = 0.40; phase <= 0.70; phase += 0.01) {
        moon_phase = phase;

        for (double alpha = -1.00; alpha <= -0.60; alpha += 0.05) {
            for (double speed = 10520.0; speed <= 11380.0; speed += 10.0) {
                for (double gamma = 0.05; gamma <= 0.4; gamma += 0.025) {
                    Result result = simulate(alpha, speed, gamma);

                    double moon_score = fabs(result.closest_moon - target_moon);
                    double return_score = fabs(result.closest_earth_after_moon - target_return);
                    double side_score = fabs(result.closest_relx - target_relx) + 0.5 * fabs(result.closest_rely - target_rely);

                    double score = moon_score + 2.0 * return_score + 0.8 * side_score;

                    if (result.closest_moon < 4000.0 * 1000.0) {
                        score += 5.0e9;
                    }

                    if (result.closest_moon > 15000.0 * 1000.0) {
                        score += 1.0e9;
                    }

                    if (result.returned == false) {
                        score += 2.0e9;
                    }

                    if (result.closest_relx < 0.0) {
                        score += 2.0e9;
                    }

                    if (result.closest_earth_after_moon < 0.0) {
                        score += 5.0e8;
                    }

                    if (result.closest_earth_after_moon > 300.0 * 1000.0) {
                        score += 5.0e8;
                    }

                    if (score < best_score) {
                        best_score = score;
                        best_phase = phase;
                        best_result = result;

                        cout << "phase = " << best_phase << ", alpha = " << best_result.alpha << ", speed = " << best_result.speed << ", gamma = " << best_result.gamma
                             << ", Moon = " << best_result.closest_moon / 1000.0 << " km, Earth return = " << best_result.closest_earth_after_moon / 1000.0
                             << " km, flyby x = " << best_result.closest_relx / 1000.0 << " km, flyby y = " << best_result.closest_rely / 1000.0 << " km, returned = " << best_result.returned << endl;

                        file << best_phase << " " << best_result.alpha << " " << best_result.speed << " " << best_result.gamma << " " << best_result.closest_moon << " "
                             << best_result.closest_moon_time << " " << best_result.closest_earth_after_moon << " " << best_result.closest_earth_time << " " << best_result.return_velocity << " "
                             << best_result.closest_relx << " " << best_result.closest_rely << " " << best_result.returned << " " << best_score << endl;
                    }
                }
            }
        }
    }
    file.close();

    cout << endl;
    cout << "best moon_phase = " << best_phase << endl;
    cout << "best alpha = " << best_result.alpha << endl;
    cout << "best speed = " << best_result.speed << endl;
    cout << "best gamma = " << best_result.gamma << endl;
    cout << "closest Moon distance = " << best_result.closest_moon / 1000.0 << " km" << endl;
    cout << "time of Moon flyby = " << best_result.closest_moon_time / 3600.0 << " h" << endl;
    cout << "closest Earth return altitude = " << best_result.closest_earth_after_moon / 1000.0 << " km" << endl;
    cout << "time of Earth return = " << best_result.closest_earth_time / 3600.0 << " h" << endl;
    cout << "return velocity = " << best_result.return_velocity / 1000.0 << " km/s" << endl;
    cout << "flyby relative x = " << best_result.closest_relx / 1000.0 << " km" << endl;
    cout << "flyby relative y = " << best_result.closest_rely / 1000.0 << " km" << endl;
    cout << "returned = " << best_result.returned << endl;

    return 0;
}