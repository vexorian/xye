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
#include "xyedit.h"
#include "xye_script.h"
#include "levels.h"
#include "options.h"
#include "vxsdl.h"
#include<string>
#include<cstring>
#include<algorithm>
#include<iostream>
using std::string;
using std::sort;

#include "browser.h"

#define themesfolder "res/"
//#define levsfolder "levels/kye/charity/"

using options::SkinInformation;

namespace SkinBrowser
{


    


window* thewindow;
button* SetButton;

Font* MenuFont;
Font* MenuSelectedFont;
Font* InfoFont;
Font* InfoBoldFont;


bool runeditor;
bool editfile;
SkinInformation SkinData;
unsigned int FileLevelsN;

string* FoundFile=NULL;




unsigned int FileN=0;
int Active;
bool ActiveIsValid;

void LoadActiveFileInfo();

#define SPACING_TEXT "                             "


class SkinList: public control
{
public:

    SkinList(int sx, int sy, int sw, int sh)
    {
        x=sx, y=sy, w=sw, h=sh;
        
    }
    
    void loop(){}
    
    void draw(SDL_Surface* target)
    {
        Uint32 back=SDL_MapRGB(target->format, options::LevelMenu_menu);
        SDL_FillRect(target, x,y,w,h,    back);

        Sint16 cy=y,cx=x+2;

        Uint16 nw,fp=cx+ InfoFont->TextWidth(" ");
        
        Uint8 fh=options::GetGridSize();//  game::FontRes->Height();
        int fof=0;
        int fohei=InfoFont->Height();

        if(fh<fohei)
        {
            fh=InfoFont->Height()+2;
            fof=1;
        }
        else fof=(fh-fohei)/2;

        Uint16 sh=h - fh;
        //int i=0;
        int i=0;
        int fxh= (int)(  (h *0.5) / fh );
        if (Active>fxh)
        {
            i=Active-fxh+1;
        }
        nw=w-2;
        while ((i<FileN) && (cy<sh))
        {
            string tm=StripPath( FoundFile[i] );
            if (Active==i)
            {

                SDL_FillRect(target, cx , cy, nw  , fh,
                    ActiveIsValid?
                        SDL_MapRGB(target->format, options::LevelMenu_selected):
                        SDL_MapRGB(target->format, options::LevelMenu_selectederror));

                MenuSelectedFont->Write(target,fp,cy+fof, tm);
            }
            else
                MenuFont->Write(target,fp,cy+fof, tm);
            i++;
            cy+=fh;
        }

        cx=nw+5; cy=30;


    }
    
    void onMouseMove(int px,int py){}
    void onMouseOut() {}
    void onMouseDown(int px,int py) {}
    
    
    void onMouseUp(int px,int py)
    {
        //Now get the initial index so we know how to calculate the stuff
        int i=0;
        Uint8 fh=game::GRIDSIZE;
        int fxh= (int)(  (h *0.5) / fh );
        if (Active>fxh)
        {
            i=Active-fxh+1;
        }

        fxh=fh;
        int j=i;
        while (py>fxh)
        {
            j++;
            fxh+=fh;
        }
        if (j<FileN)
        {
            if (Active==j)
            {
                //PlayLevel();
                return;
            }
            Active=j;
            LoadActiveFileInfo();
        }


    }
    void onMouseRightUp(int px,int py) {}
};

class SkinInfoControl: public control
{
public:

    SkinInfoControl(int sx, int sy, int sw, int sh)
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
        if(SkinData.title != "") {
            InfoBoldFont->Write(target,cx,cy,"Title:");
            cy+=fh;
            InfoFont->Write(target,nw,cy, SkinData.title);
            cy+=fh;
        }
        if (SkinData.author != "")
        {
            InfoBoldFont->Write(target,cx,cy,"Author:");
            cy+=fh;
            InfoFont->Write(target,nw,cy, SkinData.author);
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



        if(SkinData.description != "") {
            InfoBoldFont->Write(target,cx,cy,"Description:");
            cy+=fh;
            string tm=" "+SkinData.description;
            InfoFont->WriteWrap(target,nw,cy,x+w - nw-5,y+h-cy, tm);
            cy+=fh;
        }
        
        if(SkinData.preview != NULL) {
            InfoBoldFont->Write(target,cx,cy,"Preview:");
            cy+=fh;
            SDL_BlitSurface(SkinData.preview,0,0,SkinData.pw,SkinData.ph,
                                    target,cx,cy);
            cy+=SkinData.ph;

        }



    }
    
    void onMouseMove(int px,int py){}
    void onMouseOut() {}
    void onMouseDown(int px,int py) {}
    
    
    void onMouseUp(int px,int py) {}
    void onMouseRightUp(int px,int py) {}
};


bool Axyeskin(const char* f)
{
    return HasExtension(f,"xml");
}

char* getHomeDir()
{
    char* f =getenv("HOME");
    string tm;
    if (f)
    {
        tm=f;
        tm+="/.xye/res/";       
    }
    char * fm = new char[tm.length()+1];
    strcpy(fm, tm.c_str());
    return fm;
}


unsigned int CountMatchingFiles(const char* nf)
{
    Folder F(nf);

    unsigned int c=0;
    const char* N;
    if (! F.Open())
    {
        printf ("No skins found on %s \n",nf);
        return 0;
    }
    while (N=F.NextFileMatching(Axyeskin)) if (strlen(N)<=20) c++;
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
    char* nf=options::fixpath(themesfolder);
    char *hm=getHomeDir();
    unsigned int c;

    if (strlen(hm)==0)
       c=CountMatchingFiles(nf);
    else
       c=CountMatchingFiles(nf)+CountMatchingFiles(hm);

    delete[] hm;
    delete[] nf;

return(c);
}

void FillArrayWithFilenames(const char* nf, const char* lvp, unsigned int &c)
{
    const char* N;
    unsigned int L;
    Folder F(nf);
    if (! F.Open()) return;
    while ((c<FileN) && (N=F.NextFileMatching(Axyeskin)))
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
                if(x=="tutorials.xye") r=4;
                if(x=="levels.xye") r=3;
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

void FillArrayWithFilenames()
{

    char* nf=options::fixpath(themesfolder);
    Folder F(nf);

    if (! F.Open()) game::Error("cannot find a levels folder");
    unsigned int c=0;
    int i;
    const char* N;
    unsigned int L;
    string aux;

    while ((c<FileN) && (N=F.NextFileMatching(Axyeskin)))
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
        char* tm1=new char[strlen(N)+strlen(nf)+2];
        char* tm2=new char[strlen(N)+2];

        strcpy(tm1,nf);strcat(tm1,N);strcat(tm1,"/");
        strcpy(tm2,N);strcat(tm2,"/");

        //FillArrayWithFilenames(tm1,tm2,c);
        FillArrayWithFilenames(tm1,tm1,c);
        delete[]tm1;
        delete[]tm2;
    }

    char * levelsfolder = nf;

    //Add levels on %home%/.xye/res
    nf=getHomeDir();
    if ((strlen(nf)!=0) && c) FillArrayWithFilenames(nf,nf,c);
    delete[] nf;

     //sort the array alphabetically
    sort(FoundFile, FoundFile+c, LevelSorting(levelsfolder) );
    delete[] levelsfolder;

    //Finally find the value of res and if someone has it, make sure Active points to it
    string tm = options::GetSkinFile();
    for (i=0;i<c;i++)
        if (StripPath(FoundFile[i])==tm)
        {
            Active=i;
            return;
        }
    Active    =0;

}

void LoadActiveFileInfo()
{
    string &fl=FoundFile[Active];
    SkinData.pw = 21*options::GetGridSize();
    SkinData.ph = 200;
    ActiveIsValid= options::GetSkinInformation(fl.c_str(),SkinData);
    
    
    SetButton->Visible = ActiveIsValid;
    
    
}
void onKeyDown(SDLKey keysim, Uint16 unicode)
{
}

bool IsCharKeyEvent(SDLKey& k,char & a,char &b)
{

    a='\0';
    if ((k >= SDLK_a) && (k<=SDLK_z))
    {
        a= 'a'+(k-SDLK_a);
        b = 'A'+(k-SDLK_a);
    }
    else if ((k >= SDLK_0) && (k<=SDLK_9))
        a='0'+(k-SDLK_0);
    return (a!='\0');
}


void OnSetButtonClick(const buttondata* data)
{
    if(ActiveIsValid) {
        options::ChangeSkinFile(FoundFile[Active].c_str());
        thewindow->SetTransition(LevelBrowser::StartSection);
    }
}
void OnCancelButtonClick(const buttondata* data)
{
    thewindow->SetTransition(LevelBrowser::StartSection);
}

void onKeyUp(SDLKey keysim, Uint16 unicode)
{
    char a='\0',b=a;

    if (IsCharKeyEvent(keysim,a,b))
    {
        int l=Active;
        int i=Active+1;
        string fn;
        while (i!=Active)
        {
            if (i==FileN) i=0;
            fn = StripPath(FoundFile[i]);
            if ( (fn.length()!=0) && ((fn[0]==a) || (fn[0]==b)) )
            {
                Active=i;
            }
            else
                i++;

        }
        LoadActiveFileInfo();
        return;
    }


    switch (keysim)
    {

        case(SDLK_UP):
            Active--;
            if (Active<0) Active=FileN-1;
            LoadActiveFileInfo();

            break;
        case(SDLK_DOWN):

            Active++;
            if (Active>=FileN) Active=0;
            LoadActiveFileInfo();
            break;

        case(SDLK_PAGEUP):
            Active-=10;
            if (Active<0) Active=FileN-1;
            LoadActiveFileInfo();

            break;
        case(SDLK_PAGEDOWN):

            Active+=10;
            if (Active>=FileN) Active=0;
            LoadActiveFileInfo();
            break;

        case(SDLK_ESCAPE):
        case(SDLK_BACKSPACE):
             OnCancelButtonClick(NULL);
             break;

        case(SDLK_LEFT):
            break;
        case(SDLK_RIGHT):
            break;
        case(SDLK_RETURN): case(SDLK_KP_ENTER): //Enter
            OnSetButtonClick(NULL);
    }

}


void Show()
{
    runeditor=false;
    editfile=false;
    

    
    if (FoundFile!=NULL) delete[] FoundFile;
    FileN=CountMatchingFiles();
    if (! FileN) game::Error("No level files found");
    FoundFile= new string[FileN];
    FillArrayWithFilenames();

    LoadActiveFileInfo();


    bool loop=true;
    
    //
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
    wind->SetCaption("Xye - Select a theme");
    Sint16 lw = 2+game::FontRes->TextWidth(SPACING_TEXT);
    SkinList* ll = new SkinList(0,0, lw , wind->Height);
    ll->depth= 1;
    SkinInfoControl* li = new SkinInfoControl(lw, 0, wind->Width-lw, wind->Height);
    li->depth= 2;
    
    Sint16 w,cx; button* but;
    cx = lw;

    //== Use button
    w = button::recommendedWidth("Use theme");
    but = new button( cx, wind->Height - game::GRIDSIZE, w, game::GRIDSIZE);
    but->onClick = OnSetButtonClick;
    but->text = "Use theme";
    but->depth = 3;
    SetButton = but;
    wind->addControl(but);
    cx+=w+1;

    //== Quit
    w = button::recommendedWidth("Cancel");
    but = new button( wind->Width-1-w, wind->Height - game::GRIDSIZE, w, game::GRIDSIZE);
    but->text = "Cancel";
    but->depth = 3;
    but->onClick = OnCancelButtonClick;
    wind->addControl(but);
    
    //...    
    wind->addControl(ll);
    wind->addControl(li);
    wind->onKeyUp = onKeyUp;
    wind->onKeyDown = onKeyDown;
    wind->onExitAttempt = onExitAttempt;
    
    
    Show();
    
    
    return;
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
    if(SkinData.preview != NULL) {
        SDL_FreeSurface(SkinData.preview);
    }
    DeleteFonts();
}


}
