import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("results/trajectory.txt")

t = data[:, 0]
time_h = t / 3600.0

x = data[:, 1] / 1000.0
y = data[:, 2] / 1000.0
altitude = data[:, 6] / 1000.0
velocity = data[:, 7] / 1000.0
moon_x = data[:, 8] / 1000.0
moon_y = data[:, 9] / 1000.0
distance_moon = data[:, 10] / 1000.0

earth_radius = 6371.0
moon_radius = 1737.4
moon_distance = 384400.0

angle = np.linspace(0.0, 2.0 * np.pi, 500)

earth_x = earth_radius * np.cos(angle)
earth_y = earth_radius * np.sin(angle)

moon_orbit_x = moon_distance * np.cos(angle)
moon_orbit_y = moon_distance * np.sin(angle)

plt.figure()
plt.plot(x, y, label="spacecraft")
plt.plot(moon_x, moon_y, "--", label="Moon path during simulation")
plt.plot(moon_orbit_x, moon_orbit_y, ":", label="Moon orbit")
plt.plot(earth_x, earth_y, label="Earth")
plt.scatter([moon_x[0]], [moon_y[0]], label="Moon start")
plt.scatter([moon_x[-1]], [moon_y[-1]], label="Moon end")
plt.xlabel("x [km]")
plt.ylabel("y [km]")
plt.title("Earth-Moon trajectory in inertial frame")
plt.axis("equal")
plt.grid()
plt.legend()
plt.savefig("plots/trajectory_inertial.png", dpi=300)

theta = np.arctan2(moon_y, moon_x)

xr = x * np.cos(-theta) - y * np.sin(-theta)
yr = x * np.sin(-theta) + y * np.cos(-theta)

moon_xr = moon_x * np.cos(-theta) - moon_y * np.sin(-theta)
moon_yr = moon_x * np.sin(-theta) + moon_y * np.cos(-theta)

plt.figure()
plt.plot(xr, yr, label="spacecraft")
plt.plot(earth_x, earth_y, label="Earth")
plt.scatter([moon_distance], [0.0], label="Moon")
plt.xlabel("x [km]")
plt.ylabel("y [km]")
plt.title("Trajectory in Earth-Moon rotating frame")
plt.axis("equal")
plt.grid()
plt.legend()
plt.savefig("plots/trajectory_rotating.png", dpi=300)

plt.figure()
plt.plot(time_h, altitude)
plt.xlabel("time [h]")
plt.ylabel("altitude above Earth [km]")
plt.title("Altitude over time")
plt.grid()
plt.savefig("plots/altitude.png", dpi=300)

plt.figure()
plt.plot(time_h, distance_moon)
plt.xlabel("time [h]")
plt.ylabel("distance from Moon surface [km]")
plt.title("Distance from Moon over time")
plt.grid()
plt.savefig("plots/moon_distance.png", dpi=300)

plt.figure()
plt.plot(time_h, velocity)
plt.xlabel("time [h]")
plt.ylabel("velocity [km/s]")
plt.title("Velocity over time")
plt.grid()
plt.savefig("plots/velocity.png", dpi=300)