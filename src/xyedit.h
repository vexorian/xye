/*
Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/
#include "vxsdl.h"
#ifndef FONTINCLUDED
   #include "font.h"
#endif
#include<string>
#ifndef WINDOWINCLUDED
    #include "window.h"
#endif
#include "dialogs.h"

using namespace std;


string stripXML(const string h);

class button;

struct editorbuttons;
struct editorboard;


#define MAXCONTROLS 100

enum contenttype
{
    CONTENT_NOCONTENT,
    CONTENT_VARIATION,
    CONTENT_MAKEROUND,

    CONTENT_RECOLOR,
    CONTENT_DIRECTION,
    CONTENT_CHANGEOBJECT
};

enum editorobjecttype
{
    EDOT_XYE,
    EDOT_WALL,
    EDOT_GEM,

    EDOT_BLOCK,

    EDOT_EARTH,
    EDOT_GEMBLOCK,
    EDOT_MAGNET,
    EDOT_PUSHER,
    EDOT_ARROWMAKER,
    EDOT_HAZARD,
    EDOT_ONEDIRECTION,
    EDOT_BEAST,
    EDOT_TELEPORT,
    EDOT_TURNER,
    EDOT_COLORSYSTEM,
    EDOT_KEYSYSTEM,
    EDOT_BOT,
    EDOT_NUMBER,
    EDOT_SPECIALBLOCKS,
    EDOT_FIREPAD,
    EDOT_RATTLERHEAD,
    EDOT_FOOD,
    
    EDOT_LARGEBLOCK,
    EDOT_PORTAL,
    
    EDOT_COLORFACTORY,
    EDOT_DANGERFACTORY,
    
    EDOT_HINT,
    
    EDOT_ERROR,

    EDOT_NONE
};

enum editorcolor
{
    EDCO_RED=1,
    EDCO_GREEN=3,
    EDCO_BLUE=2,
    EDCO_YELLOW=0,
    EDCO_WHITE=4,
    EDCO_METAL = 5,
    EDCO_WILD = 6
    
};

struct TiXmlElement;
struct boardelement;

class editor
{
  private:

     static button * savebutton;
     static button * solutionbutton;
     
     static bool ExitPrompt;
     static string loadError;

     static void loop();




     static void onExitAttempt();
     static void onQuitClick(const buttondata* data);
     static void beforeDraw();
     static void onKeyDown(SDLKey keysim,Uint16 unicode);
     static void onKeyUp(SDLKey keysim,Uint16 unicode);

     static void cancel();
     static void cancel(const buttondata* data) { cancel(); }
     
     static window* editorwindow;
     
     static void onExitWithoutSavingClick(bool yes);
     static void onBrowseWithoutSavingClick(bool yes);
     
     static enum setText_state_enum
     {
          ASSIGN_TITLE=0,
          ASSIGN_DESCRIPTION=1,
          ASSIGN_AUTHOR=2,
          ASSIGN_HINT=3,
          ASSIGN_BYE=4,
          ASSIGN_LEVEL_TITLE=5
          
     } setText_state;
     static void continueSetText(bool okclicked, const string text, inputDialogData * dat);
     static void beginSetText(const buttondata* data);
     
     static void continueAppendFile(bool okclicked, const string text, inputDialogData * dat);
     static void beginAppendFile(const buttondata* data);

     static void continueChangeLevelNumber(bool okclicked, const string text, inputDialogData * dat);
     static void beginChangeLevelNumber(const buttondata*data);
     
     
     static void onClearConfirmation(bool yes);
     static void onClearClick(const buttondata* data);
     static void onPreviousLevelClick(const buttondata* data);
     static void onNextLevelClick(const buttondata* data);
     static void onInsertLevelClick(const buttondata* data);
     static void onEraseLevelClick(const buttondata* data);
     static void onEraseLevelConfirmation(bool yes);
     
     static void saveAs(bool okclicked, const string text, inputDialogData * dat);
     
     static void updateCountRelated();
     
     static bool appendLevels(const string filename); //implemented in editorload.cpp
     static bool load_kye(); //implemented in editorload.cpp
     static bool load_KyeFormat(TiXmlElement * el); //implemented in editorload.cpp

     static void continueAskHint(bool okclicked, const string text, inputDialogData * dat);
 public:

     static string filename;
     static string filename_name;
     static string filename_path;
     
     static string myLevelsPath;

    static editorobjecttype SelectedType();
    static bool SavedFile;
    static void SetFile(const string &path, const string &file);
    static bool save(); //implemented in editorsave.cpp
    static bool save(const string &target, bool onlyOneLevel = false);
    static void buttonSave(const buttondata* data) { save(); }
    static void onSaveAsClick(const buttondata* data);
    static void onBrowseClick(const buttondata* data);
    
    static bool load(); //implemented in editorload.cpp
    
    static void SendSolution(const char* sol);

    static void test(bool solution);
    
    static void test();
    static void playSolution(const buttondata* data=NULL);

    static void test(const buttondata* data) { test(); }



    static Font* FontRes;

    static LuminositySprites sprites;

    static int Width;
    static int Height;
    static int GRIDSIZE;
    static void Error(const char* msg);

    static editorbuttons* buttons;
    static editorboard *  board;
    static unsigned int tic4;
    static unsigned int subtic4;

    static void StartSection(window* wind)  ;
    static void ResumeSection(window* wind)  ;
    static void ResumeSectionAndQuit(window* wind)  ;
    
    static void askHint(boardelement* o);
    
    
};


struct singleobject
{
    contenttype      content;
    int              variation;
    int              direction;
    editorobjecttype type;
    bool selected;
    bool flash;
    bool round;
    editorcolor color;
};

struct boardobject
{
    bool  empty;
    int              variation;
    editorobjecttype type;
    editorcolor color;
    string hint;
};
#define EDITORDIRECTION_RIGHT 0
#define EDITORDIRECTION_UP    1
#define EDITORDIRECTION_LEFT  2
#define EDITORDIRECTION_DOWN  3


void drawObjectBySpecs( SDL_Surface * target, int x, int y, editorobjecttype ot, editorcolor color, bool round, int variation, int direction);

#define EDITORBUTTONS_COUNTX 26
#define EDITORBUTTONS_COUNTY 3
class editorbuttons: public control
{
 private:
    int lastclickedx,lastclickedy;

    singleobject buttons[EDITORBUTTONS_COUNTX][EDITORBUTTONS_COUNTY];
    singleobject * getbuttonbyxy(int x, int y);
    int detectRotation(int x0, int y0, int x1, int y1);


    void extendButtons(editorobjecttype ot, editorcolor color, bool round, int variation);
    void switchToObject( editorobjecttype ot, editorcolor color, bool round, int variation, int direction);
    void updateText( editorobjecttype ot, editorcolor color, bool round, int variation, int direction);
    void handleClick(singleobject* target);

    string text;
    bool saved;

 public:



    bool clicked;


    bool mouse;
    int mousex,mousey;

    singleobject * hover;
    singleobject * clickedempty;
    singleobject * selection;
    singleobject * clickedobject;


    bool Eraser;
    editorobjecttype SelectedObjectType;
    editorcolor      SelectedColor;
    int              SelectedVariation;
    int              SelectedDirection;
    bool             SelectedRound;

    editorbuttons(int sx, int sy, int sw, int sh);

    void draw(SDL_Surface* target);
    void loop() {}
    void onMouseMove(int px,int py);
    void onMouseOut();
    void onMouseDown(int px,int py);
    void onMouseUp(int px,int py);
    inline void onMouseRightUp(int px,int py) {}
    void drawbutton(SDL_Surface* target,singleobject &o, int x, int y);

    void setInfo(const string& msg);

    static editorbuttons copy;
    void SaveCopy();
    void LoadCopy();
};

struct boardelement
{
    editorobjecttype type;
    editorcolor      color;
    int              variation;
    bool             round;
    int              direction;
    Uint8 r1mem,r3mem,r7mem,r9mem;
    int              parentx;
    int              parenty;
    string           hint;
    
    boardelement() { parentx=parenty = -1; type=EDOT_NONE; };

};

#ifndef XYE_HORZ
    #define XYE_HORZ 30
    #define XYE_VERT 20
#endif

struct DefaultColorData {
    bool useDefault;
    SDL_Color color;
};
const int TOTAL_EDITOR_COLOR_OPTIONS = 1;
const int EDITOR_COLOR_WALLS = 0;

class editorboard: public control
{
 private:
    void applyFromButtons(int x, int y);
    
    void drawWallInBoard(SDL_Surface*target,int ox,int oy, int x,int y, int variation, bool round);
    void drawLargeBlockInBoard(SDL_Surface * target, int ox,int oy, int x, int y, editorcolor color, int variation, int direction);
    void enforceUniquePortals(int x, int y, int variation, editorcolor color);

    void assign(editorboard* other);
    static vector<editorboard> levelList;
    static int                 currentLevel;
    bool findWall(int x, int y, int variation);
    bool wallContainsRoundCorners(int x, int y);
 public:
    DefaultColorData colors[TOTAL_EDITOR_COLOR_OPTIONS];
    bool clicked;
    bool mouse;
    int mousex,mousey;

    boardelement objects[XYE_HORZ ][XYE_VERT];
    int xye_x;
    int xye_y;
    int portal_x[5][2];
    int portal_y[5][2];
    
    
    
      
    string title;
    string hint;
    
    static string filetitle;
    static string description;
    static string author;
    string bye;
    string solution;


    editorboard(int sx, int sy);
    editorboard();

    void updateWallMem(int ox, int oy);
    
    void draw(SDL_Surface* target);
    void loop() {}
    void onMouseMove(int px,int py);
    void onMouseOut();
    void onMouseDown(int px,int py);
    void onMouseUp(int px,int py);
    void onMouseRightUp(int px,int py);
    
    void makeDefaultLevel();
    
    
    static void SaveCopy(editorboard* ed);
    static void SetCopySolution(const char* sol);
    static void LoadCopy(editorboard* ed);
    
    static int  CountLevels();
    static int  CurrentLevelNumber();
    static void SaveAtLevelNumber(editorboard* ed, int num);
    static void ResetLevels(bool empty = false);
    static void LoadLevelNumber(editorboard* ed, int num);
    static void DeleteLevel(editorboard * ed);
    static void CreateLevel(editorboard * ed);
    static void MoveToLevelNumber(editorboard* ed, int x);



};

Uint8 getLargeBlockFlagsByVarDir( int variation, int direction);
Uint32 getHiddenWayFlagsByVariationAndDir(int variation, int direction);
