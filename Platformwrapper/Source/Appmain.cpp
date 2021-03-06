/*
    Initial author: Convery (tcn@ayria.se)
    Started: 2019-04-09
    License: MIT
*/

#include "Common/Common.hpp"
#include "Stdinclude.hpp"

// Keep the global state together.
namespace Ayria { Globalstate_t Global{}; }

// Exported from the various platforms.
extern void Arclight_init();
extern void Tencent_init();
extern void Steam_init();

// Entrypoint when loaded as a shared library.
#if defined _WIN32
BOOLEAN __stdcall DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
        // Ensure that Ayrias default directories exist.
        std::filesystem::create_directories("./Ayria/Logs");
        std::filesystem::create_directories("./Ayria/Assets");
        std::filesystem::create_directories("./Ayria/Plugins");

        // Only keep a log for this session.
        Logging::Clearlog();

        // Opt out of further notifications.
        DisableThreadLibraryCalls(hDllHandle);

        // Although useless for most users, we create a console if we get a switch.
        #if defined(NDEBUG)
        if (std::strstr(GetCommandLineA(), "-devcon"))
        #endif
        {
            AllocConsole();
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
            SetConsoleTitleA("Platformwrapper Console");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
        }

        // Start tracking availability.
        Ayria::Global.Startuptimestamp = time(NULL);

        // If there's a local bootstrap module, we'll load it and trigger TLS.
        if(LoadLibraryA("./Ayria/Bootstrapper64d.dll") || LoadLibraryA("./Ayria/Bootstrapper32d.dll"))
            std::thread([]() { volatile bool NOP{}; (void)NOP; }).detach();
    }

    return TRUE;
}
#else
__attribute__((constructor)) void __stdcall DllMain()
{
    // Ensure that Ayrias default directories exist.
    std::filesystem::create_directories("./Ayria/Logs");
    std::filesystem::create_directories("./Ayria/Assets");
    std::filesystem::create_directories("./Ayria/Plugins");

    // Only keep a log for this session.
    Logging::Clearlog();
}
#endif

//Callback when loaded as a plugin.
extern "C" EXPORT_ATTR void __cdecl onStartup(bool)
{
    // Initialize the various platforms.
    Tencent_init();
    Steam_init();
}
