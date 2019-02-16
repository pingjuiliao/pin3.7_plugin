#!/bin/bash

if [[ $PIN_ROOT == "" ]] ; then

    # env
    echo "export PIN_ROOT=$(pwd)" >> ${HOME}/.bashrc
    # makefile config
    echo "PIN_ROOT := $(pwd)" >> $(pwd)/source/tools/Config/makefile.config
    echo "####################################"
    echo "###  Environment exported !"
    echo "###  Please source Your bash file."
    echo "####################################"
    echo
    echo "i.e.  $ source ~/.bashrc"

fi
