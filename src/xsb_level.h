#include <string>

class XsbLevel
{
 public:
 std::string name;

 char data[30][20];
 char w;
 char h;
 XsbLevel* Prev;
 XsbLevel* Next;

 XsbLevel() { Prev=Next=NULL; levelnum=1; }

 void SetGameCaption();
 void Load();
 unsigned int levelnum;
 static int gm;
 static int tx;
 static int ty;
 static blockcolor bc;
};

class XsbLevelPack
{
    private:
       XsbLevelPack() {}
       static XsbLevel* First;
       static XsbLevel* Final;
       static XsbLevel* CurrentLevel;
       static unsigned int tn;

    public:
        static void Init();
        static void Clean();
        static void Load(const char* filename, unsigned int ln=1);
        static const char* ReadData(const char* path,unsigned int &n);
        static void LoadNthLevel(unsigned int n);
        static void Restart();
        static void Next();
        static void Last();
        static bool HasNext();
        static bool HasLast();

};


