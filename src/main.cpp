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
#include "dol/dol_loader.hpp"
#include <fstream>
#include <iostream>

int main() {
    using namespace freecube::ISOLoader;

    ISOImage iso("loz_ww.iso");

    auto dol = iso.extract_file("main.dol");
    iso.dump_fst();

    if (!dol) {
        LOG_CRITICAL("main.dol not found!");
        return -1;
    }

    freecube::dol::DOLLoader loader(*dol);

    LOG_INFO("Loaded DOL entrypoint => ", loader.image().entry_point);

    return 0;
}