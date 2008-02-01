#include <string>

class KyeLevel
{
 public:
 std::string name;
 std::string lhint;
 std::string bye;
 bool FromXyeLevel;
 bool FoundKye;

 char data[30][20];
 KyeLevel* Prev;
 KyeLevel* Next;

 KyeLevel() { FromXyeLevel=FoundKye=false; }
 KyeLevel(bool fx) { FromXyeLevel=fx; FoundKye=false; }

 void SetGameCaption();
 void Load();

};

class KyeLevelPack
{
    private:
       KyeLevelPack() {}
       static KyeLevel* First;
       static KyeLevel* Final;
       static KyeLevel* CurrentLevel;
       static unsigned int tn;

    public:
        static void Init();
        static void Clean();
        static void Load(const char* filename, unsigned int ln=1);
        static const char* ReadData(const char* path,unsigned int &n );
        static void LoadNthLevel(unsigned int n);
        static void Restart();
        static void Next();
        static void Last();
        static bool HasNext();
        static bool HasLast();

};


