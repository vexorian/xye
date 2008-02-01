#include<string>
using std::string;

//running other executables in C/++ is not very portable, mostly because of windows' implementation of
//system not matching the one you can find in *nix based systems. This component handles

namespace Command
{
    void executeWaitForProcess(string command);
    void executeParallel(string command);
}
