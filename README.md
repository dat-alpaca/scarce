# Scarce
> Are you sick and tired of game engines wasting precious memory? Do you feel like you can't stand realistic graphics anymore? Do you absolutely hate memory safety in any way, shape, or form? Look no further. 

Scarce is a suite of "_tools_" to help you develop "_games_" using an arbitrarily low amount of RSS memory. This repository has a testbed application if you're interested in its available features, but no documentation has been written so far. Stack allocations aside, the application can only access and modify the contents of a pre-allocated chunk of memory.

Scarce also provides a screen with an instanced text renderer and some useful memory management tools.

> [!WARNING]
> This project is currently only available on Linux. The Windows binaries are incomplete, even though the engine provides a GLFW windowing system. I will implement it in the near future, but it is not a priority right now. I have no plans to support any other platforms.

## Purpose
Memory is a precious asset that programmers have started taking for granted lately.

But, with RAM prices through the roof, I have decided to challenge myself by making an open world game that uses so little memory that a computer from the 90s would have no trouble running it. In fact, you could write a graphics layer to support OpenGL 1.1, slap a Windows 98 windowing system, and voilá.

That's the sole purpose of this game engine.

At the time of writing, the game application size is sitting at aroung 8KiB, and the total resident memory used is around 20MiB (Wayland, Fedora 43), with a few extra megabytes when using the GLFW windowing system. The entire game runs on a 1KiB memory chunk.

Although the game application is restricted to a predetermined memory region, using the stack and other memory regions is allowed, though I am doing my best to keep that usage to a minimum whenever possible.

# Getting Started
As is the case with all my previous projects, no prebuilt binaries have been released at the time of writing. If you want to use it, you'll need to build it yourself (sorry). At least this time a [template repository](https://github.com/dat-alpaca/scarce-template) is available.

The latest version of this project has only been tested on [Fedora Linux 43 (KDE Plasma Edition)](https://www.fedoraproject.org/) with both `X11` and `Wayland`, using GLFW and the native X11 windowing systems.

## Prerequisites
If you actually want to build this thing, you are going to need:

* [Meson](https://mesonbuild.com/index.html)
* A GPU with OpenGL 4.6 support
* Mesa drivers

Debian:
```bash
sudo apt install libgl1-mesa-dev libgl1-mesa-dri mesa-utils
```

Red Hat:
```bash
sudo dnf install mesa-libGL mesa-libGL-devel libx11-dev
```

## Installation

1. **Clone the repository**
    ```bash
    git clone https://github.com/dat-alpaca/scarce
    ```
    <br>

2. **Setup the project**
    The only options available are `BUILD_LOADER`, `BUILD_TESTBED`, and `USE_NATIVE_WINDOWING`. They should be pretty self-explanatory. The engine library (or core features) is always generated.
    
    ```bash
    ./scripts/setup
    ./scripts/configure
    ```

4. **Install the project**
    ```bash
    ./scripts/install
    ```

    After installation, you should have a working copy under `deploy/`. It should contain a copy of the `assets` folder, the scarce loader executable (`scarce`), and the `testbed` application stripped binary.

5. **Run**
    You can use the following command to run the application located in `deploy/`.
    ```bash
    ./scripts/run
    ```

    This will generate a premade `config.ini` file that you can modify. The fields should (hopefully) be easy enough to understand. 

# License
This project is licensed under the [MIT License](LICENSE).