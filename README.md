# COLA - Nuclear COllision LAyout framework

Based on pipeline architecture framework for the merging of 
a few parts of the nucleus-nucleus collision generators

## Installation

in source directory:

```bash
$ cd ../
$ mkdir build
$ cd build
$ cmake ../COLA
$ cmake --build . --target install
```

You can specify `CMAKE_INSTALL_PREFIX` to change the installation location (default for most Linux systems is `/usr/local`, which requires root)

In the installation directory there is `config.sh` file, which should be sourced to add CMake package location to indexed directories list (in case the installation directory is not standart for the system). Consider adding it to `.profile` or `.bashrc`

## COLA Modules

Currently there is only a [minimal module + program example](https://github.com/apBUSampK/COLA-min-example) with some guidelines on module writing.
