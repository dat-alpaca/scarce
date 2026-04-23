# Scarce
> Are you sick and tired of game engines wasting precious memory? Do you feel like you can't stand realistic graphics anymore? Do you absolutely hate memory safety in any shape or form? Look no further. 

Scarce is a suite of "_tools_" to help you develop "_games_" using an arbitrary amount of RSS memory. This repository has a testbed application that allows you to move a "_character_" around the screen. Disregarding some minor stack allocations, the application can only access and modify a whoping 1KiB of memory.

Scarce also provides a screen with an instanced text renderer and some useful memory management tools.

# Getting Started
As with all my previous projects, no prebuilt binaries have been released at the time of writing this readme. If you want to use it, you'll need to build it yourself (sorry).

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
    The only options available are `BUILD_LOADER` and `BUILD_TESTBED`. They should be pretty self-explanatory. The engine (or core features) is always generated.
    
    ```bash
    ./scripts/setup
    ./scripts/configure
    ```

4. **Install the project**
    ```bash
    ./scripts/install
    ```

    After installation, you should have a working copy under `deploy/`. It should contain a copy of the `res` folder, the scarce loader executable (`scarce`), and the testbin stripped binary.

# License
This project is licensed under the [MIT License](LICENSE).