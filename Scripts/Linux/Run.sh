#!/bin/bash
./Generate.sh gmake2
cd ../../

if [[ "$1" == "release" ]] ; then
    make config=release
    ./bin/Release-linux-x86_64/ExampleApp/ExampleApp
else
    make config=debug
    ./bin/Debug-linux-x86_64/ExampleApp/ExampleApp
fi