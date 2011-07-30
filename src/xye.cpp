/*
Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/


#include "xye.h"

#include<cstdio>
#include<algorithm>
#include "xye_script.h"
#include "options.h"
#include "record.h"
#include "dialogs.h"
#include "xyedit.h"
#include "skins.h"


#define sz game::GRIDSIZE //typing game::GRIDSIZE is an annoyance



/*******class game**********/
//
//  Is game really a class? or is it just a pack of global functions and variables? */
// I did not use a namespace for this and many other things because namespaces are not
// friendly with headers. Edit in 2009: Are they?

//=======================================================================================================
//class game static variables:
//

//Color of Xye
SDL_Color game::PlayerColor;




//Restard signal
bool game::GameOver=false;
bool xye_fromeditortest = false;
bool xye_recordingsolution = false;
bool xye_playsolution = false;
bool game::FinishedLevel=false;

//The gridsize of the current skin, as float and as int
Sint16        game::GRIDSIZE;

//The path of the current skin:
string       game::SKIN;

//All the 'squares' of the game.
square       game::grid[XYE_HORZ][XYE_VERT];

//Count of the unique ids
unsigned int game::id_count;

//Buffer of the sprites file
LuminositySprites game::sprites;
SDL_Surface*      game::screen;

//Game tic counter:
unsigned int game::counter;


//Using different counters instead of repetitive % operations should speed up performance.
    char game::counter2;    char game::counter3;    char game::counter4;
    char game::counter5;    char game::counter6;    char game::counter7;
    char game::counter8;    char game::counter9;
unsigned int game::beastcounter;



//Last pressed key (if it is an arrow key)
int game::lastKey;

//mouse support:
bool game::mouse_pressed;
bool game::mouse_valid;
unsigned int game::mouse_x,game::mouse_y;

//recordings:
bool game::cameraon;
bool game::playingrec;
bool game::undo=false;


int game::GameWidth;
int game::GameHeight;

unsigned int game::LastXyeMove;
square* game::deathsq2;
unsigned char game::FastForward;
edir game::LastXyeDir;

square* game::deathsq1;
float game::NextTic;
xye* game::XYE;
bool game::started;

unsigned char game::FlashPos;


bool game::DK_PRESSED;
int  game::DK_PRESSED_FIRST;
bool game::DK_UP_PRESSED;
bool game::DK_DOWN_PRESSED;
bool game::DK_LEFT_PRESSED;
bool game::DK_RIGHT_PRESSED;

bool game::DK_GO;
edir game::DK_DIR;

bool game::UpdateAll;

bool game::ShiftPressed;
Font* game::FontRes;
Font* game::FontRes_Bold;
Font* game::FontRes_White;
signed int game::HintMarquee;
unsigned char game::HintRead;

bool game::CoordMode;
window* gamewindow = NULL;
gameboard* TheGameBoard = NULL;

button* Button_NextLevel;
button* Button_PrevLevel;
button* Button_Hint;
button* Button_Solution;
button* Button_Undo;
button* Button_RecordSolution;

bool Button_ToolTipWasDrawn;

//======================================================================
//gameboard methods:

gameboard::gameboard(int sx, int sy, int sw, int sh)
{
    x=sx;y=sy;w=sw;h=sh;
    depth=0;
}
void gameboard::loop()
{
    game::loop();
}
void gameboard::draw(SDL_Surface* target)
{
    game::draw(x,y);
}
void gameboard::onMouseMove(int x,int y)
{
    game::onMouseMove(x,y);
}
void gameboard::onMouseOut()
{
    game::onMouseOut();
}
void gameboard::onMouseDown(int x,int y)
{
    game::onMouseDown(x,y);
}
void gameboard::onMouseUp(int x,int y)
{
    game::onMouseUp(x,y);
}
gameboard::~gameboard()
{
    TheGameBoard = NULL;
}

//------------------------------------------------------------------------
class gamepanel : public control
{
    public:
        gamepanel( int sx, int sy, int sw, int sh)
        {
            x=sx, y=sy, w=sw, h=sh;
        }
        
        void loop() {}
        void draw(SDL_Surface* target)
        {
            game::DrawPanel(target, x,y,w,h);
        }
        void onMouseMove(int px,int py) {}
        void onMouseOut(){}
        void onMouseDown(int px,int py){}
        void onMouseUp(int px,int py){}
        void onMouseRightUp(int px,int py) {}


};



//=======================================================================================================
// Class game methods.
//

void game::InitGraphics()
{
    xye::useDirectionSprites = options::xyeDirectionSprites;

    PlayerColor.r=options::Red();
    PlayerColor.g=options::Green();
    PlayerColor.b=options::Blue();
    PlayerColor.unused=255;

     GRIDSIZE=options::GetGridSize();
     SKIN=options::GetSpriteFile();



    const char *tm=SKIN.c_str();
    printf("Loading %s\n",tm);
    sprites.sprites=IMG_Load(tm);
    {
        sprites.luminosity = NULL;
        tm = options::GetLuminositySpriteFile();
        if(tm!=NULL)
        {
            printf("Loading %s\n",tm);
            sprites.luminosity=IMG_Load(tm);
        }
    }
    if (  sprites.sprites == NULL)  game::Error( "Invalid/Missing Sprite File");

    //Init cache
    printf("Initializing Recolor cache...\n");
    RecolorCache::restart(sprites.sprites);
    for (int i=0;i<4;i++)
    {
        RecolorCache::savecolor(&options::BFColor[i]);
        RecolorCache::savecolor(&options::BKColor[i]);
    }
    for (int i=0; i<wall::MAX_VARIATIONS; i++)
        RecolorCache::savecolor(&options::WallColor[i]);

    RecolorCache::savecolor(&PlayerColor);
     
    SDL_Color c;
    SDL_Surface* SS;
    if(options::GetFontSize()) //if not 0 then we want a truetypefont.
    {
        FontRes=new Font(options::GetFontFile(),options::GetFontSize(), 0,0,0);
        FontRes_White=new Font(options::GetFontFile(),options::GetFontSize(), 255,255,255);

        LevelBrowser::SetupNormalFonts(options::GetFontFile(),options::GetFontSize() );
        SkinBrowser::SetupNormalFonts(options::GetFontFile(),options::GetFontSize() );
    }
    else //do the giberish
    {
        SS=SDL_LoadBMP(options::GetFontFile());
        if (! SS) Error("Invalid/Missing Font bmp");

        FontRes= new Font(SS,0,0,0);
        FontRes_White= new Font(SS,255,255,255);

        LevelBrowser::SetupNormalFonts(SS);
        SkinBrowser::SetupNormalFonts(SS);

        SDL_FreeSurface(SS);
    }
    if(options::GetFontSize()) //if not 0 then we want a truetypefont.
    {
        FontRes_Bold=new Font(options::GetFontBoldFile(),options::GetFontBoldSize(), 0,0,0);
        LevelBrowser::SetupBoldFonts(options::GetFontBoldFile(),options::GetFontBoldSize());
        SkinBrowser::SetupBoldFonts(options::GetFontBoldFile(),options::GetFontBoldSize());
    }
    else
    {
        SS=SDL_LoadBMP(options::GetFontBoldFile());
        if (! SS) Error("Invalid/Missing Font Bold bmp");

        FontRes_Bold= new Font(SS,0,0,0);


        LevelBrowser::SetupBoldFonts(SS);
        SkinBrowser::SetupBoldFonts(SS);

        SDL_FreeSurface(SS);
    }
    dialogs::FontResource=game::FontRes;
    dialogs::BackgroundColor = options::LevelMenu_info;
    dialogs::TextBoxColor = options::LevelMenu_menu;
    
    button::FontResource=FontRes;
    button::SourceSurface=sprites.sprites;
    button::LongTextureX=7;
    button::ShortTextureX=6;
    button::PressedTextureY=18;
    button::NormalTextureY=17;
    button::Size=sz;
    button::ToolTipColor = options::HintColor;
    
    editor::FontRes = game::FontRes;
    editor::sprites = game::sprites;
    editor::GRIDSIZE = game::GRIDSIZE;

    GameWidth=XYE_HORZ*GRIDSIZE+XYE_GAMEX+XYE_XTRA_X;
    GameHeight=XYE_VERT*GRIDSIZE+XYE_GAMEY+2*XYE_XTRA_Y+ GRIDSIZE+2+1+GRIDSIZE;
 
}

void game::CleanGraphics()
{
    delete FontRes ;
    delete FontRes_White ;
    delete FontRes_Bold;

    printf("cleaning recolor cache\n");
    RecolorCache::clean();

    printf("cleaning sprites\n");
    SDL_FreeSurface(sprites.sprites);
    if(sprites.luminosity==NULL) {
        SDL_FreeSurface(sprites.luminosity);
    }

}

void game::RefreshGraphics()
{
    CleanGraphics();
    game::InitGraphics();
    if(gamewindow != NULL) {
        gamewindow->Resize(GameWidth, GameHeight);
    }
}

//Setups the game, initialize variable and that stuff
int game::Init(const char* levelfile)
{
    gamewindow = NULL;
    printf("loading options...\n");
    options::Init();
    printf("initializing recorder...\n");
    recording::init();
    printf("initializing recycler...\n");
    recycle::init();

    printf("initializing level support...\n");
    LevelPack::Init();


    char i,j;
    int ix,iy;
    FastForward=false;
    started=false;
    
    Randomize();

    //Init SDL;
    printf("Initializing SDL...\n");
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)<0)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return(0);
    }

    #ifndef NOTRUETYPE
        printf("Initializing SDL_ttf\n");
        TTF_Init();
    #endif



    printf("Initializing Recolor Table...\n");
    Init_RecolorTable();



    game::InitGraphics();
    
    if (!window::InitSDL()) return 0;
    printf("Setting video mode...\n");
    gamewindow=window::create(GameWidth, GameHeight   ,"Xye");
    screen= gamewindow->getDrawingSurface();
     
    

    //<window icon> "Ahh, the horror!"
    printf("Initializing window icon...\n");
    SDL_Surface* icon=SDL_CreateRGBSurface(0,32,32,32,SDL_ENDIAN32MASKS);

    //D.Draw(sprites,4,4);
    //sz=48;
    if (sz>32)
    {
        SDL_FillRect(icon, 0,0,32,32, SDL_MapRGB(icon->format,0,0,0 ) );
        SDL_FillRect(icon, 2,2,28,28, SDL_MapRGB(icon->format,options::Red(),options::Green(),options::Blue()) );
    }
    else
    {
        Drawer D(sprites,0,0,sz,sz);
        D.SetColors(PlayerColor,255);
        Uint32          colorkey=SDL_MapRGB(icon->format,255,0,255);
        SDL_FillRect(icon, 0, colorkey );
        SDL_SetColorKey(icon, SDL_SRCCOLORKEY, colorkey);

        D.Draw(icon,(unsigned int)((32-sz)/2),(unsigned int)((32-sz)/2));
    }
    SDL_WM_SetIcon(icon,NULL)    ;
    
    SDL_FreeSurface(icon);
    //</window icon>

    //SDL_ShowCursor(SDL_DISABLE);
    DK_PRESSED=DK_UP_PRESSED=DK_DOWN_PRESSED=DK_LEFT_PRESSED=DK_RIGHT_PRESSED=DK_GO=false;
    DK_PRESSED_FIRST=0;
    DK_DIR=D_DOWN;



    const char* r=options::GetLevelFile();
    int ln = options::GetLevelNumber(r);

    if (r!=NULL)
    {
       LevelBrowser::AssignLevelFile(r, ln);
    }
    if ( levelfile == NULL)
    {
        gamewindow->SetTransition( LevelBrowser::StartSection );
    }
    else if (strcmp(levelfile, "/START_EDITOR/") == 0) //bad hack , BAD hack
    {
        gamewindow->SetTransition( editor::StartSection );
    }
    else
    {
        r = levelfile;
        game::InitLevelFile = r;
        game::InitLevelFileN = ln;
        gamewindow->SetTransition( game::InitGameSection );

    }
       

    //while (r[0]!='\0')
   // {
        
        //game::start();
        ln = 1;
        if (r!=NULL)
             ln= options::GetLevelNumber(r);
        /*
        if (! AppLoop()) break;
        if( levelfile != NULL) break;
        ln = 1;
        r= LevelBrowser::GetLevelFile();*/
    //}
        
    gamewindow->loop(XYE_FPS);

    printf("Cleaning level data...\n");
    LevelPack::Clean();


    printf("\ncleaning recording data\n");
    recording::clean();
    printf("cleaning options data\n");
    options::Clean();

    printf("cleaning fonts\n");
    //Delete things that have to be deleted
    game::CleanGraphics();

    LevelBrowser::Clean();
    SkinBrowser::Clean();


    printf("cleaning remaining objects\n");
    recycle::run();


    #ifndef NOTRUETYPE
        printf("Shutting down SDL_ttf\n");
        TTF_Quit();
    #endif


    printf("Shutting down SDL\n");
    delete gamewindow;
    window::QuitSDL();


    return (0);


}

void game::PlayLevel( const char *levelfile, int level)
{
    xye_fromeditortest = false;
    xye_recordingsolution = false;
    xye_playsolution = false;

    InitLevelFile = levelfile;
    InitLevelFileN= level;
    gamewindow->SetTransition(game::InitGameSection);
}

void game::TestLevel( const char *levelfile, int level, bool playsolution)
{
    xye_fromeditortest = true;
    xye_recordingsolution = false;
    xye_playsolution = playsolution;
    

    InitLevelFile = levelfile;
    InitLevelFileN= level;
    gamewindow->SetTransition(game::InitGameSection);
}


void game::Error(const char* msg)
{
    fprintf(stderr,"%s", msg);
    fprintf(stderr,"\n");
    throw (msg);
}


bool game::EvalDirGrid(Uint16 x1,Uint16 y1,Uint16 x2,Uint16 y2,edir &dir)
{
    Uint8 sz_2=(Uint8)(sz / 2);

    x1+=sz_2;
    y1+=sz_2;

    Sint16 dx=x1-x2;
    Sint16 dy=y1-y2;


    dx=dx<0?-dx:dx;
    dy=dy<0?-dy:dy;



    if ((dx<=sz_2) && (dy<=sz_2)) return false;

    if (dx<=sz_2)
    {
        if (y2>y1)
            dir=D_DOWN;
        else
            dir=D_UP;
        return true;
    }
    else if (dy<=sz_2)
    {
        if (x2>x1)
            dir=D_RIGHT;
        else
            dir=D_LEFT;
        return true;
    }
 return false;
}

bool game::EvalDirGrid(obj* object,Uint16 x2,Uint16 y2,edir &dir)
{
    square* sq= Square(object->X(),object->Y());
    return EvalDirGrid(sq->x - TheGameBoard->x,sq->y - TheGameBoard->y,x2,y2,dir);
}


bool game::EvalDirKeys()
{
    if (DK_LEFT_PRESSED)
    {
        DK_DIR=D_LEFT;
        return (true);
    }
    else if (DK_UP_PRESSED)
    {
        DK_DIR=D_UP;
        return (true);
    }
    else if (DK_RIGHT_PRESSED)
    {
        DK_DIR=D_RIGHT;
        return (true);
    }
    else if (DK_DOWN_PRESSED)
    {
        DK_DIR=D_DOWN;
        return (true);
    }
 return(false);
}
void game::PlayRecording(const string rc)
{
    recording::load(rc.c_str());
    playingrec=true;
}

void game::AfterLevelLoad()
{
    // load game?
    loadGame();

    if(!xye_fromeditortest)
        options::SaveLevelFile( LevelPack::OpenFile, LevelPack::OpenFileLn);
    Button_Solution->Enabled = ( (LevelPack::HasSolution()) && !playingrec);
    Button_Hint->Enabled= (hint::GlobalHintExists());
    Button_Hint->resetToggle();
    Button_NextLevel->Enabled = Button_PrevLevel->Enabled  = (LevelPack::n > 1);
    Button_Undo->Visible = (game::IsUndoAllowed() || xye_fromeditortest);
    Button_Undo->Enabled = game::IsUndoAllowed();
    Button_RecordSolution->Visible = xye_fromeditortest;
    Button_RecordSolution->Enabled = !xye_recordingsolution;
}

void game::RestartCommand( const buttondata*bd)
{
    if (!playingrec) SaveReplay();
    end();
    start();
    LevelPack::Restart();
    AfterLevelLoad();
}


void game::ExitCommandYesHandler( bool yesClicked ) {
    UpdateAll=true;
    if ( yesClicked ) {
        saveGame();
        gamewindow->Close();
    }
}
void game::ExitCommand( const buttondata*bd)
{
    if(xye_fromeditortest)
    {
        gamewindow->SetTransition(editor::ResumeSectionAndQuit);
    }
    else {
        dialogs::makeYesNoDialog(gamewindow, "Are you sure you want to exit the game? Your progress will be saved.","Yes", "No" , game::ExitCommandYesHandler);
    }
}

void game::loadGame()
{
    string s = options::LoadLevelGame(LevelPack::OpenFile.c_str(), LevelPack::OpenFileLn);
    if (s != "") {
        recording::load(s.c_str());
        options::ForgetLevelGame(LevelPack::OpenFile.c_str(), LevelPack::OpenFileLn);
        undo = true;
        string oldcap = LevelPack::CurrentLevelTitle;
        oldcap += " (Your previous game has been loaded.)";
        SDL_WM_SetCaption(oldcap.c_str(),0);
    }

}


void game::saveGame()
{
    char * tm = recording::save();
    string s = tm;
    options::SaveLevelGame(LevelPack::OpenFile.c_str(), LevelPack::OpenFileLn, s);
    delete[]tm;
}

void game::GoPreviousCommand( const buttondata*bd)
{
    saveGame();
    end();
    start();
    LevelPack::Last();
    AfterLevelLoad();
}

void game::GoNextCommand( const buttondata*bd)
{
    saveGame();
    end();
    start();
    LevelPack::Next();
    AfterLevelLoad();
}


void game::HintDownCommand( const buttondata*bd)
{
    hint::GlobalHint(true);
}

void game::HintUpCommand( const buttondata*bd)
{
    hint::GlobalHint(false);
}

void game::FFDownCommand( const buttondata*bd)
{
    FastForward=true;
}

void game::FFUpCommand( const buttondata*bd)
{
    FastForward=false;
    UpdateAll=true;
}

void game::UndoCommand( const buttondata*bd)
{
    if (! playingrec) Undo();
}

void game::RecordSolutionCommand( const buttondata*bd)
{
    xye_recordingsolution = true;
    game::RestartCommand(bd);
}


void game::BrowseCommand( const buttondata*bd)
{
    
    if( xye_fromeditortest) {
        gamewindow->SetTransition(editor::ResumeSection);
    } else {
        saveGame();
        gamewindow->SetTransition(LevelBrowser::StartSection);
    }
}

void game::SolutionCommand( const buttondata*bd)
{
    if (LevelPack::HasSolution())
    {
        end();
        start();
        LevelPack::Restart();
        recording::load(LevelPack::Solution.c_str() );
        playingrec=true;
        AfterLevelLoad();
    }
}

void game::onKeyDown(SDLKey keysim, Uint16 unicode)
{
    bool done=false;
    bool browse=false;

                switch (keysim)
                {
                    case (SDLK_c) :
                        CoordMode=true;
                        break;
                    case (SDLK_h) :
                        HintDownCommand();
                        break;
                        
                        break;

                    case (SDLK_ESCAPE):
                        ExitCommand();
                        break;

                    case (SDLK_DELETE):
                        UndoCommand();
                        break;
                    case(SDLK_RETURN): case(SDLK_KP_ENTER): //Enter
                        RestartCommand();

                        break;

                    case(SDLK_s): //s
                        SolutionCommand();
                        break;



                    case(SDLK_PLUS): case(SDLK_KP_PLUS): case(SDLK_n): //Plus - N
                        GoNextCommand();
                        break;

                    case(SDLK_MINUS): case(SDLK_KP_MINUS): case(SDLK_b): case(SDLK_p): //Minus - b - p
                        GoPreviousCommand();
                        break;


                    //Dir arrows handling key press:
                    case(SDLK_UP):
                        DK_UP_PRESSED=DK_PRESSED=DK_GO=true; DK_PRESSED_FIRST=0; DK_DIR=D_UP; break;
                    case(SDLK_DOWN):
                        DK_DOWN_PRESSED=DK_PRESSED=DK_GO=true; DK_PRESSED_FIRST=0; DK_DIR=D_DOWN; break;
                    case(SDLK_LEFT):
                        DK_LEFT_PRESSED=DK_PRESSED=DK_GO=true; DK_PRESSED_FIRST=0; DK_DIR=D_LEFT; break;
                    case(SDLK_RIGHT):
                        DK_RIGHT_PRESSED=DK_PRESSED=DK_GO=true; DK_PRESSED_FIRST=0; DK_DIR=D_RIGHT; break;

                    case(SDLK_RCTRL):case(SDLK_LCTRL):
                        FFDownCommand();
                        break;
                    case(SDLK_RSHIFT):case(SDLK_LSHIFT):
                        ShiftPressed=true; break;

                }
    
}
void game::onKeyUp(SDLKey keysim, Uint16 unicode)
{
    bool done=false;
    bool browse=false;

                switch (keysim)
                {

                    case (SDLK_c) :
                        CoordMode=false;
                        break;

                    case (SDLK_h) :
                        HintUpCommand();
                        break;

                    //Dir arrows release:
                    case(SDLK_UP):
                        DK_UP_PRESSED=false;
                        if (DK_PRESSED) DK_PRESSED = ((DK_DIR!=D_UP) || (EvalDirKeys())); break;
                    case(SDLK_DOWN):
                        DK_DOWN_PRESSED=false;
                        if (DK_PRESSED) DK_PRESSED = ((DK_DIR!=D_DOWN) || (EvalDirKeys())); break;
                    case(SDLK_LEFT):
                        DK_LEFT_PRESSED=false;
                        if (DK_PRESSED) DK_PRESSED = ((DK_DIR!=D_LEFT) || (EvalDirKeys())); break;
                    case(SDLK_RIGHT):
                        DK_RIGHT_PRESSED=false;
                        if (DK_PRESSED) DK_PRESSED = ((DK_DIR!=D_RIGHT) || (EvalDirKeys())); break;

                    case(SDLK_RCTRL):case(SDLK_LCTRL):
                        FFUpCommand(); break;

                    case(SDLK_RSHIFT):case(SDLK_LSHIFT):
                        ShiftPressed=false; break;
                    case (SDLK_BACKSPACE):
                        BrowseCommand();
                        break;


                }

}

void game::FlashXyePosition()
{
    FlashPos=70;

}

void game::onExitAttempt()
{
    gamewindow->stop();
}

Sint16 Mouse2GridX(Uint16 mouse)
{
    Sint16 x=mouse;
    if (x<0) return -1;
    x= (Sint16)(x / sz);
    if (x>=XYE_HORZ) return -1;
    return x;
}

Sint16 Mouse2GridY(Uint16 mouse)
{
    Sint16 y=mouse;
    if (y<0) return -1;
    y= (Sint16)(y / sz);
    if (y>=XYE_VERT) return -1;
    return (XYE_VERT-y-1);

}


void game::onMouseMove(int x,int y)
{
    mouse_x=x, mouse_y=y;
    if (mouse_pressed)
    {
        if (EvalDirGrid(XYE, mouse_x, mouse_y,DK_DIR) && (!mouse_valid) )
        {
            DK_PRESSED_FIRST=0;
            DK_GO=mouse_valid=true;
        }
        else if (mouse_valid)
            mouse_valid=false;
    }

}
void game::onMouseOut()
{
}
void game::onMouseDown(int x,int y)
{
    mouse_x=x, mouse_y=y;   
    if (EvalDirGrid(XYE, mouse_x, mouse_y,DK_DIR))
    {
        DK_PRESSED_FIRST=0;
        DK_GO=mouse_valid=true;
    }
    mouse_pressed=true;

}
void game::onMouseUp(int x,int y)
{
    mouse_pressed=mouse_valid=false;
}


string game::InitLevelFile;
int game::InitLevelFileN=0;

void game::InitGameSection(window* wind)
{
    Sint16 oy = 0;
    if(! xye_fromeditortest)
        gamewindow->Resize(GameWidth,GameHeight);
    /*else
    {
        oy = XYE_XTRA_Y;
    }*/
    Sint16 sz32 = (game::GRIDSIZE*3)/2;
    //button * but = new button(0,0,100,100);
    //but->depth = 100;
    //gamewindow->addControl(but);
   
    rectangle* rc = new rectangle(0,oy, wind->Width, game::GRIDSIZE, options::LevelMenu_info );
    wind->addControl(rc);
    
    //fun with buttons
    
    const char* cap;
    
    Button_ToolTipWasDrawn = false;
    //*** button tooltip
    buttontooltip* btt = new buttontooltip();
    btt->depth = 2;
    wind->addControl(btt);
    btt->minx = 0;
    btt->maxx = GameWidth;
    btt->miny = 0;
    btt->maxy = game::GRIDSIZE*2;
    btt->drawnSwitch = &Button_ToolTipWasDrawn;
    
    
    //*** Browse button:
    cap = "Browse";
    button* bt  = new button(1,oy, sz32, game::GRIDSIZE);
    //bt->text = cap;
    bt->Icon(5,15);
    bt->depth=1;
    bt->onClick = BrowseCommand;
    bt->toolTipControl = btt;
    bt->toolTip = "[Backspace] Level browser";
    wind->addControl(bt);
    

    //*** Restart button:
    cap = "Restart";
    bt  = new button(bt->x + bt->w + 1,oy, sz32, game::GRIDSIZE);
    //bt->text = cap;
    bt->Icon(7,3);
    bt->depth=1;
    bt->onClick = RestartCommand;
    bt->toolTipControl = btt;
    bt->toolTip = "[Enter] Restart";
    wind->addControl(bt);

    //*** Prev button:
    cap = "-";
    bt  = new button(bt->x + bt->w + 1,oy, sz32, game::GRIDSIZE);
    //bt->text = cap;
    bt->Icon(4,18);
    bt->depth=1;
    bt->onClick = GoPreviousCommand;
    bt->toolTipControl = btt;
    bt->toolTip = "[P] Previous level";
    wind->addControl(bt);
    Button_PrevLevel = bt;
    
    ;
    
    //*** Next button:
    cap = "+";
    bt  = new button(bt->x + bt->w + 1,oy, sz32, game::GRIDSIZE);
    //bt->text = cap;
    bt->Icon(5,18);
    bt->depth=1;
    bt->onClick = GoNextCommand;
    bt->toolTipControl = btt;
    bt->toolTip = "[N] Next level";
    wind->addControl(bt);
    Button_NextLevel=bt;

    //*** FF button:
    cap = ">>";
    bt  = new button(bt->x + bt->w + 1,oy, sz32, game::GRIDSIZE);
    //bt->text = cap;
    bt->Icon(8,3);
    bt->depth=1;
    bt->onPress = FFDownCommand;
    bt->onRelease = FFUpCommand;
    bt->toolTipControl = btt;
    bt->toolTip = "[Ctrl] Fast forward";
    wind->addControl(bt);


    //*** Hint button:
    cap = "?";
    bt  = new button(bt->x + bt->w + 1,oy, sz32, game::GRIDSIZE);
    //bt->text = cap;
    bt->Icon(11,18);
    bt->depth=1;
    bt->onPress = HintDownCommand;
    bt->onRelease = HintUpCommand;
    bt->ToggleButton = true;
    bt->toolTipControl = btt;
    bt->toolTip = "[H] Show hint";
    wind->addControl(bt);
    Button_Hint=bt;

    //*** Solution button:
    cap = "S";
    bt  = new button(bt->x + bt->w + 1,oy, sz32, game::GRIDSIZE);
    bt->Icon(8,4);
    //bt->text = cap;
    bt->depth=1;
    bt->onClick = SolutionCommand;
    bt->toolTipControl = btt;
    bt->toolTip = "[S] Play solution";
    wind->addControl(bt);
    Button_Solution=bt;

    //*** Undo button:
    cap = "Undo";
    bt  = new button(bt->x + bt->w + 1,oy, sz32, game::GRIDSIZE);
    //bt->text = cap;
    bt->Icon(11,19);
    bt->depth=1;
    bt->onClick = UndoCommand;
    Button_Undo = bt;
    bt->toolTipControl = btt;
    bt->toolTip = "[Del] Undo last movement";
    wind->addControl(bt);

    //*** Record button:
    cap = "RecordSolution";
    bt  = new button(bt->x + bt->w + 1,oy, sz32, game::GRIDSIZE);
    //bt->text = cap;
    bt->Icon(10,9);
    bt->depth=1;
    bt->onClick = RecordSolutionCommand;
    Button_RecordSolution = bt;
    bt->toolTip = "Record solution";
    bt->toolTipControl = btt;
    wind->addControl(bt);


    //*** Quit button:
    cap = "Quit";
    bt  = new button( wind->Width - button::recommendedWidth(cap) -1,oy, button::recommendedWidth(cap), game::GRIDSIZE);
    bt->text = cap; 
    bt->depth=1;
    bt->onClick = ExitCommand;
    bt->toolTipControl = btt;
    wind->addControl(bt);

    
    rc = new rectangle(0, game::GRIDSIZE+oy, wind->Width, XYE_XTRA_Y, 0,0,0); 
    wind->addControl(rc);
    
    Sint16 cx=0, cy = oy+XYE_XTRA_Y + game::GRIDSIZE;
    
    TheGameBoard = new gameboard(XYE_XTRA_X,cy, game::GRIDSIZE*XYE_HORZ, game::GRIDSIZE*XYE_VERT);
    wind->addControl(TheGameBoard);

    rc = new rectangle(0, TheGameBoard->y, XYE_XTRA_X, TheGameBoard->h, 0,0,0); 
    wind->addControl(rc);
    rc = new rectangle(TheGameBoard->x+TheGameBoard->w, TheGameBoard->y, wind->Width-(TheGameBoard->x+TheGameBoard->w), TheGameBoard->h, 0,0,0); 
    wind->addControl(rc);


    
    wind->onKeyDown = game::onKeyDown;
    wind->onKeyUp = game::onKeyUp;
    wind->onExitAttempt = game::onExitAttempt;

    rc = new rectangle(0, TheGameBoard->y + TheGameBoard->h, wind->Width, XYE_XTRA_Y, 0,0,0); 
    wind->addControl(rc);

    
    gamepanel* gp = new gamepanel(XYE_XTRA_X, rc->y + rc->h, wind->Width- 2*XYE_XTRA_X, wind->Height - (rc->y + rc->h) - XYE_XTRA_Y  );
    gp->depth = 2;
    gamewindow->addControl(gp);

    rc = new rectangle(0, gp->y, wind->Width, gp->h+XYE_XTRA_Y, 0,0,0); 
    rc->depth = 0;
    wind->addControl(rc);

    //
    game::started=true;
    game::end();
    game::start();
    if(game::InitLevelFile!="")
        game::InitLevelFileN = options::GetLevelNumber(game::InitLevelFile.c_str());
    
    LevelPack::FromEditor = xye_fromeditortest;
    LevelPack::Load( game::InitLevelFile.c_str(), game::InitLevelFileN);
    AfterLevelLoad();
    
    if(xye_playsolution)
    {
        xye_playsolution = false;
        game::SolutionCommand(NULL);
    }
   
}

//Starts the game.
void game::start(bool undotime)
{
    char i,j;
    int ix,iy;
    started=true;

    cameraon=!undotime;
    playingrec=false; //true;

    if (undo=undotime) {
    } else {
        recording::clean();
    }

    deathqueue::reset();


    UpdateAll=true;
    mouse_pressed=false;
    CoordMode=false;
    DK_PRESSED=DK_GO=false;
    ShiftPressed=false;

    deathsq1=NULL;
    deathsq2=NULL;
    lastKey=0;
    FlashXyePosition();
    LastXyeMove=counter=counter2=counter3=counter4=counter5=counter7=counter8=counter9=beastcounter=0;
    marked::Reset(); //reset counts of marked zones;
    toggle::Reset(); //Reset state of toggle blocks
    surprise::Reset(); //Reset state of remaining surprise blocks to morph.
    gem::ResetCounts(); //reset gem counts
    star::ResetCounts(); //reset star counts
    key::ResetCounts(); //reset key counts
    wall::ResetDefaults();
    earth::ResetDefaults();
    tdoor::ResetDefaults();
    portal::Reset();
    windowblock::ResetCounts();
    beast::ResetCounts();
    hint::Reset();
    HintMarquee=HintRead=0;
    //game::InitControls();
    
    
    


    LastXyeDir=D_DOWN; //Default last dir is down


    id_count=0;
    NextTic=0;

    square* sq;

    iy=TheGameBoard->y;
    for (j=XYE_VERT-1;j>=0;j--)
    {
        ix=TheGameBoard->x;
        for (i=0;i<XYE_HORZ;i++)
        {
            sq=&grid[i][j];
            sq->gobject=NULL;
            sq->object=NULL;
            sq->ex=NULL;
            sq->x=ix;
            sq->y=iy;
            sq->sqx=i;
            sq->sqy=j;
            sq->gs=GROUND_1;
            sq->R=options::FloorColor.r;
            sq->G=options::FloorColor.g;
            sq->B=options::FloorColor.b;
            sq->Update=true;
            sq->UpdateLater=false;

            ix+=GRIDSIZE;
        }
        iy+=GRIDSIZE;
    }
    started=true;
}


void game::loop_Sub(char i, char j)
{
    square* sq;
    obj* object;
    bool died=false;
    sq=&grid[i][j];
    object=sq->object;
    gobj* gobject;
    if (gobject=sq->gobject) gobject->Loop();

    if ((object!=NULL) && (object->tic!=counter) && (object->Loop(&died)) && (! died))
         object->tic=counter;

}

void game::loop_gameplay()
{
    char i,j;
    surprise::TransformAll(); //Change every surprise block that needs to be changed.


    incCounters();

    MoveXye();

    for (j=XYE_VERT-1;j>=0;j--) for (i=0;i<XYE_HORZ;i++)
        loop_Sub(i,j);



    deathqueue::KillNow();
    //draw();
}
void game::loop()
{
    if (FinishedLevel) 
    {
        incCounters();
        //Just draw;
        //draw();
    }
    else
    {
        int i=0;
        do
        {
            loop_gameplay();
            i++;
        }
        while (undo || (FastForward&&(i<XYE_FASTFORWARD_SPEED)));
            
    }

}

bool CloseEnough(int x1, int y1, int x2, int y2)
{
 int d=x1-x2;
 d=(d<0)?-d:d;
 if (d<=1)
 {
     d=y1-y2;
     d=(d<0)?-d:d;
     return (d<=1);
 }
 return (false);
}


void game::DrawPanelInfo(Drawer& D, Sint16 &cx,Sint16 &cy, Uint8 spx, Uint8 spy , unsigned int num, Uint8 fonadd, Uint8 R, Uint8 G, Uint8 B, Uint8 A)
{
    D.ChangeRect(spx*GRIDSIZE,spy*GRIDSIZE,GRIDSIZE,GRIDSIZE);
    D.SetColors(R,G,B,A);
    D.Draw(screen,cx,cy);
    D.SetColors(255,255,255,255);
    cx+=GRIDSIZE;
    Uint8 g2=GRIDSIZE/2;
    D.ChangeRect(4*GRIDSIZE,15*GRIDSIZE,g2,GRIDSIZE);
    D.Draw(screen,cx,cy);

    cx+=g2;
    char tx[5];
    sprintf(tx,"%d",num);
    FontRes->Write(screen,cx, cy+fonadd ,tx);
    cx+=FontRes->TextWidth(tx)+4;

}

void game::DrawPanelInfo(Drawer& D, Sint16 &cx,Sint16 &cy, Uint8 spx, Uint8 spy , unsigned int num, Uint8 fonadd, SDL_Color col)
{
    DrawPanelInfo(D,cx,cy,spx,spy,num,fonadd, col.r , col.g, col.b, col.unused);
}



string LastPanelHint="";

#define FADETICS 6
#define MARQUEETICS 24
void game::DrawPanel(SDL_Surface* target, Sint16 x, Sint16 y, Sint16 w, Sint16 h)
{
    Sint16 cx,cy;
    Sint16 tem1,tem2;
    Sint16 Aw=w;
    Sint16 Ah=h;
    Uint32 black=SDL_MapRGB(target->format, 0, 0, 0);
    Uint32 white=SDL_MapRGB(target->format, 255, 255, 255);

    SDL_FillRect(screen, x,y, Aw, Ah, black);
    SDL_FillRect(screen, x,y, Aw, Ah, white);
    //Aw-=XYE_GAMEX+XYE_XTRA_X;
    //Ah-=XYE_XTRA_Y;
    //x+=XYE_GAMEX;
    Sint16 dif= (GRIDSIZE+2-FontRes->Height())/2;

    char tx[30];
    string hintx="";


    if (playingrec)
    {
        if(xye_fromeditortest) {
            if (GameOver) {
                hintx="** The movie has ended ** Press [Enter] to play the level - [Backspace] to return to the level editor.";
            } else {
                hintx="** Playing Movie ** - Press [Enter] to play the level. [Ctrl] - Fast Forward. [Backspace] - Return to the level editor.";
            }
        }
        else if (GameOver) {
            hintx="** The movie has ended ** Press [Enter] to play the level - [Backspace] to return to the level file menu";
        } else {
            hintx="** Playing Movie ** - Press [Enter] to play the level. [Ctrl] - Fast Forward";
        }

    }
    else if (CoordMode)
    {
        HintMarquee=HintRead=0;
        sprintf(tx,"x : %d ; y : %d",Mouse2GridX(mouse_x),Mouse2GridY(mouse_y));
        hintx = tx;
    }
    else if (FinishedLevel)
    {
        hintx = LevelPack::CurrentLevelBye;
    }
    else if (GameOver)
    {
        hintx="Game over , press [Enter] to restart the level";
    }

    else if (hint::Active())
    {
        hintx = "Hint - "+string(hint::GetActiveText() );
    }
    else if (LevelPack::LevelError != "")
    {
        hintx = LevelPack::LevelError;
    }



    SDL_FillRect(screen, x,y+1, Aw, Ah-1, white);
    //Lives
    int L=XYE->GetLives()-1;

    Drawer D(game::sprites,0,0,0,0);
    if (L>0)
    {
        XYE->Draw(x+2,y+2);
        D.ChangeRect(4*GRIDSIZE,15*GRIDSIZE,GRIDSIZE,GRIDSIZE);
        D.Draw(screen,x+2+GRIDSIZE,y+2);
        sprintf(tx,"%d",L);
        FontRes->Write(screen,GRIDSIZE+(GRIDSIZE/2)+x+2, y+2+dif ,tx);
    }
    cx=x+2+GRIDSIZE*3;
    SDL_FillRect(screen,cx,y,2,Ah,black);
    cx+=5;

    //Levels
    char levelmsg[20];
    snprintf(levelmsg,20,"Level: %d/%d",LevelPack::OpenFileLn,LevelPack::n);
    FontRes->Write(screen,cx, y+2+dif ,levelmsg);


    tem1=FontRes->TextWidth(levelmsg);
    tem2=FontRes->TextWidth("Level: 99/99");
    cx+=(tem1>tem2)?tem1:tem2;
    cx+=3;
    SDL_FillRect(screen, cx,y, 2, Ah, black);
    cx+=5;

    unsigned int yl,rd,bl,gr;
    cy=y+2;
    if(xye_fromeditortest)
    {
        const char*tx="Testing a level from Xyedit";
        if(xye_recordingsolution) tx="Recording your solution for Xyedit";
        FontRes->Write(screen, x+w-GRIDSIZE-FontRes->TextWidth(tx),y+h-XYE_XTRA_Y-FontRes->Height()  ,tx);
    }
    if (gem::GetRemanents(yl,rd,bl,gr))
    {
        FontRes->Write(screen,cx, y+2+dif ,"Remaining: ");
        cx+= FontRes->TextWidth("Remaining: ");
        if (bl)
            DrawPanelInfo(D, cx,cy, 2, 3, bl,dif);
        if (gr)
            DrawPanelInfo(D, cx,cy, 4, 3, gr,dif);
        if (yl)
            DrawPanelInfo(D, cx,cy, 5, 3, yl,dif);
        if (rd)
            DrawPanelInfo(D, cx,cy, 3, 3, rd,dif);

    }

    if (key::GetXyesKeys(yl,rd,bl,gr) || star::GetAcquired() )
    {
        cx+=5;
        FontRes->Write(screen,cx, y+2+dif ,"Got: ");
        cx+= FontRes->TextWidth("Got: ");
        if (bl)
            DrawPanelInfo(D, cx,cy, 6, 4, bl,dif, options::BKColor[B_BLUE] );
        if (gr)
            DrawPanelInfo(D, cx,cy, 6, 4, gr,dif, options::BKColor[B_GREEN] );
        if (yl)
            DrawPanelInfo(D, cx,cy, 6, 4, yl,dif, options::BKColor[B_YELLOW] );
        if (rd)
            DrawPanelInfo(D, cx,cy, 6, 4, rd,dif, options::BKColor[B_RED] );
        
        if(star::GetAcquired()) {
            DrawPanelInfo(D, cx,cy, 9, 12, star::GetAcquired() ,dif);
        }

    }
    
    /*if (hint::GlobalHintExists())
    {
        cx+=3;
        SDL_FillRect(screen, cx,y, 2, Ah, black);
        cx+=3;
        hint::Draw(cx,cy,false);
        cx+=20;
    }

    if (LevelPack::HasSolution())
    {
        cx+=3;
        SDL_FillRect(screen, cx,y, 2, Ah, black);
        cx+=3;
        D.ChangeRect(8*GRIDSIZE,4*GRIDSIZE,GRIDSIZE,GRIDSIZE);
        D.Draw(screen,cx,cy);
        cx+=20;
    }*/

    //---
    bool hintactive=(hintx!="");
    if (hintactive || (HintRead>0))
    {
        if (!hintactive)
        {
            if (HintRead>FADETICS) HintRead=FADETICS;
            hintx=LastPanelHint;
        }
        else LastPanelHint=hintx;

        SDL_Surface *HintSurf;
        if(HintRead>FADETICS)
            HintSurf=NULL;
        else
            HintSurf =CreateFixedRGBASurface(SDL_SRCALPHA, Aw, Ah);
        cx=x;
        cy=y;

        if(HintRead<=FADETICS) {
            SDL_FillRect(HintSurf, 0,0, Aw, Ah, SDL_MapRGB(HintSurf->format, options::HintColor));
        } else {
            SDL_FillRect(screen, cx,cy, Aw, Ah, SDL_MapRGB(screen->format, options::HintColor));
        }
        Uint16 TW=FontRes->TextWidth(hintx.c_str());

        static int marqueeloop  = 0;
        marqueeloop ++;
        
        int fc = ( FastForward ? ( (marqueeloop%10==0) ? 100 : 0 ) : 1 );
        if ((cx+TW>Aw) && (HintRead>=MARQUEETICS)) {
            HintMarquee -= fc;
            if (cx+TW+HintMarquee<=0) {
                HintMarquee=Aw;
            }
        }


        Uint8 alpha;
        if (HintRead<=FADETICS)
        {
            alpha=(Uint8)(int)(  (double)(HintRead*HintRead) * 255.0/(FADETICS*FADETICS)   );
            SDL_BlitSurface(HintSurf,0,0,Aw,Ah, screen, cx,cy,255,255,255,alpha);
            SDL_FreeSurface(HintSurf);
            FontRes->Write(screen,3+HintMarquee+x,cy+1+dif,hintx.c_str(), 255,255,255,alpha);
        } else {
            alpha=255;
            FontRes->Write(screen,3+HintMarquee+x,cy+1+dif,hintx.c_str());
        }

        if (hintactive) {
            if(HintRead<=MARQUEETICS) {
                HintRead+= fc;
            }
        } else {
            HintRead -= fc;
        }
    } else {
        HintMarquee = HintRead = 0;
    }



}

void game::draw(Sint16 px, Sint16 py)
{
    game::screen = gamewindow->getDrawingSurface();
    if(undo) return;
    
    int i,j;

    Uint32 black=SDL_MapRGB(screen->format, 0, 0, 0);
    //Uint32 Aw=XYE_GAMEX+XYE_XTRA_X+XYE_HORZ*GRIDSIZE;
    //Uint32 Ah=XYE_VERT*GRIDSIZE;
    //SDL_FillRect(screen, px,py, Aw, XYE_GAMEY, black);
    //SDL_FillRect(screen, px,py+XYE_GAMEY, XYE_GAMEX, Ah, black);
    //SDL_FillRect(screen, px,py+XYE_GAMEY+Ah, Aw, XYE_XTRA_Y, black);
    //SDL_FillRect(screen, px+Aw-XYE_XTRA_X, py+XYE_GAMEY, XYE_XTRA_X, Ah, black);
    
    //DrawPanel(0,XYE_GAMEY+XYE_XTRA_Y+Ah-1);


    square* sq;
    Drawer D(game::sprites,0,0,0,0);


    //let's paint the ground
    unsigned int x,y;
    obj* object;
    gobj* gobject;
    explosion* ex;
    int xx=XYE->X(),xy=XYE->Y();
    bool shadow=false;
    bool dodraw;
    bool drawTopRow = false;
    if(Button_ToolTipWasDrawn) {
        drawTopRow = true;
        Button_ToolTipWasDrawn = false;
    }
    for (i=0;i<XYE_HORZ;i++)
    {
        for (j=0;j<XYE_VERT;j++)
        {
            sq=&grid[i][j];
            gobject=sq->gobject;
            ex=sq->ex;
            // the row 19 objects are always drawn provisionally due to tooltips
            // being able to draw on top of them...
            dodraw=( (drawTopRow && (j==19) ) || (UpdateAll) || (sq->Update) || (gobject!=NULL) || (ex!=NULL) || CloseEnough(xx,xy,i,j)) ;

            sq->Update=false;
            if(sq->UpdateLater)
            {
                sq->Update=true;
                sq->UpdateLater=false;
            }



            if (dodraw)
            {
            x=sq->x;
            y=sq->y;
            object=sq->object;
            SDL_FillRect(screen, x,y,GRIDSIZE,GRIDSIZE, SDL_MapRGB(screen->format, sq->R , sq->G, sq->B));

            int xo,yo;
            if(ex!=NULL)
            {
                ex->getDrawingXYOffset(xo,yo);
                int k,l;
                if(xo||yo) for (k=-1;k<=1;k++) for (l=-1;l<=1;l++)
                {
                    if(( k+i<XYE_HORZ) && ( k+i>=0) && ( l+j<XYE_VERT) && ( l+j>=0) )
                    {
                        if( (k<0) || ((k==0) && (l<0) ) )
                           grid[k+i][l+j].Update=true;
                        else
                           grid[k+i][l+j].UpdateLater=true;
                    }
                }
            }
            else
            {
                xo=yo=0;
            }

            if ( (gobject!=NULL) && (! gobject->RenderAfterObjects) )
            {
                if (shadow) switch (gobject->GetType())
                {
                    case(OT_EXIT): case(OT_MARKEDAREA): case(OT_BLOCKDOOR):
                    //darkspr->Render(x,y);
                    shadow=false;
                }
                gobject->Draw(x+xo,y+yo);
            }

            if (object!=NULL)
            {
                if (shadow) switch(object->GetType())
                {
                    case(OT_XYE): case(OT_GEM): case(OT_GEMBLOCK):
                    //darkspr->Render(x,y);
                    shadow=false;
                }

                if ( (!undo) || (object->GetType()!=OT_XYE))
                {
                    object->Draw(x + xo,y + yo);
                }
            }

            if ( (gobject!=NULL) && ( gobject->RenderAfterObjects) )
            {
                if ((shadow) && (gobject->GetType()==OT_MARKEDAREA))
                {
                    //darkspr->Render(x,y);
                    shadow=false;
                }

                gobject->Draw(x+xo,y+yo);
            }

            if (ex!=NULL) ex->Draw(x,y);


            } // if (dodraw)

        }
    }
    //delete spr1;delete spr2;

    bool DoArrowThing=false;
    if (FlashPos>0)
    {
        DoArrowThing=((FlashPos%2==0) && (FlashPos<50));
        if (counter5==0) FlashPos--;

    }
    DoArrowThing = (!GameOver &&  (DoArrowThing || (ShiftPressed && (counter9<5)) ));

    if (DoArrowThing && (!playingrec))
    {
            signed char incx[4]={ 1, 1,-1,-1};
            signed char incy[4]={ 1,-1, 1,-1};
            signed char atx[4]= { 5, 5, 4,4};
            signed char aty[4]= {13,14,13,14 };
            signed char xx=XYE->X(),xy=XYE->Y();
            signed char nx,ny;
            for (i=0;i<4;i++)
            {
                nx=xx+incx[i];
                ny=xy+incy[i];
                if ((nx>=0) && (nx<XYE_HORZ) && (ny>=0) && (ny<XYE_VERT))
                {
                    sq= SquareN(xx+incx[i],xy+incy[i]);
                    sq->Update=true;
                    SDL_BlitSurface(sprites.sprites,atx[i]*GRIDSIZE,aty[i]*GRIDSIZE,GRIDSIZE,GRIDSIZE,
                                    screen,sq->x,sq->y);
                }
            }

    }
    if (deathsq1!=NULL)
    {
        D.ChangeRect(0,GRIDSIZE,GRIDSIZE,GRIDSIZE);
        D.SetColors(&PlayerColor,255);
        D.Draw(screen,deathsq1->x,deathsq1->y);
        deathsq1->Update=true;
        deathsq2=deathsq1;
        deathsq1=NULL;
    }
    else if (deathsq2!=NULL)
    {
        D.ChangeRect(0,GRIDSIZE*2,GRIDSIZE,GRIDSIZE);
        D.SetColors(&PlayerColor,255);
        D.Draw(screen,deathsq2->x,deathsq2->y);
        deathsq2->Update=true;
        deathsq2=NULL;
    }





    //SDL_Flip(screen);
    UpdateAll=false;
}

bool ScreenCoordsToGameCoords(float inx, float iny, char &rx, char &ry, float scale)
{
    inx=inx-XYE_GAMEX;
    iny=iny-XYE_GAMEY;
    scale*=16;
    rx=(char)(inx / scale);
    ry=XYE_VERT-1-(char)(iny / scale);

    return ((rx>=0) && (rx<XYE_HORZ) && (ry>=0) && (ry<XYE_VERT));

}

edir GetDir(char ix, char iy, char ex,char ey)
{
    char dx= ex-ix;
    char dy= ey-iy;
    if ((dy*dy)>(dx*dx))
        if (dy>0) return D_UP;
        else return D_DOWN;
    else
        if (dx>0) return D_RIGHT;
    return D_LEFT;
}

bool game::TryMoveXye(char dx, char dy, edir dir)
{
    char nx,ny,cx,cy;
    bool go=false;
    square* sq=Square(dx,dy);
    obj* object=sq->object;
    teleport* tele; dangerous* blck;
    if (object==NULL) go=true;
    else
    {
        switch (object->GetType())
        {
            case(OT_TELEPORT):
                tele=static_cast<teleport*>(object);
                go = tele->tryteleport(dir, XYE, nx, ny ,blck, NULL);
                if (go) { dx=nx; dy=ny;sq=Square(dx,dy); }
                else if (blck!=NULL)
                {
                    if (blck->Busy(XYE)) go=false;
                    else
                    {
                        //Kill Xye
                        blck->Eat();
                        XYE->Kill();
                        return (true);
                    }
                }

                break;
            case(OT_BLACKHOLE): case(OT_MINE): case(OT_FIREBALL):
                blck=static_cast<dangerous*>(object);
                if (blck->Busy(XYE)) go=false;
                else
                {
                    //Kill Xye
                    blck->Eat();
                    XYE->Kill();
                    return (true);

                }

                break;
            }

            if ((! go) && (object->trypush(dir,XYE))) go=true;
        }
    if (go)
    {
        XYE->moved=true;
        gobj* gobject=sq->gobject;
        if ((gobject==NULL) || (gobject->CanEnter(XYE,dir)))
        {
            XYE->move(dx,dy);
            XYE->lastdir=dir;
            LastXyeDir=dir;
            return (true);
        }
    }
 return(false);
}

bool game::LastMoveWasHorizontal()
{
    switch(LastXyeDir)
        case(D_LEFT): case(D_RIGHT): return true;
 return false;
}

bool game::TryMoveXye(edir dir)
{
    char dx=XYE->X();
    char dy=XYE->Y();
    switch(dir)
    {
        case(D_UP): dy++; break;
        case(D_DOWN): dy--; break;
        case(D_RIGHT): dx++; break;
        default: dx--;
    }
    dx= (dx>=XYE_HORZ)?0:(dx<0)?XYE_HORZ-1:dx;
    dy= (dy>=XYE_VERT)?0:(dy<0)?XYE_VERT-1:dy;
    return (TryMoveXye(dx,dy,dir));
}

void game::MoveXye()
{
    if (GameOver) return;
    
    if ( (LastXyeMove+1) < counter)
    {
        if (playingrec)
        {
            bool nm;

            if (! recording::get(DK_DIR,nm))
                game::TerminateGame(false);

            if ((!nm) && TryMoveXye(DK_DIR) )
                LastXyeMove=counter;
            return;

        }
        if (undo)
        {
            bool nm;
            if (! recording::get_undo(DK_DIR,nm))
            {
                cameraon=true;
                undo=false;
                UpdateAll=true;
                game::XYE->alpha=200;
            }
            else
            {
                if ( (!nm) && TryMoveXye(DK_DIR) )
                      LastXyeMove=counter;
                return;
            }
        }


        //char dx=0,dy=0,nx,ny,cx,cy;
        //edir dir=D_UP;
        //int ky=0;

        if ( ( DK_PRESSED || mouse_valid || DK_GO) && (DK_PRESSED_FIRST!=1))
        {

            DK_GO=false;
            unsigned char fp=FlashPos;
            FlashPos=0;
            if (cameraon) recording::add(DK_DIR);
            if (TryMoveXye(DK_DIR))
                 LastXyeMove=counter;

            else FlashPos=fp;
        }
        else
        {
            //No more flash after idle time. if ((FlashPos==0) && (LastXyeMove+200<counter))
            //    FlashXyePosition();
            if (cameraon) recording::add_nomove();
        }

        DK_PRESSED_FIRST++;
    }
    if (mouse_pressed)
    {
                if (EvalDirGrid(XYE,mouse_x,mouse_y,DK_DIR) && (!mouse_valid) )
                {
                    DK_PRESSED_FIRST=0;
                    DK_GO=mouse_valid=true;
                }
                else if (mouse_valid)
                    mouse_valid=false;
    }


}

square* game::Square(unsigned char x,unsigned char y)
{
    return &grid[x][y];
}

square* game::SquareN(signed int x,signed int  y)
{
    while (x>=XYE_HORZ) x-=XYE_HORZ;
    while (x<0) x+=XYE_HORZ;

    while (y>=XYE_VERT) y-=XYE_VERT;
    while (y<0) y+=XYE_VERT;

    return &grid[x][y];
}


unsigned int game::NewId()
{
    return id_count++;
}

void game::incCounters()
{
    counter++;
    counter2++;    if (counter2>=2) counter2=0;
    counter3++;    if (counter3>=3) counter3=0;
    counter4++;    if (counter4>=4) counter4=0;
    counter5++;    if (counter5>=5) counter5=0;
    counter7++;    if (counter7>=7) counter7=0;
    counter8++;    if (counter8>=8) counter8=0;
    counter9++;    if (counter9>=9) counter9=0;
    if ((! counter2) &&(! counter5)) beastcounter++;
}

inline bool game::Mod2() { return (counter2==0);}
inline bool game::Mod3() { return (counter3==0);}
inline bool game::Mod4() { return (counter4==0);}
inline bool game::Mod5() { return (counter5==0);}
inline bool game::Mod7() { return (counter7==0);}
inline bool game::Mod8() { return (counter8==0);}
inline bool game::Mod9() { return (counter9==0);}

inline bool game::Mod2(unsigned int m) { return (counter2==m);}
inline bool game::Mod3(unsigned int m) { return (counter3==m);}
inline bool game::Mod4(unsigned int m) { return (counter4==m);}
inline bool game::Mod5(unsigned int m) { return (counter5==m);}
inline bool game::Mod7(unsigned int m) { return (counter7==m);}
inline bool game::Mod8(unsigned int m) { return (counter8==m);}
inline bool game::Mod9(unsigned int m) { return (counter9==m);}



inline unsigned int game::Counter() { return counter; }



void game::end()
{



    char i,j;
    GameOver=FinishedLevel=false;
    square* sq;
    recycle::run();
    for (i=0;i<XYE_HORZ;i++) for (j=0;j<XYE_VERT;j++)
    {
        sq=&grid[i][j];
        if (sq->gobject!=NULL)
        {
            delete (sq->gobject);
            sq->gobject=NULL;
        }
        if (sq->object!=NULL)
        {
            delete (sq->object);
            sq->object=NULL;
        }
        if (sq->ex!=NULL)
        {
            delete (sq->ex);
            sq->ex=NULL;
        }

    }
    started=false;
    deathqueue::reset();

}


struct FindGoodPoint_List
{
    char x;
    char y;
    FindGoodPoint_List* next;
};



bool game::FindGoodPoint(char cx, char cy, char &rx, char &ry, obj* togo, bool (*cond)(square* sq,obj* togo))
{
    if (cond(Square(cx,cy),togo))
    {
         rx=cx;
         ry=cy;
         return true;
    }

    bool mem[XYE_HORZ][XYE_VERT];
    bool found=false;
    char tx,ty;
    char ix[8],iy[8];
    ix[0]= 1;iy[0]= 0;
    ix[1]= 1;iy[1]= 1;
    ix[2]= 0;iy[2]= 1;
    ix[3]=-1;iy[3]= 1;
    ix[4]=-1;iy[4]= 0;
    ix[5]=-1;iy[5]=-1;
    ix[6]= 0;iy[6]=-1;
    ix[7]= 1;iy[7]=-1;
    char i,j;
    for (i=0;i<XYE_HORZ;i++)
        for (j=0;j<XYE_VERT;j++)
            mem[i][j]=false;

    FindGoodPoint_List *rv= new FindGoodPoint_List,*next,*aux,*curr,*tmp;
    rv->x=cx;
    rv->y=cy;
    rv->next=NULL;
    aux=NULL;
    curr=aux;
    next=rv;
    mem[cx][cy]=true;

    do
    {

        while ((next!=NULL) && (!found))
        {

             tx=next->x;
             ty=next->y;
             next=next->next;

             i=0;
             while ((! found) && (i<8))
             {
                   rx=tx+ix[i];
                   ry=ty+iy[i];

                   if ((rx<0) || (ry<0) || (rx>=XYE_HORZ) || (ry>=XYE_VERT) ||  (mem[rx][ry]));
                   else if (cond(Square(rx,ry),togo))
                          found=true;
                   else
                   {
                         mem[rx][ry]=true;
                         if (aux==NULL)
                         {
                               curr=new FindGoodPoint_List;
                               curr->x=rx;
                               curr->y=ry;
                               curr->next=NULL;
                               aux=curr;
                         }
                         else
                         {

                             tmp=new FindGoodPoint_List;

                              tmp->x=rx;
                             tmp->y=ry;
                             tmp->next=NULL;
                          curr->next=tmp;
                             curr=tmp;
                             tmp=NULL;
                         }

                   }
                   i++;
             }

        }

        // Remove Old linked list
        next=rv;
        while (next!=NULL)
        {
            tmp=next;
            next=next->next;
            delete tmp;
            tmp=NULL;
        }

        //save new linked list
        rv=aux;
        next=rv;
        aux=NULL;
        curr=NULL;

    } while ((! found) && (next!=NULL));

    // Remove linked list
    next=rv;
    while (next!=NULL)
    {
        tmp=next;
        next=next->next;
        delete tmp;
    }

    return found;
}


bool AllowedForRevive(square* sq,obj* togo)
{

    if (sq->object==NULL)
        return (sq->gobject==NULL);
 return false;
}

bool game::GetRevivePoint( char cx, char cy, char &nx, char &ny)
{
    return FindGoodPoint(cx,cy,nx,ny, XYE, AllowedForRevive );
}



bool game::AllowRoboXyeMovement()
{
    return (LastXyeMove+1==counter) && (LastXyeMove);
}

bool ObjectResistsFire(obj * o)
{
wall* wl;
    switch(o->GetType())
    {
        case (OT_METALBLOCK): case(OT_GEMBLOCK): case(OT_LOCK): case(OT_FIREBALL): case(OT_GEM): case(OT_WINDOW): return true;
        case(OT_WALL):
            //Yep those walls resist fire, thus they resist moving dangerous too
            wl=static_cast<wall*>(o);
            return (wl->ResistsFire());
    }
    return false;
}

// A small explosion that kills any non fire resistant object in the square
void game::SmallBoom(square* sq,bool ConditionalFx, int xo, int yo)
{

    obj *object=sq->object;
    unsigned char kind = 2;
    if (  ( (object!=NULL) && !ObjectResistsFire(object) ) || (!ConditionalFx))
    {
        kind = 1;
    }
    explosion* ex=new explosion(sq,kind);
    ex->setDrawingXYOffset(xo,yo);

    if ((object!=NULL) && ! (ObjectResistsFire(object)))
    {
        if(object->GetType()==OT_XYE)
        {
            (static_cast<xye*>(object))->Kill() ;
        }
        else
        {
            object->Kill();
        }
    }

}



bool game::Moved(edir d)
{

    return ((LastXyeMove==counter) && (LastXyeDir==d));
}

bool game::IsUndoAllowed()
{
    return ( ((options::UndoEnabled() || xye_fromeditortest) && ! xye_recordingsolution) || LevelPack::AllowUndo() );
}

void game::Undo()
{

    if ( game::IsUndoAllowed() && recording::undo() )
    {
       undo=true;
       //game::draw();
       game::end();
       game::start(true);
       LevelPack::Restart();
       AfterLevelLoad();
    }

}


void game::SaveReplay()
{
    //save the replay.
    const char* home=getenv("HOME");
    char* lastgame;
    if (home)
    {
        lastgame=new char[strlen(home)+strlen("/.xye/levels/lastgame.xyr")+1];
        strcpy(lastgame,home);
        strcat(lastgame,"/.xye/levels/lastgame.xyr");
    }
    else
    {
        lastgame=new char[strlen("./levels/lastgame.xyr")+1];
        strcpy(lastgame,"./levels/lastgame.xyr");
    }

    recording::saveInFile(lastgame,LevelPack::OpenFile.c_str(),LevelPack::OpenFileLn);
    delete[] lastgame;
}

void game::TerminateGame(bool good)
{
   undo = false;
   if (!playingrec)
   {
        SaveReplay();

        if (good)
        {
            if(xye_recordingsolution)
            {
                //SDL_WM_SetCaption("Xye - Your solution has been saved!",0);
                char * tm = recording::save();
                editor::SendSolution(tm);
                delete[]tm;
                BrowseCommand();

            } else { 
                SDL_WM_SetCaption("Xye - YOU WIN!",0);
            }
        } else {
            SDL_WM_SetCaption("Xye - Game over!",0);
        }
   }
   if (good) {
       FinishedLevel = true;
   }
   //else if(xye_fromeditortest)
   //{
   //    BrowseCommand();
   //}
   
   if(xye_recordingsolution) {
       xye_recordingsolution=false;
   }
    //counter=counter2=counter3=counter4=counter5=counter7=counter8=counter9=1;
    GameOver=true;
}

/**end fake class game**/


/**Class ent**/
char ent::X() { return x; }
char ent::Y() { return y; }
unsigned int ent::GetId() { return id;}
otype ent::GetType() { return type;}

void ent::UpdateSquare()
{
    game::Square(x,y)->Update=true;
}
/**Class ent ends**/

/**Class Object**/
void obj::move(char px,char py)
{
    square* sq=game::Square(x,y);
    sq->Update=true; //Make sure to update it
    gobj *gobject = sq->gobject;
    if (gobject!=NULL) gobject->OnLeave(this);
    sq->object=NULL;
    x=px;
    y=py;
    sq=game::Square(x,y);
    sq->Update=true; //Make sure to update it
    obj* object=sq->object;
    if (object!=NULL)
    {
        if (object->GetType()==OT_XYE)
            ((xye*)(object))->Kill();
        else
            object->Kill();
    }
    gobject = sq->gobject;
    sq->object=this;
    if (gobject!=NULL) gobject->OnEnter(this);


}




void obj::ObjectConstruct(square* sq)
{
    KilledByBlackHole=false;
    id=game::NewId();
    tic=0;
    x=sq->sqx;y=sq->sqy;
    if (sq->object!=NULL) sq->object->Kill();
    sq->object=this;
    sq->Update=true;
    if (sq->gobject!=NULL) sq->gobject->OnEnter(this);

}

void obj::Kill()
{
    Kill(false);
}

void obj::Kill(bool byBlackHole)
{
    UpdateSquare();
    if (type==OT_XYE)
    {
        game::XYE->Kill();
        return;
    }


    KilledByBlackHole=byBlackHole;
    OnDeath();
    square* sq=game::Square(x,y);
    gobj *gobject = sq->gobject;
    if (gobject!=NULL) gobject->OnLeave(this);
    sq->object=NULL;
    recycle::add(this);
}

bool obj::trypush_common(edir dir,obj* pusher,bool AsRoundObject, bool* died)
{
    char dx=0,dy=0;
    if (died!=NULL) *died=false;
    if ((pusher!=NULL) && (pusher!=this) &&  ! CanPush(pusher->GetType())) return false;
    if (AsRoundObject)
    {
         square* sq=RoundAdvance(this,dir,x,y);
         if (sq!=NULL)
         {
              move(sq->sqx,sq->sqy);
              return true;
         }
    }

    switch(dir)
    {
        case(D_UP): dy++; break;
        case(D_DOWN): dy--; break;
        case(D_RIGHT): dx++; break;
        case(D_LEFT): dx--; break;
    }
    if ((dx!=0) || (dy!=0))
    {
        dx=x+dx;
        dy=y+dy;
        dx=(dx>=XYE_HORZ?0:(dx<0?XYE_HORZ-1:dx) );
        dy=(dy>=XYE_VERT?0:(dy<0?XYE_VERT-1:dy) );

        square * sq=game::Square(dx,dy);
        obj* inobj=sq->object;
        teleport* tele;dangerous* blck;
        if (inobj!=NULL)
        {
            switch (inobj->GetType())
            {
                case(OT_TELEPORT):
                    //It was a teleport!
                    tele=static_cast<teleport*>(inobj);
                    char nx,ny;
                    if (tele->tryteleport(dir, this, nx, ny ,blck,NULL))
                    {
                         sq=game::Square(nx,ny);
                           dx=nx;
                         dy=ny;
                         inobj=sq->object;
                    }
                    else if (blck!=NULL)
                    {
                        if (blck->Busy(this)) return false;
                        else
                        {
                             //Kill The object
                             if (died!=NULL) *died=true;
                              blck->Eat();
                            Kill(blck->GetType()==OT_BLACKHOLE); //Died because of a blackhole
                           return true;
                        }
                    }
                    break;
                case(OT_BLACKHOLE): case(OT_MINE): case(OT_FIREBALL):
                    blck=static_cast<dangerous*>(inobj);
                    if (blck->Busy(this)) return false;
                    else if((inobj->GetType()==OT_MINE) && (ObjectResistsFire(this)  ) )
                    {
                        blck->Kill();
                        game::SmallBoom(game::Square(dx,dy),false,x-dx,dy-y);
                        inobj=NULL;
                        //move(dx,dy);
                        //return true;
                    }
                    else
                    {
                        //Kill The object
                        if (died!=NULL) *died=true;
                        blck->Eat();
                        Kill(blck->GetType()==OT_BLACKHOLE); //Died because of a blackhole
                        return true;
                    }

                    break;
            }
        }
        if ((inobj==NULL) && ((sq->gobject==NULL) || sq->gobject->CanEnter(this,dir) ) )
        {
            move(dx,dy);
            return true;
        }
    }
    return false;
}

bool FindMagnetic(square* sq, bool rSticky, bool rHorz, magnetic* &mg, bool ds=false, edir rd=D_DOWN)
{
    obj *object=sq->object;
    if (! magnetic::IsInstance(object)  ) return false;
    mg= static_cast<magnetic*>(object);
    if (mg->IsHorizontal()!=rHorz) return false;

    if (ds && (rSticky) )
        return (mg->IsSticky(rd));


    return (mg->IsSticky()==rSticky);

}

bool SpotAffectedByAMagnetic_Sub(obj* forObject,edir dir, char mx, char my, char ox, char oy, bool rSticky, bool rHorz)
{
    magnetic* mg=NULL;

    if (! FindMagnetic(game::SquareN(mx,my),rSticky,rHorz,mg,true,dir)) return false;
    square *sq= game::SquareN(ox,oy);
    if (sq->object!=NULL) return false;
    gobj* gobject= sq->gobject;
    return ((gobject==NULL) || (gobject->CanEnter(forObject,dir)));
}

bool SpotAffectedByAMagnetic(obj* forObject, char x, char y)
{

    char nx, ny;
    magnetic* mg=NULL;
    //First the obvious
    if (FindMagnetic(game::SquareN(x+1,y),true,true,mg)) return true;
    if (FindMagnetic(game::SquareN(x,y+1),true,false,mg)) return true;
    if (FindMagnetic(game::SquareN(x-1,y),true,true,mg)) return true;
    if (FindMagnetic(game::SquareN(x,y-1),true,false,mg)) return true;



    //Now the non obvious:
    if (SpotAffectedByAMagnetic_Sub(forObject,D_RIGHT,x+2,y,x+1,y,true,true)) return true;
    if (SpotAffectedByAMagnetic_Sub(forObject,D_LEFT,x-2,y,x-1,y,true,true)) return true;
    if (SpotAffectedByAMagnetic_Sub(forObject,D_UP,x,y+2,x,y+1,true,false)) return true;
    if (SpotAffectedByAMagnetic_Sub(forObject,D_DOWN,x,y-2,x,y-1,true,false)) return true;

 //Nothing found yet, return false
 return false;
}

bool SpotGetAntiStickies(char x, char y,bool &up, bool &dw, bool &lf, bool &rg)
{

    magnetic* mg;
    up=(FindMagnetic(game::SquareN(x,y+1),false,false,mg));
    dw=(FindMagnetic(game::SquareN(x,y-1),false,false,mg));
    lf=(FindMagnetic(game::SquareN(x-1,y),false,true,mg));
    rg=(FindMagnetic(game::SquareN(x+1,y),false,true,mg));
 return (up || dw || lf || rg);
}

bool SpotDirectionAllowedByMagnetics(obj* forObject, char x, char y, edir dir, bool up, bool dw, bool lf, bool rg)
{
    square* sq;
    bool r;
    switch(dir)
    {
        case (D_LEFT): case (D_RIGHT):
            if (dir==D_LEFT) r=(! lf);
            else r= (! rg);
            if (dw)
            {
                sq=game::SquareN(x,y+1);
                r=r && (sq->object!=forObject) && (! Allowed(forObject,D_UP,sq));
            }
            if (up)
            {
                sq=game::SquareN(x,y-1);
                r=r && (sq->object!=forObject) && (! Allowed(forObject,D_DOWN,sq));
            }
            return (r);

       default: //up or down:

            if (dir==D_UP) r=(! up);
            else r= (! dw);
            if (lf)
            {
                sq=game::SquareN(x+1,y);
                r=r && (sq->object!=forObject) && (! Allowed(forObject,D_RIGHT,sq));
            }
            if (rg)
            {
                sq=game::SquareN(x-1,y);
                r=r && (sq->object!=forObject) && (! Allowed(forObject,D_LEFT,sq));
            }

            return (r);

    }


 return true;
}








bool obj::Magnetism(char ox, char oy, char mx, char my, bool rSticky, bool rHorz, edir godir)
{
    magnetic *mg;
    if (! FindMagnetic(game::SquareN(mx,my),rSticky,rHorz,mg,true,godir) )
        return false; //Square didn't have the magnetic we want.

    //We have found a valid magnetic block, let's see if it is possible to move the object to
    // the desired point.

    return trypush(godir,mg);
}


bool obj::DoMagnetism(bool horz, bool vert, bool *Moved)
{
    *Moved=true;
    if (horz)
    {
        // #->][
        if (Magnetism(x+1,y,x+2,y,true,true, D_RIGHT)) return true;

        // ][<-#
        if (Magnetism(x-1,y,x-2,y,true,true, D_LEFT)) return true;

        // <-#][
        if (Magnetism(x-1,y,x+1,y,false,true, D_LEFT)) return true;

        // ][#->
        if (Magnetism(x+1,y,x-1,y,false,true, D_RIGHT)) return true;
    }

    if (vert)
    {
        // ][
        // /\
        // #
        if (Magnetism(x,y+1,x,y+2,true,false, D_UP)) return true;

        // #
        // \/
        // ][
        if (Magnetism(x,y-1,x,y-2,true,false, D_DOWN)) return true;

        // /\
        // #
        // ][
        if (Magnetism(x,y+1,x,y-1,false,false, D_UP)) return true;

        // ][
        // #
        // \/
        if (Magnetism(x,y-1,x,y+1,false,false, D_DOWN)) return true;

    }


    //Now check if a sticky magnetic is holding our object:
    magnetic *mg;
    *Moved=false;
    if (horz)
    {
        if (FindMagnetic(game::SquareN(x+1,y),true,true,mg)) return true;
        if (FindMagnetic(game::SquareN(x-1,y),true,true,mg)) return true;
    }
    if (vert)
    {
        if (FindMagnetic(game::SquareN(x,y+1),true,false,mg)) return true;
        if (FindMagnetic(game::SquareN(x,y-1),true,false,mg)) return true;
    }


    return false;
}

struct AIList
{
    unsigned char x;
    unsigned char y;
    signed char firstStep;
    unsigned int steps;
    AIList* next;
};

bool obj::GoFindXye(edir res[], int &resn, bool &foundpath, bool ignoreloopingedge, bool ignoreSp, bool considerTeleports, bool considerStickies, unsigned int range, bool Randomize )
{
    return GoFindASquare(IsXyeAt,res,resn,foundpath,ignoreloopingedge,ignoreSp,considerTeleports,considerStickies,range, Randomize );
}

bool obj::GoFindASquare(bool (*cond)(square* sq), edir res[], int &resn, bool &foundpath, bool ignoreloopingedge, bool ignoreSp, bool considerTeleports, bool considerStickies, unsigned int range, bool Randomize )
{

    signed char gox[4]={1,-1,0,0};
    signed char goy[4]={0,0,1,-1};

    bool allow[4]={false,false,false,false};
    edir godir[4]= { D_RIGHT, D_LEFT, D_UP, D_DOWN };
    bool go;
    bool halt=false, ret=false;


    bool savemem=true;
    char i,j;
    char tx,ty,cx,cy,nx,ny;
    char step;
    unsigned int stepsNumber;
    unsigned int place=(x*XYE_VERT+y);


    sqMem::reset(true);


    //Used by anti anti-sticky block AI:
    bool up,dw,lf,rg,AntiStickyRisk=false;


    if (Randomize)
    //"Randomize" arrays:
    for (i=0;i<3;i++)
    {
        j=(signed char)(i + (place% (4-i)) );
        if (i!=j)
        {
            signed char aux=gox[i];
            //swap:
            gox[i]=gox[j];
            gox[j]=aux;
            aux=goy[i];
            goy[i]=goy[j];
            goy[j]=aux;
            edir daux=godir[i];
            godir[i]=godir[j];
            godir[j]=daux;

        }
    }

    AIList *tm= new AIList,*curr, *next, *end;
    obj *object;gobj *gobject;
    teleport *tele;
    otype ot;
    sqMem::mem[x][y]=false;
    square* sq;
    tm->x=x;
    tm->y=y;
    tm->firstStep=-1;
    tm->steps=0;
    tm->next=NULL;
    end=next=tm;
    bool able;

    do
    {
        curr=next;
        cx=curr->x;
        cy=curr->y;
        stepsNumber=curr->steps+1;
        //explosion * ex= new explosion(GM->SquareN(cx,cy));
        step= curr->firstStep;
        AntiStickyRisk=( (considerStickies) && SpotGetAntiStickies(cx,cy,up,dw,lf,rg) );

        for (i=0;((i<4) && (! halt) ) ;i++) if ((! AntiStickyRisk) || (SpotDirectionAllowedByMagnetics(this,cx,cy,godir[i],up,dw,lf,rg)) )
        {
            //Save test x and y
            tx=cx+gox[i];ty=cy+goy[i];
            savemem=true;
            go=true;
            if (! ignoreloopingedge)
            {
                //Consider looping edges:
                tx=tx>=XYE_HORZ?0:(tx<0?XYE_HORZ-1:tx);
                ty=ty>=XYE_VERT?0:(ty<0?XYE_VERT-1:ty);
            }
            else if (( tx>=XYE_HORZ) || (tx<0) || (ty>=XYE_VERT) || (ty<0))
            {
                go =false;
            }

            if (go)
            {
                sq=game::Square(tx,ty);
                go=sqMem::mem[tx][ty];
            }

            if (go)
            {
                object=sq->object;
                go=(object==NULL);
                if (! go)
                {
                    ot=object->GetType();
                    if (cond(sq))
                    {
                        go=true;
                    }
                    else switch(ot)
                    {
                        case(OT_BLACKHOLE): case(OT_MINE): case(OT_FIREBALL): go=ignoreSp; break;
                        case(OT_TELEPORT):
                            //Found a teleport
                            if (ignoreSp || considerTeleports)
                            {

                                savemem=false;
                                tele=static_cast<teleport*>(object);

                                if  (tele->dir==Opposite(godir[i]) )
                                {
                                    //Possible to go through the teleport!
                                    if (ignoreSp) go=true; // Will try to go there
                                    else
                                    {
                                        go=tele->FindCleanExit(this, nx, ny, true);
                                        if (go)
                                        {
                                            go= sqMem::mem[nx][ny];
                                            sq=game::Square(nx,ny);
                                            sqMem::mem[tx][ty]=false; //So we don't test the teleport entrance again
                                            tx=nx;
                                            ty=ny;
                                            sqMem::mem[tx][ty]=false; //So we don't test the teleport exit again

                                        }
                                    }
                                }


                            }
                    }


                }
                if (go)
                {
                    gobject=sq->gobject;
                    go= ((gobject==NULL) || ((gobject->CanEnter(this,godir[i]))  && ( (!pit::IsEnabledInstance(gobject)) || (! pit::CanConsume(this) )    )  ) ); //everything dislikes pits
                }

            }


            //begin sticky check:
            if (go && considerStickies) go= ((cond(sq)) || (! SpotAffectedByAMagnetic(this,tx,ty) ));


            if (go)
            {
                if (cond(sq))
                {

                    //We found a correct one!
                    if (step<0) //Xye was just in front of the object, do nothing
                        res[0]=godir[i];
                    else
                        res[0]=godir[step];
                    halt=true;
                    ret=true;


                }
                else if ((range==0) || (stepsNumber<=range))
                {
                    if (savemem) sqMem::mem[tx][ty]=false; //So we don't go here anymore.

                    //Add the point to the list:
                    tm= new AIList;
                    tm->next=NULL;
                    tm->steps=stepsNumber;
                    if (step<0) //First point in that direction
                    {
                        allow[i]=true;
                        //In case there is no path found, this will help the random decision
                        //know which direction is allowed;
                        tm->firstStep=i;
                    }
                    else
                        tm->firstStep= step;
                    tm->x=tx;
                    tm->y=ty;

                    //Make it the new end:
                    end->next=tm;
                    end=tm;
                }

            }
            else if ( ((tx>=0) && (tx<XYE_HORZ) && (ty>=0) && (ty<XYE_VERT)) && savemem)
                 sqMem::mem[tx][ty]=false; //So we don't check the same square over and over


        }

        next=curr->next;
        delete curr;

    } while (next!=NULL);



    if (! ret)
    {
        if (considerStickies)
        {
            //soon, make it check if those place are actually safe instead of just halting

            //The only danger are green stickies.

            for (i=0;i<4;i++) if (allow[i])
            {
                tx=cx+gox[i];ty=cy+goy[i];
                tx=tx>=XYE_HORZ?0:(tx<0?XYE_HORZ-1:tx);
                ty=ty>=XYE_VERT?0:(ty<0?XYE_VERT-1:ty);
                allow[i] = (! SpotGetAntiStickies(tx,ty,up,dw,lf,rg));
            }


        }

        //Choose a random, valid direction.


        ret=allow[0];
        i=1;
        while ((! ret) && (i<4))
        {
            ret=allow[i];
            i++;
        }
        resn=0;
        if (ret)
               for (i=0;i<4;i++)
                if (allow[i])
                {
                    res[resn++]=godir[i];
                }
        foundpath=false;
    }
    else
    {
        foundpath=true;
        resn=1;
    }

    return(ret);
}






bool obj::AffectedByMagnetism(bool horz)
{
    switch(type)
    {
        case(OT_XYE): case(OT_ROBOXYE): case(OT_WALL):
        case(OT_EARTH): case(OT_AUTO): case(OT_BLACKHOLE): case(OT_GEM):
        case(OT_FILLER): case(OT_SNIPER): case(OT_RATTLER): case(OT_RATTLERNODE):
             return false;

        case(OT_WINDOW): return windowblock::CanMove(static_cast<windowblock*>(this));

        case(OT_MAGNETIC):
            magnetic* mg=static_cast<magnetic*>(this);
            return (mg->IsHorizontal()!=horz);
    }

 return true;
}

/**End Class Object**/

/**Class Ground Object**/
char gobj::X() { return x; }
char gobj::Y() { return y; }

void gobj::GObjectConstruct(square* sq)
{
     id=game::NewId();
     x=sq->sqx;y=sq->sqy;
     sq->gobject=this;
     RenderAfterObjects=false;
     if (sq->object!=NULL) OnEnter(sq->object);
}

void gobj::move(char px,char py)
{
    game::Square(x,y)->gobject=NULL;
    x=px;
    y=py;
    game::Square(x,y)->gobject=this;
}



void gobj::Kill()
{
    //UpdateSquare();
    square* sq=game::Square(x,y);
    sq->Update=true;
    sq->gobject=NULL;

    OnDeath();
    delete this;
}




/**End Class Ground Object**/




/**Class xye**/
bool xye::useDirectionSprites = false;
void InitRoundRandom(int w, int z);
xye::xye(square* sq)
{
    alpha=255;
    lives=4;
    type=OT_XYE;
    checkpoint=sq;
    deadtic=0;
    moved=false;
    ObjectConstruct(sq);
    InitRoundRandom(checkpoint->sqx, checkpoint->sqy);
}

void xye::Draw(unsigned int x, unsigned int y)
{
    if (lives>0)
    {
        Uint8 tx=0,ty=0;
        
        if(moved && xye::useDirectionSprites) switch(lastdir)
        {
            case D_UP: tx=9,ty=19; break;
            case D_DOWN: tx=7,ty=19; break;
            case D_LEFT: tx=8,ty=19; break;
            case D_RIGHT: tx=6,ty=19; break;
        }
        
        
        Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);

        D.SetColors( &game::PlayerColor,alpha);

        if (alpha<255)
            alpha = (alpha<=223)?alpha+32:255;

        D.Draw(game::screen,x,y);
    }

}

inline bool xye::trypush(edir dir,obj* pusher)
{
    if (pusher->GetType()==OT_ROBOXYE) return game::TryMoveXye(dir);
 return false;
}

inline bool xye::HasRoundCorner(roundcorner rnc) { return false; }

inline unsigned char xye::GetLives() { return(lives); }

void xye::SetLives(unsigned char nlives)
{
    if (nlives==0)
        game::TerminateGame();
    lives=nlives;
}

inline void xye::SetCheckPoint(square* sq) { checkpoint=sq; }
void xye::GetCheckPoint(char &cx, char &cy)
{
    cx=checkpoint->sqx;
    cy=checkpoint->sqy;
}

inline bool xye::HasBlockColor(blockcolor bc) { return false; }

void xye::Kill()
{
    if(!lives) return;
    unsigned int counter=game::Counter();
    if (counter!=deadtic)
    {
        deadtic=counter;
        if (lives>1)
        {
            lives--;
            game::deathsq1=game::Square(x,y);
            char cx,cy,nx,ny;
            GetCheckPoint(cx,cy);
            if (game::GetRevivePoint(cx,cy,nx,ny))
                move(nx,ny);
            else game::Error(  "Level is full!");
            game::FlashXyePosition();

        }
        else
        {
            game::deathsq1=game::Square(x,y);
            lives=0;
            game::TerminateGame();

               square* sq=game::Square(x,y);
            gobj *gobject = sq->gobject;
            if (gobject!=NULL) gobject->OnLeave(this);
            sq->object=NULL;

            recycle::add(this);
        }
    }
}

/**End class xye**/


/**Class Robo- xye**/
roboxye::roboxye(square* sq)
{
    anim = GetRandomInt(0,2);
    type=OT_ROBOXYE;
    ObjectConstruct(sq);
}

void roboxye::Draw(unsigned int x,unsigned int y)
{
    Drawer D(game::sprites,3*sz,anim*sz,sz,sz);
    D.Draw(game::screen,x,y);
}

inline bool roboxye::trypush(edir dir,obj* pusher) {
    if (pusher->GetType()==OT_XYE)
        return trypush_common(dir,pusher, false,NULL);
 return (false);
}

inline bool roboxye::HasRoundCorner(roundcorner rnc) { return false; }

inline bool roboxye::HasBlockColor(blockcolor bc) { return false; }


bool roboxye::Loop(bool* died)
{
    UpdateSquare(); //One of the few objects that are always drawn
    *died=false;
    bool Moved=false;


    bool pushed=false;
    bool go=false;

    //unsigned int ClockTic=GM->Counter();
    if (game::AllowRoboXyeMovement()) {
        
        anim = (anim+GetRandomInt(1,2)) % 3;

        edir D[4]= { D_UP, D_DOWN, D_LEFT, D_RIGHT };
        unsigned char DN=4,i,j;
        char XYEx=game::XYE->X();
        bool prihorz=game::LastMoveWasHorizontal();
        char XYEy=game::XYE->Y();
        edir fac;
        while (PickDirectDir(fac,D,DN,x,y,XYEx,XYEy,true,prihorz))
        {
            go=false;
            //if  (DoMagnetism(true,true,&Moved)) return Moved;
            char dx=x,dy=y;
            switch(fac)
            {
                case(D_UP): dy++;break;
                case(D_DOWN): dy--;break;
                case(D_RIGHT): dx++;break;
                default: dx--;
            }

            dx=(dx>=XYE_HORZ?0:(dx<0?XYE_HORZ-1:dx));
            dy=(dy>=XYE_VERT?0:(dy<0?XYE_VERT-1:dy));
            square* sq=game::Square(dx,dy);
            obj* object=sq->object;
            teleport* tele; dangerous* blck;

            char nx=0,ny=0;

            if (object==NULL)
                go=true;
            else
            {

                switch (object->GetType())
                {
                    case(OT_TELEPORT):
                        tele=static_cast<teleport*>(object);
                        go = tele->tryteleport(fac, this, nx, ny ,blck, &pushed);

                        if (go) { dx=nx; dy=ny;sq=game::Square(dx,dy); object=sq->object; }
                        else if (blck!=NULL)
                        {
                            if (blck->Busy(this)) go=false;
                            else
                            {
                                //Kill The Bot
                                blck->Eat();
                                Kill(blck->GetType()==OT_BLACKHOLE); //because of a blackhole
                                *died=true;
                                return true;
                            }
                        }
                        break;
                    case(OT_BLACKHOLE): case(OT_MINE): case(OT_FIREBALL):
                        blck=static_cast<dangerous*>(object);
                        if (blck->Busy(this)) go=false;
                        else
                        {
                            //Kill The impacter
                            blck->Eat();
                            Kill(blck->GetType()==OT_BLACKHOLE); //because of a blackhole
                            *died=true;

                            return true;
                        }
                        break;
                }

                if (! go)
                {
                    if (object==NULL)
                          go=true;
                    else if (object->trypush(fac,this))
                    {
                        go=true;
                        pushed=true;
                    }

                }
            }

            if (go)
            {

                gobj* gobject=sq->gobject;
                if ((gobject==NULL) || (gobject->CanEnter(this,fac)))
                {
                    move(dx,dy);
                    break;
                }

            }
            if (pushed) break; //It actually pushed an object, but couldn't go because of a ground object.

            //Remove failed dir from the array then try again.
            j=DN;
            DN=0;
            for (i=0;i<j;i++)
                if (D[i]!=fac)
                {
                    DN++;
                    D[DN-1]=D[i];
                }



        }
    }

    return true;
}







/**End class Robo-xye**/


/**Class wall**/
SDL_Color wall::DefaultColor[wall::MAX_VARIATIONS];
unsigned char wall::defkind=0;

void wall::ResetDefaults()
{
    for (int i=0; i<MAX_VARIATIONS; i++)
        DefaultColor[i] = options::WallColor[i];
    defkind=0;
}

void wall_multiplyColor(Uint8 a, Uint8 b, Uint8 &c)
{
    int t = a;
    t = (t*b/ 255.0 );
    c=(unsigned char)(t);
    
}

void wall_convertColor(Uint8 sprite, Uint8 wanted, Uint8 &c)
{
    if(sprite==255) {
        c=wanted;
    } else {
        int x = 255.0*(int)(wanted) / (int)(sprite);
        x = std::min<int>(x,255);
        c = (Uint8)(x);
    }
}


void wall::ChangeColor(Uint8 nR, Uint8 nG, Uint8 nB, bool multiply)
{
    if(multiply)
    {
        wall_multiplyColor( options::WallColor[kind].r, nR, R);
        wall_multiplyColor( options::WallColor[kind].g, nG, G);
        wall_multiplyColor( options::WallColor[kind].b, nB, B);
    }
    else
    {
        wall_convertColor( options::WallSpriteColor[kind].r, nR, R);
        wall_convertColor( options::WallSpriteColor[kind].g, nG, G);
        wall_convertColor( options::WallSpriteColor[kind].b, nB, B);
    }
}

void wall::SetDefaultColor(SDL_Color cc, bool multiply, int var)
{
    if(multiply)
    {
        wall_multiplyColor( options::WallColor[var].r, cc.r, DefaultColor[var].r);
        wall_multiplyColor( options::WallColor[var].g, cc.g, DefaultColor[var].g);
        wall_multiplyColor( options::WallColor[var].b, cc.b, DefaultColor[var].b);
    }
    else
    {
        wall_convertColor( options::WallSpriteColor[var].r, cc.r, DefaultColor[var].r);
        wall_convertColor( options::WallSpriteColor[var].g, cc.g, DefaultColor[var].g);
        wall_convertColor( options::WallSpriteColor[var].b, cc.b, DefaultColor[var].b);
    }
    RecolorCache::savecolor(&DefaultColor[var]);
}


void wall::SetDefaultColor(SDL_Color cc, bool multiply)
{
    SetDefaultColor(cc,multiply, 0);
    for (int i=1;i<MAX_VARIATIONS; i++)
        DefaultColor[i] = DefaultColor[0];
}




void wall::SetDefaultType(signed int def)
{
    defkind= def>=MAX_VARIATIONS? 0:def<0?0:def;
}

wall::wall(square* sq,unsigned char t)
{
     round7=round1=round3=round9=false;
    type=OT_WALL;
    if(t>=MAX_VARIATIONS) t=0;
    ChangeKind(t);

    R=DefaultColor[t].r;
    G=DefaultColor[t].g;
    B=DefaultColor[t].b;
    ObjectConstruct(sq);
}

wall::wall(square* sq)
{
     round7=round1=round3=round9=false;
    type=OT_WALL;
    kind=defkind;
    R=DefaultColor[defkind].r;
    G=DefaultColor[defkind].g;
    B=DefaultColor[defkind].b;
    ObjectConstruct(sq);


}


void wall::ChangeKind(unsigned char t)
{
    kind=t;
    if(kind>=MAX_VARIATIONS) kind=0;
    R=DefaultColor[kind].r;
    G=DefaultColor[kind].g;
    B=DefaultColor[kind].b;
}

bool wall::containsRoundCorner()
{
    return (round1||round7||round9||round3);
}

void wall::Draw(unsigned int x, unsigned int y)
{
    Drawer D(game::sprites,0,0,0,0);
    int sz2=sz/2;
    Sint16 ty;
    ty=sz*(kind);
    D.SetColors(R,G,B,255);
    
    char px=this->x, py=this->y;
    char rx=px+1, lx=px-1, uy=py+1, dy=py-1;
    if(rx>=XYE_HORZ) rx=0;
    if(uy>=XYE_VERT) uy=0;
    if(lx<0) lx=XYE_HORZ-1;
    if(dy<0) dy=XYE_VERT-1;
    
    bool up =   (find( px, uy, kind)!=NULL);
    bool down = (find( px, dy, kind)!=NULL);
    bool left = (find( lx, py, kind)!=NULL);
    bool right = (find( rx, py, kind)!=NULL);

    bool upright = find(rx,uy,kind);
    bool downright =find(rx,dy,kind);
    bool upleft = find(lx,uy,kind);
    bool downleft = find(lx,dy,kind);


    
    up = up && !round7 && !round9;
    down = down && !round1 && !round3;
    right = right && !round9 && !round3;
    left = left && !round7 && !round1;
    
    bool inborder = (!left||!up||!right||!down);
    if( !inborder && (!upright || !upleft || !downright ||!downleft) )
    {
        /*inborder = !(   find( px, uy, kind)->containsRoundCorner() 
                    || find( px, dy, kind)->containsRoundCorner()
                    || find( lx, py, kind)->containsRoundCorner()
                    || find( rx, py, kind)->containsRoundCorner() );*/
        inborder=true;

    }
    

    if (round7)
        D.ChangeRect(10*sz,ty,sz2,sz2);
    else if( up && left && !inborder)
        D.ChangeRect(15*sz,ty,sz2,sz2);
    else if(up&&left&&upleft)
        D.ChangeRect(14*sz,ty,sz2,sz2);
    else if(up&&left)
        D.ChangeRect(13*sz,ty,sz2,sz2);
    else if ( up)
        D.ChangeRect(12*sz,ty,sz2,sz2);
    else if ( left)
        D.ChangeRect(11*sz,ty,sz2,sz2);
    else
        D.ChangeRect(9*sz,ty,sz2,sz2);
    

    D.Draw(game::screen,x,y);

    if (round9)
        D.ChangeRect(21*sz2,ty,sz2,sz2);
    else if( up && right && !inborder)
        D.ChangeRect(15*sz+sz2,ty,sz2,sz2);
    else if(up&&right&&upright)
        D.ChangeRect(14*sz+sz2,ty,sz2,sz2);
    else if(up&&right)
        D.ChangeRect(13*sz+sz2,ty,sz2,sz2);
    else if ( up)
        D.ChangeRect(12*sz+sz2,ty,sz2,sz2);
    else if ( right)
        D.ChangeRect(11*sz+sz2,ty,sz2,sz2);
    else
        D.ChangeRect(19*sz2,ty,sz2,sz2);

    //D.SetColors(255,255,255,50);
    D.Draw(game::screen,x+sz2,y);
    //D.SetColors(255,255,255,255);

    if (round1)
        D.ChangeRect(10*sz,ty+sz2,sz2,sz2);
    else if( down && left && !inborder)
        D.ChangeRect(15*sz,ty+sz2,sz2,sz2);
    else if(down&&left&&downleft)
        D.ChangeRect(14*sz,ty+sz2,sz2,sz2);
    else if(down&&left)
        D.ChangeRect(13*sz,ty+sz2,sz2,sz2);
    else if ( down)
        D.ChangeRect(12*sz,ty+sz2,sz2,sz2);
    else if ( left)
        D.ChangeRect(11*sz,ty+sz2,sz2,sz2);
    else
        D.ChangeRect(9*sz,ty+sz2,sz2,sz2);

    
    D.Draw(game::screen,x,y+sz2);

    if (round3)
        D.ChangeRect(21*sz2,ty+sz2,sz2,sz2);
    else if( down && right && !inborder)
        D.ChangeRect(15*sz+sz2,ty+sz2,sz2,sz2);
    else if(down&&right&&downright)
        D.ChangeRect(14*sz+sz2,ty+sz2,sz2,sz2);
    else if(down&&right)
        D.ChangeRect(13*sz+sz2,ty+sz2,sz2,sz2);
    else if ( down)
        D.ChangeRect(12*sz+sz2,ty+sz2,sz2,sz2);
    else if ( right)
        D.ChangeRect(11*sz+sz2,ty+sz2,sz2,sz2);
    else
        D.ChangeRect(19*sz2,ty+sz2,sz2,sz2);

    D.Draw(game::screen,x+sz2,y+sz2);
}


bool wall::Loop(bool* died)
{
    *died=false;
    return true;
}

bool wall::trypush(edir dir,obj* pusher) { return false; }

wall* wall::find(char sx, char sy, unsigned char kind)
{
    square *sq=game::SquareN(sx,sy);
    obj* object;
    if (( object=sq->object) && (object->GetType()==OT_WALL))
    {
        wall* wl = static_cast<wall*>(object);
        if( (kind==6) || (wl->kind==6) || (wl->kind==kind))
            return (wl);
    }
    return(NULL);

}

void wall::IntelligentUpdateCorners(wall* by)
{
    switch(PickDirectDir(x,y,by->X(),by->Y(),true,true))
    {
        case(D_DOWN): round1=round3=false; break;
        case(D_LEFT): round1=round7=false; break;
        case(D_RIGHT): round3=round9=false; break;
        default: round7=round9=false; break;
    }
    UpdateSquare();
}

void wall::SetRoundCorners(bool r7, bool r1, bool r3, bool r9)
{
    round7=r7;    round1=r1;    round3=r3;    round9=r9;
}

bool wall::HasRoundCorner(roundcorner rnc)
{
    switch(rnc)
    {
      case(RC_1): return round1;
      case(RC_3): return round3;
      case(RC_7): return round7;
    }
    return round9;
}

inline bool wall::HasBlockColor(blockcolor bc) { return false; }

inline bool wall::ResistsFire()
{
    if(kind==6) return false;
    return (kind>3);
}
/**End class wall**/

unsigned int BC2Color(blockcolor bc)
{
    switch(bc)
    {
        case(B_YELLOW): return(0xFFFFFF00);
        case(B_RED): return(0xFFFF0000);
        case(B_BLUE): return(0xFF0000FF);
        default: return(0xFF00AA00);
    }
}

unsigned int BC2FColor(blockcolor bc)
{
    switch(bc)
    {
        case(B_YELLOW): return(0xFFFF0000);
        case(B_RED): return(0xFFFFFF00);
        case(B_BLUE): return(0xFF00FFFF);
        default: return(0xFFFFFFFF);
    }
}


/** General Stuff (Round alg)**/

Uint32 round_random_w = 452;
Uint32 round_random_z = 777;
void InitRoundRandom(int w, int z) {
    round_random_w = w;
    round_random_z = z;
}


Uint32 GetRandom32() {
    round_random_z = 36969 * (round_random_z & 65535) + (round_random_z >> 16);
    round_random_w = 18000 * (round_random_w & 65535) + (round_random_w >> 16);
    return (round_random_z << 16) + round_random_w;  /* 32-bit result */
}


square* RoundAvance_Sub(obj* ToMove,
     edir dir,char wx, char wy,
     roundcorner rca, edir adir, char ax1, char ay1, char ax2, char ay2,
     roundcorner rcb, edir bdir, char bx1, char by1, char bx2, char by2)
{
    //HGE* hge=game::hge;
    square* osq;
    obj* object;
    gobj* gobject;
    square *sq11,*sq12,*sq21,*sq22;
    bool a,b;

    osq=game::SquareN(wx,wy);
    if (Allowed(ToMove,dir,osq)) return osq;
    object=osq->object;
    if (! object) return NULL;
    a = object->HasRoundCorner(rca);
    if (a)
    {
        sq11=game::SquareN(ax1,ay1);
        sq12=game::SquareN(ax2,ay2);
        a= (Allowed(ToMove,adir,sq11) && Allowed(ToMove,dir,sq12) );
    }
    b = object->HasRoundCorner(rcb);
    if (b)
    {
        sq21=game::SquareN(bx1,by1);
        sq22=game::SquareN(bx2,by2);
        b= (Allowed(ToMove,bdir,sq21) && Allowed(ToMove,dir,sq22) );
    }
    if (a && b) {
        int x = (GetRandom32()+game::Counter()+ax1*XYE_VERT+ay1)%120;
        if  ( x<60 ) {
            return sq12;
        } else {
            return sq22;
        }
    } else if (a) {
        return sq12;
    } else if (b) {
        return sq22;
    } else {
        return NULL;
    }



}

square* RoundAdvance(obj* ToMove,edir dir,char i, char j)
{

    //HGE* hge=game::hge;
    square* osq;
    obj* object;
    gobj* gobject;

    square *sq11,*sq12,*sq21,*sq22;

    bool a=false,b=false;
    switch(dir)
    {
          //RoundAvance_Sub does all the checks for us
        case(D_UP): return
             RoundAvance_Sub(ToMove,D_UP,i,j+1,
             RC_1,D_LEFT,i-1,j,i-1,j+1,
             RC_3,D_RIGHT,i+1,j,i+1,j+1);

        case(D_DOWN): return
             RoundAvance_Sub(ToMove,D_DOWN,i,j-1,
             RC_7,D_LEFT,i-1,j,i-1,j-1,
             RC_9,D_RIGHT,i+1,j,i+1,j-1);

        case(D_LEFT): return
             RoundAvance_Sub(ToMove,D_LEFT,i-1,j,
             RC_9,D_UP  ,i  ,j+1,i-1,j+1,
             RC_3,D_DOWN,i  ,j-1,i-1,j-1);

    }
      /*D_RIGHT*/     return
             RoundAvance_Sub(ToMove,D_RIGHT,i+1,j,
             RC_7,D_UP  ,i  ,j+1,i+1,j+1,
             RC_1,D_DOWN,i  ,j-1,i+1,j-1);
}

/**(Round alg end)**/


/**Start Class Block**/
block::block(square* sq,blockcolor tc,bool makeround)
{
    round=makeround;
    type=OT_BLOCK;
    c=tc;
    colorless=false;
    ObjectConstruct(sq);
}

void block::Draw(unsigned int x, unsigned int y,bool colorless, blockcolor c, bool round)
{
    Drawer D(game::sprites,0, 0 ,0,0);
    if (round)
        D.ChangeRect(game::GRIDSIZE*2 , 0 , game::GRIDSIZE, game::GRIDSIZE);
    else
        D.ChangeRect(game::GRIDSIZE , 0 , game::GRIDSIZE, game::GRIDSIZE);

    if (! colorless) D.SetColors( &(options::BKColor[c]),255);
    D.Draw(game::screen,x,y);
}

void block::Draw(unsigned int x, unsigned int y)
{
    Draw(x,y,colorless,c,round);
}

bool block::trypush(edir dir,obj* pusher) { return trypush_common(dir,pusher, round,NULL); }

bool block::HasRoundCorner(roundcorner rnc) { return round; }

bool block::Loop(bool* died)
{

    *died=false;

    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved))
        return Moved;
        edir go;

    return false;
}

inline bool block::HasBlockColor(blockcolor bc) { return ((! colorless) && (c==bc)); }

/**End Class Block**/

/**Start Class Large Block**/
largeblock::largeblock(square* sq,blockcolor tc, bool up, bool right, bool down, bool left )
{
    type=OT_LARGEBLOCK;
    c=tc;
    colorless=false;
    ObjectConstruct(sq);
    flags = (up<<1)|(right<<3)|(down<<5)|(left<<7) ;
    root = NULL;
}


void largeblock::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    Uint8 sz2 = sz>>1;
    //flags = 0b1101;
    
    
    Uint8 up = (flags>>1)&1;
    Uint8 right = (flags>>3)&1;
    Uint8 down = (flags>>5)&1;
    Uint8 left = (flags>>7)&1;
    
    Uint8 upleft = (flags>>0)&1;
    Uint8 upright = (flags>>2)&1;
    Uint8 downright = (flags>>4)&1;
    Uint8 downleft = (flags>>6)&1;
    
    //top left corner:   
    Uint8 var = 0;
    if( up&&left&&upleft) var=4;
    else if( up&&left) var = 3;
    else if(up) var = 2;
    else if(left) var = 1;
    
    tx = 10;
    ty = var + 15;
    Drawer D(game::sprites, tx*sz,  ty*sz,sz2,sz2);
    Uint8 alpha = 255, white=255;   
    if(! colorless) D.SetColors(&options::BKColor[(int)(c)],alpha);
    else D.SetColors(white,white,white,alpha);
    D.Draw(game::screen,x,y);

    //top right corner:   
    var = 0;
    if( up&&right&&upright) var=4;
    else if( up&&right) var = 3;
    else if(up) var = 2;
    else if(right) var = 1;

    tx = 10;
    ty = var + 15;
    D.ChangeRect(tx*sz+sz2, ty*sz, sz2,sz2);
    D.Draw(game::screen,x+sz2,y);

    //bottom left corner:   
    var = 0;
    if( down&&left&&downleft) var=4;
    else if( down&&left) var = 3;
    else if(down) var = 2;
    else if(left) var = 1;
    
    tx = 10;
    ty = var + 15;
    D.ChangeRect(tx*sz,  ty*sz + sz2,sz2,sz2);
    D.Draw(game::screen,x,y+sz2);

    //bottom right corner:   
    var = 0;
    if( down&&right&& downright) var=4;
    else if( down&&right) var = 3;
    else if(down) var = 2;
    else if(right) var = 1;

    tx = 10;
    ty = var + 15;
    D.ChangeRect(tx*sz+sz2, ty*sz +sz2, sz2,sz2);
    D.Draw(game::screen,x+sz2,y+sz2);

}

inline bool largeblock::HasRoundCorner(roundcorner rnc) { return false; }

#include<queue>
#include<iostream>
using std::cout;
using std::endl;
std::queue<largeblock* > foundLargeBlocks;

void largeblock::blockDFS( largeblockroot* aroot)
{
    
    if( root != NULL) return;
    root = aroot;
    root->children ++;
    foundLargeBlocks.push(this);
    int dy[4] = {1,0,-1,0};
    int dx[4] = {0,1,0,-1};
    int dd[4] = {1,3,5,7};
    int dr[4] = {5,7,1,3};
    for (int i=0; i<4; i++)
    {
        int nx = x+dx[i], ny=y+dy[i];
        if(nx<0) nx = XYE_HORZ-1;
        if(ny<0) ny = XYE_VERT-1;
        if(ny>=XYE_VERT) ny = 0;
        if(nx>=XYE_HORZ) nx = 0;
        square* sq= game::Square(nx,ny);
        obj* ob = sq->object;
        if( (ob == NULL)  || (ob->GetType() != OT_LARGEBLOCK) )
            continue;
        largeblock* lb = static_cast<largeblock*>(ob);
        Uint8 nflags = lb->flags;
        if( ( (flags&(1<<dd[i])) && (nflags&(1<<dr[i])) ) && ( (lb->colorless==colorless) && (lb->c == c) ) )
        {
            lb->blockDFS(root);
        }
        
    }
}
void largeblock::setupBlock()
{
    largeblockroot* troot = new largeblockroot();
    troot->children =0;
    blockDFS(troot);
    while( !foundLargeBlocks.empty() )
    {
        largeblock* lb = foundLargeBlocks.front();
        
        foundLargeBlocks.pop();
        int x= lb->x, y=lb->y;
        game::Square(x,y)->Update=true;
        int dy[8] = { 1,1,1,0,-1,-1,-1, 0};
        int dx[8] = {-1,0,1,1, 1, 0,-1,-1};
        lb->flags = 0;
        for (int i=0;i<8; i++)
        {
            int nx = x+dx[i], ny=y+dy[i];
            if(nx<0) nx = XYE_HORZ-1;
            if(ny<0) ny = XYE_VERT-1;
            if(ny>=XYE_VERT) ny = 0;
            if(nx>=XYE_HORZ) nx = 0;
            square* sq= game::Square(nx,ny);
            obj* ob = sq->object;
            if( (ob == NULL)  || (ob->GetType() != OT_LARGEBLOCK) )
                continue;
            largeblock* lb2 = static_cast<largeblock*>(ob);
            if( (lb->root == lb2->root) && ( (lb2->colorless==lb->colorless) && (lb->c == lb2->c) )  )
            {               
                lb->flags |= (1<<i);
            }
            
        }
        
    }
}

bool largeblock::Loop(bool* died)
{
    if(root == NULL)
    {
        setupBlock();       
    }

    if( !game::Mod5() || !game::Mod2() ) return false;
    *died=false;

    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved))
        return Moved;
        edir go;

    return false;
}
largeblock* largeblock::getPart( obj* object, largeblockroot* root)
{
    if(object == NULL) return NULL;
    if(object->GetType() != OT_LARGEBLOCK) return NULL;
    largeblock* lb = static_cast<largeblock*>(object);
    if (lb->root == root) return lb;
    return NULL;
    
}

bool largeblock::pushingBlocks(edir dir, int ix, int x0, int x1, int iy, int y0, int y1, int dx ,int dy)
{
    bool doable = true;
    for (int i=x0; (i-ix!= x1) && doable; i+=ix)
        for (int j=y0; (j-iy!=y1)  && doable ; j+=iy)
        {
            largeblock * lb = getPart( game::Square(i,j)->object, root);
            if(lb==NULL) continue;
            gobj* gobject = game::Square(i,j)->gobject;
            if( (gobject!=NULL) && (! gobject->CanLeave(lb, dir)) ) doable=false;
            
            if(lb->tic == game::Counter())
                doable=false;
           
            Sint8 nx = lb->x + dx, ny = lb->y + dy;
            /*if(nx<0) nx=XYE_HORZ-1;
            if(ny<0) ny=XYE_VERT-1;
            if(nx>=XYE_HORZ) nx = 0;
            if(ny>=XYE_VERT) ny = 0;*/
            if(nx<0) doable=false;
            if(ny<0) doable=false;
            if(nx>=XYE_HORZ) doable=false;
            if(ny>=XYE_VERT) doable=false;
            if(! doable) break;
            obj * object = game::Square(nx,ny)->object;

            if( (object!=NULL) && (getPart(object,root)==NULL) )
                doable=false;
                
            gobject = game::Square(nx,ny)->gobject;
            if( (gobject!=NULL) && (! gobject->CanEnter(lb, dir)) ) doable=false;


        }
    if(! doable) return false;

    for (int i=x0; i-ix!= x1; i+=ix)
        for (int j=y0; j-iy!=y1; j+=iy)
        {
            largeblock * lb = getPart( game::Square(i,j)->object, root);
            if(lb==NULL) continue;
            if(lb->tic == game::Counter() ) continue;
            Sint8 nx = lb->x + dx, ny = lb->y + dy;
            
            if(nx<0) nx=XYE_HORZ-1;
            if(ny<0) ny=XYE_VERT-1;
            if(nx>=XYE_HORZ) nx = 0;
            if(ny>=XYE_VERT) ny = 0;

            lb->move(nx,ny);
            lb->tic = game::Counter() ;
        }
    
    return true;
    
}

largeblock* largeblock_pushgroup[XYE_HORZ*XYE_VERT];
int         largeblock_pushn = 0;

largeblock* largeblock_stack[XYE_HORZ*XYE_VERT];
int         largeblock_stackn = 0;

void largeblock::getPushGroup()
{
    char checked[XYE_HORZ][XYE_VERT] = {};
    largeblock_pushn = 0;
    largeblock_stackn=1;
    largeblock_stack[0]= this;
    checked[x][y]=1;
    while(largeblock_stackn>0)
    {
        largeblock* lb = largeblock_stack[--largeblock_stackn];
        largeblock_pushgroup[ largeblock_pushn++] = lb;
        int x=lb->x, y=lb->y;
        int dx[4]={0,0,1,-1};
        int dy[4]={-1,1,0,0};
        for (int d=0; d<4; d++)
        {
            int nx=dx[d]+x, ny=dy[d]+y;
            if(nx<0) nx=XYE_HORZ-1;
            if(ny<0) ny=XYE_VERT-1;
            if(nx>=XYE_HORZ) nx = 0;
            if(ny>=XYE_VERT) ny = 0;
            largeblock * lb = getPart(game::Square(nx,ny)->object,root);
            if( (checked[nx][ny]==0) && (lb!=NULL) )
            {
                checked[nx][ny] = 1;
                largeblock_stack[ largeblock_stackn++] = lb;
            }
        }
    }
    
}

void largeblock::doPush(edir dir, int dx, int dy)
{
    char checked[XYE_HORZ][XYE_VERT] = {};
    largeblock_stackn=0;
    for (int i=0; i<largeblock_pushn; i++)
    {
        largeblock_stack[ largeblock_stackn ++]= largeblock_pushgroup[i];
        while(largeblock_stackn > 0)
        {
            largeblock* lb = largeblock_stack[--largeblock_stackn];
            int x = lb->x, y=lb->y;
            if(checked[x][y]==1) continue;
            bool performMove= true;
            int nx = x+dx, ny = y+dy;
            if(nx<0) nx=XYE_HORZ-1;
            if(ny<0) ny=XYE_VERT-1;
            if(nx>=XYE_HORZ) nx = 0;
            if(ny>=XYE_VERT) ny = 0;

            if( checked[x][y] == 0)
            {
                //start
                checked[x][y] = 2;
                if(checked[nx][ny]==2) //found a cycle!
                {
                    performMove = false;
                    checked[x][y]=1;
                    Uint8 cflags = lb->flags;
                    largeblock * prev = lb;
                    lb->tic = game::Counter();
                    game::Square(x,y)->Update=true;
                    while(largeblock_stackn > 0)
                    {
                        lb = largeblock_stack[--largeblock_stackn];
                        lb->tic = game::Counter();
                        x=lb->x, y=lb->y;
                        checked[x][y] = 1;
                        game::Square(x,y)->Update=true;
                        prev->flags = lb->flags;
                        prev = lb;
                    }
                    prev->flags = cflags;
                }
                else if(checked[nx][ny]==0)
                {
                    obj* object = game::Square(nx,ny)->object;
                    largeblock * lb2= getPart(object, root);
                    if(lb2!=NULL)
                    {
                        performMove=false;
                        ++largeblock_stackn;
                        largeblock_stack[largeblock_stackn ++] = lb2;
                    }
                    else if( (object!=NULL) && ((object->GetType() == OT_BLACKHOLE) || (object->GetType() == OT_MINE)) )
                    {
                        lb->Kill();
                        static_cast<dangerous*>(object)->Eat();
                        performMove=false;
                        checked[nx][ny] = 1;
                        checked[x][y]=1;
                    }
                }
            }
            if(performMove)
            {
                lb->tic = game::Counter();
                lb->move(nx,ny);
                checked[nx][ny] = 1;
                checked[x][y]=1;
            }             
        } 
    }

}

bool largeblock::canPush(edir dir, int dx, int dy)
{
    char checked[XYE_HORZ][XYE_VERT] = {};
    largeblock_stackn=0;
    for (int i=0; i<largeblock_pushn; i++)
    {
        largeblock_stack[ largeblock_stackn ++]= largeblock_pushgroup[i];
        while (largeblock_stackn > 0)
        {
            largeblock* lb = largeblock_stack[--largeblock_stackn];
            int x = lb->x, y=lb->y;
            if(checked[x][y]==1) continue;
            gobj* gobject = game::Square(x,y)->gobject;
            if( (gobject!=NULL) && (! gobject->CanLeave(lb, dir)) )
                return false;

            int nx = x+dx, ny = y+dy;
            if(nx<0) nx=XYE_HORZ-1;
            if(ny<0) ny=XYE_VERT-1;
            if(nx>=XYE_HORZ) nx = 0;
            if(ny>=XYE_VERT) ny = 0;

            gobject = game::Square(nx,ny)->gobject;
            if( (gobject!=NULL) && (! gobject->CanEnter(lb, dir)) )
                return false;

            if( checked[x][y] == 0)
            {
                //start
                checked[x][y] = 2;
                if(checked[nx][ny]==2); //found a cycle, do nothing
                else if(checked[nx][ny]==0)
                {
                    obj* object = game::Square(nx,ny)->object;
                    largeblock * lb2= getPart(object, root);
                    if(lb2!=NULL)
                    {
                        ++largeblock_stackn;
                        largeblock_stack[largeblock_stackn ++] = lb2;
                    }
                    else if(object!=NULL)
                    {
                        if ((object->GetType() == OT_BLACKHOLE) || (object->GetType() == OT_MINE))
                        {
                            if( static_cast<dangerous*>(object)->Busy(lb) )
                                return false;
                        }
                        else return false;
                    }
                }
            }
        } 
    }
    return true;

}

bool largeblock::pushingBlocks2(edir dir, int dx, int dy)
{
    if(tic==game::Counter()) return false;
    getPushGroup();   
    if(canPush(dir,dx,dy))
    {
        doPush(dir,dx,dy);
        return true;
    }
    return false;
}

bool largeblock::isReallyASmallBlock()
{
    if(root == NULL) setupBlock();
    
    return (root->children == 1);
    
}

bool largeblock::trypush(edir dir,obj* pusher)
{
    if(root == NULL) setupBlock();
    
    if(root->children == 1) return trypush_common(dir,pusher, false,NULL);
    
    switch(dir)
    {
/*        case D_RIGHT: return pushingBlocks(dir,-1, XYE_HORZ-1, 0, 1,0,XYE_VERT-1, 1,0);
        case D_LEFT: return pushingBlocks(dir,1, 0, XYE_HORZ-1, 1,0,XYE_VERT-1, -1,0);
        case D_UP: return pushingBlocks(dir,-1, XYE_HORZ-1, 0,   -1, XYE_VERT-1,0, 0,1);
        case D_DOWN: return pushingBlocks(dir,-1, XYE_HORZ-1, 0,  1,0,XYE_VERT-1, 0,-1);*/
        case D_RIGHT: return pushingBlocks2(dir,1,0);
        case D_LEFT: return pushingBlocks2(dir,-1,0);
        case D_UP: return pushingBlocks2(dir,0,1);
        case D_DOWN: return pushingBlocks2(dir,0,-1);

            
    }
    return false;
}
void largeblock::OnDeath()
{
    if(root != NULL)
    {
        largeblockroot* lbr = root;
        for (int i=0; i<XYE_HORZ; i++)
            for (int j=0; j<XYE_VERT; j++)
            {
                largeblock * lb = getPart(game::Square(i,j)->object, lbr);
                if(lb != NULL) lb->root = NULL;
            }
        delete lbr;
        
    }
}


inline bool largeblock::HasBlockColor(blockcolor bc) { return ((! colorless) && (c==bc)); }

/**End Class large Block**/



/**Start Class metalBlock**/
metalblock::metalblock(square* sq,bool makeround)
{
    round=makeround;
    type=OT_METALBLOCK;
    ObjectConstruct(sq);
}

void metalblock::Draw(unsigned int x, unsigned int y, bool round)
{
    Uint8 tx=9;
    if(round) tx=10;
    Drawer D(game::sprites,tx*sz, 8*sz ,sz,sz);

    D.Draw(game::screen,x,y);
}

void metalblock::Draw(unsigned int x, unsigned int y)
{
    Draw(x,y,round);
}

bool metalblock::trypush(edir dir,obj* pusher) { return trypush_common(dir,pusher, round,NULL); }

bool metalblock::HasRoundCorner(roundcorner rnc) { return round; }

bool metalblock::Loop(bool* died)
{

    *died=false;

    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved))
        return Moved;
        edir go;

    return false;
}


/**End Class metal Block**/


/**Start Class Scroll-Block**/
scrollblock::scrollblock(square* sq,blockcolor tc,bool makeround,edir d)
{
    fac=d;
    round=makeround;
    type=OT_SCROLLBLOCK;
    c=tc;
    colorless=false;
    ObjectConstruct(sq);
}
void scrollblock::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;

    ty=0;
    if (round) tx=2;
    else tx=1;

    Drawer D(game::sprites,sz*tx,sz*ty,sz,sz);
    D.SetColors(&options::BKColor[c],255);
    D.Draw(game::screen,x,y);

    switch(fac)
    {
        case(D_RIGHT): tx=4; ty=16; break;
        case(D_LEFT): tx=5; ty=16; break;
        case(D_UP): tx=5; ty=17; break;
        default: tx=4; ty=17;
    }

    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.SetColors(&options::BFColor[c],255);
    D.Draw(game::screen,x,y);
}


bool scrollblock::trypush(edir dir,obj* pusher)
{
    if ((dir!=fac) && (pusher) && (pusher->GetType()!=OT_MAGNETIC)) return false;
    if (trypush_common(dir,pusher, round,NULL)) return true;
    char nx=x,ny=y;
    switch(fac)
    {
        case (D_UP): ny++; break;
        case (D_DOWN): ny--; break;
        case (D_LEFT): nx--; break;
        default: nx++;
    }
    obj *object= game::SquareN(nx,ny)->object;
    if (object==NULL) return false; //A ground object was the cause.
    //
    surprise* sp;
    turner *t;



    switch(object->GetType())
    {
        case(OT_TURNER): //a clocker/aclocker found
            //time=0;
            t=static_cast<turner*>(object);
            if (t->cwise) fac=Clock(fac);
            else fac=AClock(fac);
            UpdateSquare();
            break;

        case(OT_SURPRISE):
            sp=static_cast<surprise*>(object);
            if (sp->HasBlockColor(B_RED))
            {
                sp->FinalExplode();
            }
    }

    return false;

}

bool scrollblock::HasRoundCorner(roundcorner rnc) { return round; }

bool scrollblock::Loop(bool* died)
{

    *died=false;

    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved))
        return Moved;
        edir go;

    return false;
}

inline bool scrollblock::HasBlockColor(blockcolor bc) { return ((! colorless) && (c==bc)); }
/** End ScrollBlock**/

/**Start Class Wildcard Block**/
wildcard::wildcard(square* sq,bool makeround)
{
    type=OT_WILDCARD;
    round=makeround;
    ObjectConstruct(sq);
}
void wildcard::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx;
    if (round) tx=2;
    else tx=1;
    Drawer D(game::sprites,tx*sz,sz*2,sz,sz);
    D.Draw(game::screen,x,y);

}

bool wildcard::trypush(edir dir,obj* pusher) { return trypush_common(dir,pusher, round,NULL); }

bool wildcard::HasRoundCorner(roundcorner rnc) { return round; }

bool wildcard::Loop(bool* died)
{

    *died=false;

    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved))
        return Moved;
        edir go;

    return false;
}

inline bool wildcard::HasBlockColor(blockcolor bc) { return (true); }

/**End Class Wild Card**/


/**Start Class Gem Block**/
gemblock::gemblock(square* sq,blockcolor tc)
{
    type=OT_GEMBLOCK;
    c=tc;
    ObjectConstruct(sq);
}

void gemblock::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty=9;
    bool movable=gem::GotAllGems(c);
    tx=(movable?7:6);
    Drawer D(game::sprites,sz*tx,sz*ty,sz,sz);
    D.SetColors(options::BKColor[c],255);
    D.Draw(game::screen,x,y);

    D.ChangeRect(sz*(tx+2),sz*ty,sz,sz);
    D.SetColors(options::BFColor[c],255);
    D.Draw(game::screen,x,y);


}

bool gemblock::trypush(edir dir,obj* pusher) {
    return ( (gem::GotAllGems(c)) && trypush_common(dir,pusher, false,NULL));
}

bool gemblock::HasRoundCorner(roundcorner rnc) { return false; }

bool gemblock::Loop(bool* died)
{
    UpdateSquare(); //In theory only a few gemblocks exist in a level so this shouldn't hurt
    bool movable=gem::GotAllGems(c);
    if (! movable) return false;

    *died=false;

    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved))
        return Moved;
        edir go;

    return false;
}

inline bool gemblock::HasBlockColor(blockcolor bc) { return (c==bc); }

/**End Class Gem Block**/



/**Start Class Magnetic**/
magnetic::magnetic(square* sq, mgtype kind, bool horizontal)
{
    horz=horizontal;
    mt=kind;
    type=OT_MAGNETIC;
    LastPushDir=D_DOWN;
    ObjectConstruct(sq);
}
bool magnetic::IsSticky() { return (    (mt==T_MAGNET) || (mt==T_STICKY)); }

bool magnetic::IsSticky(edir reqdir)
{
    if(mt==T_MAGNET) return true;
    if ((mt==T_STICKY) && (MovedTic+1>=game::Counter()) && (LastPushDir==reqdir))
    {

        return true;
    }
 return false;
}

bool magnetic::IsHorizontal() { return horz; }

void magnetic::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;

    if (horz) ty=13;
    else ty=12;
    switch (mt)
    {
        case(T_MAGNET): tx=6; break;
        case(T_ANTIMAGNET): tx=7; break;
        default: tx=8;
    }
    Drawer D(game::sprites,sz*tx,sz*ty,sz,sz);
    D.Draw(game::screen,x,y);

}

bool magnetic::trypush(edir dir,obj* pusher) {
    if (trypush_common(dir,pusher, false,NULL))
    {
        LastPushDir=dir;
        MovedTic=game::Counter();
        return true;
    }
    return false;
}

bool magnetic::HasRoundCorner(roundcorner rnc) { return false; }

bool magnetic::TryMagneticMove(char ox, char oy, char xx, char xy, edir godir, bool *died, char sx, char sy)
{
    *died=false;
    if (! IsXyeOrBotAt(game::SquareN(xx,xy))) return false; //Xye wasn't there

    if( (mt==T_STICKY) && ! IsXyeAt(game::SquareN(xx,xy) ) ) return false;

    //Xye was there attempt to move :

    if (mt==T_ANTIMAGNET)
    {
        if (trypush_common(godir,this, false,died))
            return true;
        return false;
    }


    square *sq=game::SquareN(ox,oy);
    if (sq->object!=NULL) return false;
    gobj* gobject=sq->gobject;
    if ((gobject!=NULL) && (! (gobject->CanEnter(this,godir))  )) return false;

    //Everything is OK, move the object:
    char oldx=x, oldy=y;

    move(sq->sqx,sq->sqy);
    MovedTic=game::Counter();
    LastPushDir=godir;
    if (mt!=T_STICKY) return true;
    
    sq=game::SquareN(sx,sy);
    obj* object=sq->object;

    if ((object!=NULL) && (object->tic!=game::counter) && (object->AffectedByMagnetism(horz)))
    {
        object->trypush(godir, this);
    }

    return true;
}

bool magnetic::Loop(bool* died)
{
    //Find Xye:
    *died=false;

    if (horz)
    {
        if (mt!=T_ANTIMAGNET)
        {
            if (((mt==T_MAGNET) || (game::Moved(D_RIGHT) )) && (TryMagneticMove(x+1,y,x+2,y,D_RIGHT,died,x-1,y))) return true;
            if (((mt==T_MAGNET) || (game::Moved(D_LEFT) )) && (TryMagneticMove(x-1,y,x-2,y,D_LEFT,died,x+1,y))) return true;
        }
        else
        {
            if (TryMagneticMove(x+1,y,x-1,y,D_RIGHT,died)) return true;
            if (TryMagneticMove(x-1,y,x+1,y,D_LEFT,died)) return true;
        }
    }
    else
    {
        if (mt!=T_ANTIMAGNET)
        {
            if (((mt==T_MAGNET) || (game::Moved(D_UP) )) && (TryMagneticMove(x,y+1,x,y+2,D_UP,died,x,y-1))) return true;
            if (((mt==T_MAGNET) || (game::Moved(D_DOWN) )) && (TryMagneticMove(x,y-1,x,y-2,D_DOWN,died,x,y+1))) return true;
        }
        else
        {
            if (TryMagneticMove(x,y+1,x,y-1,D_UP,died)) return true;
            if (TryMagneticMove(x,y-1,x,y+1,D_DOWN,died)) return true;
        }

    }
    bool Moved=false;
    if (DoMagnetism(! horz, horz, &Moved)) return Moved;
    return false;
}

bool magnetic::IsInstance(obj* object)
{
    if (object==NULL) return false;
    return (object->GetType()==OT_MAGNETIC);
}

inline bool magnetic::HasBlockColor(blockcolor bc) { return false; }
/**End Class Magnetic**/





/**Start Class Number**/
number::number(square* sq,blockcolor tc, unsigned char t,bool makeround)
{
    time=(t>9?9:t);
    round=makeround;
    type=OT_NUMBER;
    c=tc;
    GreenActive=0;
    GreenFlash=false;
    tic=0;
    ignoreDeath=false;
    ObjectConstruct(sq);
}
void number::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    tx = 16+round;
    ty = 6 + time;
    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);

    SDL_Color mx = options::BKColor[c];

    if ((c==B_GREEN) && (GreenFlash))
    {
        UpdateSquare();
        SDL_Color tm;
        tm.r=tm.g=tm.b=255;
        mx=MixColors(tm,mx);
        GreenFlash=false;
    }
    Uint8 alpha = 255;
    /*if (c!=B_RED)
        alpha= 255 - (9-time)*14;
    else
        alpha=255;*/
    D.SetColors(&mx,alpha);
    D.Draw(game::screen,x,y);

    tx=18;
    ty = 6 + time;

    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.SetColors( &options::BFColor[c],255);
    D.Draw(game::screen,x,y);
}

void number::explode()
{
    deathqueue::add(x,y,KT_FIRE);
    explosion *ex = new explosion(game::SquareN(x,y),1);
}

bool number::trypush(edir dir,obj* pusher) {
    bool died=false;
    if (trypush_common(dir,pusher, round,&died))
    {
        if ((c==B_RED) && (!died) && (! magnetic::IsInstance(pusher) ))
        {
            if (time==0)
            {
                explode();
                Kill();
            }
            else time--;
        }
        return true;
    }
    return false;
}

bool number::HasRoundCorner(roundcorner rnc) { return round; }


bool number::green_activator_present(square * sq)
{
    obj *object=sq->object;
    if (object==NULL) return false;
    otype ot=object->GetType();
    if (ot==OT_XYE) return true;
    if (ot!=OT_NUMBER) return false;
    number* num=static_cast<number*>(object);
    return ((num->c==B_GREEN) && (num->GreenActive!=0) && (num->GreenActive!=game::counter)  );
}


bool number::Loop(bool* died)
{
    bool Moved=false;
    *died=false;
    DoMagnetism(true,true,&Moved);

    if (c==B_RED) return Moved;
    tic++;
    switch(c)
    {
        case(B_BLUE): tic=(tic>=300?0:tic); break;
        default: tic=(tic>=60?0:tic); break;
    }
    if (c==B_GREEN)
    {
        if (GreenActive==0)
        {
            if ((green_activator_present(game::SquareN(x+1,y) ))
             || (green_activator_present(game::SquareN(x,y+1) ))
             || (green_activator_present(game::SquareN(x-1,y) ))
             || (green_activator_present(game::SquareN(x,y-1) )))
            {
                UpdateSquare();
                GreenFlash=true;
                GreenActive=game::counter;
                if(time==0) tic=30;
                else tic=0;
            }
            else return Moved;
        }
    }
    if (tic==0)
    {
        UpdateSquare();
        if (time==0)
        {
            *died=true;
            Kill();
            return Moved;
        }
        else time--;
    }

    return Moved;
}

inline bool number::HasBlockColor(blockcolor bc) { return (c==bc); }

void number::OnDeath()
{
    if ((c==B_RED) && (! KilledByBlackHole) && (! ignoreDeath) )
    {
        ignoreDeath=true;
        explode();
    }
}
/**End Class Number**/







/**Start Class Impacter**/
impacter::impacter(square* sq,blockcolor tc,edir facing)
{
    fac=facing;
    type=OT_PUSHER;
    c=tc;
    time=0;
    ObjectConstruct(sq);
}
void impacter::Draw(unsigned int x, unsigned int y,blockcolor c,edir fac)
{
    Uint8 tx,ty;
    switch(fac)
    {
        case(D_RIGHT): tx=4; ty=5; break;
        case(D_LEFT): tx=5; ty=5; break;
        case(D_UP): tx=5; ty=6; break;
        default: tx=4; ty=6;
    }
    Drawer D(game::sprites,sz*tx,sz*ty,sz,sz);
    D.SetColors(options::BKColor[c],255);
    D.Draw(game::screen,x,y);

    D.ChangeRect(sz*tx,sz*(ty+2),sz,sz);
    D.SetColors(options::BFColor[c],255);
    D.Draw(game::screen,x,y);

}
void impacter::Draw(unsigned int x, unsigned int y)
{
    Draw(x,y,c,fac);
}

bool impacter::trypush(edir dir,obj* pusher) {
 unsigned int ClockTic=game::Counter();
    if ((tic!=ClockTic) && trypush_common(dir,pusher, false,NULL))
    {
        /*switch(pusher->GetType())
        {
            case(OT_PUSHER): case(OT_XYE): case(OT_ROBOXYE): tic=ClockTic;
        }*/
        //if (dir == Opposite(fac)) turn();
        return true;
    }
 return false;
}

bool impacter::HasRoundCorner(roundcorner rnc) { return false; }

void impacter::turn()
{
    switch(c)
    {
          case(B_YELLOW): fac=Opposite(fac); break;
           case(B_BLUE): fac=Clock(fac); break;
           case(B_GREEN): fac=AClock(fac); break;
           case(B_RED): time=9;
    }
    if (c!=B_RED) UpdateSquare();

}

bool impacter::Loop(bool* died)
{
    *died=false;
    bool Moved=false;


    bool pushed=false;

    unsigned int ClockTic=game::Counter();
    time++;
    if (time>=10) {

    time=0;
    if  (DoMagnetism(true,true,&Moved)) return Moved;
    char dx=x,dy=y;
    switch(fac)
    {
        case(D_UP): dy++;break;
        case(D_DOWN): dy--;break;
        case(D_RIGHT): dx++;break;
        default: dx--;
    }

    dx=(dx>=XYE_HORZ?0:(dx<0?XYE_HORZ-1:dx));
    dy=(dy>=XYE_VERT?0:(dy<0?XYE_VERT-1:dy));
    square* sq=game::Square(dx,dy);
    obj* object=sq->object;
    teleport* tele; dangerous* blck;


    char nx=0,ny=0;
    bool go=false;
    if (object==NULL)
        go=true;
    else
    {
        switch (object->GetType())
        {
            case(OT_TELEPORT):
                tele=static_cast<teleport*>(object);
                go = tele->tryteleport(fac, this, nx, ny ,blck, &pushed);

                if (go) { dx=nx; dy=ny;sq=game::Square(dx,dy); object=sq->object; }
                else if (blck!=NULL)
                {
                    if (blck->Busy(this)) go=false;
                    else
                    {
                        //Kill The impacter
                        blck->Eat();
                        Kill(blck->GetType()==OT_BLACKHOLE);
                        *died=true;
                        return true;
                    }
                }
                break;
            case(OT_BLACKHOLE): case(OT_MINE): case(OT_FIREBALL):
                blck=static_cast<dangerous*>(object);
                if (blck->Busy(this)) go=false;
                else
                {
                    //Kill The impacter
                    blck->Eat();
                    Kill(blck->GetType()==OT_BLACKHOLE);
                    *died=true;

                    return true;
                }
                break;
        }

        if (! go)
        {
            if (object==NULL)
                  go=true;
            else if (object->trypush(fac,this))
            {
                go=(c==B_RED);
                pushed=true;
            }

        }
    }

    if (go)
    {

        gobj* gobject=sq->gobject;
        if ((gobject==NULL) || (gobject->CanEnter(this,fac)))
        {
            move(dx,dy);
            return true;
        }
        else pushed=true;
    }

    if ((! go) || pushed) turn();

    }

    return false;
}
inline bool impacter::HasBlockColor(blockcolor bc) { return (c==bc); }
/**End Class Impacter**/


/**Start Class Arrow**/
arrow::arrow(square* sq,blockcolor tc,edir facing, bool roundblock)
{
    fac=facing;
    type=OT_ARROW;
    c=tc;
    round=roundblock;
    //mod=game::Mod;
    time=0;
    ObjectConstruct(sq);
}
void arrow::DrawF(unsigned int x, unsigned int y,blockcolor c, edir fac,bool round)
{

    Uint8 tx,ty;
    switch(fac)
    {
        case(D_RIGHT): tx=4; ty=9; break;
        case(D_LEFT): tx=5; ty=9; break;
        case(D_UP): tx=5; ty=10; break;
        default: tx=4; ty=10;
    }
    if (round) ty+=2;

    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.SetColors(&options::BFColor[c],255);
    D.Draw(game::screen,x,y);

}
void arrow::Draw(unsigned int x, unsigned int y)
{
    block::Draw(x, y, false,c,round);
    DrawF(x,y,c,fac,round);

}

bool arrow::trypush(edir dir,obj* pusher) {
   return (trypush_common(dir,pusher, round && (dir==fac),NULL));
}

bool arrow::HasRoundCorner(roundcorner rnc) { return round; }

bool arrow::Loop(bool* died)
{
    *died=false;
    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved)) return Moved;


    time++;
    bool go=true;
    switch(c)
    {
        case(B_YELLOW): go=(time==2); break;
        case(B_GREEN): go=(time==3); break;
        case(B_BLUE): go=(time==4); break;
        //Red is always true
    }
    if (!go) return false;
    time=0;
    if (trypush_common(fac,NULL, round,died))
        return true;

    switch(c)
    {
        case(B_YELLOW): time=1 ; break;
        case(B_GREEN): time=2 ; break;
        case(B_BLUE): time=3; break;
        //Red is always true
    }



    char nx=x,ny=y;
    switch(fac)
    {
        case (D_UP): ny++; break;
        case (D_DOWN): ny--; break;
        case (D_LEFT): nx--; break;
        default: nx++;
    }
    obj *object= game::SquareN(nx,ny)->object;
    if (object==NULL) return false; //A ground object was the cause.

    surprise* sp;
    turner *t;



    switch(object->GetType())
    {
        case(OT_TURNER): //a clocker/aclocker found
            time=0;
            t=static_cast<turner*>(object);
            if (t->cwise) fac=Clock(fac);
            else fac=AClock(fac);
            UpdateSquare();
            return true;
        case(OT_SURPRISE):
            sp=static_cast<surprise*>(object);
            if (sp->HasBlockColor(B_RED))
            {
                sp->FinalExplode();
            }
    }






    return false;
}
inline bool arrow::HasBlockColor(blockcolor bc)  { return (c==bc); }
/**End Class Arrow**/

/**Start Class autoArrow**/
autoarrow::autoarrow(square* sq,blockcolor tc,edir facing, bool roundblock)
{
    fac=facing;
    type=OT_AUTO;
    c=tc;
    round=roundblock;
    count=0;
    ObjectConstruct(sq);
}
void autoarrow::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    ty=7;
    if (round) tx=1;
    else tx=2;

    Drawer D(game::sprites,sz*tx,sz*ty,sz,sz);

    D.SetColors(options::BKColor[c],255);
    D.Draw(game::screen,x,y);

    switch(fac)
    {
        case(D_RIGHT): tx=1; ty=8; break;
        case(D_LEFT): tx=2; ty=8; break;
        case(D_UP): tx=2; ty=9; break;
        default: tx=1; ty=9;
    }

    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.SetColors(options::BFColor[c],255);
    D.Draw(game::screen,x,y);
}

bool autoarrow::trypush(edir dir,obj* pusher) {
   return (trypush_common(dir,pusher, false,NULL));
}

bool autoarrow::HasRoundCorner(roundcorner rnc) { return false; }

bool autoarrow::Loop(bool* died)
{

    if (game::Mod2() && game::Mod7())
    {
        UpdateSquare();
        fac=AClock(fac);
        if (count>(XYE_VERT-y-2))
        {
            char nx=x,ny=y;
            switch(fac)
            {
                case(D_UP): ny++; break;
                case(D_DOWN): ny--; break;
                case(D_LEFT): nx--; break;
                default: nx++;
            }
            square *sq=game::SquareN(nx,ny);
            if ((sq->object==NULL) && ((sq->gobject==NULL) || (sq->gobject->CanEnter(this,fac) )))
            {
                count=0;
                arrow* ar=new arrow(sq,c,fac,round);
                ar->tic=game::Counter();
            }
        }
        else count++;
    }
    return false;
}
inline bool autoarrow::HasBlockColor(blockcolor bc) { return (c==bc); }

edir autoarrow::GetDefaultEdirByColumn(unsigned int cx)
{
    switch (cx%4)
    {
        case(0): return (D_RIGHT); break;
        case(1): return (D_UP); break;
        case(2): return (D_LEFT);

    }
 //default:
 return (D_DOWN);
}
/**End Class Autoarrow**/


/**Start Class Factory**/
factory::factory(square* sq,otype r,blockcolor tc, edir facing, edir switchfacing, bool roundblock,bool nocolor,btype bs)
{
    limit=100000;
    fac=facing;
    swf=switchfacing;
    beasttype=bs;
    type=OT_FACTORY;
    c=tc;
    res=r;
    factory::nocolor=nocolor;
    round=roundblock;
    created=0;
    pressed=0;
    upd=false;
    ObjectConstruct(sq);
}

void factory::DrawSub(unsigned int x,unsigned int y)
{
    switch(res)
    {
        case(OT_MINE):
            dangerous::DrawMine(x,y); return;
        case(OT_PUSHER):
            impacter::Draw(x,y,c,fac); return;
        case(OT_BEAST):
            beast::Draw(x,y,beasttype,fac,0,0); return;
        case(OT_RATTLERFOOD):
            rfood::Draw(x,y,false); return;
        case(OT_RATTLER):
            rattler::Draw(x,y,false,fac,NULL); return;
        case(OT_GEM):
            gem::Draw(x,y,c,false); return;

    }
    block::Draw(x, y, (nocolor && (res==OT_BLOCK)),c,round);
    switch(res)
    {
        case(OT_BLOCK): return;
        case(OT_ARROW):
            arrow::DrawF(x,y,c,fac,round);
            break;
        case(OT_LOWDENSITY):
            lowdensity::DrawF(x,y,c, round);

    }


}

void factory::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    Drawer D(game::sprites,0,0,0,0);
    //step 1: Draw the result:

    if ( ((!created) || (created+2<=game::counter)) && (limit))
        DrawSub(x,y);

    //step 2 : Draw the factory:
    switch(fac)
    {
        case(D_RIGHT): tx=6; break;
        case(D_DOWN): tx=7; break;
        case(D_LEFT): tx=8; break;
        default: tx=9;
    }
    ty=14;
    D.SetColors(255,255,255,255);
    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.Draw(game::screen,x,y);


    //step 3 : Draw the switch:
    switch(swf)
    {
        case(D_RIGHT): tx=6; break;
        case(D_DOWN): tx=7; break;
        case(D_LEFT): tx=8; break;
        default: tx=9;
    }

    ty=15+(pressed?1:0);
    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.Draw(game::screen,x,y);

}



bool factory::trypush(edir dir,obj* pusher) {
   if (pressed || (dir != Opposite(swf) ) ) return false;
    //Success to press the switch, try to create a block.
    pressed=11;
    if (!limit) return false;
    char nx=x,ny=y;
    switch(fac)
    {
        case(D_UP): ny++; break;
        case(D_DOWN): ny--; break;
        case(D_LEFT): nx--; break;
        default: nx++;
    }
    square *sq=game::SquareN(nx,ny);
    if ((sq->object==NULL) && ((sq->gobject==NULL) || (sq->gobject->CanEnter(this,fac) )))
    {
        obj* object;lowdensity* lb;block* b;
        created=game::counter;limit--;
        beast* be;
        switch(res)
        {
            case(OT_MINE):
                object=new dangerous(sq,OT_MINE);
                break;
            case(OT_ARROW):
                object=new arrow(sq,c,fac,round);
                object->tic=game::Counter();
                break;
            case(OT_LOWDENSITY):
                lb=new lowdensity(sq,c,round);
                lb->tic=game::Counter();
                lb->Activate(fac);
                break;
            case(OT_PUSHER):
                object=new impacter(sq,c,fac);
                break;
            case(OT_BEAST):
                object=new beast(sq,beasttype,fac);
                object->tic=game::Counter();
                if(beasttype == BT_STATIC) static_cast<beast*>(object)->AIValue=true;
                break;
            case(OT_RATTLERFOOD):
                object=new rfood(sq);
                break;

            case(OT_RATTLER):
                object=new rattler(sq,fac,0);
                break;

            case(OT_GEM):
                object=new gem(sq,c);
                break;

            case(OT_BLOCK):default :
                b=new block(sq,c,round);
                b->tic=game::Counter();
                b->colorless=nocolor;
                break;
        }
    }
   UpdateSquare();
   return false;
}

bool factory::Loop(bool* died)
{
    if (upd) upd=false;

    if (pressed)
    {
        pressed--;
        if (! pressed)
            upd=true;
    }


    if(pressed || (created && (created+3<=game::counter)) || upd) UpdateSquare();

    return false;
}

/**End Class factory**/



/**Start Class filler**/
filler::filler(square* sq,blockcolor tc,edir facing, bool roundblock)
{
    fac=facing;
    type=OT_FILLER;
    c=tc;
    round=roundblock;
    anim=Chance(0.5);
    ObjectConstruct(sq);
    count=0;
}
void filler::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    ty=15;
    if (round) tx=0;
    else tx=1;

    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.SetColors(options::BKColor[c],255);

    D.Draw(game::screen,x,y);
    switch(fac)
    {
        case(D_RIGHT): tx=0; ty=16; break;
        case(D_LEFT): tx=1; ty=16; break;
        case(D_UP): tx=1; ty=17; break;
        default: tx=0; ty=17;
    }
    if (anim)
        ty+=2;

    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.SetColors(options::BFColor[c],255);
    D.Draw(game::screen,x,y);

}

bool filler::trypush(edir dir,obj* pusher) {
   return (trypush_common(dir,pusher, false,NULL));
}

bool filler::HasRoundCorner(roundcorner rnc) { return false; }

bool filler::Loop(bool* died)
{
    char nx=x,ny=y;
    switch(fac)
    {
        case(D_UP): ny++; break;
        case(D_DOWN): ny--; break;
        case(D_LEFT): nx--; break;
        default: nx++;
    }
    square *sq=game::SquareN(nx,ny);
    if ((sq->object==NULL) && ((sq->gobject==NULL) || (sq->gobject->CanEnter(this,fac) )))
    {
        count++;
        if (count>=14)
        {
            UpdateSquare();
            anim=!anim;
            arrow* ar=new arrow(sq,c,fac,round);
            ar->tic=game::Counter();
            count=0;
        }
    }
    return false;
}
inline bool filler::HasBlockColor(blockcolor bc) { return (c==bc); }
/**End Class Filler**/

/**Start Class sniper**/
sniper::sniper(square* sq,blockcolor tc, bool roundblock)
{
    fac=D_DOWN;
    type=OT_SNIPER;
    c=tc;
    round=roundblock;
    anim=Chance(0.5);
    count=0;
    ObjectConstruct(sq);
}
void sniper::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    ty=15;
    if (round) tx=2;
    else tx=3;

    Drawer D(game::sprites,sz*tx,sz*ty,sz,sz);
    D.SetColors(options::BKColor[c],255);
    D.Draw(game::screen,x,y);
    switch(fac)
    {
        case(D_RIGHT): tx=2; ty=16; break;
        case(D_LEFT): tx=3; ty=16; break;
        case(D_UP): tx=3; ty=17; break;
        default: tx=2; ty=17;
    }
    if (anim)
        ty+=2;

    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.SetColors(options::BFColor[c],255);
    D.Draw(game::screen,x,y);

}

bool sniper::trypush(edir dir,obj* pusher) {
   return (trypush_common(dir,pusher, false,NULL));
}

bool sniper::HasRoundCorner(roundcorner rnc) { return false; }

bool sniper::Loop(bool* died)
{
    count++;
    char nx=x,ny=y;
    char xx=game::XYE->X();
    char xy=game::XYE->Y();
    edir old=fac;
    fac=PickDirectDir(x,y,xx,xy,false,/*horz priority */true);
    if (old!=fac) UpdateSquare();

    if (count<7)
           return true;

    if (x==xx)
    {
        if (y>xy) ny--;
        else ny++;

    }
    else if (y==xy)
    {
        if (x>xx) nx--;
        else nx++;

    }
    else
        return false;


    square *sq=game::SquareN(nx,ny);
    if ((sq->object==NULL) && ((sq->gobject==NULL) || (sq->gobject->CanEnter(this,fac) )))
    {
        anim=!anim;
        UpdateSquare();
        arrow* ar=new arrow(sq,c,fac,round);
        ar->tic=game::Counter();
        count=0;
    }
    return false;

}
inline bool sniper::HasBlockColor(blockcolor bc) { return (c==bc); }
/**End Class Sniper**/



/**Start Low Density**/
lowdensity::lowdensity(square* sq,blockcolor tc, bool roundblock)
{
    fac=D_UP;
    type=OT_LOWDENSITY;
    c=tc;
    active=false;
    round=roundblock;
    ObjectConstruct(sq);
}

void lowdensity::Activate(edir D)
{
    fac=D;
    active=true;
    time=0;
}
void lowdensity::DrawF(unsigned int x, unsigned int y,blockcolor c, bool round)
{
    Drawer D(game::sprites,3*sz,12*sz,sz,sz);
    D.SetColors(&options::BFColor[c],255);
    D.Draw(game::screen,x,y);

}
void lowdensity::Draw(unsigned int x, unsigned int y)
{
    block::Draw(x, y, false,c,round);
    DrawF(x,y,c,round);

}

bool lowdensity::trypush(edir dir,obj* pusher)
{
   if (trypush_common(dir,pusher, round && ( (! active) || (fac==dir) ),NULL))
   {
          active=true;
          fac=dir;
          time=0;
          tic=game::Counter();
          return true;
   }
 return false;
}

bool lowdensity::HasRoundCorner(roundcorner rnc) { return round; }

bool lowdensity::Loop(bool* died)
{
    bool Moved=false;

    if (DoMagnetism(true,true,&Moved))
    {
        if (! Moved) active=false;
        return (Moved);
    }
    if (! active) return false;

    bool go=true;
    time++;
    switch(c)
    {
        case(B_YELLOW): go=(time==2); break;
        case(B_GREEN): go=(time==3); break;
        case(B_BLUE): go=(time==4); break;
        //Red is always true
    }
    if (!go) return false;
    time=0;
    if (trypush_common(fac,NULL, round,NULL))
        return true;
    active=false;
    return false;
}

inline bool lowdensity::HasBlockColor(blockcolor bc) { return (c==bc); }
/**End Low density**/





/**Start Class Turner**/
turner::turner(square* sq,blockcolor tc,bool clockwise,bool roundblock)
{
    type=OT_TURNER;
    round=roundblock;
    c=tc;
    cwise=clockwise;
    colorless=false;
    ObjectConstruct(sq);
}
void turner::Draw(unsigned int x, unsigned int y)
{
    block::Draw(x,y,colorless,c,round);
    Uint8 tx,ty;

    tx=3;
    if (cwise) ty=11;
    else ty=10;

    Drawer D(game::sprites,sz*tx,sz*ty,sz,sz);
    if (colorless) D.SetColors(0,0,0);
    else D.SetColors(options::BFColor[c]);
        D.Draw(game::screen,x,y);

}

bool turner::trypush(edir dir,obj* pusher) {
 return trypush_common(dir,pusher, round,NULL);
}

bool turner::HasRoundCorner(roundcorner rnc) { return round; }

bool turner::Loop(bool* died)
{
    *died=false;
    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved)) return Moved;
    return false;
}

inline bool turner::HasBlockColor(blockcolor bc) { return ((! colorless) && (c==bc)); }


/**End Class Turner**/





/**Start Class Surprise**/
surprise::surprise(square* sq,blockcolor tc,bool makeround)
{
    round=makeround;
    type=OT_SURPRISE;
    c=tc;
    changing=false;
    ObjectConstruct(sq);
}

//The transformation process relies on the draw method for eye candy reasons
//But not calling draw methods should not affect the gameplay. So this dotransform
//stuff is used to ensure that even if the block is not drawn it will transform without
//upsetting the gameplay
unsigned int surprise::Pending;

void surprise::TransformAll()
{
    if (! Pending) return;
    unsigned char i,j;
    square* sq;
    obj* object;
    for (i=0; Pending && (i<XYE_HORZ) ;i++)
        for (j=0; Pending && (j<XYE_VERT) ;j++)
        {
            sq=game::Square(i,j);
            if ((object=sq->object) && (object->GetType()==OT_SURPRISE))
            {
                (static_cast<surprise*>(object))->Transform();
            }
        }
}

void surprise::Reset()
{
    Pending=0;
}

void surprise::Transform()
{
    if (! changing) return;
    Pending--;
    square *sq=game::Square(surprise::x,surprise::y);
    sq->object=NULL;
      gobj *gobject = sq->gobject;
    if (gobject!=NULL) gobject->OnLeave(this);
    wall* wl;
    dangerous* bh;
    impacter* bn;

        switch(c)
        {
            case(B_YELLOW):
                bn=new impacter(sq,B_YELLOW,YellowDir);
                break;

            case(B_GREEN):
                sq->object=this;
                changing=false;
                c=B_BLUE;
                if (gobject!=NULL) gobject->OnEnter(this);
                UpdateSquare();
                   break;
            case(B_BLUE):
            {
                wl=new wall(sq,0);
                SDL_Color BC = options::BKColor[c];

                char ox=obj::X(),oy=obj::Y();
                wall *b8= wall::find(ox,oy+1),
                     *b4= wall::find(ox-1,oy),
                     *b2= wall::find(ox,oy-1),
                     *b6= wall::find(ox+1,oy);

                if (round)
                    wl->SetRoundCorners( !(b4 || b8) , !(b4 || b2) ,! (b2 || b6), !(b6 || b8) );
                if (b8) b8->IntelligentUpdateCorners(wl);
                if (b4) b4->IntelligentUpdateCorners(wl);
                if (b2) b2->IntelligentUpdateCorners(wl);
                if (b6) b6->IntelligentUpdateCorners(wl);

                wall *b7= wall::find(ox-1,oy+1),
                     *b9= wall::find(ox+1,oy+1),
                     *b3= wall::find(ox+1,oy-1),
                     *b1= wall::find(ox-1,oy-1);
                // After the addition of special borders to the skin capabilities, it is necessary
                // to update the walls in the corners as well:
                
                //wl->ChangeColor(BC.r+(options::WallColor[0].r-BC.r)/2,BC.g+(options::WallColor[0].g-BC.g)/2,BC.b+(options::WallColor[0].b-BC.b)/2, false);
                int r = BC.r;
                int g = BC.g;
                int b = BC.b;
                r = (r+wall::DefaultColor[0].r)/2;
                g = (g+wall::DefaultColor[0].g)/2;
                b = (b+wall::DefaultColor[0].b)/2;
                wl->ChangeColor(r,g,b, false);
 
                if(b7) b7->UpdateSquare();
                if(b9) b9->UpdateSquare();
                if(b3) b3->UpdateSquare();
                if(b1) b1->UpdateSquare();

                //wl->ChangeColor(BC.r,BC.g,BC.b, false);
                break;
            }


        }

    if (changing)
        recycle::add(this);;


}

void surprise::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    Drawer D(game::sprites,0,0,0,0);
    SDL_Color SprColor;
    SDL_Color BC = options::BKColor[c] ,FBC= options::BFColor[c];


    if (changing)
    {
        Pending--;
        square *sq=game::Square(surprise::x,surprise::y);
        sq->object=NULL;
          gobj *gobject = sq->gobject;
        if (gobject!=NULL) gobject->OnLeave(this);
        wall* wl;
        dangerous* bh;
        impacter* bn;

        switch(c)
        {
            case(B_YELLOW):
                bn=new impacter(sq,B_YELLOW,YellowDir);
                SprColor=BC;
                bn->Draw(x,y);
                break;

            case(B_GREEN):
                sq->object=this;
                changing=false;
                BC=MixColors(BC,options::BKColor[B_BLUE]);
                FBC=MixColors(FBC,options::BFColor[B_BLUE]);
                c=B_BLUE;
                if (gobject!=NULL) gobject->OnEnter(this);
                UpdateSquare();
                   break;
            case(B_BLUE):
                wl=new wall(sq,0);
                SprColor=BC;

                    char ox=obj::X(),oy=obj::Y();
                    wall *b8= wall::find(ox,oy+1),
                         *b4= wall::find(ox-1,oy),
                         *b2= wall::find(ox,oy-1),
                         *b6= wall::find(ox+1,oy);

                    if (round)
                        wl->SetRoundCorners( !(b4 || b8) , !(b4 || b2) ,! (b2 || b6), !(b6 || b8) );
                    if (b8) b8->IntelligentUpdateCorners(wl);
                    if (b4) b4->IntelligentUpdateCorners(wl);
                    if (b2) b2->IntelligentUpdateCorners(wl);
                    if (b6) b6->IntelligentUpdateCorners(wl);
                    wall *b7= wall::find(ox-1,oy+1),
                         *b9= wall::find(ox+1,oy+1),
                         *b3= wall::find(ox+1,oy-1),
                         *b1= wall::find(ox-1,oy-1);
                    if(b7) b7->UpdateSquare();
                    if(b9) b9->UpdateSquare();
                    if(b3) b3->UpdateSquare();
                    if(b1) b1->UpdateSquare();

                int r = SprColor.r;
                int g = SprColor.g;
                int b = SprColor.b;
                r = (r+wall::DefaultColor[0].r)/2;
                g = (g+wall::DefaultColor[0].g)/2;
                b = (b+wall::DefaultColor[0].b)/2;
                wl->ChangeColor(r,g,b, false);

                wl->Draw(x,y);
                break;


        }
    }


    if (round)
        D.ChangeRect(sz*2,0,sz,sz);
    else
        D.ChangeRect(sz,0,sz,sz);

    SprColor=BC;
    if (changing)
        D.SetColors(SprColor,128);
    else
        D.SetColors(SprColor,255);

    D.Draw(game::screen,x,y);
    D.ChangeRect(3*sz,7*sz,sz,sz);
    SprColor=FBC;
    if (changing)
        D.SetColors(SprColor,128);
    else
        D.SetColors(SprColor,255);
    D.Draw(game::screen,x,y);
    if (changing)
        recycle::add(this);;


}

bool surprise::trypush(edir dir,obj* pusher)
{
    if (changing) return false;
    bool died=false;
    if (trypush_common(dir,pusher, round,&died))
    {
        if ((died) || (magnetic::IsInstance(pusher) ) || (c==B_RED))  return true;
        if (c==B_YELLOW) YellowDir=dir;
        if (! died)
        {
            changing=true;
            Pending++;
        }
        return true;
    }
    return false;
}

bool surprise::HasRoundCorner(roundcorner rnc) { return round; }

bool surprise::Loop(bool* died)
{
    if (changing) return false;
    *died=false;
    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved)) return Moved;
    return false;
}

inline bool surprise::HasBlockColor(blockcolor bc) { return (c==bc); }

void surprise::FinalExplode()
{
    char i,j;

    for (i=-1;i<=1;i++)
        for (j=-1;j<=1;j++)
            game::SmallBoom(game::SquareN(x+i,y+j), false, i,-j );

    explosion* ex=new explosion(game::Square(x,y));;



}

/**End Class Surprise**/

/**Start Class Toggle**/

bool toggle::State[4] = {false,false,false,false};
Uint32 toggle::ChangeTic;

void toggle::Reset()
{
    ChangeTic=0;
    State[0]=State[1]=State[2]=State[3]=false;
}

toggle::toggle(square* sq,blockcolor tc,bool makeround, bool state)
{
    round=makeround;
    kind=state;
    type=OT_TOGGLE;
    c=tc;
    Updated=game::counter;
    ObjectConstruct(sq);
}
void toggle::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    Drawer D(game::sprites,0,0,0,0);
    if (round)
        D.ChangeRect(sz*2,0,sz,sz);
    else
        D.ChangeRect(sz,0,sz,sz);

    D.SetColors(options::BKColor[c],255);
    D.Draw(game::screen,x,y);

    if (kind==State[(unsigned int)(c)])
          D.ChangeRect(3*sz,8*sz,sz,sz);
    else
        D.ChangeRect(3*sz,9*sz,sz,sz);

    D.SetColors(options::BFColor[c],255);
    D.Draw(game::screen,x,y);


}

bool toggle::trypush(edir dir,obj* pusher)
{
    unsigned int indx=(unsigned int)(c);
    if (kind!=State[indx]) return false;
    bool died;
    if (trypush_common(dir,pusher, round,&died))
    {
        if (! died)
        {
            if (magnetic::IsInstance(pusher) ) return true;
            ChangeTic=game::counter;
            Updated=ChangeTic;
            State[indx]=! State[indx];
        }
        return true;
    }
    return false;
}

bool toggle::HasRoundCorner(roundcorner rnc) { return round; }

bool toggle::Loop(bool* died)
{
    if (Updated<ChangeTic)
    {
        UpdateSquare();
        Updated=ChangeTic;
    }
    *died=false;
    bool Moved=false;
    if ((kind==State[(unsigned int)(c)]) && (DoMagnetism(true,true,&Moved))) return Moved;
    return false;
}

inline bool toggle::HasBlockColor(blockcolor bc) { return (c==bc); }
/**End Class Toggle Block**/




/**Start Class Teleport**/
teleport::teleport(square* sq,edir facing)
{
    type=OT_TELEPORT;
    dir=facing;
    anim=Chance(0.5);
    ObjectConstruct(sq);
}

teleport* teleport::FindExit(char stx,char sty)
{
    edir oposs;
    char dx=0,dy=0,cx,cy;
    //Find opposite and diffs

    obj* object;
    if (stx<0) stx=x;
    if (sty<0) sty=y;
    switch(dir)
    {
        case(D_UP):
            oposs=D_DOWN;
            dy=1;
            break;
        case(D_DOWN):
            oposs=D_UP;
            dy=-1;
            break;
        case(D_LEFT):
            oposs=D_RIGHT;
            dx=-1;
            break;
        /*case(D_RIGHT)*/
        default:
            oposs=D_LEFT;
            dx=1;
            break;
    }
    cx=stx+dx;cy=sty+dy;
    cx=(cx>=XYE_HORZ?0:(cx<0?XYE_HORZ-1:cx));
    cy=(cy>=XYE_VERT?0:(cy<0?XYE_VERT-1:cy));
    if ((cx==x) && (cy==y)) return NULL;
    do
    {
        object=game::Square(cx,cy)->object;
        if ((object!=NULL) && (object->GetType()==OT_TELEPORT))
        {
            //A friendly teleport found, is it of the opposite facing?
            teleport* res=static_cast<teleport*>(object);
            if (res->dir==oposs) return res;
        }

        if (dx!=0)
        {
                 cx+=dx;
            cx=(cx>=XYE_HORZ?0:(cx<0?XYE_HORZ-1:cx));
        }
        else
        {
            cy+=dy;
            cy=(cy>=XYE_VERT?0:(cy<0?XYE_VERT-1:cy));
        }
    }
    while ((cx!=x) || (cy!=y));
    return NULL;
}

bool teleport::tryteleport(edir odir, obj* moving, char& nx, char& ny ,dangerous* &reason, bool* pushed)
{
    if (pushed!=NULL) *pushed=false;
    reason=NULL;
    bool go=false;
    switch(dir)
    {
         case(D_UP):
             if (odir!=D_DOWN) return false;
             else break;
         case(D_DOWN):
             if (odir!=D_UP) return false;
             else break;
         case(D_LEFT):
             if (odir!=D_RIGHT) return false;
             else break;
         default:
             if (odir!=D_LEFT) return false;
    }

    //Find exit
    teleport* exit=FindExit(-1,-1);

    obj* Other;
    while (exit!=NULL)
    {
        nx=exit->X();
        ny=exit->Y();
        switch(odir)
        {
            case(D_UP)   : ny++; break;
            case(D_DOWN) : ny--; break;
            case(D_RIGHT): nx++; break;
            default      : nx--;
        }
        nx=(nx<0?XYE_HORZ-1:(nx>=XYE_HORZ?0:nx));
        ny=(ny<0?XYE_VERT-1:(ny>=XYE_VERT?0:ny));
        if ((nx==moving->X()) && (ny==moving->Y()));
        else
        {
             Other=game::Square(nx,ny)->object;
             if (Other!=NULL)
             {
                 otype ty=Other->GetType();
                 if (Other->trypush(odir,moving))
                 {
                      if (pushed!=NULL) *pushed=true;
                     return true;
                 }
                else if ((reason==NULL) && ((ty==OT_BLACKHOLE) || (ty==OT_MINE) || (ty==OT_FIREBALL) )) reason=static_cast<dangerous*>(Other);
             }
             else return true;
        }
        exit=FindExit(exit->X(),exit->Y());
    }
 return false;
}

bool teleport::FindCleanExit(obj* ForObject, char &nx, char &ny, bool AI)
{
    //Find exit
    teleport* exit=FindExit(-1,-1);

    square* sq;
    gobj *gobject;
    edir odir=Opposite(dir);
    while (exit!=NULL)
    {
        nx=exit->X();
        ny=exit->Y();
        switch(odir)
        {
            case(D_UP)   : ny++; break;
            case(D_DOWN) : ny--; break;
            case(D_RIGHT): nx++; break;
            default      : nx--;
        }
        nx=(nx<0?XYE_HORZ-1:(nx>=XYE_HORZ?0:nx));
        ny=(ny<0?XYE_VERT-1:(ny>=XYE_VERT?0:ny));
            sq=game::Square(nx,ny);
            if (sq->object==NULL || (AI &&(ForObject==sq->object) ) )
            {
                gobject=sq->gobject;
                if ((gobject==NULL) || (gobject->CanEnter(ForObject,odir)))
                    return true;
            }

        exit=FindExit(exit->X(),exit->Y());
    }
    return false;
}


void teleport::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    switch(dir)
    {
        case(D_RIGHT): tx=4; break;
        case(D_DOWN):  tx=5; break;
        case(D_LEFT):  tx=6; break;
        //case(D_UP):
        default:       tx=7; break;
    }
    if (FindExit(-1,-1)==NULL) ty=2;
    else
    {
         if (anim) ty=0;
        else ty=1;
        if (game::Mod2()) anim=! anim;
    }
    Drawer D(game::sprites,sz*tx,sz*ty,sz,sz);
    D.Draw(game::screen,x,y);
}

inline bool teleport::trypush(edir dir,obj* pusher) { return trypush_common(dir,pusher, false,NULL); }
inline bool teleport::HasRoundCorner(roundcorner rnc) { return false; }

bool teleport::Loop(bool* died)
{
    if (game::Mod2())
    {
        UpdateSquare();
        if(game::Mod5())
        {

            *died=false;
            bool Moved=false;
            if  (DoMagnetism(true,true,&Moved)) return Moved;
        }
    }
    return false;
}

inline bool teleport::HasBlockColor(blockcolor bc) { return false; }

/**End Class Teleport**/

/**Start Class Black Hole**/
// todo: use inheritance for god's sake!
dangerous::dangerous(square* sq,otype kind)
{
    switch (kind)
    {
        case (OT_MINE) : LandMine(); break;
        case (OT_FIREBALL) : FireBall(D_DOWN); break;
        default:BlackHole();
    }
    fromRound = false;
    absorb=0;
    anim=1;
    R=G=B=255;
    ObjectConstruct(sq);
}

dangerous::dangerous(square* sq,edir dir,bool d)
{
    fromRound = false;
    FireBall(dir);
    disb=d;
    absorb=0;
    anim=1;
    R=G=B=255;
    ObjectConstruct(sq);
}

void dangerous::LandMine()
{
    mine=true;
    fire=false;
    type=OT_MINE;
}

void dangerous::FireBall(edir dir)
{
    mov=0;
    D=dir;
    mine=false;
    fire=true;
    type=OT_FIREBALL;
}

void dangerous::BlackHole()
{
    mine=fire=false;
    type=OT_BLACKHOLE;
}

void dangerous::DrawMine(unsigned int x, unsigned int y)
{
    Drawer D(game::sprites,0,7*sz,sz,sz);
    D.Draw(game::screen,x,y);
}

void dangerous::Draw(unsigned int x, unsigned int y)
{
    if (mine)
    {
        DrawMine(x,y);
        return;
    }
    Uint8 tx,ty;
    Drawer D(game::sprites,0,0,0,0);

    if (fire)
    {
        /*switch(dangerous::D)
        {
            case(D_UP): tx=14; break;
            case(D_DOWN): tx=12; break;
            case(D_LEFT): tx=13; break;
            default: tx=11;
        }*/
        /*
        ty=6;
        tx=11+anim;
        
        anim=(anim==3)?0:anim+1;
        D.ChangeRect(tx*sz,ty*sz,sz,sz);
        D.Draw(game::screen,x,y);*/
        block::Draw(x,y, false, B_RED, fromRound);
        //arrow::DrawF(x,y, B_RED,  dangerous::D, true);
        D.ChangeRect(0*sz,8*sz,sz,sz);
        D.SetColors(options::BFColor[B_RED], 255);
        D.Draw(game::screen,x,y); /**/
        
    }
    else
    {
        if (absorb>0)
        {
            tx=1;
            ty=(int(  (absorb-1) / 10)+3);
        }
        else
        {
            tx=0;
            ty=(anim+3);
        }

        D.ChangeRect(tx*sz,ty*sz,sz,sz);
        D.Draw(game::screen,x,y);


        D.ChangeRect(0,sz*3,sz,sz);
        D.SetColors(R,G,B,255);
        D.Draw(game::screen,x,y);


    }
}

inline bool dangerous::trypush(edir dir,obj* pusher) {
    if (mine || fire)
    {
        if (magnetic::IsInstance(pusher) || (pusher==this) )
             if (trypush_common(dir,pusher, false,NULL))
                 return true;
             else
             {
                 char nx=x, ny=y;
                switch(dir)
                {
                  case (D_UP): ny++; break;
                  case (D_DOWN): ny--; break;
                  case (D_LEFT): nx--; break;
                  default: nx++;
                }

                if (fire)
                {
                    obj* object= game::SquareN(nx,ny)->object;
                    if ((object!=NULL) && (object->GetType()==OT_FIREBALL))
                    {
                        object->Kill();
                        game::SmallBoom(game::SquareN(x,y));


                        //return false;
                    }

                }
                switch(dir)
                {
                    case D_UP: game::SmallBoom(game::SquareN(nx,ny),true, 0, -1 ); break;
                    case D_DOWN: game::SmallBoom(game::SquareN(nx,ny),true, 0, 1 ); break;
                    case D_RIGHT: game::SmallBoom(game::SquareN(nx,ny),true, 1, 0 ); break;
                    case D_LEFT: game::SmallBoom(game::SquareN(nx,ny),true, -1, 0 ); break;
                }
                if(fire) new explosion(game::Square(x,y), 0);

                Kill();
                 return true;
             }
        return false;
    }
    if ((absorb>0) && (pusher->GetType()!=OT_XYE)) return trypush_common(dir,pusher, false,NULL);
    return false;
}
inline bool dangerous::HasRoundCorner(roundcorner rnc) { return false; }

inline void dangerous::Eat() {
    if (mine || fire)
    {
        explosion* ex=new explosion(game::SquareN(x,y),1);
        Kill();
    }
    else
        absorb=1;
}
inline bool dangerous::Busy(obj* entering) {
    if (fire && ((disb) || (entering->GetType()==OT_FIREBALL))) return true;
    return (absorb>0) && (absorb<32);
}

inline void dangerous::ChangeColor(Uint8 nR, Uint8 nG, Uint8 nB) { R=nR; G=nG; B=nB; }
inline bool dangerous::HasBlockColor(blockcolor bc) { return (fire && (bc==B_RED) ); }






bool dangerous::Loop(bool* died)
{
    *died=false;
    if (mine)
    {
        bool Moved=false;
        if(( ! game::Mod5() ) || (! game::counter2)) return false;
        if (DoMagnetism(true,true,&Moved)) return Moved;
    }
    else if (fire)
    {
        bool Moved=false;
        if (DoMagnetism(true,true,&Moved) && Moved) return Moved;

        UpdateSquare();
        mov=(mov>=1)?0:mov+1;
        mov=0;
        if(! mov)
        {
            //new explosion(game::Square(x,y),1);
            disb=true;
            if (trypush(D,this))
                
                return true;
            else
            {
                //new explosion(game::Square(x,y),1);
                mov=2;
            }
        }
        return false;
    }
    else
    {
        if (absorb>0)
        {
            absorb++;
            if (absorb>39) absorb=0;
            UpdateSquare();
        }
        else if (game::Mod4())
        {
            anim=GetRandomInt(1,3);
            UpdateSquare();
        }
        return false;
    }

    return false;
}

/**End Class Black Hole**/

/** Beasts' AI **/
bool BeastAI_Default(beast* dabeast, edir &r)
//gnasher's
{
    edir D[4];
    unsigned int desc=dabeast->X()*XYE_VERT+dabeast->Y()+game::beastcounter;
    //ignore sp 1/3 , do nothing 1/20 , direct 4/5 , pick random 1/4


    //desc = (desc%301)%300;


    int n;
    bool found;
    if
     ((dabeast->GoFindXye(D, n, found,
     /*Ignoreloop*/ true,
     /*IgnoreSp*/ (desc%3==1),
     /*considerTeleports*/ false,
     /*considerStickies*/ false,
     /*range*/ 3 ))
      && (n>0) )
         {

            if (! found)
            {
                if (!(desc%20))
                    return false; //Do nothing
                   if (desc%5!=0) //direct step
                    return OldBeastPickDirectDir(r,D,n,dabeast->X(),dabeast->Y(),game::XYE->X(), game::XYE->Y());
                r=D[desc%n];
            }
            else r=D[0];
            return true;
         }
 return false;

}

bool BeastAI_Blob(beast* dabeast, edir &r)
//blob's
{
    bool intmode= (beast::BeastCount(BT_BLOBBOSS));
    unsigned int desc=dabeast->X()*XYE_VERT+dabeast->Y()+game::beastcounter;
    edir D[4];
    int n;
    bool found;
    if
     ((dabeast->GoFindXye(D, n, found,
     /*Ignoreloop*/ true,
     /*IgnoreSp*/ (desc%4==1) && ! intmode,
     /*considerTeleports*/ intmode,
     /*considerStickies*/ intmode,
     /*range*/ (intmode && (desc%10<=7))?150:5 ))
      && (n>0) )
         {

            if (! found)
            {
                if (  desc%3==1   )
                    return false; //Do nothing
                   if (desc%5<=3) //direct step
                    return OldBeastPickDirectDir(r,D,n,dabeast->X(),dabeast->Y(),game::XYE->X(), game::XYE->Y());
                r=D[desc%n]; //pick random
            }
            else r=D[0];
            return true;
         }
 return false;
}

bool BeastAI_Virus(beast* dabeast, edir &r)
//virus'
{
    edir D[4];
    unsigned int desc=dabeast->X()*XYE_VERT+dabeast->Y()+game::beastcounter;
    int n;
    bool found;
    if
     ((dabeast->GoFindXye(D, n, found,
     /*Ignoreloop*/ true,
     /*IgnoreSp*/ desc%2,
     /*considerTeleports*/ false,
     /*considerStickies*/ false,
     /*range*/ 3 ))
      && (n>0) )
         {

            if (! found)
            {
                unsigned char tm=desc%10;
                if (tm==7)
                    return false; //Do nothing
                   if (tm<=6) //direct step
                    return OldBeastPickDirectDir(r,D,n,dabeast->X(),dabeast->Y(),game::XYE->X(), game::XYE->Y());
                r=D[desc%n]; //pick random
            }
            else r=D[0];
            return true;
         }
 return false;


}

bool BeastAI_Spike(beast* dabeast, edir &r)
//spike's
{
    edir D[4];
    unsigned int desc=dabeast->X()*XYE_VERT+dabeast->Y()+game::beastcounter;
    int n;
    bool found;
    if
     ((dabeast->GoFindXye(D, n, found,
     /*Ignoreloop*/ true,
     /*IgnoreSp*/ desc%10<=5,
     /*considerTeleports*/ false,
     /*considerStickies*/ false,
     /*range*/ 4 ))
      && (n>0) )
         {

            if (! found)
            {
                if (desc%100==45)
                    return false; //Do nothing
                   if (desc%101<=100) //direct step
                    return OldBeastPickDirectDir(r,D,n,dabeast->X(),dabeast->Y(),game::XYE->X(), game::XYE->Y());
                r=D[desc%n]; //pick random
            }
            else r=D[0];
            return true;
         }
 return false;

}

bool BeastAI_Twister(beast* dabeast, edir &r)
//twister's
{
    edir D[4];
    unsigned int desc=dabeast->X()*XYE_VERT+dabeast->Y()+game::beastcounter;
    int n;
    bool found;
    if
     ((dabeast->GoFindXye(D, n, found,
     /*Ignoreloop*/ true,
     /*IgnoreSp*/ desc%10<=7,
     /*considerTeleports*/ false,
     /*considerStickies*/ false,
     /*range*/ 10 ))
      && (n>0) )
         {

            if (! found)
            {
                if (desc%2==1)
                    return false; //Do nothing
                   if (desc%80<=60) //direct step
                    return OldBeastPickDirectDir(r,D,n,dabeast->X(),dabeast->Y(),game::XYE->X(), game::XYE->Y());
                r=D[desc%n]; //pick random
            }
            else r=D[0];
            return true;
         }
 return false;

}


bool BeastAI_Dard(beast* dabeast, edir &r)
//dards have a very special AI
{
    int *v = &(dabeast->AIValue);
    bool go=true;

    char xx=game::XYE->X(), bx=dabeast->X();
    char xy=game::XYE->Y(), by=dabeast->Y();

    if (bx==xx)
    {
        if (xy>by) r= D_UP;
        else r=D_DOWN;
    }
    else if (by==xy)
    {
        if (xx>bx) r= D_RIGHT;
        else r=D_LEFT;

    }
    else
        go=false;


    edir f=dabeast->fac;
    if ( ((! go) || (r!=f)) && ( (*v)>0))
    {
        r=f;
        (*v)--;
        return true;
    }
    if (go)
    {
        (*v)=10;
        return true;
    }


 return false;
}

bool IsPatienceAt(square* sq)
{
    obj *object=sq->object;
    if ((object!=NULL) && (object->GetType()==OT_BEAST))
    {
        beast* b= static_cast<beast*>(object);
        if (b->BelongsToBeastType(BT_PATIENCE)) return (b->AIValue);
    }

    return false;
}


bool BeastAI_Patience(beast* dabeast, edir &r)
{

    edir D[4];
    int n;
    bool found;



    if(game::AllowRoboXyeMovement())
    {
        if(dabeast->AIValue) dabeast->AIValue--;


        if(dabeast->GoFindXye(D, n, found,
     /*Ignoreloop*/ false,
     /*IgnoreSp*/ false,
     /*considerTeleports*/ true,
     /*considerStickies*/ true,
     /*range*/ 600 , /*random*/ false)&&(n==1)&&found)
        {
            r=D[0];
            dabeast->AIValue=1;
            return true;
        }

        if(dabeast->GoFindASquare(IsPatienceAt,D, n, found,
     /*Ignoreloop*/ false,
     /*IgnoreSp*/ false,
     /*considerTeleports*/ true,
     /*considerStickies*/ true,
     /*range*/ 600 , /*random*/ false) &&(n==1)&&found )

         {
             //dabeast->AIValue=1;
            r=D[0];
            return true;
         }
    }

 return false;

}

bool BeastAI_Tiger(beast* dabeast, edir &r)
{

    edir D[4];
    int n;
    bool found;

    if
     (( game::AllowRoboXyeMovement() || !(game::counter2||game::counter5) ) && (dabeast->GoFindXye(D, n, found,
     /*Ignoreloop*/ false,
     /*IgnoreSp*/ false,
     /*considerTeleports*/ true,
     /*considerStickies*/ true,
     /*range*/ 600 , /*random*/ false)) //Most powerful AI possible.
      && (n==1) )
         {
             if(!found) return false;
            r=D[0];
            return true;
         }
 return false;

}


bool AI_BlobBoss_Gem(square* sq)
{
    obj* object=sq->object;
    if (object==NULL) return false;
    return (object->GetType()==OT_GEM);
}



bool BeastAI_BlobBoss(beast* dabeast, edir &r)
//BlobBoss
{

    edir D[4];
    int n;
    bool found;
    //dabeast->AIValue= (dabeast->AIValue<20)?dabeast->AIValue+1:0;


    if (dabeast->GoFindASquare( IsXyeAt,D, n, found,/*Ignoreloop*/ false,/*IgnoreSp*/ false,/*considerTeleports*/ true, /*considerStickies*/ true, /*range*/ 6 , /*random*/ false))
         {
             if (n==1)
             {
                 r=D[0];
                 return (true);
             }
             if ((dabeast->GoFindASquare( AI_BlobBoss_Gem,D, n, found,/*Ignoreloop*/ false,/*IgnoreSp*/ false,/*considerTeleports*/ true, /*considerStickies*/ true, /*range*/ 100 , /*random*/ false))
                 && (n==1))
            {
                r=D[0];
                return (true);
            }

             if (dabeast->GoFindASquare( IsXyeAt,D, n, found,/*Ignoreloop*/ false,/*IgnoreSp*/ false,/*considerTeleports*/ true, /*considerStickies*/ true, /*range*/ 100 , /*random*/ false))
            {
                  if ((n>1) && PickDirectDir(r,D,n,dabeast->X(),dabeast->Y(),game::XYE->X(), game::XYE->Y(),false,true));
                  else
                     r=D[0];
                 return true;
            }


         }

 return false;

}


bool BeastAI_Ranger(beast* dabeast, edir &r)
//ranger's
{
    bool force=false; edir forced;
    char x=dabeast->X(),y=dabeast->Y(),xx=game::XYE->X(),xy=game::XYE->Y();
    if (game::Mod2() && game::Mod5());
    else if ((x==xx) && game::Mod2())
    {
        forced=(xy>y)?D_UP:D_DOWN;
        force=true;
    }
    else if ((y==xy) && game::Mod2())
    {
        forced=(xx>x)?D_RIGHT:D_LEFT;
        force=true;
    }
    else
        return false;
    if (force && (dabeast->fac!=forced)) return false;
    edir D[4];
    int n;
    bool found;
    if
     ((dabeast->GoFindXye(D, n, found,
     /*Ignoreloop*/ true,
     /*IgnoreSp*/ false,
     /*considerTeleports*/ false,
     /*considerStickies*/ true,
     /*range*/ 30 ))
      && (n==1) )
         {
             r=D[0];
            return ((!force) || (r==forced));
         }
 return false;

}


bool BeastAI_Static(beast* dabeast, edir &r)
{
    if (dabeast->AIValue)
    {
        r= (dabeast->fac);
        return true;
    }
 return false;
}

bool BeastAI_StaticFails(beast* dabeast, edir &r)
{
    dabeast->AIValue=false;
    dabeast->NewAnim();
 return(false);

}


bool BeastAI_ReallyDumb(beast* dabeast, edir &r)
{
    r= (dabeast->fac);
 return true;
}

bool BeastAI_WardFails(beast* dabeast, edir &r)
{
    r= Opposite(dabeast->fac);
    return (true);
}

bool BeastAI_SpinnerFails(beast* dabeast, edir &r)
{
    r= Clock(dabeast->fac);
    return (true);
}

bool BeastAI_AspinnerFails(beast* dabeast, edir &r)
{
    r= AClock(dabeast->fac);
    return (true);
}


bool BeastAI_OnFail_DoNothing(beast* dabeast, edir &r) { return false; }



/** Start Class Beast **/
unsigned int beast::count[BTYPEN];

bool beast::Floats()
{
    return ((kind==BT_PATIENCE) || (kind==BT_DARD));
}

void beast::ResetCounts()
{
    for (int i=0; i<BTYPEN; i++)  beast::count[i]=0;
}

unsigned int beast::BeastCount(btype k)
{
    return (beast::count[k]);
}


beast::beast(square* sq, btype personality, edir inidir)
{
    type=OT_BEAST;
    kind=personality;
    beast::count[kind]++;
    PickAI();
    fac=inidir;
    PickFirstAnim();
    AIValue=0;
    ObjectConstruct(sq);
}

void beast::PickFirstAnim()
{
    switch(kind)
    {
        case (BT_BLOB): case (BT_BLOBBOSS):
            anim=GetRandom<char>(0,2);break;
        case (BT_SPINNER) : case(BT_ASPINNER):
            anim=GetRandom<char>(0,1)*2;break;
        default: anim=GetRandom<char>(0,1);
    }
}

void beast::PickAI()
{
    switch(kind)
    {
        case(BT_VIRUS): AI=BeastAI_Virus; break;
        case(BT_TWISTER): AI=BeastAI_Twister; break;
        case(BT_RANGER): AI=BeastAI_Ranger; break;
        case(BT_SPIKE): AI=BeastAI_Spike; break;
        case(BT_BLOB): AI=BeastAI_Blob; break;
        case(BT_BLOBBOSS): AI=BeastAI_BlobBoss; break;
        case(BT_DARD): AI=BeastAI_Dard; break;
        case(BT_PATIENCE): AI=BeastAI_Patience; break;
        case(BT_TIGER): AI=BeastAI_Tiger; break;
        case(BT_WARD): case(BT_SPINNER): case(BT_ASPINNER): AI=BeastAI_ReallyDumb; break;
        case(BT_STATIC): AI=BeastAI_Static;  break;
        default: AI=BeastAI_Default;
    }
    switch(kind)
    {
        case(BT_WARD): OnFail=BeastAI_WardFails; break;
        case(BT_SPINNER): OnFail=BeastAI_SpinnerFails; break;
        case(BT_ASPINNER): OnFail=BeastAI_AspinnerFails; break;
        case(BT_STATIC) : OnFail=BeastAI_StaticFails; break;
        default: OnFail=BeastAI_OnFail_DoNothing;
    }
}

void beast::NewAnim()
{
    char old=anim;
    switch(kind)
    {
        case(BT_STATIC):
            if (AIValue)
            {
                anim=(anim>2)?0:anim+1;
                break;
            }

        case(BT_PATIENCE):
            if (anim==2) anim=GetRandom<char>(0,1);
            else anim=2;
            break;

        case(BT_TIGER):
            if (anim==2) anim=GetRandom<char>(0,1);
            else anim=2;
            break;

        case(BT_BLOB):  case(BT_BLOBBOSS):
            anim=anim+(GetRandom<char>(1,2));
            while(anim>2) anim-=3;
            break;
        case(BT_SPINNER):case(BT_ASPINNER):
            anim=anim+1;
            while(anim>3) anim-=4;
            break;

        default:
            anim=(anim==1?0:1);
    }
    if (old!=anim) UpdateSquare();
}

void beast::Draw(unsigned int x, unsigned int y,btype kind, edir fac, unsigned char anim,int AIValue)
{
    Uint8 tx,ty;
    switch(kind)
    {
        case(BT_TWISTER):
            tx=17; ty=anim;
            break;
        case(BT_SPIKE):
            tx=19; ty=anim;
            break;
        case(BT_VIRUS):
            tx=18;
            ty=anim;
            break;
        case(BT_BLOB):
            tx=19;
            ty=anim+6;
            break;
        case(BT_BLOBBOSS):
            tx=19;
            ty=anim+12;
            break;

        case(BT_TIGER):
            tx=10;
            ty=12+anim;
            break;

        case(BT_PATIENCE):
            tx=19;
            ty=9+anim;
            break;

        case(BT_STATIC):
            if (AIValue)
            {
                tx=11+anim;
                ty=8;
            }
            else
            {
                tx=11;
                ty=15+anim;

            }
            break;


        case(BT_DARD):
            switch(fac)
            {
                case(D_UP): tx=19; break;
                case(D_LEFT): tx=18; break;
                case(D_DOWN): tx=17; break;
                default: tx=16;
            }
            ty= anim+2;
            break;
        case(BT_WARD):
            switch(fac)
            {
                case(D_UP): tx=19; break;
                case(D_LEFT): tx=18; break;
                case(D_DOWN): tx=17; break;
                default: tx=16;
            }
            ty= anim+4;
            break;

        case(BT_RANGER):
            switch(fac)
            {
                case(D_UP): tx=15; break;
                case(D_LEFT): tx=14; break;
                case(D_DOWN): tx=13; break;
                default: tx=12;
            }
            ty= anim+18;
            break;


        case(BT_SPINNER):case(BT_ASPINNER):
            tx=11;
            if (kind==BT_ASPINNER)
                ty=12-anim;
            else
                ty=9+anim;
            break;


        default: //gnasher
            tx=16;
            if (anim) ty=1;
            else ty=0;

    }


    Drawer D(game::sprites,sz*tx,ty*sz,sz,sz);
    D.Draw(game::screen,x,y);
}

void beast::Draw(unsigned int x, unsigned int y)
{
    Draw(x,y,kind,fac,anim,AIValue);
}

bool beast::trypush(edir dir,obj* pusher)
{
    if ((pusher) && (pusher->GetType()!=OT_XYE) )
    {
        if (trypush_common(dir,pusher, false,NULL))
        {
            if (kind==BT_STATIC)
            {
                AIValue=true;
                fac=dir;
            }
            tic=game::Counter();
            return true;
        }
    }
    return(false);
}

bool beast::HasRoundCorner(roundcorner rnc) {return false; }
bool beast::HasBlockColor(blockcolor bc) { return false; } ;

bool beast::Loop_Sub(bool* died)
{

    *died=false;
    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved))
        return Moved;


    edir go=D_DOWN;
    if (AI(this,go))
    {
        fac=go;
        if (trypush_common(fac,NULL,false,died))
            return true;
        else
        {
            if ((! (*died) ) && OnFail(this,go) )
            {
                fac=go;
                if (trypush_common(fac,NULL,false,died)) return true;
                else return false;
            }
            else return false;
        }
        //blerg
    }
    return false;
}


bool beast::Loop(bool* died)
{
    bool OnTime=false;
    
    switch(kind)
    {
        case BT_SPINNER: case BT_ASPINNER:
            if ( ( game::counter5==0) || ( game::counter5==3) )
                NewAnim();
            break;
        case BT_STATIC:
            if( AIValue || (game::counter7==0) )
                NewAnim();
            break;
                
        default:
            if(game::counter7==0)
                 NewAnim(); 
    };
    
    switch(kind)
    {
        case(BT_PATIENCE): case(BT_TIGER): case(BT_DARD): case(BT_RANGER): OnTime=true; break;
        case(BT_WARD): case(BT_SPINNER): case(BT_ASPINNER):  OnTime=(! game::counter5); break;
        case(BT_STATIC): OnTime=(game::counter2==0); break;

        default: OnTime= ((! game::counter2) && (! game::counter5));
    }

    if (OnTime && Loop_Sub(died))
    {
        if (! (*died) ) {
            deathqueue::add(x,y,KT_KILLXYE);
        }
        return (true);
    }
    deathqueue::add(x,y,KT_KILLXYE);
 return false;
}
/** End Class Beast **/





/** Rattler and Rnode**/
rattler::rattler(square* sq, edir inidir, unsigned int G)
{
    grow=G;
    type=OT_RATTLER;
    fac=inidir;
    anim=Chance(0.5);
    first=last=NULL;
    tic=0;
    ObjectConstruct(sq);
}

bool rattler::Floats()
{
    return ((first!=NULL)||(grow!=0));
}

edir GetConnectionDir(int x1,int y1, int x2, int y2)
{
    if (x2 == x1) //Connection is vertical
    {
        if ( FixVert(y1+1) == y2)
            return (D_UP);
        else
            return (D_DOWN);
    }
    //Connection is horizontal

        if ( FixHorz(x1-1) == x2)
            return (D_LEFT);
 return (D_RIGHT);
}


void rattler::OnDeath()
{
    if (first) //replace first node with a new head
    {
        unsigned char fx=first->x, fy=first->y;
        edir D=GetConnectionDir(fx,fy,x,y);
        rnode* nx=first->next;
        first->forgetkill=true;
        first->Kill();
        rattler*nh= new rattler(game::Square(fx,fy),D,grow);
        nh->first=nx;
        nh->last=NULL;
        if (nx)
        {
            nx->prevhead=true;
            nx->prev=NULL;
        }
        while (nx)
        {
            nx->UpdateSquare();
            nx->head=nh;
            nh->last=nx;
            nx= nx->next;
        }

    }
}

void rattler::NewAnim()
{
    if (! game::counter4)
    {
        UpdateSquare();
        anim=(! anim);
    }
    if ((! (game::counter9)) && (Chance(0.5) ))
    {
        rnode* cur=first;
        while (cur)
        {
            cur->NewAnim();
            cur=cur->next;
        }

    }
}

void rattler::Grow(unsigned int G)
{
    grow+=G;
}

void rattler::Node(square* sq)
{
    unsigned char sx=sq->sqx,sy=sq->sqy;
    unsigned char lx,ly;
    if (last==NULL)
    {
        lx=x;
        ly=y;
    }
    else
    {
        lx=last->x;
        ly=last->y;
    }

    bool doit=false;
    if ((sx==lx) && (( sy==FixVert(ly+1) ) ||  (sy== FixVert(ly-1) )))
        doit=true;
    else if ((sy==ly) && (( sx==FixHorz(lx+1) ) ||  (sx==FixHorz(lx-1) )))
        doit=true;

    if (doit)
    {
        rnode* nd=new rnode(sq,this);
        if (first==NULL)
        {
            first=nd;
            last=nd;
            nd->prevhead=true;
            fac=GetConnectionDir(sx,sy,x,y);
        }
        else
        {
            last->next=nd;
            nd->prev=last;
            last=nd;

        }
    }

}

void rattler::Draw(unsigned int x, unsigned int y,bool anim,edir fac, rnode* first)
{
    Uint8 tx,ty;
    ty=9+anim+ ((first!=NULL)*2) ;
    switch(fac)
    {
            case(D_UP):
                tx=13; break;
            case(D_DOWN):
                tx=15;break;
            case(D_LEFT):
                tx=12;break;
            default:
                tx=14;
    }
    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.Draw(game::screen,x,y);
}

void rattler::Draw(unsigned int x, unsigned int y)
{
    Draw(x,y,anim,fac,first);
}


bool AI_Rattler_XYEorFood(square* sq)
{
    obj* object=sq->object;
    if (object==NULL) return false;
    switch(object->GetType())
    {
        case(OT_XYE): case(OT_RATTLERFOOD): return true;
    }
 return false;
}

bool AI_Rattler(rattler* dabeast, edir &r)
{

    edir D[4];
    int n;
    bool found;
    if
     ((dabeast->GoFindASquare(AI_Rattler_XYEorFood,D, n, found,
     /*loop*/ false,
     /*IgnoreSp*/ false,
     /*considerTeleports*/ false,
     /*considerStickies*/ false,
     /*range*/ 600 , /*UseRandom*/ false))
      && (n>0) )
         {
            if (n>1) return (PickMostDirectDir(r,D,n,dabeast->X(),dabeast->Y(),game::XYE->X(),game::XYE->Y()));

             //else
                r=D[0];
            return true;
         }
 return false;

}


bool rattler::Loop(bool* died)
{
    //for now just play with anim:
    NewAnim();
    tic=(tic>=7)?0:tic+1;
    if (! tic)
    {
        char lx=x, ly=y,tx,ty;
        char dx=0,dy=0;

        if (AI_Rattler(this,fac))
        {
            switch(fac)
            {
                case(D_UP): dy++; break;
                case(D_DOWN): dy--; break;
                case(D_LEFT): dx--; break;
                case(D_RIGHT): dx++; break;
            }
            dx=x+dx;
            dy=y+dy;
            dx= (dx>=XYE_HORZ)?0:(dx<0)?XYE_HORZ-1:dx;
            dy= (dy>=XYE_VERT)?0:(dy<0)?XYE_VERT-1:dy;

            if (AI_Rattler_XYEorFood(game::Square(dx,dy)))
            {
                grow++;
                if ((game::XYE->X()==dx) && (game::XYE->Y()==dy))
                    game::XYE->Kill();
            }



            move( dx,dy);
            rnode* cur=first;
            while (cur)
            {
                tx=cur->X();
                ty=cur->Y();
                cur->move(lx,ly);
                lx=tx;
                ly=ty;
                cur=cur->next;
            }
            if (grow>0)
            {
                Node(game::Square(lx,ly));
                grow--;
            }
            return(true);
        }
        else tic=7;
    }
    return (false);
}

rnode::rnode(square* sq, rattler* rhead)
{
    forgetkill=false;
    head=rhead;
    type=OT_RATTLERNODE;
    prev=NULL;
    next=NULL;
    prevhead=false;
    anim=Chance(0.5);
    ObjectConstruct(sq);
}

void rnode::OnDeath()
{
    if (forgetkill) return;

    //Make sure previous rattler/node knows they lost this node.
    if (prevhead)
    {
        head->first=NULL;
        head->last=NULL;
        head->UpdateSquare();
    }
    else
    {
        prev->next=NULL;
        prev->UpdateSquare();
        
    }
    if ( head->last == this ) {
        head->last = prev;
    }

    if (next) //replace next node with a new head
    {
        unsigned char fx=next->x, fy=next->y;
        edir D=GetConnectionDir(fx,fy,x,y);
        rnode* nx=next->next;
        next->forgetkill=true;
        next->Kill();
        rattler*nh= new rattler(game::Square(fx,fy),D,0);
        nh->first=nx;
        nh->last=NULL;
        if (nx)
        {
            nx->prevhead=true;
            nx->prev=NULL;
        }
        while (nx)
        {
            nx->head=nh;
            nh->last=nx;
            nx= nx->next;
        }
    }
}

void rnode::NewAnim()
{
    if ((! (game::counter9)) && (Chance(0.5) ))
    {
        UpdateSquare();
        anim=(! anim);
    }
}




void rnode::Draw(unsigned int drawx, unsigned int drawy)
{

    Uint8 tx=0,ty=0,px,py;

    //I am assuming that everything is always going to be allright

    if (prevhead)
    {
        px=head->X();
        py=head->Y();
    }
    else
    {
        px=prev->x;
        py=prev->y;
    }


    if (next==NULL) //Just show tail
    switch (GetConnectionDir(x,y,px,py))
    {
        case(D_UP):
            tx=12+anim;
            ty=17;
            break;
          case(D_DOWN):
            tx=12+anim;
            ty=16;
            break;
        case(D_LEFT):
            tx=14+anim;
            ty=17;
            break;
        default: //Right
            tx=14+anim;
            ty=16;
    }
    else
    { //enum edir { D_UP=0,D_DOWN=1,D_LEFT=2,D_RIGHT=3};
        edir d1=GetConnectionDir(x,y,px,py);
        edir d2=GetConnectionDir(x,y,next->x,next->y);
        edir d;
        if (d2<d1) //This seriously makes everything much easier
        {
            d=d2;
            d2=d1;
            d1=d;
        }
        switch(d1)
        {
            case(D_UP):
                switch(d2)
                {
                    case(D_DOWN): //|
                        tx=12+anim; ty=13; break;
                    case(D_LEFT): // _|
                        tx=13; ty=14+anim; break;
                    default: // |_
                        tx=12; ty=14+anim;
                }
                break;
            case(D_DOWN):
                ty=14+anim;

                switch(d2)
                {
                    case(D_LEFT): // ¯|
                        tx= 15; ;break;
                    default: // |¯
                        tx= 14;
                }
                break;
            default : //100% sure it is the --
                tx=14+anim;
                ty=13;
        }

    }
    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.Draw(game::screen,drawx,drawy);

}


/**Start Class RFood**/
rfood::rfood(square* sq)
{
    anim=Chance(0.5);
    type=OT_RATTLERFOOD;
    ObjectConstruct(sq);
}
void rfood::Draw(unsigned int x,unsigned int y,bool anim)
{
    Drawer D(game::sprites,11*sz, (13+anim)*sz ,sz,sz);
    D.Draw(game::screen,x,y);
}

void rfood::Draw(unsigned int x, unsigned int y)
{
    Draw(x,y,anim);
}

bool rfood::trypush(edir dir,obj* pusher) { return trypush_common(dir,pusher, true,NULL); }

bool rfood::Loop(bool* died)
{
    if (Chance(0.25))
    {
        anim=! anim;
        UpdateSquare();
    }
    *died=false;

    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved))
        return Moved;
        edir go;

    return false;
}


/**End Class Block**/







/**Start Class Earth**/
Uint8 earth::dR,earth::dG,earth::dB;
earth::earth(square* sq)
{
    type=OT_EARTH;
    R=dR;
    G=dG;
    B=dB;
    round=false;
    ObjectConstruct(sq);
}

void earth::ResetDefaults()
{
    dR = options::EarthColor.r;
    dG = options::EarthColor.g;
    dB = options::EarthColor.b;
}

void earth::SetDefaultColor(Uint8 nR,Uint8 nG,Uint8 nB)
{
    dR=nR;
    dG=nG;
    dB=nB;
}

void earth::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx;
    if (round) tx=2;
    else tx=1;
    Drawer D(game::sprites,sz*tx,sz,sz,sz);
    D.SetColors(R,G,B,255); //alpha now depends on the skin
    D.Draw(game::screen,x,y);

}

void earth::SetRound(bool flag)
{
    round=flag;
}

bool earth::trypush(edir dir,obj* pusher)
{
    gobj* gobject;
    switch (pusher->GetType())
    {
        case (OT_XYE): case(OT_ROBOXYE):


        if (gobject=game::Square(x,y)->gobject)
        {
            if (! gobject->CanEnter(pusher,dir))
                return false;
        }


        game::Square(x,y)->object=NULL;
        recycle::add(this);;
        return true;
    }
    return false;
}

bool earth::HasRoundCorner(roundcorner rnc) { return round; }

void earth::ChangeColor(Uint8 nR, Uint8 nG, Uint8 nB)
{
    R=nR;G=nG;B=nB;
}

inline bool earth::HasBlockColor(blockcolor bc) { return false; }
/**End Class earth**/



/**Start Class Key**/
unsigned int key::Got[4];

key::key(square* sq, blockcolor color)
{
    type=OT_KEY;
    c=color;
    ObjectConstruct(sq);
}
void key::Draw(unsigned int x, unsigned int y)
{
    Drawer D(game::sprites,sz*6,sz*4,sz,sz);
    D.SetColors(options::BKColor[c],255);
    D.Draw(game::screen,x,y);

}

bool key::Loop(bool* died)
{

    *died=false;

    bool Moved=false;
    if  (DoMagnetism(true,true,&Moved))
        return Moved;
        edir go;

    return false;
}


bool key::trypush(edir dir,obj* pusher)
{
    gobj* gobject;
    if (pusher->GetType()==OT_XYE)
    {

        if (gobject=game::Square(x,y)->gobject)
        {
            if (! gobject->CanEnter(pusher,dir))
                return (trypush_common(dir,pusher,false,NULL));
        }


        Got[(unsigned int)(c)]++;
        game::Square(x,y)->object=NULL;
        Kill(false);
        return true;
    }
      return (trypush_common(dir,pusher,false,NULL));
}

bool key::HasRoundCorner(roundcorner rnc) { return false; }

inline bool key::HasBlockColor(blockcolor bc) { return false; }

void key::UseKey(blockcolor ofcolor)
{
    Got[(unsigned int)(ofcolor)]--;
}
void key::ResetCounts()
{
    Got[0]=Got[1]=Got[2]=Got[3]=0;
}
bool key::GotKey(blockcolor ofcolor)
{
    return (Got[(unsigned int)(ofcolor)] > 0 );
}

bool key::GetXyesKeys(unsigned int &yl,unsigned int &rd,unsigned int &bl,unsigned int &gr)
{
    yl=Got[0];
    rd=Got[1];
    bl=Got[2];
    gr=Got[3];
    return (yl || rd || bl|| gr);
}


/**End Class Key**/

/**Start Class Lock**/

lock::lock(square* sq, blockcolor color)
{
    type=OT_LOCK;
    c=color;
    ObjectConstruct(sq);
}
void lock::Draw(unsigned int x, unsigned int y)
{
    Drawer D(game::sprites,sz*7,sz*4,sz,sz);
    D.SetColors(options::BKColor[c],255);
    D.Draw(game::screen,x,y);
}

bool lock::trypush(edir dir,obj* pusher)
{
    if ((pusher->GetType()==OT_XYE) && (key::GotKey(c)))
    {
        key::UseKey(c);
        game::Square(x,y)->object=NULL;
        recycle::add(this);;
        return true;
    }
    return false;
}

bool lock::HasRoundCorner(roundcorner rnc) { return false; }

inline bool lock::HasBlockColor(blockcolor bc) { return false; }


/**End Class Lock**/




/**Start Class Gem**/

unsigned int gem::count[5];

gem::gem(square* sq,gemtype t)
{
    count[4]++;
    count[(unsigned int)(t)]++;
    type=OT_GEM;
    gemkind=t;
    anim=!(Chance(0.5));
    ObjectConstruct(sq);
}

void gem::ResetCounts()
{
    count[0]=count[1]=count[2]=count[3]=count[4]=0;
}

bool gem::GotAllGems()
{
    return (count[4]==0);
}

bool gem::GotAllGems(blockcolor c)
{
    return (count[(unsigned int)(c)]==0);
}

bool gem::GetRemanents(unsigned int &yl,unsigned int &rd,unsigned int &bl,unsigned int &gr)
{
    yl=count[0];
    rd=count[1];
    bl=count[2];
    gr=count[3];
    return ((yl) || (rd) || (bl) || (gr));
}

bool gem::Loop(bool* died)
{
    *died=false;
    if ((game::Mod4()) && (Chance(0.04)))
    {
        UpdateSquare();
        anim=! anim;
    }
    return false;
}

void gem::Draw(unsigned int x, unsigned int y,gemtype gemkind,bool anim)
{
    Uint8 tx,ty;
    if (anim) ty=3;
    else ty=4;
    switch(gemkind)
    {
        case(GEM_DIAMOND): tx=2; break;
        case(GEM_RUBY): tx=3; break;
        case(GEM_EMERALD): tx=4; break;
        default: tx=5; break;
    }
    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.Draw(game::screen,x,y);
}


void gem::Draw(unsigned int x, unsigned int y)
{
    Draw(x,y,gemkind,anim);
}

bool gem::trypush(edir dir,obj* pusher)
{
    if (pusher->GetType()==OT_XYE)
    {
        gobj* gobject;
        if (gobject=game::Square(x,y)->gobject)
        {
            if (! gobject->CanEnter(pusher,dir))
                return false;
        }


        game::Square(x,y)->object=NULL;
        count[4]--;
        count[(unsigned int)(gemkind)]--;
        if (! count[4])
            game::TerminateGame(true);


        recycle::add(this);;
        return true;
    }
    return false;
}

inline bool gem::HasRoundCorner(roundcorner rnc) { return false; }
inline bool gem::HasBlockColor(blockcolor bc) { return false; }

/**End Class Gem**/


/**Start Class Gem**/

unsigned int star::count, star::acquired;

star::star(square* sq)
{
    count++;
    type=OT_STAR;
    anim=!(Chance(0.5));
    ObjectConstruct(sq);
}

void star::ResetCounts()
{
    count=acquired=0;
}

int star::GetRemaining()
{
    return (count-acquired);
}

int star::GetAcquired()
{
    return (acquired);
}

bool star::Loop(bool* died)
{
    *died=false;
    if ((game::Mod4()) && (Chance(0.04)))
    {
        UpdateSquare();
        anim=! anim;
    }
    return false;
}


void star::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    tx=9;
    if (anim) ty=12;
    else ty=13;
    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.Draw(game::screen,x,y);
}

bool star::trypush(edir dir,obj* pusher)
{
    if (pusher->GetType()==OT_XYE)
    {
        gobj* gobject;
        if (gobject=game::Square(x,y)->gobject)
        {
            if (! gobject->CanEnter(pusher,dir))
                return false;
        }


        game::Square(x,y)->object=NULL;
        acquired++;
        recycle::add(this);
        return true;
    }
    return false;
}

inline bool star::HasRoundCorner(roundcorner rnc) { return false; }
inline bool star::HasBlockColor(blockcolor bc) { return false; }

/**End Class Gem**/


/**Begin Trick Door Class**/

/** Trick Door**/

SDL_Color tdoor::DefColor;
SDL_Color tdoor::ForceArrowDefColor;

tdoor::tdoor(square* sq,tdtype t,bool up,bool right, bool down, bool left)
{
     ALLOW_UP=up;
     ALLOW_RIGHT=right;
     ALLOW_DOWN=down;
     ALLOW_LEFT=left;
     tt=t;
     switch(tt)
     {
        case(td_FORCEARROW_UP): case(td_FORCEARROW_LEFT): case(td_FORCEARROW_DOWN): case(td_FORCEARROW_RIGHT):
           ALLOW_NONXYE=true;
           R=ForceArrowDefColor.r;
           G=ForceArrowDefColor.g;
           B=ForceArrowDefColor.b;

           break;
        default:
           ALLOW_NONXYE=false;
           R=DefColor.r;
           G=DefColor.g;
           B=DefColor.b;

     }
      type=OT_TRICKDOOR;
     anim=true;
     GObjectConstruct(sq);
}

void tdoor::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;


    switch (tt)
    {
        case(td_HORZ): tx=8; break;
        case(td_VERT): tx=7; break;
        case(td_FORCEARROW_DOWN): case(td_FORCEARROW_RIGHT): tx=10; break;
        case(td_FORCEARROW_UP): case(td_FORCEARROW_LEFT): tx=9; break;
        default: tx=6;
    }

    switch (tt)
    {
        case(td_FORCEARROW_UP): case(td_FORCEARROW_DOWN): ty=11; break;
        case(td_FORCEARROW_LEFT): case(td_FORCEARROW_RIGHT): ty=10; break;
        default:
        if (anim)
            ty=10;
        else
            ty=11;
    }

    if ((game::counter8==0)) anim=!anim;
    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.SetColors(R,G,B,255);
    D.Draw(game::screen,x,y);



}

void tdoor::ChangeColor(Uint8 nR,Uint8 nG,Uint8 nB)
{
    R=nR;
    G=nG;
    B=nB;
}

void tdoor::OnEnter(obj *entering) {}
void tdoor::OnLeave(obj *entering) {}
bool tdoor::CanEnter(obj *entering, edir dir)
{
    otype ot=entering->GetType();
    if ((!ALLOW_NONXYE) && (ot!=OT_XYE) && (ot!=OT_ROBOXYE)) return false;
    switch(dir)
    {
        case(D_UP): return ALLOW_DOWN;
        case(D_RIGHT): return ALLOW_LEFT;
        case(D_LEFT): return ALLOW_RIGHT;
    }
    return ALLOW_UP;
}
bool tdoor::CanLeave(obj *entering, edir dir) { return true; }

void tdoor::ResetDefaults()
{
    DefColor = options::OneWayDoorColor; 
    ForceArrowDefColor = options::ForceArrowColor;
}

void tdoor::ChangeDefaultColor(Uint8 nR,Uint8 nG,Uint8 nB)
{
    DefColor.r=nR;
    DefColor.g=nG;
    DefColor.b=nB;
    DefColor.unused=255;
}

void tdoor::ChangeForceArrowDefaultColor(Uint8 nR,Uint8 nG,Uint8 nB)
{
    ForceArrowDefColor.r=nR;
    ForceArrowDefColor.g=nG;
    ForceArrowDefColor.b=nB;
    ForceArrowDefColor.unused=255;
}


/**End Trick Door Class**/



/** Door **/
blockdoor::blockdoor(square* sq, bool AsTrap, bool startopen, blockcolor bc)
{

     mode=startopen;
     trap=AsTrap;
     if (startopen) anim=0;
     else anim=3;
     c=bc;
      type=OT_BLOCKDOOR;
     GObjectConstruct(sq);

}

void blockdoor::Loop()
{

    if (IsOpen())
    {
        if (anim<3) anim++;
    }
    else if (anim>0)
    {
        obj* object;
        object= (game::Square(this->x,this->y)->object);
        if ((! object ) || InsideKind(object->GetType()) )
           anim--;
    }

}

bool blockdoor::IsOpen()
{
    if (trap)
        return (marked::AtLeastOneActive(c)==mode);
    return (marked::AllActive(c)==mode);
}

void blockdoor::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx=7,ty=anim+5;

    if(trap) tx++;


    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.SetColors(options::BKColor[c],255);
    D.Draw(game::screen,x,y);

}



inline void blockdoor::OnEnter(obj *entering)
{
if ( (! IsOpen()) && (InsideKind(entering->GetType()) ))
{
    RenderAfterObjects=true;
}

}
void blockdoor::OnLeave(obj *entering)
{RenderAfterObjects=false;}

bool blockdoor::CanEnter(obj *entering, edir dir)
{
    return (IsOpen());
}
bool blockdoor::CanLeave(obj *entering, edir dir) { return true; }

bool blockdoor::IsOfColor(blockcolor bc)
{
    return ((bc==c));
}


/**End door Class**/

/** Marked Area**/
marked::marked(square* sq,blockcolor bc)
{
     c=bc;
     count[(unsigned int)(c)]++;
     anim=0;
     active=false;
      type=OT_MARKEDAREA;
     GObjectConstruct(sq);
}

void marked::OnDeath()
{
    count[(unsigned int)(c)]--;
}

void marked::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx=6,ty=anim+5;
    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.SetColors(options::BKColor[c],255 );
    D.Draw(game::screen,x,y);
    if (active)
        anim= (anim>=3?0:anim+1);

}



void marked::OnEnter(obj *entering)
{
    if (active=RenderAfterObjects=(entering->HasBlockColor(c)))
    {
        activeN[(unsigned int)(c)]++;
    }
}
void marked::OnLeave(obj *entering)
{
    if (active)
    {
        active=RenderAfterObjects=false;
        activeN[(unsigned int)(c)]--;

    }
}
bool marked::CanEnter(obj *entering, edir dir)
{
    return true;
}
bool marked::CanLeave(obj *entering, edir dir) { return true; }

unsigned int marked::count[4];
unsigned int marked::activeN[4];

void marked::Reset()
{
    count[0]=count[1]=count[2]=count[3]=activeN[0]=activeN[1]=activeN[2]=activeN[3]=0;

}

bool marked::AllActive(blockcolor c)
{
    unsigned int indx=(unsigned int)(c);
    return ( count[indx]==activeN[indx]);
}

bool marked::AtLeastOneActive(blockcolor c)
{
    unsigned int indx=(unsigned int)(c);
    return (activeN[indx]>0);
}


/**End Marked Area Class**/

/** Fire pad**/
firepad::firepad(square* sq)
{
     anim=0;
      type=OT_FIREPAD;
     D=D_DOWN;
     GObjectConstruct(sq);
}

void firepad::OnDeath()
{}

void firepad::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    switch(anim)
    {
        case(3): tx=3; ty=6; break;
        case(2): tx=2; ty=6; break;
        case(1): tx=3; ty=5; break;
        default: tx=2; ty=5;
    }
    if (anim) anim=(anim==3)?0:anim+1;
    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.Draw(game::screen,x,y);

}



void firepad::OnEnter(obj *entering)
{
    if ((entering->HasBlockColor(B_RED) ) && (entering->GetType() !=  OT_FIREBALL)  )
    {
        anim= 1;
        bool round = entering->HasRoundCorner(RC_1);
        entering->Kill(true); //true: Don't trigger funny things like the red timer's explosion
        dangerous* dg= new dangerous(game::Square(x,y),D,true);
        dg->tic = game::Counter();
        dg->fromRound = round;
        new explosion(game::Square(x,y), 1);
    }
}
void firepad::OnLeave(obj *entering)
{
}
bool firepad::CanEnter(obj *entering, edir dir)
{
    D=dir;
    return true;
}
bool firepad::CanLeave(obj *entering, edir dir) { return true; }


/**End Fire Pad Class**/

/** Pit**/
bool pit::IsEnabledInstance(gobj* gobject)
{
    if (! gobject) return false;
    if (gobject->GetType()!=OT_PIT) return false;
    pit* p=static_cast<pit*>(gobject);
    return (! p->end);
}

pit::pit(square* sq)
{
     end=dis=false;
     alpha=255;
     dec=12;
     sur=NULL;
      type=OT_PIT;
     GObjectConstruct(sq);
}

void pit::OnDeath()
{
    if (sur) SDL_FreeSurface(sur);
    sur=NULL;
}

void pit::Loop()
{
    if(!end && !dis) {
        //verify lone big blocks...
        square* sq=game::Square(pit::x,pit::y);
        obj* object = sq->object;
        if (object!=NULL) {
            OnEnter(object);
        }
    }
    if (dis || !end) {
        return;
    }
    
    if (alpha>=31) {
        if (! dec) {
            alpha-=31;
        } else {
            dec--;
        }
    } else {
        alpha=0;
    }
}

void pit::Draw(unsigned int x, unsigned int y)
{
    if (dis) return;
    square* sq=game::Square(pit::x,pit::y);

    if (end) {
        if (alpha<=31) {
            alpha=0;
            SDL_FreeSurface(sur);
            sur=NULL;
            end=false;
            dis=true;
            Kill();

        } else {
            Drawer D(sur,0,0,sz,sz);
            D.SetColors(255,255,255,alpha);
            D.Draw(game::screen,x,y);
        }
    } else {

        Drawer D(game::sprites,4*sz,19*sz,sz,sz);
        D.SetColors(sq->R,sq->G,sq->B,255);
        D.Draw(game::screen,x,y);
    }
}

bool pit::CanConsume(obj* object)
{
    beast *b;
    rattler *r;
        switch (object->GetType())
        {
            case(OT_FIREBALL): return false;
            case(OT_BEAST):
                b= static_cast<beast*>(object);
                if (b->Floats()) return false;
                break;
            case(OT_RATTLER):
                r= static_cast<rattler*>(object);
                if (r->Floats()) return false;
                break;
            case(OT_RATTLERNODE): return false;
            case(OT_LARGEBLOCK):
            {
                largeblock* lb=static_cast<largeblock*>(object);
                return ( lb->isReallyASmallBlock() );
            }
        }
 return true;
}

void pit::OnEnter(obj *entering)
{
    if ( (! end) && (! dis) )
    {
        if(! pit::CanConsume(entering)) return;

        SDL_Surface* tm=game::screen;
        game::screen=sur= CreateFixedRGBASurface(0,sz,sz);
        square* sq=game::Square(x,y);
        SDL_FillRect(sur, 0,0,sz,sz, SDL_MapRGB(sur->format,sq->R,sq->G,sq->B ) );
        Draw(0,0);
        end= true;
        Sint16 off = std::max<Sint16>( (Sint16)(sz/15.0+0.5),1);
        entering->Draw(off,off);

        Drawer D(game::sprites,5*sz ,19*sz,sz,sz);

        D.SetColors(sq->R,sq->G,sq->B,255);
        D.Draw(sur,0,0);
        entering->Kill(true); //true: Don't trigger funny things like the red timer's explosion
        game::screen=tm;

    }
}
void pit::OnLeave(obj *entering)
{
}
bool pit::CanEnter(obj *entering, edir dir)
{
    if (end) return true;
    switch (entering->GetType())
    {
        case(OT_RATTLER): case(OT_RATTLERNODE): return true;
    }
    return true;
}
bool pit::CanLeave(obj *entering, edir dir) { return true; }


/**End Pit Class**/



/**Begin Class Hint **/
hint* hint::active;

hint::hint(square* sq, string ihint,bool warning)
{
    xcla=warning;
    type=(xcla)?OT_WARNING:OT_HINT;
    text=ihint;
    GObjectConstruct(sq);
}

void hint::Draw(unsigned int x, unsigned int y)
{
    Drawer D(game::sprites,0,0,0,0);
    if (xcla)
        D.ChangeRect(7*sz,3*sz,sz,sz);
    else
        D.ChangeRect(6*sz,3*sz,sz,sz);

    D.Draw(game::screen,x,y);

}
void hint::Draw(unsigned int x, unsigned int y,bool xcla)
{
    Drawer D(game::sprites,0,0,0,0);
    if (xcla)
        D.ChangeRect(7*sz,3*sz,sz,sz);
    else
        D.ChangeRect(6*sz,3*sz,sz,sz);

    D.Draw(game::screen,x,y);

}
void hint::OnEnter(obj *entering)
{
    if (entering->GetType()==OT_XYE)
        active=this;
}
void hint::OnLeave(obj *entering)
{
    if (entering->GetType()==OT_XYE)
        active=NULL;
}

bool hint::CanEnter(obj *entering, edir dir) { return true; }
bool hint::CanLeave(obj *entering, edir dir) { return true; }


/*void hint::ChangeColor(unsigned int nbc,unsigned int nfc)
{
       bcolor=nbc;fcolor=nfc;
}*/

string hint::globaltext;

void hint::SetHint(string hint) { text=hint; }

void hint::Reset()
{
    globaltext="";
    active=NULL;
}
bool hint::Active()
{
    return (active!=NULL);
}
const char* hint::GetActiveText()
{
    string res;
    if (active==(hint*)(1))
        res = globaltext;
    else if (active) 
        res=active->text;

    return res.c_str();
}

void hint::SetGlobalHint(const char* gl)
{
    globaltext = string(gl);
    //globaltext=gl;
}

void hint::SetGlobalHint(string &gl)
{
    globaltext=gl;
}

bool hint::GlobalHintExists()
{
    return (globaltext!="");
}

void hint::GlobalHint(bool enb)
{
    if (enb && (globaltext!="")) active=(hint*)(1);
    else active=NULL;
}

/**End Class Hint**/

/**Begin Class Portal **/
bool portal::ignore=false;

void portal::Reset()
{
    ignore=false;
}

portal::portal(square* sq,Uint8 cR, Uint8 cG, Uint8 cB, signed int TargetX, signed int TargetY)
{
    //portals are ground objects that change constantly and use custom colors, better cache the color they use.
    if ((cR!=255) || (cG!=255) || (cB!=255))
       RecolorCache::savecolor(cR,cG,cB,255);

    type=OT_PORTAL;
    target=game::SquareN(TargetX,TargetY);
    canim=GetRandomInt(0,2);
    R=cR;
    G=cG;
    B=cB;
    xyealpha=0x00000000;
    GObjectConstruct(sq);
}

void portal::Loop()
{
    if (game::Mod4())
    {
        canim++;
        if (canim>=3) canim=0;
    }
    if (xyealpha>0)
    {
        xyealpha= (xyealpha==255)?254:(xyealpha>32)?(xyealpha-32):0;
    }
}

void portal::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;


    Drawer D(game::sprites,8*sz,canim*sz,sz,sz);

    D.SetColors(R,G,B,255);
    D.Draw(game::screen,x,y);

    if (xyealpha>0)
    {
        D.ChangeRect(0,0,sz,sz);
        D.SetColors(game::PlayerColor,xyealpha);
        D.Draw(game::screen,x,y);
    }

}

bool AllowedForPortalTeleport(square* sq, obj* togo)
{
    if (sq->object!=NULL) return false;
    gobj* gobject=sq->gobject;
    return ((gobject==NULL) || (gobject->GetType()==OT_PORTAL));
}

void portal::OnEnter(obj *entering)
{
    if ((! ignore) && (entering->GetType()==OT_XYE))
    {
        char cx=target->sqx,cy=target->sqy,rx,ry;

        if (! game::FindGoodPoint(cx,cy,rx,ry, game::XYE, AllowedForPortalTeleport )) game::Error(  "level is full?!");
        ignore=true;
        game::XYE->alpha=32;
        game::FlashXyePosition();
        xyealpha=255;
        game::XYE->move(rx,ry);

        ignore=false;
    }
}
void portal::OnLeave(obj *entering) {}

bool portal::CanEnter(obj *entering, edir dir) { return true; }
bool portal::CanLeave(obj *entering, edir dir) { return true; }

void portal::ChangeColor(Uint8 cR, Uint8 cG, Uint8 cB)
{
    R=cR;
    G=cG;
    B=cB;

}



/**End Class Portal**/








/**Start class window block **/
/** Window Block **/

unsigned int windowblock::count[4];
unsigned int windowblock::activeN[4];

windowblock::windowblock(square* sq,blockcolor b)
{
    bc=b;
    type=OT_WINDOW;
    anim=0;
    count[bc]++;
    active=false;
    ObjectConstruct(sq);
}
bool windowblock::HasBlockColor(blockcolor bca) { return (bc==bca); }
bool windowblock::trypush(edir dir,obj* pusher)
{
    if (activeN[bc]==count[bc]) return trypush_common(dir,pusher,false,NULL);
    return false;
}



void windowblock::Draw(unsigned int x, unsigned int y)
{
    Uint8 tx,ty;
    ty=7;
    tx=9+anim;
    Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
    D.SetColors(options::BKColor[bc],255);
    D.Draw(game::screen,x,y);
}

bool windowblock::Detect(char dx, char dy)
{
    square* sq=game::SquareN(dx,dy);
    obj* o;
    if (o= sq->object)
        return (o->HasBlockColor(bc) && (o->GetType()!=OT_WINDOW) );
    return(false);

}
bool windowblock::CanMove(windowblock* w)
{
    blockcolor c=w->bc;
    return (count[c]==activeN[c]);
}


bool windowblock::Loop(bool* died)
{
    bool activate=(Detect(x+1,y) || Detect(x-1,y) || Detect(x,y+1) || Detect(x,y-1)) ;

    if (activate!=active)
    {
        if (activate) activeN[bc]++;
        else
        {
            activeN[bc]--;
            if (anim>3) anim=0;
        }
    }
    if ((activate!=active) || active)
        obj::UpdateSquare();

    active=activate;

    if (activeN[bc]==count[bc])
    {
        if ((anim<4) || (anim>5) || (game::Mod5()) )
        {
           anim=(anim==4)?5:4;
           obj::UpdateSquare();
        }


        obj::DoMagnetism(true,true, &activate);
        *died=false;
        return(activate);
    }
    else if (active)
        anim= anim>=3?0:anim+1;


    *died=false;
    return(false);
}

void windowblock::ResetCounts()
{
    for(int i=0;i<4;i++) activeN[i]=count[i]=0;
}

/** End class windowblock**/






/** Pseudo Class deathqueue **/

killzone* deathqueue::first;
killzone* deathqueue::current;
signed char deathqueue::incx[5]= {0,1,0,-1, 0};
signed char deathqueue::incy[5]= {0,0,1, 0,-1};

void deathqueue::reset()
{
    killzone* t=first;
    while(t)
    {
        killzone* c=t;
        t=t->next;
        delete c;
    }
    first=current=NULL;
}
void deathqueue::add(char ax,char ay, killtype t)
{

    if (first==NULL)
    {
        first=new killzone;
        first->kt=t;
        first->x=ax;
        first->y=ay;
        first->next=NULL;
        current=first;
    }
    else
    {
        killzone *tm=new killzone;
        tm->kt=t;
        tm->x=ax;
        tm->y=ay;
        tm->next=NULL;
        current->next=tm;
        current=tm;

    }
}

void KillZone_KillXye(killzone *K)
{

    xye* XYE = game::XYE;
    char dx=K->x - XYE->X();
    char dy=K->y - XYE->Y();
    dx= dx<0?-dx:dx;
    dy= dy<0?-dy:dy;
    if (( (dx==0) && (dy<=1)     ) || ((dy==0) && (dx<=1) )) {
        XYE->Kill();
    }

}

void KillZone_Fire(killzone *K)
{
    xye* XYE = game::XYE;
    char kx=K->x,ky=K->y , i;
    for (i=0;i<5;i++) {
        game::SmallBoom(game::SquareN(kx+deathqueue::incx[i],ky+deathqueue::incy[i]),true,deathqueue::incx[i],-deathqueue::incy[i]);
    }
}


void ExecuteKillZone(killzone *K)
{
    switch (K->kt)
    {
        case(KT_KILLXYE):
            KillZone_KillXye(K);
            break;
        case(KT_FIRE):
            KillZone_Fire(K);
            break;
    }
}

bool deathqueue::KillNow()
{
    if (first==NULL) return false;

    killzone *tm, *c=first;
    first=current=NULL;
    do
    {
        ExecuteKillZone(c);
        tm=c;
        c=c->next;
        delete tm;
    } while (c!=NULL);
    return true;

}


/** deathqueue ends**/


/** Explosion !! Begins **/

void explosion::ex(square *sq, unsigned char kind)
{
    creation=game::counter;
    pos=sq;
    explosion* other= sq->ex;
    if (other!=NULL) {
        kind = std::min<unsigned char>(kind,sq->ex->type);
        delete other;
    }
    sq->ex=this;
    type= kind>2? 2: kind;
    xobjectoffset=0;
    yobjectoffset=0;

}
void explosion::setDrawingXYOffset(int xo, int yo)
{
    xobjectoffset=xo;
    yobjectoffset=yo;
}


void explosion::getDrawingXYOffset(int &xo, int &yo)
{
    int anim=game::counter-creation;
    if (anim==1)
    {
        xo=xobjectoffset;
        yo=yobjectoffset;
    }
    else
    {
        xo=0;
        yo=0;

    }

    if (type==1)
    {
        xo*=2;
        yo*=2;
    }

}

explosion::explosion(square *sq, unsigned char kind)
{ ex(sq,kind); }

explosion::explosion(square *sq)
{ ex(sq,0); }

explosion::~explosion()
{
    pos->ex=NULL;
}

void explosion::Draw(unsigned int x, unsigned int y)
{
    int anim=game::counter-creation;
    if ((anim>=4)||(anim<0))
    {
        pos->Update=true;
        delete this;
    }
    else if (type!=2)
    {
        Uint8 tx,ty;
        switch (anim)
        {
            case(0): tx=0; ty=13; break;
            case(1): tx=1; ty=13; break;
            case(2): tx=0; ty=14; break;
            default: tx=1; ty=14;
        }
        tx+=type*2;
        Drawer D(game::sprites,tx*sz,ty*sz,sz,sz);
        D.Draw(game::screen,x,y);
    }
}




/** Explosion Ends **/






/** Pseudo class square memory **/

bool sqMem::mem[XYE_HORZ][XYE_VERT];

void sqMem::reset(bool val)
{
    char j;
    for (char i=0;i<XYE_HORZ;i++) for (j=0;j<XYE_VERT;j++)
        mem[i][j]=val;

}




/** Gen functions **/

bool CanPush(otype t)
{
    switch(t)
    {
        case(OT_XYE): case(OT_PUSHER): case(OT_MAGNETIC): case(OT_ROBOXYE): return (true);
    }
    return (false);
}


bool Allowed(obj* object, edir dir, square* sq)
{

    if (sq->object==NULL)
    {
        gobj *gobject=sq->gobject;
        return ((gobject==NULL) || (gobject->CanEnter(object,dir)));

    }
    return false;
}

bool Allowed(obj* object, edir dir, square* sq, otype except)
{

    obj* sqo=sq->object;
    if ((sqo==NULL) || (sqo->GetType()==except))
    {
        gobj *gobject=sq->gobject;
        return ((gobject==NULL) || (gobject->CanEnter(object,dir)));

    }
    return false;
}



void DebugMsg(const char *msgText, void *outParam)
{
    //game::hge->System_Log(msgText,outParam);
}


edir Opposite(edir dir)
{
    switch(dir)
    {
        case(D_UP): return D_DOWN;
        case(D_DOWN): return D_UP;
        case(D_RIGHT): return D_LEFT;
    }
    return D_RIGHT;
}

// I sometimes have the feeling that it is better to use goto and label, I had to use this thing for the
// do while in the other function
bool PickDirectDir_Sub(bool &repet)
{
    if (repet)
    {
        repet=false;
        return (true);
    }
 return false;
}


bool OldBeastPickDirectDir(edir &r, edir dirs[], unsigned char dirn, char x1, char y1, char x2, char y2)
{

    r= PickDirectDir(x1,y1,x2,y2,false,true);
    for (int i=0;i<dirn;i++)
        if (dirs[i]==r)
            return true;
    if ((x1==x2) || (y1==y2)) return false;
    if ((r==D_UP) || (r==D_DOWN))
    {
        if (x2>x1)
            r=D_RIGHT;
        else
            r=D_LEFT;
    }
    else if (y2>y1)
        r=D_UP;
    else
        r=D_DOWN;
    for (int i=0;i<dirn;i++)
        if (dirs[i]==r)
            return true;

 return false;
}

bool PickDirectDir(edir &r, edir dirs[], unsigned char dirn, char x1, char y1, char x2, char y2,bool mode, bool horzpriority)
{
    if (dirn==0) return false;
    else if (dirn==1)
    {
        r=dirs[0];
        return true;
    }
    char dx=x2-x1, adx=(dx<0?-dx:dx);
    char dy=y2-y1, ady=(dy<0?-dy:dy);
    char i;

    bool horz=     (  (mode==(adx<ady)) || ( (adx==ady) && (horzpriority) )  );
    bool repet=true;
    do
    for (i=0; i<dirn; i++)
       switch(dirs[i])
    {
        case(D_UP):
            if ( ((!repet) || (! horz)) && (dy>0)) { r=D_UP; return (true); }
            break;
        case(D_DOWN):
            if ( ((!repet) || (! horz)) && (dy<0)) { r=D_DOWN; return (true); }
            break;
        case(D_LEFT):
            if ( ((!repet) || horz) && (dx<0)) { r=D_LEFT; return (true); }
            break;
        default:
            if (  ((!repet) || horz) && (dx>0)) { r=D_RIGHT; return (true); }

    }
    while (PickDirectDir_Sub(repet));

 return (false);
}

bool PickMostDirectDir(edir &r, edir dirs[], unsigned char dirn,  char x1, char y1, char x2, char y2)
{
    if (dirn==0) return false;
    if (dirn==1)
        r=dirs[0];

    char dx=x2-x1, adx=(dx<0?-dx:dx);
    char dy=y2-y1, ady=(dy<0?-dy:dy);

    edir prio[4];

    if (ady>adx) //Prio vert;
    {
        if (dy>0) //Prio up
        {
           prio[0]=D_UP;
           prio[3]=D_DOWN;
        }
        else
        {
            prio[0]=D_DOWN;
            prio[3]=D_UP;
        }
        if (dx>0) //Prio right
        {
           prio[1]=D_RIGHT;
           prio[2]=D_LEFT;
        }
        else
        {
            prio[1]=D_LEFT;
           prio[2]=D_RIGHT;
        }

    }
    else //Prio Horz;
    {
        if (dx>0) //Prio right
        {
           prio[0]=D_RIGHT;
           prio[3]=D_LEFT;
        }
        else
        {
            prio[0]=D_LEFT;
           prio[3]=D_RIGHT;
        }
        if (dy>0) //Prio up
        {
           prio[1]=D_UP;
           prio[2]=D_DOWN;
        }
        else
        {
            prio[2]=D_DOWN;
            prio[1]=D_UP;
        }

    }
    unsigned int i=0,j;
    for (i=0;i<4;i++)
        for (j=0;j<dirn;j++)
            if (prio[i]==dirs[j])
            {
                r=prio[i];
                return true;
            }


 return false;
}

edir PickDirectDir(char x1, char y1, char x2, char y2,bool mode, bool horzpriority)
{
    edir D[4]= {D_UP,D_DOWN,D_LEFT,D_RIGHT};
    edir r;
    PickDirectDir(r,D,4,x1,y1,x2,y2,mode,horzpriority);
    return (r);
}

edir Clock(edir dir)
{
    switch(dir)
    {
        case(D_UP): return D_RIGHT;
        case(D_RIGHT): return D_DOWN;
        case(D_DOWN): return D_LEFT;
    }
    return D_UP;
}

edir AClock(edir dir)
{
    switch(dir)
    {
        case(D_UP): return D_LEFT;
        case(D_LEFT): return D_DOWN;
        case(D_DOWN): return D_RIGHT;
    }
    return D_UP;
}

edir RandomDir()
{
    switch(GetRandomInt(0,3))
    {
        case(0): return D_LEFT;
        case(1): return D_DOWN;
        case(2): return D_RIGHT;
    }
    return D_UP;
}

bool IsXyeAt(square* sq)
{
    obj *object=sq->object;
    if (object!=NULL) return (object->GetType()==OT_XYE ) ;


    signed int x=sq->sqx,y=sq->sqy;
    signed int  xx=game::XYE->X(),xy=game::XYE->Y();
    x=xx-x;
    y=xy-y;
    x=(x>0)?x:-x;
    y=(y>0)?y:-y;
    return ( ((x==0) && (y<=1)) || ((y==0) && (x<=1)) );
}


bool IsXyeOrBotAt(square* sq)
{
    obj *object=sq->object;
    if (object==NULL) return false;
    switch(object->GetType())
        case(OT_XYE): case(OT_ROBOXYE): case(OT_RATTLER): case(OT_RATTLERNODE): return true;

 return (false);
}

char FixVert(char vert)
{
    return (vert>=XYE_VERT)?0:(vert<0)?XYE_VERT-1:vert;
}
char FixHorz(char horz)
{
    return (horz>=XYE_HORZ)?0:(horz<0)?XYE_HORZ-1:horz;
}
void FixCoordinates(char &horz, char &vert)
{
    vert=(vert>=XYE_VERT)?0:(vert<0)?XYE_VERT-1:vert;
    horz=(horz>=XYE_HORZ)?0:(horz<0)?XYE_HORZ-1:horz;

}



/** Object recycle queue **
struct recycle_entry
{
    obj* o;
    recycle_entry* next;
}

class recycle
{
private:
 static recycle_entry* first;
public:
 static void init();
 static void run();
 static void add(obj* o);
}

/** /Object recycle queue **/

/** class recycle **/
recycle_entry* recycle::first=NULL;
void recycle::init()
{
    first=NULL;
}

void recycle::run()
{
    recycle_entry* tm;
    while(first!=NULL)
    {
        tm=first;
        first=first->next;
        delete (tm->o);
        delete tm;
    }
}

void recycle::add(obj* o)
{
    recycle_entry* nw=new recycle_entry;
    nw->next=first;
    nw->o=o;
    first=nw;
}

/** end class recycle **/

