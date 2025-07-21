#pragma once

#include "tinyxml/xye_tinyxml.h"
#include "vxsdl.h"

using std::string;

const int XYE_WALL_VARIATIONS = 6;

namespace options {

    struct SkinInformation
    {
        int    pw,ph;
        int    dimx,dimy;
        string title;
        string author;
        string description;
        SDL_Surface* preview;
    };

    void IgnoreLevelSave();
    string GetDir();
    const string& GetDataHomeFolder();
    const string& GetConfigHomeFolder();
    string GetLevelsHomeFolder(); // ~/.local/share/xye/levels or nothing
    string GetResHomeFolder(); // ~/.local/share/xye/levels or nothing
    string GetMyLevelsFolder(); //~/.local/share/xye/levels or mylevels

    string  fixpath(const string path, bool dohomecheck=false);
    void Init();
    void Clean();
    const char* GetLevelFile();
    unsigned int GetLevelNumber( const char* levelfile);


    const char* GetSpriteFile();
    const char* GetLuminositySpriteFile();
    const string GetWindowIconFile();

    int GetFontSize();
    int GetFontBoldSize();
    const char* GetFontFile();
    const char* GetFontBoldFile();
    int GetGridSize();


    void SaveLevelFile(string filename, int levelNumber = 0);
    void SaveLevelGame(string filename, int levelNumber, string moves);

    string LoadLevelGame(string filename, int levelNumber);
    void ForgetLevelGame(string filename, int levelNumber);


    unsigned char Red();
    unsigned char Green();
    unsigned char Blue();

    bool LevelColorsDisabled();
    bool UndoEnabled();
    bool HasConsciouslyChosenTheme();

    extern bool  xyeDirectionSprites;
    extern string Dir;

    extern SDL_Color OneWayDoorColor;
    extern SDL_Color ForceArrowColor;
    extern SDL_Color EarthColor;
    extern SDL_Color FloorColor;
    extern SDL_Color WallColor      [XYE_WALL_VARIATIONS];
    extern SDL_Color WallSpriteColor[XYE_WALL_VARIATIONS];

    extern SDL_Color BFColor[6];
    extern SDL_Color BKColor[6];

    extern SDL_Color HintColor;

    extern SDL_Color LevelMenu_info;
    extern SDL_Color LevelMenu_selected;
    extern SDL_Color LevelMenu_selectederror;
    extern SDL_Color LevelMenu_menu;
    extern SDL_Color LevelMenu_menutext;
    extern SDL_Color LevelMenu_selectedtext;
    extern SDL_Color LevelMenu_infotext;


    extern string ExecutablePath;

    bool GetSkinInformation(const char* file, SkinInformation & si);
    void ChangeSkinFile(const char* file, bool enableLevelColors);

    string GetSkinFile(bool stripPath = true);
}
