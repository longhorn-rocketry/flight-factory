# Flight Factory

> May Lars Blackmore render our convexification lossless.

Flight Factory is LRA's workbench for developing and testing avionics software.
It is primarily composed of a simulation API that easily interfaces with Arduino
sketches for hardware-free ground testing of production flight software.

![We fly high, no lie.](https://stefandebruyn.github.io/assets/images/flight-factory-banner.png)

![You know this.](https://stefandebruyn.github.io/assets/images/flight-factory-window.png)

---

## Submodules

The following are LRA tools created and used in the factory.

* [Photonic](https://github.com/longhorn-rocketry/photonic), our flight computer
library
* [Aimbot](https://github.com/longhorn-rocketry/aimbot), the control algorithm
for our experimental airbrakes

---

## Setup

This codebase was developed⁠—and consequently only verified to work⁠—on
Ubuntu 16.04. We prefer this release because it plays very nicely with GCC.

First, collect the dependencies.

```
sudo apt-get install git build-essential python3.6 matplotlib
```

Then, clone the repository into your location of choice.

```
git clone https://github.com/longhorn-rocketry/flight-factory
cd flight-factory
```

Pull the contents of all submodules.

```
git submodule update --recursive --remote
```

Projects are built with make. The `make.py` script will automatically
generate a makefile from the contents of submodules and the `src` and `include`
directories. This search is recursive. The project entrypoint must be within
`src/main.cpp`.

The makefile must be generated anew when adding new source files.

```
python3 make.py /path/to/sketch
```

`/path/to/sketch` is the root of the Arduino sketch being simulated.
The sketch's dependencies, if not elsewhere in the factory, must be in this
folder as well. This path must also contain a configuration file `.ff`. The
format of this file is covered further below.

Then, build and run.

```
make clean; make ff; ./ff /path/to/sketch
```

Alternatively, `fly.sh` will do all of the above for you. This is your go-to
compile & run one-liner.

```
./fly.sh /path/to/sketch
```

`build.sh`, an almost identical script that simply omits the execution of the
final binary, may be preferable in some debugging cases.

---

## Writing Configuration Files

The configuration file, a plaintext file named `.ff` in the sketch root,
describes the simulation and the rocket being simulated.

Configuration files follow the basic INI format, with a few exceptions. Valid
sections and keys are as follows:
* `simulation` simulator and environment parameters
  - `initial_altitude` launchpad altitude
  - `type` one of (`dof1`); simulator type
  - `t_ignition` time of motor ignition
  - `dt` time resolution
  - `stop_condition` one of (`impact`, `apogee`); simulation stop event
* `rocket` physical rocket properties
  - `mass` rocket dry mass
  - `radius` body tube radius
  - `surface_area` surface area exposed to air stream; `auto` uses body tube
    cross section
  - `airbrake_surface_area` airbrake surface area exposed to air stream at
    100% extension
  - `drag_coefficient` static rocket Cd or `auto` to use profile
  - `nosecone_length` length of nose cone
  - `fineness` rocket fineness ratio
  - `skin_roughness` rocket skin finish
* `cd` rocket drag coefficient profile
  - A list of `M Cd` pairs with monotonically increasing Mach numbers `M`
    mapped to drag coefficients `Cd`
* `motor` motor thrust profile
  - A list of `t F` pairs with monotonically increasing timesteps `t` mapped to
    thrust scalars `F`

Lines which begin with `#` are comments.

For reference, below is the config file for Torchy, LRA's 2019 SAC rocket.

```
[simulation]
dt=0.1
stop_condition=impact
# Altitude of Truth or Consequences, NM
initial_altitude=1293.876
type=dof1
t_ignition=1.0

[rocket]
mass=35
radius=0.0762
surface_area=auto
airbrake_surface_area=0.0070866
drag_coefficient=auto

[cd]
# M/Cd profile derived from OpenRocket
0 0.46
0.5 0.55
0.9 0.67
1.0 0.91
1.1 0.76
1.4 0.55
2 0.23

[motor]
# Thrust profile for an N2200-PK
0 0
0.05 2750
0.2 2450
1.0 2550
2.0 2600
4.0 2200
4.7 2100
5.25 500
6 0
```

---

## Writing Factory-Compliant Code

There are many ways to write code that interfaces with Flight Factory. Our
preferred method is using Photonic's hardware abstractions to create virtual and
physical I/O that can be toggled between with a single `#define`. Flight Factory
includes its own abstractions for certain Arduino-specific objects, such as
`Serial`.

For example:

```c++
#define GROUND_TEST // TODO: REMOVE BEFORE FLIGHT

...

Imu *imu =
#ifdef GROUND_TEST
  // A fake IMU attached to the Flight Factory simulation
  new VirtualImu();
#else
  // A real IMU attached to an I2C port on the flight computer
  new BNO055Imu();
#endif
```

This concept can be generalized to any dependency.

Internal flight computer telemetry can be printed mid-simulation or visualized
post-simulation with a host of logging methods in the `ff` namespace. Telemetry
streams are assigned names and piped to files in `/path/to/sketch/telem`.

```c++
#include "ff_telemetry.hpp"

...

// Causes /path/to/sketch/telem/accel.dat to populate
ff::topen("accel");

...

imu->update();
// Log (pipe name, timestamp, value)
ff::tout("accel", rocket_time(), imu->get_acc_z());

```

The script `vis.py` will generate matplots of `.dat` files generated in this
fashion:

```
python3 vis.py /path/to/sketch/telem/accel.dat
```

---

## Maintainers

Maintained by the experimental branch of the
[Longhorn Rocketry Association](http://www.longhornrocketry.org/) at the
University of Texas at Austin.
