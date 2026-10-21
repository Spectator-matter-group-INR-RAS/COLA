# COLA - Nuclear COllision LAyout framework

[![CI](https://github.com/Spectator-matter-group-INR-RAS/COLA/actions/workflows/ci.yml/badge.svg)](https://github.com/Spectator-matter-group-INR-RAS/COLA/actions/workflows/ci.yml)
[![Docs](https://img.shields.io/badge/docs-Doxygen-2c5f7c)](https://github.com/Spectator-matter-group-INR-RAS/COLA/blob/main/README.md#documentation)

Architectural framework for merging nucleus-nucleus collision models.

## Installation

In source directory:

```bash
cd ../
mkdir build
cd build
cmake ../COLA
cmake --build . --target install
```

You can specify `CMAKE_INSTALL_PREFIX` to change the installation location (default for most Linux systems is `/usr/local`, which requires root)

In the installation directory there is a `config.sh` file, which should be sourced to add CMake package location to indexed directories list (in case the installation directory is not standard for the system). Consider adding it to `.profile` or `.bashrc`

## Documentation

COLA comes with Doxygen documentation. To generate it, install doxygen and run from source directory:

```bash
cd docs
doxygen
```

Generated documentation will be located in the ```docs/compiled/``` folder.

## COLA Modules

Currently there is a [minimal module + program example](https://github.com/apBUSampK/COLA-min-example) with some guidelines on module writing, as well as [AAMCC-COLA](https://github.com/Spectator-matter-group-INR-RAS/AAMCC-COLA), a COLA implementation of AAMCC-MST model, along with relevant modules.
