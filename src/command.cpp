#include "command.h"


#ifdef _WIN32
    #include <windows.h>
    #include <cstdlib>
#endif

namespace Command
{
   #ifdef _WIN32
    void useCreateProcess(const string command, bool wait)
    {
        // Some variables initialization
        STARTUPINFO siStartupInfo;
        PROCESS_INFORMATION piProcessInfo;
        memset(&siStartupInfo, 0, sizeof(siStartupInfo));
        memset(&piProcessInfo, 0, sizeof(piProcessInfo));
        siStartupInfo.cb = sizeof(siStartupInfo);

        DWORD dwExitCode=0;

        // Run
        if (CreateProcess(NULL, (CHAR*)(command.c_str()), NULL, NULL, false,
                CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &siStartupInfo,&piProcessInfo) != false)
        {

           if(wait) do
           {
               GetExitCodeProcess(piProcessInfo.hProcess, &dwExitCode);
           } while (dwExitCode==STILL_ACTIVE);

        }

        // Release
        CloseHandle(piProcessInfo.hProcess);
        CloseHandle(piProcessInfo.hThread);
    }
   #endif





    void executeWaitForProcess(string command)
    {
        #ifdef _WIN32
             useCreateProcess(command,true);
        #else
             system(command.c_str());
        #endif
    }
    void executeParallel(string command)
    {
        #ifdef _WIN32
             useCreateProcess(command,false);
        #else

             system( (command+" &").c_str());
        #endif
    }
}
