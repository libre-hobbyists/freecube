#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Windows.h>
    #include <shellapi.h>
#endif

#include "util/log.hpp"
#include "util/macros.hpp"
#include "loader/loader.hpp"
#include "dol/dol_loader.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

int main(int argc, char **argv) {
    using namespace freecube::ISOLoader;
    using namespace freecube::dol;

    LOG_INFO("FreeCube is starting...");
    LOG_INFO("Built at: ", FREECUBE_CMAKE_BUILD_TIME);
    LOG_INFO("Target: ", FREECUBE_CMAKE_SYSTEM_TARGET, " (", FREECUBE_CMAKE_ARCH_TARGET, ")");


    std::string iso_path;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.rfind("--iso=", 0) == 0) {
            iso_path = arg.substr(6);
        } else if (arg == "--iso" && i + 1 < argc) {
            // Next arg is the path (it has to be)
            iso_path = argv[++i];
        }
    }

    if (iso_path.empty()) {
        LOG_CRITICAL("No ISO file specified!");
        LOG_INFO("Use: freecube --iso=\"path/to/data.iso\"");
        return -1;
    }

    ISOImage iso(iso_path);

    // Basic DOL header info 
    const auto dol_data = iso.get_dol();
    LOG_INFO("DOL Size: ", dol_data.size());

    std::string hex_dump;
    for (size_t i = 0; i < min(size_t(32), dol_data.size()); i++) {
        char buf[4];
        snprintf(buf, sizeof(buf), "%02X ", dol_data[i]);
        hex_dump += buf;
    }
    LOG_INFO("DOL Header (32bytes): ", hex_dump);

    // Begin parsing actual header data
    try {
        DOLLoader dol(dol_data);
        const auto& image = dol.image();
        
        LOG_INFO("DOL parsed successfully!");
        
        char ep_buf[32];
        snprintf(ep_buf, sizeof(ep_buf), "0x%08X", image.entry_point);
        LOG_INFO("Entry point: ", ep_buf);
        
        char bss_buf[128];
        snprintf(bss_buf, sizeof(bss_buf), "0x%08X - 0x%08X (size: 0x%X)", 
                 image.bss_address, 
                 image.bss_address + image.bss_size,
                 image.bss_size);
        LOG_INFO("BSS: ", bss_buf);
        
        // Log text sections
        for (size_t i = 0; i < image.text.size(); i++) {
            if (image.text[i].size > 0) {
                char text_buf[64];
                snprintf(text_buf, sizeof(text_buf), "Text[%zu]: 0x%08X (size: 0x%X)", 
                         i, image.text[i].load_address, image.text[i].size);
                LOG_DEBUG(text_buf);
            }
        }
        
        // Log data sections
        for (size_t i = 0; i < image.data.size(); i++) {
            if (image.data[i].size > 0) {
                char data_buf[64];
                snprintf(data_buf, sizeof(data_buf), "Data[%zu]: 0x%08X (size: 0x%X)", 
                         i, image.data[i].load_address, image.data[i].size);
                LOG_DEBUG(data_buf);
            }
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse DOL: ", e.what());
        return -1;
    }

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