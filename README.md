# vutu

`vutu` uses the the [loris](http://www.cerlsoundgroup.org/Loris/) library to analyze and synthesize sounds. 

## building

The following libraries are required:
- [Loris](https://github.com/madronalabs/loris)
- [madronalib](https://github.com/madronalabs/madronalib)
- [mlvg](https://github.com/madronalabs/mlvg)
- [SDL2](https://www.libsdl.org)

Vutu can be built with CMake:
```
- mkdir build
- cd build
- cmake -GXcode .. (for Mac)
```

The libsndfile and libresample libraries are also included. Because some configuration is required to get these to work, they are compiled here from source. 

On MacOS, we link to SDL2.framework. Get the latest SDL2 .dmg, place SDL2.framework into /Library/Frameworks, and CMake should take care of the rest. 

Everything is theoretically cross-platform but I'm currently working on Mac and have not been testing on Windows.

### Installing Loris

To install the Loris package, follow these steps:

1. Clone the Loris repository:
```
git clone https://github.com/madronalabs/loris.git
```

2. Build and install Loris:
```
cd loris
mkdir build
cd build
cmake ..
make
sudo make install
```

3. Specify the Loris package configuration file path in your `CMakeLists.txt` file:
```cmake
set(Loris_DIR "/path/to/loris/cmake")
find_package(Loris REQUIRED)
```
