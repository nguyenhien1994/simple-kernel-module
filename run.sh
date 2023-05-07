#!/bin/bash

MODULE_NAME=reverse_string

make

if [ -f module/${MODULE_NAME}.ko ]
then
    lsmod | grep ${MODULE_NAME} >& /dev/null
    if [ $? -eq 0 ]
    then
        sudo rmmod ${MODULE_NAME}
    fi

    sudo insmod module/${MODULE_NAME}.ko

    sudo ./test/test ./test/test.txt

    sudo rmmod ${MODULE_NAME}
else
    echo "Kernel module: ${MODULE_NAME} not found!"
fi

make clean
