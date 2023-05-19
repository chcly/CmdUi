# CmdUi

CmdUi is a basic cross platform console UI. 

Could be develped further if there is any use for it.

## Testing

The testing directory is setup to work with [googletest](https://github.com/google/googletest).
It contains the initial setup for testing this library as a standalone module using GitHub actions.

## Building

It builds with cmake, and it has been tested on Windows and Unix platforms using
the Visual Studio an Unix MakeFile cmake generators.

### Building with CMake and Make

```sh
mkdir build
cd build
cmake .. -DCmdUi_BUILD_TEST=ON -DCmdUi_AUTO_RUN_TEST=ON
make
```

### Optional defines

| Option                      | Description                                          | Default |
|:----------------------------|:-----------------------------------------------------|:-------:|
| CmdUi_BUILD_TEST         | Build the unit test program.                         |   ON    |
| CmdUi_AUTO_RUN_TEST      | Automatically run the test program.                  |   OFF   |
| CmdUi_USE_STATIC_RUNTIME | Build with the MultiThreaded(Debug) runtime library. |   ON    |
