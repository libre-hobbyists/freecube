// Entrypoint
// SPDX-License-Identifier: GPL-3.0-only
//

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Windows.h>

    int main(int argc, char **argv);

    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
        return main();
    }
#endif

#include "util/log.hpp"

int main(int argc, char **argv) {
    LOG_TRACE("System starting up...");
}