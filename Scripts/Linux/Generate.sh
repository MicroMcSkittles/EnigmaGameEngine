#!/bin/bash
cd ../../

if [[ "$1" == "" ]] ; then
    echo ""
    echo "Enter 'Generate.sh action' where action is one of the following:"
    echo ""
    echo "   clean           Remove all binaries and intermediate binaries."
    echo "   clean_make      Remove all binaries and intermediate binaries, and make files."
    echo "   codelite        Generate CodeLite project files"
    echo "   gmake2          Generate GNU makefiles for linux"
    echo ""

else
    Vendor/premake/Linux/premake5 $1
    if [[ "$2" == "" ]] ; then
        echo ""
        echo "Please specify the configuration."
        echo ""
        echo "    debug    Includes debug symbols, includes debug tools"
        echo "    release  Optimized, faster, harder to debug"
        echo ""
    else
        if [[ "$2" == "release" ]] ; then
            make config=release
        else
            make config=debug
        fi
    fi
fi
