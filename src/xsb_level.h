#include<string>

class XsbLevel;

class XsbLevelPack
{
    private:
       XsbLevelPack() {}
       static XsbLevel* First;
       static XsbLevel* Final;
       static XsbLevel* CurrentLevel;
       static unsigned int tn;

       static const char* ReadDataSLC(const char* path,unsigned int &n, string&author, string &description, string & title);
       static void LoadSLC(const char* filename, unsigned int ln=1);

    public:
        static void Init();
        static void Clean();
        static void Load(const char* filename, unsigned int ln=1);
        static const char* ReadData(const char* path,unsigned int &n, string&author, string &description, string & title);
        static void LoadNthLevel(unsigned int n);
        static void Restart();
        static void Next();
        static void Last();
        static bool HasNext();
        static bool HasLast();

};
