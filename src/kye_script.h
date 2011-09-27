#include <string>
#ifndef KYE_SCRIPT
#define KYE_SCRIPT

class KyeLevel
{
 public:
 std::string name;
 std::string lhint;
 std::string bye;
 bool FromXyeLevel;
 bool FoundKye;

 char data[XYE_HORZ][XYE_VERT];
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
       
       static string LoadService(const char* filename, unsigned int ln=1);
       

    public:
        static void Init();
        static void Clean();
        
        static string LoadForEditor(const char* filename, vector<KyeLevel> &out);
        
        static void Load(const char* filename, unsigned int ln=1);
        static const char* ReadData(const char* path,unsigned int &n );
        static void LoadNthLevel(unsigned int n);
        static void Restart();
        static void Next();
        static void Last();
        static bool HasNext();
        static bool HasLast();

};


#endif
