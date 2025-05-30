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

If everything is set up correctly, you can build the project by going to the root directory of the project and run:
```batch
build.bat
```
