#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Windows.h>
#endif

#include "util/log.hpp"
#include "loader/loader.hpp"
#include "dol/dol_loader.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

int main(int argc, char **argv) {
    using namespace freecube::ISOLoader;

    std::string iso_path;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.rfind("--iso=", 0) == 0) {
            iso_path = arg.substr(6);
        } else if (arg == "--iso" && i + 1 < argc) {
            // Next arg is the path
            iso_path = argv[++i];
        }
    }

    if (iso_path.empty()) {
        LOG_CRITICAL("No ISO file specified!");
        LOG_INFO("Use: freecube --iso=\"path/to/data.iso\"");
        return -1;
    }

    ISOImage iso(iso_path);

    return 0;
}

#if defined(_WIN32) || defined(_WIN64)
    // Safe conversion of Windows args to more nix-like args
    int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
        int argc;

        LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);

        char **argv = new char*[argc];
        for (int i = 0; i < argc; i++) {
            int size = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
            argv[i] = new char[size];
            WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, argv[i], size, nullptr, nullptr);
        }

        int result = main(argc, argv);

        // Cleanup our argv mess
        for (int i = 0; i < argc; i++) {
            delete[] argv[i];
        }
        delete[] argv;
        LocalFree(wargv);
        
        return result;
    }
#endif