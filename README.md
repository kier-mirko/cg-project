## How to build on Windows

Open a `x64 Native Tools Command Prompt for VS <year>`.  You can ensure that the MSVC compiler is accessible by running:
```batch
cl
```
If everything is set up correctly, you should have output very similar to the following:
```batch
Microsoft (R) C/C++ Optimizing Compiler Version 19.29.30151 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

usage: cl [ option... ] filename... [ /link linkoption... ]
```
If don't have a `x64 Native Tools Command Prompt for <year>`. Install Visual Studio Community 2022 and in the installer click the option `Desktop Development with C++` in the workload section.

To build the project, go to the project root directory and run:
```batch
build.bat
```

## How to build for Linux

>[!WARNING]
>The project was tested only on WSL2

Go to the project root directory and run:
```sh
chmod +x ./build.sh
./build.sh
```
