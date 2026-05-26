import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, PillowWriter

data = np.loadtxt("results/trajectory.txt")

t = data[:, 0] / 3600.0
x = data[:, 1] / 1000.0
y = data[:, 2] / 1000.0
moon_x = data[:, 8] / 1000.0
moon_y = data[:, 9] / 1000.0
distance_moon = data[:, 10] / 1000.0

earth_radius = 6371.0
moon_radius = 1737.4
moon_distance = 384400.0

angle = np.linspace(0.0, 2.0 * np.pi, 300)

earth_x = earth_radius * np.cos(angle)
earth_y = earth_radius * np.sin(angle)

moon_circle_x = moon_radius * np.cos(angle)
moon_circle_y = moon_radius * np.sin(angle)

moon_orbit_x = moon_distance * np.cos(angle)
moon_orbit_y = moon_distance * np.sin(angle)

step = max(1, len(t) // 300)
frames = np.arange(0, len(t), step)

xmin = min(np.min(x), np.min(moon_x), -500000.0)
xmax = max(np.max(x), np.max(moon_x), 500000.0)
ymin = min(np.min(y), np.min(moon_y), -420000.0)
ymax = max(np.max(y), np.max(moon_y), 420000.0)

pad_x = 20000.0
pad_y = 20000.0

theta = np.arctan2(moon_y, moon_x)

xr = x * np.cos(-theta) - y * np.sin(-theta)
yr = x * np.sin(-theta) + y * np.cos(-theta)

moon_xr = moon_x * np.cos(-theta) - moon_y * np.sin(-theta)
moon_yr = moon_x * np.sin(-theta) + moon_y * np.cos(-theta)

closest_idx = np.argmin(distance_moon)

fig1, ax1 = plt.subplots(figsize=(8, 8))
ax1.set_aspect("equal")
ax1.set_xlim(xmin - pad_x, xmax + pad_x)
ax1.set_ylim(ymin - pad_y, ymax + pad_y)
ax1.set_xlabel("x [km]")
ax1.set_ylabel("y [km]")
ax1.set_title("Earth-Moon trajectory in inertial frame")
ax1.grid()

ax1.plot(moon_orbit_x, moon_orbit_y, ":", label="Moon orbit")
ax1.plot(earth_x, earth_y, label="Earth")

path1, = ax1.plot([], [], linewidth=2, label="spacecraft")
point1, = ax1.plot([], [], "o")
moon_path1, = ax1.plot([], [], "--", label="Moon path")
moon_body1, = ax1.plot([], [], linewidth=2)
moon_point1, = ax1.plot([], [], "o", label="Moon")
time_text1 = ax1.text(0.02, 0.95, "", transform=ax1.transAxes)
ax1.legend()

def init1():
    path1.set_data([], [])
    point1.set_data([], [])
    moon_path1.set_data([], [])
    moon_body1.set_data([], [])
    moon_point1.set_data([], [])
    time_text1.set_text("")
    return path1, point1, moon_path1, moon_body1, moon_point1, time_text1

def update1(k):
    i = frames[k]

    path1.set_data(x[:i + 1], y[:i + 1])
    point1.set_data([x[i]], [y[i]])

    moon_path1.set_data(moon_x[:i + 1], moon_y[:i + 1])
    moon_body1.set_data(moon_x[i] + moon_circle_x, moon_y[i] + moon_circle_y)
    moon_point1.set_data([moon_x[i]], [moon_y[i]])

    time_text1.set_text(
        f"time = {t[i]:.1f} h\n"
        f"Moon distance = {distance_moon[i]:.1f} km"
    )

    return path1, point1, moon_path1, moon_body1, moon_point1, time_text1

ani1 = FuncAnimation(fig1, update1, frames=len(frames), init_func=init1, blit=True)
ani1.save("plots/flight_inertial.gif", writer=PillowWriter(fps=20))
plt.close(fig1)

fig2, ax2 = plt.subplots(figsize=(8, 8))
ax2.set_aspect("equal")
ax2.set_xlim(-220000.0, 420000.0)
ax2.set_ylim(-220000.0, 220000.0)
ax2.set_xlabel("x [km]")
ax2.set_ylabel("y [km]")
ax2.set_title("Trajectory in Earth-Moon rotating frame")
ax2.grid()

ax2.plot(earth_x, earth_y, label="Earth")
ax2.plot(moon_distance + moon_circle_x, moon_circle_y, label="Moon")

path2, = ax2.plot([], [], linewidth=2, label="spacecraft")
point2, = ax2.plot([], [], "o")
time_text2 = ax2.text(0.02, 0.95, "", transform=ax2.transAxes)
ax2.legend()

def init2():
    path2.set_data([], [])
    point2.set_data([], [])
    time_text2.set_text("")
    return path2, point2, time_text2

def update2(k):
    i = frames[k]

    path2.set_data(xr[:i + 1], yr[:i + 1])
    point2.set_data([xr[i]], [yr[i]])

    time_text2.set_text(
        f"time = {t[i]:.1f} h\n"
        f"Moon distance = {distance_moon[i]:.1f} km"
    )

    return path2, point2, time_text2

ani2 = FuncAnimation(fig2, update2, frames=len(frames), init_func=init2, blit=True)
ani2.save("plots/flight_rotating.gif", writer=PillowWriter(fps=20))
plt.close(fig2)

fig3, ax3 = plt.subplots(figsize=(8, 8))
ax3.set_aspect("equal")
ax3.set_xlabel("x [km]")
ax3.set_ylabel("y [km]")
ax3.set_title("Moon flyby zoom")
ax3.grid()

zoom = 30000.0
ax3.set_xlim(moon_x[closest_idx] - zoom, moon_x[closest_idx] + zoom)
ax3.set_ylim(moon_y[closest_idx] - zoom, moon_y[closest_idx] + zoom)

path3, = ax3.plot([], [], linewidth=2, label="spacecraft")
point3, = ax3.plot([], [], "o")
moon_body3, = ax3.plot([], [], linewidth=2, label="Moon")
time_text3 = ax3.text(0.02, 0.95, "", transform=ax3.transAxes)
ax3.legend()

i0 = max(0, closest_idx - 80 * step)
i1 = min(len(t) - 1, closest_idx + 80 * step)
flyby_frames = np.arange(i0, i1 + 1, step)

def init3():
    path3.set_data([], [])
    point3.set_data([], [])
    moon_body3.set_data([], [])
    time_text3.set_text("")
    return path3, point3, moon_body3, time_text3

def update3(k):
    i = flyby_frames[k]

    j0 = max(0, i - 200)
    path3.set_data(x[j0:i + 1], y[j0:i + 1])
    point3.set_data([x[i]], [y[i]])
    moon_body3.set_data(moon_x[i] + moon_circle_x, moon_y[i] + moon_circle_y)

    time_text3.set_text(
        f"time = {t[i]:.1f} h\n"
        f"Moon distance = {distance_moon[i]:.1f} km"
    )

    return path3, point3, moon_body3, time_text3

ani3 = FuncAnimation(fig3, update3, frames=len(flyby_frames), init_func=init3, blit=True)
ani3.save("plots/flyby_zoom.gif", writer=PillowWriter(fps=20))
plt.close(fig3)