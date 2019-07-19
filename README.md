# Flight Factory

> May Lars Blackmore render our convexification lossless.

![We fly high, no lie. You know this.](https://stefandebruyn.github.io/assets/images/flight-factory-banner.png)

Flight Factory is LRA's workbench for developing and testing control theory and
avionics software. It is primarily composed of a simulation API that is easily
interfaced with Arduino flight software for hardware-free ground testing.

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
sudo apt-get install git build-essential python3.6
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
`src/flight_factory.cpp`.

The makefile must be generated anew when adding new source files.

```
python3 make.py /path/to/sketch
```

`/path/to/sketch` is the root of the Arduino sketch being simulated.
The sketch's dependencies, if not elsewhere in the factory, must be in this
folder as well.

Then, build and run.

```
make clean; make ff; ./ff
```

Alternatively, `fly.sh` will do all of the above for you. This is your go-to
compile & run one-liner.

```
./fly.sh /path/to/sketch
```

`build.sh`, an almost identical script that simply omits the execution of the
final binary, may be preferable in some debugging cases.

---

## Writing Factory-Compliant Sketches

There are many ways to write code that interfaces with Flight Factory. Our
preferred method is using Photonic's hardware abstractions to create virtual and
physical I/O that can be toggled between with a single `#define`.

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

Flight Factory includes its own abstractions for certain Arduino-specific
objects, such as `Serial`.

---

## Maintainers

Maintained by the experimental branch of the
[Longhorn Rocketry Association](http://www.longhornrocketry.org/) at the
University of Texas at Austin.
