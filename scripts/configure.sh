#!/usr/bin/env bash
echo "<<< CDIT-MA re Configure >>>"

#Get the location of the script.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
RE_PATH=$SCRIPT_DIR/../
RE_LIB_PATH=$RE_PATH/lib/

if [ "$(uname)" == "Darwin" ]; then
    PUGIXML_ROOT="/Users/dan/pugixml-1.8/"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    PUGIXML_ROOT="/home/dig/Downloads/pugixml-1.8/"
fi

echo "Library Path: $RE_LIB_PATH"
echo "PugiXML Path: $PUGIXML_ROOT"

#Set library path to include the re_core location
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$RE_LIB_PATH
DYLD_FALLBACK_LIBRARY_PATH=${DYLD_FALLBACK_LIBRARY_PATH}:$RE_LIB_PATH
export RE_PATH PUGIXML_ROOT LD_LIBRARY_PATH DYLD_FALLBACK_LIBRARY_PATH
