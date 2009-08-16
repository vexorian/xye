#include "tinyxml/xye_tinyxml.h"
#include "vxsdl.h"

using std::string;

class options
{
 private:
    static bool bini;


//    static TiXmlDocument* cnf;
//    static TiXmlElement* ele;
    static string LevelFile;
    static char* Texture;
    static char* LuminosityTexture;
    static int   FontSize;
    static int   FontBoldSize;
    static char* Font;
    static char* FontBold;
    static bool  enundo;


    static int GridSize;
    static unsigned int lvnum;
    static bool Error(const char* msg);
    static unsigned char r,g,b;
    static void Default();
    static void LoadColors(TiXmlElement* skn);
    static TiXmlElement* GetOptionsElement(TiXmlDocument* cnf);
    static TiXmlDocument* getxyeconf(TiXmlElement *&options  );
    static TiXmlDocument* defaultxyeconf(const char* path,TiXmlElement *&options);
    static string homefolder;
    static void LoadLevelFile();
    static void PerformLevelFileSave();

 public:
    static void IgnoreLevelSave();
    static bool  xyeDirectionSprites;
    static string Dir;
    options() {}
    static string GetDir();
    static const string& GetHomeFolder();
    static string* fixpath(string& path,bool dohomecheck);
    static string* fixpath(string& path) { return fixpath(path,false); }
    static char* fixpath(const char * path,bool dohomecheck);
    static char* fixpath(const char * path) { return fixpath(path,false); }
    static void Init();
    static void Clean();
    static const char* GetLevelFile();
    static unsigned int GetLevelNumber( const char* levelfile);
    
    
    static const char* GetSpriteFile();
    static const char* GetLuminositySpriteFile();

    static int GetFontSize();
    static int GetFontBoldSize();
    static const char* GetFontFile();
    static const char* GetFontBoldFile();
    static int GetGridSize();
    
    
    static void SaveLevelFile(const char* filename, int levelNumber = 0);
    
    
    static unsigned char Red();
    static unsigned char Green();
    static unsigned char Blue();

    inline static bool UndoEnabled() {return enundo; }

    static SDL_Color WallColor;
    static SDL_Color WallSpriteColor;
    
    static SDL_Color BFColor[4];
    static SDL_Color BKColor[4];

    static SDL_Color LevelMenu_info;
    static SDL_Color LevelMenu_selected;
    static SDL_Color LevelMenu_selectederror;
    static SDL_Color LevelMenu_menu;
    static SDL_Color LevelMenu_menutext;
    static SDL_Color LevelMenu_selectedtext;   
    static SDL_Color LevelMenu_infotext;

    static string ExecutablePath;
    

};
