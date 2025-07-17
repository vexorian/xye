/*
There is a terrible bug when compiling tinyxml with STL support in combination of some compilers.

#ifndef TIXML_USE_STL
    #define TIXML_USE_STL
#endif
*/

#ifndef PALETTE_BASE_SIZE
    #define PALETTE_BASE_SIZE 20
#endif

#include "tinyxml/xye_tinyxml.h"
#include<string>

using std::string;


void TempMap();

class LevelPack
{
 private:
     static TiXmlDocument* Doc;
     static TiXmlElement* pack;
     static TiXmlElement* CurrentLevel;
     static TiXmlElement* FirstLevel;
     static void LoadInformation();

     static void LoadFirstLevel();
     static void LoadNthLevel(unsigned int n);
     static bool defmode;
     static bool kyemode;
     static bool xsbmode;

     static bool HasPrevious();
     static bool HasNext();

     LevelPack() {}
 public:
    static bool FromEditor;
    static void Init();
    static void Clean();
    static void Default(const char* msg = NULL);
    static void Error(const char * msg);

    static string CurrentLevelTitle;
    static unsigned int n;
    static string Author;
    static string Name;
    static string CurrentLevelBye;
    static string LevelError;

    static string OpenFile;
    static unsigned int OpenFileLn;

    static bool HasSolution();
    static string Solution;

    static void SetLevelBye(const char* bye);
    static string Desc;

    static void Load(const char *filename, unsigned int ln=0, const string replay="");
    //static void Load(const char *filename, unsigned int ln);
    //static void Load(const char *filename);
    static void Restart();
    static void Next();
    static void Last();
    static bool GetFileData(const char* filename, string &au, string &ds, string &ti, unsigned int &leveln);

    static bool AllowUndo();
};

void LoadLevel(TiXmlElement* level);
struct KyeLevel;
bool LoadKyeFormatTag(TiXmlElement* kft, KyeLevel* out);
bool LoadKyeFormat(TiXmlElement* kf);



class Level
{
 private:
    Level() {}
 public:

    static unsigned int n;
    static string Author;
    static string Name;
    static string Title;

    static void Reset();


};


enum palette_mode
{
    PM_MULTIPLY,
    PM_RECOLOR
};
struct colorentry
{
    unsigned int id;
    unsigned int color;
    Uint8 R;
    Uint8 G;
    Uint8 B;
    palette_mode pm;
    colorentry* next;
};

class palette
{
 private:
    static colorentry* ls[PALETTE_BASE_SIZE];
    static bool ready;
    static colorentry* GetEntry(int id, bool create=false);
    palette() {}
 public:
    static void SetColor(int id, unsigned int color, palette_mode pm=PM_MULTIPLY);
    static void SetColor(int id, int r, int g, int b, palette_mode pm=PM_MULTIPLY);
    static unsigned int GetColor(int id);
    static void GetColor(int id, Uint8 &R, Uint8 &G,Uint8 &B);
    static void GetColor(int id, Uint8 &R, Uint8 &G,Uint8 &B, palette_mode& pm);
    static void Clear();
};


blockcolor GetElementBlockColor(TiXmlElement* el,blockcolor def=B_YELLOW);
edir GetElementDir(TiXmlElement* el,edir def=D_LEFT,const char* tag="dir");
