# doomgeneric (dynamic lib)
This version of [doomgeneric](https://github.com/ozkl/doomgeneric) was modified to expose a set of functions as a dynamic library instead.

Screen melting effects were disabled as they can cause issues interacting with the host's game loop properly and result in freezes.

## How to build

### Tools
You'll need to install `clang` and `make`.
* On Windows, using [chocolatey](https://chocolatey.org/) to install both is recommended. The packages are `llvm` and `make` respectively.
* On Linux, these can both be installed using your distro's package manager.
* On MacOS, these can both be installed using Homebrew. Apple clang won't work, as you need a mips target for building the mod code.

### Building
* Run `make`. The dynamic library should be in the `build` directory.
