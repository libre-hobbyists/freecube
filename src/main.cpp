#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Windows.h>

    int main();

    int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
        main();
    }
#endif

#include "util/log.hpp"
#include "loader/loader.hpp"
#include <fstream>
#include <iostream>

int main() {
    LOG_DEBUG("System starting up...");

    freecube::ISOLoader::ISOImage iso("loz_ww.iso");

    const uint8_t *boot = iso.data().data();

    std::cout << boot << std::endl;
    return 0;
}