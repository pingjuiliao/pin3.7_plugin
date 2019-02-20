# Pin 3.7 plugins
## My plugin 
    
    DTA/obj-intel64/ret_addr_untainted.so // check if the return addr is tainted 



## To Run (dynamic instrumentation)

```
$ ./pin -t <path/to/plugin.so> -- <program_to_be_instrumented>

```

## To write a new plugin (under DTA directory)

Write the script and compile

```
$ vim ./DTA/MyNewPlugin.cpp

$ make PIN_ROOT=$PIN_ROOT MyNewTool

```
Or just wanna type 'make'

```

$ vim ./DTA/MyNewPlugin.cpp
$ ## after setting up environments ...
$ ##    ( See: Some advanced setups in this README )
$ vim makefile.rules
...
    TOOL_ROOTS := OldTool1 OldTool2 MyNewTool
...

$ make 

```


## Some advanced setups

### To just type 'make' instead of 'make PIN_ROOT=${PIN_ROOT} MyNewTools.so'

requirements:
1. env variable       : ${PIN_ROOT}
2. makefile variable  : $[PIN_ROOT]
3. makefile.rules with target tools script name

    TOOL_ROOTS := MyNewTools

```
$ cd ${PIN_ROOT}
$ ./setup
$ ## setting up environment variable ${PIN_ROOT} in ~/.bashrc
$ ## setting up    makefile config   $(PIN_ROOT) in ${PIN_ROOT}/source/tools/Config/makefile.config
$ cd <path to AnyDirectoryYouWant>
$ make                                ## : )
```
### To create new directories for plugins

```
$ cp -r $(PIN_ROOT)/source/tools/MyPinTools AnyDirectoryYouWant
$ vi AnyDirectoryYouWant/makefile.rules
...
 TOOL_ROOT : MyNewTool
...
$ vi MyNewTool.cpp
...

#include "pin.H"

$ make PIN_ROOT=${PIN_ROOT} 
$ # The target will be at AnyDirectoryYouWant/obj-intel64/MyNewTools.so

```


## Files I created

```
./DTA/*
    ret_addr_untainted.cpp      ## script for taint analysis
./README.md                     ## this file
./setup.sh                      ## setup for env, makefile env
./bin/*                         ## binaries to be instrumented
   ./bin/buf_ovfl/*                 ## vulnerable binaries
   ./bin/test/*                     ## some c small program for learning assembly or c/c++ library

```
```
