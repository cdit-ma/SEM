echo "<<< CDIT-MA re Configure >>>"

#Get the location of the script.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
RE_PATH=$SCRIPT_DIR/..
RE_LIB_PATH=$RE_PATH/lib

echo "Library Path: $RE_LIB_PATH"

#Set library path to include the re_core location
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$RE_LIB_PATH
export LD_LIBRARY_PATH
