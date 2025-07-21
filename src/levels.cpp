/*
 Xye License (it is a PNG/ZLIB license)

Copyright (c) 2025 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/

#include "xye.h"
#include "xyedit.h"
#include "xye_script.h"
#include "levels.h"
#include "skins.h"
#include "options.h"
#include "vxsdl.h"
#include "listbox.h"
#include<string>
#include<cstring>
#include<algorithm>
#include<iostream>
using std::string;
using std::sort;

#include "browser.h"




namespace LevelBrowser
{

const char* LEVELS_FOLDER = "levels/";


void PlayLevel();
void OpenEditor();
void EditLevel();


window* thewindow;
button* PlayButton;
button* EditButton;

listbox* levellistbox;


Font* MenuFont;
Font* MenuSelectedFont;
Font* InfoFont;
Font* InfoBoldFont;


string CurrentFileName;
int    CurrentFileLevelN=1;
bool runeditor;
bool editfile;
string FileDesc;
string FileTitle;
string FileAuthor;
unsigned int FileLevelsN;

string* FoundFile=NULL;




unsigned int FileN=0;
int Active;
bool ActiveIsValid;
bool ActiveIsEditable;

void LoadActiveFileInfo();

#define SPACING_TEXT "                             "

class LevelInfo: public control
{
public:

    LevelInfo(int sx, int sy, int sw, int sh)
    {
        x=sx, y=sy, w=sw, h=sh;

    }

    void loop(){}

    void draw(SDL_Surface* target)
    {
        Uint32 back=SDL_MapRGB(target->format, options::LevelMenu_info);
        SDL_FillRect(target, x,y,w,h,    back);
        Sint16 sh = h;

        Sint16 fh=options::GetGridSize();//  game::FontRes->Height();
        int fof=0;
        int fohei=InfoFont->Height();

        if(fh<fohei)
        {
            fh=InfoFont->Height()+2;
            fof=1;
        }
        else fof=(fh-fohei)/2;


        Sint16 cy=y+h/8,cx=x+2;


        cx+=5;
        Sint16 nw = cx+InfoFont->TextWidth("        ");;

        InfoBoldFont->Write(target,cx,cy,"Title:");
        cy+=fh;
        InfoFont->Write(target,nw,cy, FileTitle);
        cy+=fh;
        if (FileAuthor[0]!='\0')
        {
            InfoBoldFont->Write(target,cx,cy,"Author:");
            cy+=fh;
            InfoFont->Write(target,nw,cy, FileAuthor);
            cy+=fh;
        }

        if(FileLevelsN>1)
        {
            char tml[6];
            snprintf(tml,5,"%d",FileLevelsN);
            InfoBoldFont->Write(target,cx,cy,"Levels:");
            cy+=fh;
            InfoFont->Write(target,nw,cy,tml);
            cy+=fh;

        }



        InfoBoldFont->Write(target,cx,cy,"Location:");
        cy+=fh;
        InfoFont->Write(target,nw,cy, FoundFile[Active]);
        //delete pt;
        cy+=fh;




        InfoBoldFont->Write(target,cx,cy,"Description:");
        cy+=fh;
        string tm=" "+FileDesc;
        InfoFont->WriteWrap(target,nw,cy,x+w - nw-5,y+h-cy, tm);

     /*
        if(ActiveIsValid) InfoFont->Write(game::screen,cx,sh-3*fh,"[Enter] - Play");
        if(ActiveIsEditable)
        {
            int temwa=InfoFont->TextWidth("[Enter] - Play");
            int temwb=InfoFont->TextWidth("[F1] - Editor");
            int temw;
            InfoFont->Write(game::screen,cx,sh-2*fh,"[F1] - Editor");
            if(temwa>temwb) temw=temwa;
            else temw=temwb;

            InfoFont->Write(game::screen,cx+temw+10,sh-2*fh,"[F2] - Edit level");
        }
        else
        {
            InfoFont->Write(game::screen,cx,sh-2*fh,"[F1] - Editor");
        }
        **/



    }

    void onMouseMove(int px,int py){}
    void onMouseOut() {}
    void onMouseDown(int px,int py) {}


    void onMouseUp(int px,int py) {}
    void onMouseRightUp(int px,int py) {}
};


bool Akyexyelevel(const char* f)
{
    return (HasExtension(f,"slc") || HasExtension(f,"xsb") || HasExtension(f,"kye") || HasExtension(f,"KYE") || HasExtension(f,"xye") || HasExtension(f,"xyr") );
    // the standard forces xye extension to be lower case.
}

unsigned int CountMatchingFiles(const char* nf)
{
    Folder F(nf);

    unsigned int c=0;
    const char* N;
    if (! F.Open())
    {
        printf ("No levels found on %s \n",nf);
        return 0;
    }
    while (N=F.NextFileMatching(Akyexyelevel)) if (strlen(N)<=20) c++;
    F.Reset();
    while (N=F.NextSubFolder())
    {
        char * tm=new char[strlen(nf)+strlen(N)+2];
        strcpy(tm,nf);
        strcat(tm,N);
        strcat(tm,"/");
        c+=CountMatchingFiles(tm);
        delete[] tm;
    }

 return c;
}

unsigned int CountMatchingFiles()
{
    string nf = options::fixpath(LEVELS_FOLDER);
    string hm = options::GetLevelsHomeFolder();
    unsigned int c;

    c = CountMatchingFiles(nf.c_str());

    if ( hm.length() != 0 ) {
       c += CountMatchingFiles(hm.c_str());
    }

    return(c);
}

void FillArrayWithFilenames(const char* nf, const char* lvp, unsigned int &c)
{
    const char* N;
    unsigned int L;
    Folder F(nf);
    if (! F.Open()) return;
    while ((c<FileN) && (N=F.NextFileMatching(Akyexyelevel)))
    {
        L=strlen(N);

        if (L<=20)
        {
            (FoundFile[c])=lvp;
            (FoundFile[c])+=N;
            c++;
        }
    }
    F.Reset();
    while ((c<FileN) && (N=F.NextSubFolder()))
    {
        char* tm1=new char[strlen(N)+strlen(nf)+2];
        char* tm2=new char[strlen(N)+strlen(lvp)+2];

        strcpy(tm1,nf);strcat(tm1,N);strcat(tm1,"/");
        strcpy(tm2,lvp);strcat(tm2,N);strcat(tm2,"/");

        FillArrayWithFilenames(tm1,tm2,c);
        delete[]tm1;
        delete[]tm2;
    }

}

struct LevelSorting
{
    string lf;

    LevelSorting(const char* levelfolder)
    {
        lf = levelfolder;
    }

    int getRank(const string &s)
    {
        int r=0;
        if( s.length() > lf.length() )
        {
            if( std::equal(lf.begin(), lf.end(), s.begin()) )
            {
                r=1;
                string x = s.substr(lf.length());
                if(x=="tutorials.xye") r=5;
                if(x=="levels.xye") r=4;
                if(x=="Phipps.xye") r=3;
                if(x=="kye.xye") r=2;

            }

        }

        return r;
    }

    bool operator()( const string &a, const string &b)
    {
       // printf("LF is %s \n",lf);
        int arank = getRank(a);
        int brank = getRank(b);
        int asla = count(a.begin(), a.end(), '/')+count(a.begin(), a.end(), '\\');
        int bsla = count(b.begin(), b.end(), '/')+count(b.begin(), b.end(), '\\');;

        if( arank==brank)
        {
            if(asla == bsla)
                return (a<b);


            return (asla<bsla);
        }
        return (arank>brank);


    }
};

bool onItemSelected(listbox* lb)
{
    Active = lb->getSelectedIndex();
    LoadActiveFileInfo();
    return ActiveIsValid;
}

void onItemDoubleClick(listbox* lb)
{
    if (ActiveIsValid) {
        PlayLevel();
    }
}

void FillArrayWithFilenames()
{

    string nf=options::fixpath(LEVELS_FOLDER);
    Folder F(nf.c_str() );

    if (! F.Open()) {
        game::Error("cannot find a levels folder");
    }
    unsigned int c=0;
    int i;
    const char* N;
    unsigned int L;
    string aux;

    while ((c<FileN) && (N=F.NextFileMatching(Akyexyelevel)))
    {
        L=strlen(N);
        if (L<=20)
        {
            i=0;
            FoundFile[c]=nf;
            FoundFile[c]+=N;
            c++;
        }
    }

    // Add levels on subfolders of levels/
    F.Reset();
    while ((c<FileN) && (N=F.NextSubFolder()))
    {
        char* tm1=new char[strlen(N) + nf.length() + 2];
        char* tm2=new char[strlen(N)+ 2];

        strcpy(tm1,nf.c_str());strcat(tm1,N);strcat(tm1,"/");
        strcpy(tm2,N);strcat(tm2,"/");

        //FillArrayWithFilenames(tm1,tm2,c);
        FillArrayWithFilenames(tm1,tm1,c);
        delete[]tm1;
        delete[]tm2;
    }

    string levelsfolder = nf;

    //Add levels on %home%/.xye/levels
    nf = options::GetLevelsHomeFolder();
    if ((nf.length()!=0) && c) {
        FillArrayWithFilenames(nf.c_str(),nf.c_str(),c);
    }


     //sort the array alphabetically
    sort(FoundFile, FoundFile+c, LevelSorting(levelsfolder.c_str()) );

    Active    =0;
    //Finally find the value of res and if someone has it, make sure Active points to it
    for (i=0;i<c;i++) {
        if (FoundFile[i]==CurrentFileName)
        {
            Active=i;
        }
    }

    //Add the files to the listbox
    for (i=0;i<c;i++) {
        levellistbox->addItem(StripPath(FoundFile[i]), FoundFile[i]);
    }

    levellistbox->selectItem(Active);

}

void LoadActiveFileInfo()
{
    string &fl=FoundFile[Active];

    ActiveIsValid= LevelPack::GetFileData(fl.c_str(),FileAuthor,FileDesc,FileTitle,FileLevelsN);

    string mylevs = options::GetMyLevelsFolder();
    ActiveIsEditable= ActiveIsValid && (fl.substr(0, mylevs.size()) == mylevs);
    string flext = "";
    flext = fl.substr(fl.length()-4);
    ActiveIsEditable= ActiveIsEditable && (flext==".xye" ||flext==".kye" ||flext==".KYE");


    PlayButton->Visible = ActiveIsValid;
    EditButton->Visible = ActiveIsEditable;


}

void EditFile()
{
    CurrentFileName = FoundFile[Active];
    string mylevs= options::GetMyLevelsFolder();
    editor::SetFile(mylevs, CurrentFileName.substr(mylevs.size() ) );
    thewindow->SetTransition(editor::StartSection);
/*        string commandline=options::ExecutablePath;
        commandline+=" --edit ";
        commandline+=CurrentFileName.substr(editor::myLevelsPath.size() );
        commandline+=" ";
        commandline+=options::Dir;


        Command::executeParallel(commandline);


        CurrentFileName="";
        thewindow->stop();*/
}

void OpenEditor()
{
    string mylevs = options::GetMyLevelsFolder();
    CurrentFileName = mylevs+"editortest.xye";
    editor::SetFile(mylevs, "editortest.xye");
    thewindow->SetTransition(editor::StartSection);
/*        string commandline=options::ExecutablePath;
        commandline+=" --edit editortest.xye ";
        commandline+=options::Dir;

        Command::executeParallel(commandline);

        CurrentFileName = "";
        thewindow->stop();*/
}

void PlayLevel()
{
    if(!ActiveIsValid) return;
    CurrentFileName = FoundFile[Active];
    CurrentFileLevelN = 1;
    game::PlayLevel(CurrentFileName.c_str(), CurrentFileLevelN);


}

void onKeyDown(SDLKey keysim, Uint16 unicode)
{
}

void attemptEditFile()
{
    if(ActiveIsEditable)
    {
        EditFile();
    }
}

void OnSkinButtonClick(const buttondata* data)
{
    thewindow->SetTransition(SkinBrowser::StartSection);
}
void OnWelcomeSkinButtonClick(bool x)
{
    thewindow->SetTransition(SkinBrowser::StartSection);
}



void OnPlayButtonClick(const buttondata* data)
{
    PlayLevel();
}
void OnEditorButtonClick(const buttondata* data)
{
    OpenEditor();
}
void OnEditButtonClick(const buttondata* data)
{
    attemptEditFile();
}


void OnQuitButtonClick(const buttondata* data)
{
    thewindow->Close();
}

void onKeyUp(SDLKey keysim, Uint16 unicode)
{
    levellistbox->onKeyUp(keysim, unicode);
    switch (keysim)
    {
        case(SDLK_F1):
             OpenEditor();
             //return false;
             break;
        case(SDLK_ESCAPE):
             thewindow->Close();
             break;

        case(SDLK_F2):
             attemptEditFile();
             break;

        case(SDLK_F3):
             OnSkinButtonClick(NULL);
             break;

        case(SDLK_LEFT):
            break;
        case(SDLK_RIGHT):
            break;
        case(SDLK_RETURN): case(SDLK_KP_ENTER): //Enter
            PlayLevel();
    }

}





void Show()
{
    runeditor=false;
    editfile=false;



    if (FoundFile!=NULL) delete[] FoundFile;
    FileN = CountMatchingFiles();
    if (! FileN) {
        game::Error("No level files found");
    }
    FoundFile= new string[FileN];
    FillArrayWithFilenames();

    LoadActiveFileInfo();


    bool loop=true;

    //
    //CurrentFileName = FoundFile[Active];
    //delete[] FoundFile;
    //FoundFile=NULL;

}

void onExitAttempt()
{
    thewindow->stop();
}

void StartSection(window* wind)
{
    thewindow = wind;
    wind->SetCaption("Xye - Select a level file");
    Sint16 lw = 2+game::FontRes->TextWidth(SPACING_TEXT);

    listbox* ll = listbox::makeNew(0,0,lw, wind->Height);
    levellistbox = ll;
    ll->NormalFont = MenuFont;
    ll->SelectedFont = MenuSelectedFont;
    ll->BackgroundColor = options::LevelMenu_menu;
    ll->SelectedColor = options::LevelMenu_selected;
    ll->InvalidColor = options::LevelMenu_selectederror;
    ll->BarColor = options::LevelMenu_info;
    ll->onSelect = onItemSelected;
    ll->onItemDoubleClick = onItemDoubleClick;
    ll->depth= 1;
    wind->addControl(ll);


    LevelInfo* li = new LevelInfo(lw, 0, wind->Width-lw, wind->Height);
    li->depth= 2;

    Sint16 w,cx; button* but;
    cx = lw;

    //== Play button
    w = button::recommendedWidth("Play");
    but = new button( cx, wind->Height - game::GRIDSIZE, w, game::GRIDSIZE);
    but->onClick = OnPlayButtonClick;
    but->text = "Play";
    but->depth = 3;
    PlayButton = but;
    wind->addControl(but);
    cx+=w+1;

    //== Run Editor button
    w = button::recommendedWidth("[F1] Editor");
    but = new button( cx, wind->Height - game::GRIDSIZE, w, game::GRIDSIZE);
    but->text = "[F1] Editor";
    but->depth = 3;
    but->onClick = OnEditorButtonClick;

    wind->addControl(but);
    cx+=w+1;

    //== Edit Level
    w = button::recommendedWidth("[F2] Edit Level");
    but = new button( cx, wind->Height - game::GRIDSIZE, w, game::GRIDSIZE);
    but->text = "[F2] Edit Level";
    but->depth = 3;
    EditButton = but;
    but->onClick = OnEditButtonClick;
    wind->addControl(but);
    cx+=w+1;

    //== Pick skin
    w = button::recommendedWidth("[F3] Theme");
    but = new button( cx, wind->Height - game::GRIDSIZE, w, game::GRIDSIZE);
    but->text = "[F3] Theme";
    but->depth = 3;
    but->onClick = OnSkinButtonClick;
    wind->addControl(but);
    cx+=w+1;



    //== Quit
    w = button::recommendedWidth("Quit");
    but = new button( wind->Width-1-w, wind->Height - game::GRIDSIZE, w, game::GRIDSIZE);
    but->text = "Quit";
    but->depth = 3;
    but->onClick = OnQuitButtonClick;
    wind->addControl(but);

    //...
    wind->addControl(li);
    wind->onKeyDown = onKeyDown;
    wind->onKeyUp = onKeyUp;
    wind->onExitAttempt = onExitAttempt;

    if (! options::HasConsciouslyChosenTheme() ) {
        dialogs::makeMessageDialog(wind, "Welcome to Xye. Let us take you to the theme selector. There you will be able to pick the look and feel for the game that best suits your taste/screen size.","Ok",OnWelcomeSkinButtonClick);

    }


    Show();


    return;
}

const char* GetLevelFile()
{
    return CurrentFileName.c_str();
}



void SetupNormalFonts(SDL_Surface* SS)
{
    MenuFont=new Font(SS, options::LevelMenu_menutext);
    MenuSelectedFont=new Font(SS, options::LevelMenu_selectedtext);
    InfoFont=new Font(SS, options::LevelMenu_infotext);
}

void SetupNormalFonts(const char* ttf, const int & size)
{
    MenuFont=new Font(ttf, size , options::LevelMenu_menutext);
    MenuSelectedFont=new Font(ttf, size, options::LevelMenu_selectedtext);
    InfoFont=new Font(ttf, size, options::LevelMenu_infotext);
}



void SetupBoldFonts(SDL_Surface* SS)
{
  InfoBoldFont=new Font(SS, options::LevelMenu_infotext);
}

void SetupBoldFonts(const char* ttf, const int & size)
{
    InfoBoldFont=new Font(ttf, size, options::LevelMenu_infotext);
}

void DeleteFonts()
{
    delete MenuFont;
    delete MenuSelectedFont;
    delete InfoFont;
    delete InfoBoldFont;
}

void Clean()
{
    DeleteFonts();
    if(FoundFile != NULL) delete[] FoundFile;
}

void AssignLevelFile( const char * path, int n)
{
    CurrentFileName = path;
    CurrentFileLevelN = n;
}


}
