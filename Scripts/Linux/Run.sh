#!/bin/bash
./Generate.sh gmake2
cd ../../

if [[ "$1" == "release" ]] ; then
    make config=release
    #./bin/Release-linux-x86_64/EnigmaEditor/EnigmaEditor
    ./bin/Release-linux-x86_64/SpaceSim/SpaceSim
else
    make config=debug
    #./bin/Debug-linux-x86_64/EnigmaEditor/EnigmaEditor
    ./bin/Debug-linux-x86_64/SpaceSim/SpaceSim
fi