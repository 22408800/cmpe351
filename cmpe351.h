#include <unistd.h>

#if !defined(_POSIX_VERSION)
    #error "Non-POSIX compliant OS environment detected. Please compile your code in Linux,MacOS or BSD enviroment"
#endif
