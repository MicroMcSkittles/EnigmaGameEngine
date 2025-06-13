#!/bin/bash
cd ../../

if [[ "$1" == "" ]] ; then
    echo ""
    echo "Enter 'build.sh action' where action is one of the following:"
    echo ""
    echo "   clean           Remove all binaries and intermediate binaries."
    echo "   clean_make      Remove all binaries and intermediate binaries, and make files."
    echo "   codelite        Generate CodeLite project files"
    echo "   gmake2          Generate GNU makefiles for linux"
    echo ""

else
    Vendor/premake/Linux/premake5 $1
fi
