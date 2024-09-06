# COLA - Nuclear COllision LAyout framework

Architectural framework for merging nucleus-nucleus collision models.

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

## Documentation

COLA comes with Doxygen documentation. To generate it, install doxygen and run from source directory:
```bash
$ cd docs
$ doxygen
```

Generated documentation will be located in the ```docs/compiled/``` folder.

## COLA Modules

Currently there is only a [minimal module + program example](https://github.com/apBUSampK/COLA-min-example) with some guidelines on module writing.
