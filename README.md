# Scarce
> Are you sick and tired of game engines wasting precious memory? Do you feel like you can't stand realistic graphics anymore? Do you absolutely hate memory safety in any shape or form? Look no further. 

Scarce is a suite of "_tools_" to help you develop "_games_" using an arbitrarily low amount of RSS memory. This repository has a testbed application if you're interested in its available features, but no documentation has been written so far. Disregarding stack allocations, the application can only access and modify the contents of a pre-allocated chunk of memory.

Scarce also provides a screen with an instanced text renderer and some useful memory management tools.

> [!WARNING]
> This project is currently only available on Linux. The Windows binaries are incomplete, even though it provides GLFW as a windowing system. I will implement it in the near future, but it is not a priority. I do not plan on supporting MacOS and other platforms.

## Purpose
Memory is a precious asset that programmers recently started taking for granted. However, RAM prices are through the roof now. So, with that in mind, I have decided to challenge myself and make an open world game that uses so little memory that a computer from the 1998's would have no trouble running it. In fact, it's possible to rewrite the graphics layer to support OpenGL 1.1, and write a Windows 98 windowing system and voilá.

That's the sole purpose of this game engine.

At the time of writing this, the game application size is sitting at aroung 6KiB, and the total resident memory used is around 20MiB (X11), with a couple megabytes more when using GLFW. The entire game runs on a 1KiB memory chunk. Some minor stack allocations are inevitable, but mitigated whenever possible.

# Getting Started
As with all my previous projects, no prebuilt binaries have been released at the time of writing this readme. If you want to use it, you'll need to build it yourself (sorry). A template repository is available in my account.

The latest version of this project has only been tested on [Fedora Linux 43 (KDE Plasma Edition)](https://www.fedoraproject.org/) with both `X11` and `Wayland`. The Window binaries are incomplete.

## Prerequisites
Building this project requires the following:

* [Meson](https://mesonbuild.com/index.html)

Additionally, you may need to install the Mesa drivers. 
Debian:
```bash
sudo apt install libgl1-mesa-dev libgl1-mesa-dri mesa-utils
```

Red Hat:
```bash
sudo dnf install mesa-libGL mesa-libGL-devel
```

## Installation

1. **Clone the repository**
    ```bash
    git clone https://github.com/dat-alpaca/scarce
    ```
    <br>

2. **Setup the project**
    The only options available are `BUILD_LOADER`, `BUILD_TESTBED`, and `USE_NATIVE_WINDOWING`. They should be pretty self-explanatory. The engine (or core features) is always generated.
    
    ```bash
    ./scripts/setup
    ./scripts/configure
    ```

4. **Install the project**
    ```bash
    ./scripts/install
    ```

    After installation, you should have a working copy under `deploy/`. It should contain a copy of the `res` folder, the scarce loader executable (`scarce`), and the testbin stripped binary.

5. **Run**
    You can use the following command to run the application under `deploy/`.
    ```bash
    ./scripts/run
    ```

    This will generate a premade `config.ini` file that you can modify. Hopefully, the fields are easy enough to understand. 

# License
This project is licensed under the [MIT License](LICENSE).