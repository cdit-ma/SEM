SET(Proton_INCLUDE_DIRS
$ENV{QPID_ROOT}/include/proton
)

FIND_LIBRARY(Proton_CPP_LIBRARY
qpid-proton-cpp
PATHS
$ENV{QPID_ROOT}/lib/
)

FIND_LIBRARY(Proton_LIBRARY
qpid-proton
PATHS
$ENV{QPID_ROOT}/lib/
)

SET(Proton_LIBRARIES
${Proton_LIBRARY} 
${Proton_CPP_LIBRARY} 
dl 
pthread
)