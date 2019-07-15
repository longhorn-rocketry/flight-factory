# flight-factory

> May Lars Blackmore render our convexification lossless.

Flight Factory is LRA's workbench for developing and testing control theory and
avionics software. It is primarily composed of a simulation API that is easily
interfaced with Arduino flight computer software for hardware-free ground testing.

---

## Products

The following are LRA tools created in the factory.

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
python3 make.py
```

Then, build and run.

```
make clean; make ff; ./ff
```

Alternatively, `fly.sh` will do all of the above for you. This is your go-to
compile & run one-liner.

```
./fly.sh
```

`build.sh`, an almost identical script that simply omits the execution of the
final binary, may be preferable in some debugging cases.

---

## Maintainers

Maintained by the experimental branch of the
[Longhorn Rocketry Association](http://www.longhornrocketry.org/) at the
University of Texas at Austin.
