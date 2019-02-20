# Pin 3.7 plugins
## My plugin 
    
    DTA/obj-intel64/ret_addr_tainted.so -- check if the return addr is tainted 



## To Run (dynamic instrumentation)

$ ./pin -t ./source/tools/SimpleExamples/obj-Intel64/icount.so -- ./vuln/bufovfl_w_check
   ($ ./pin -t ./obj-Intel64/icount.so -- ./vuln/bufovfl_w_check ## because I symlink it here. ) 



## To write a new plugin




```
$ cp -r $(PIN_ROOT)/source/tools/MyPinTools AnyDirectoryYouWant
$ vi AnyDirectoryYouWant/makefile.rules
...
 TOOL_ : MyNewTool
...
$ vi MyNewTool.cpp
...

#include "pin.H"

$ make PIN_ROOT=${PIN_ROOT} 
$ # The target will be at AnyDirectoryYouWant/obj-intel64/MyNewTools.so
```



## To just type 'make' instead of 'make PIN_ROOT=${PIN_ROOT} MyNewTools.so'


```
$ cd ${PIN_ROOT}
$ ./setup
## setting up environment variable ${PIN_ROOT} in ~/.bashrc
## setting up    makefile config   $(PIN_ROOT) in ${PIN_ROOT}/source/tools/Config/makefile.config
$ cd <path to AnyDirectoryYouWant>
$ make                                ## : )
```



## Files I created

-- ./DTA/*
-- ./README
-- ./setup.sh
-- ./bin/vuln/*              == vulnerable binaries
-- ./bin/test/*              == some c tests for learning assembly

