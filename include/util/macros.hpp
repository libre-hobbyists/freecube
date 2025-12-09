#pragma once

#if !defined(_WIN32) || !defined(_WIN64)
    #define min(a,b) (((a) < (b)) ? (a) : (b))
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif