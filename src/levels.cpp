/*
 Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/

//levels.cpp contains the level browser, this is my first experience at GUI development so no wonder it actually
//blows. It will eventually improve or under the license you can always contribute.
//


#include "xye.h"
#include "xyedit.h"
#include "xye_script.h"
#include "levels.h"
#include "options.h"
#include "vxsdl.h"
#include "command.h"
#include<string>
#include<cstring>
#include<algorithm>
#include<iostream>
using std::string;
using std::sort;

#include "browser.h"

#define levsfolder "levels/"
//#define levsfolder "levels/kye/charity/"


namespace LevelBrowser
{

Font* MenuFont;
Font* MenuSelectedFont;
Font* InfoFont;
Font* InfoBoldFont;


string CurrentFileName;
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


char* RemovePath(string *in)
{
    string &v=*in;
    unsigned int L=v.length();
    signed int i=L-1;

    while ((i>=0) && (v[i]!='/'))
        i--;

    // 0  1  2  3  4  5  6
    //[a][/][a][.][x][y][e]


    char* tm=new char[L-i];
    unsigned int j=0;
    i++;
    while (i<L)
        tm[j++]=v[i++];
    tm[j]='\0';
return tm;


}

void Draw()
{

    Uint32 back=SDL_MapRGB(game::screen->format, options::LevelMenu_menu);
    SDL_FillRect(game::screen, 0, back);

    Sint16 cy=0,cx=2;

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

    Uint16 sh=game::screen->h - fh;
    //int i=0;
    int i=0;
    int fxh= (int)(  (game::screen->h *0.5) / fh );
    if (Active>fxh)
    {
        i=Active-fxh+1;
    }

    



    nw=game::FontRes->TextWidth(SPACING_TEXT);
    while ((i<FileN) && (cy<sh))
    {
        char * tm=RemovePath( FoundFile+i  );
        if (Active==i)
        {

            SDL_FillRect(game::screen, cx , cy, nw  , fh,
                ActiveIsValid?
                    SDL_MapRGB(game::screen->format, options::LevelMenu_selected):
                    SDL_MapRGB(game::screen->format, options::LevelMenu_selectederror));

            MenuSelectedFont->Write(game::screen,fp,cy+fof, tm);
        }
        else
            MenuFont->Write(game::screen,fp,cy+fof, tm);
        delete[] tm;
        i++;
        cy+=fh;
    }

    cx=nw+5; cy=30;

    SDL_FillRect(game::screen, cx , 2, game::screen->w-cx  , game::screen->h-2 , SDL_MapRGB(game::screen->format,  options::LevelMenu_info  ));

    cx+=5;
    nw=cx+ (nw / 2);

    InfoBoldFont->Write(game::screen,cx,cy,"Title:");
    cy+=fh;
    InfoFont->Write(game::screen,nw,cy, FileTitle);
    cy+=fh;
    if (FileAuthor[0]!='\0')
    {
        InfoBoldFont->Write(game::screen,cx,cy,"Author:");
        cy+=fh;
        InfoFont->Write(game::screen,nw,cy, FileAuthor);
        cy+=fh;
    }

    if(FileLevelsN>1)
    {
        char tml[6];
        snprintf(tml,5,"%d",FileLevelsN);
        InfoBoldFont->Write(game::screen,cx,cy,"Levels:");
        cy+=fh;
        InfoFont->Write(game::screen,nw,cy,tml);
        cy+=fh;

    }



    InfoBoldFont->Write(game::screen,cx,cy,"Location:");
    cy+=fh;
    InfoFont->Write(game::screen,nw,cy, FoundFile[Active]);
    //delete pt;
    cy+=fh;




    InfoBoldFont->Write(game::screen,cx,cy,"Description:");
    cy+=fh;
    string tm=" "+FileDesc;
    InfoFont->WriteWrap(game::screen,nw,cy,game::screen->w - nw-5,sh-cy-4*fh, tm);


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




    SDL_Flip(game::screen);
    
    
    

}

bool Akyexyelevel(const char* f)
{
    return (HasExtension(f,"xsb") || HasExtension(f,"kye") || HasExtension(f,"KYE") || HasExtension(f,"xye") || HasExtension(f,"xyr") );
    // the standard forces xye extension to be lower case.
}

char* getHomeDir()
{
    char* f =getenv("HOME");
    string tm;
    if (f)
    {
        tm=f;
        tm+="/.xye/levels/";       
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
    char* nf=options::fixpath(levsfolder);
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
        
        if( arank==brank)
            return (a<b);
        return (arank>brank);
        
        
    }
};

void FillArrayWithFilenames()
{

    char* nf=options::fixpath(levsfolder);
    Folder F(nf);

    if (! F.Open()) game::Error("cannot find a levels folder");
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

    //Add levels on %home%/.xye/levels
    nf=getHomeDir();
    if ((strlen(nf)!=0) && c) FillArrayWithFilenames(nf,nf,c);
    delete[] nf;

     //sort the array alphabetically
    sort(FoundFile, FoundFile+c, LevelSorting(levelsfolder) );
    delete[] levelsfolder;

    //Finally find the value of res and if someone has it, make sure Active points to it
    for (i=0;i<c;i++)
        if (FoundFile[i]==CurrentFileName)
        {
            Active=i;
            return;
        }
    Active    =0;

}

void LoadActiveFileInfo()
{
    string &fl=FoundFile[Active];
    
    ActiveIsValid= LevelPack::GetFileData(fl.c_str(),FileAuthor,FileDesc,FileTitle,FileLevelsN);
    
    
    ActiveIsEditable= ActiveIsValid && (fl.substr(0,editor::myLevelsPath.size()) == editor::myLevelsPath);
    ActiveIsEditable= ActiveIsEditable && (fl.substr(fl.length()-4)==".xye");
    
    
}


bool KeyDownEvent(SDL_Event& event)
{
    switch (event.key.keysym.sym)
    {
        case(SDLK_UP):
            break;
        case(SDLK_DOWN):
            break;
        case(SDLK_LEFT):
            break;
        case(SDLK_RIGHT):
            break;
    }
 return true;
}


bool attemptEditFile()
{
    if(ActiveIsEditable)
    {
        editfile=true;
        return true;
    }
    return false;
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

bool KeyUpEvent(SDL_Event& event)
{
    char a='\0',b=a;

    if (IsCharKeyEvent(event.key.keysym.sym,a,b))
    {
        int l=Active;
        int i=Active+1;
        char * fn;
        while (i!=Active)
        {
            if (i==FileN) i=0;
            if ((fn= RemovePath(FoundFile+i) ) && (strlen(fn)>0) && ((fn[0]==a) || (fn[0]==b)) )
            {
                Active=i;
            }
            else
                i++;
            if(fn)
            {
                delete[] fn;
                fn=NULL;
            }

        }
        LoadActiveFileInfo();
        return true;
    }


    switch (event.key.keysym.sym)
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

        case(SDLK_F1):
             runeditor=true;
             return false;

        case(SDLK_F2):
             return !attemptEditFile();


        case(SDLK_LEFT):
            break;
        case(SDLK_RIGHT):
            break;
        case(SDLK_RETURN): case(SDLK_KP_ENTER): //Enter
            return(! ActiveIsValid);
    }
 return true;
}



bool MouseUpEvent(Uint16 x,Uint16 y)
{
    //TODO: Improve my GUI creating skills
   //Calculate in which list item the mouse is located:

   //First of all verify if the user actually clicked list item

   if (x>game::FontRes->TextWidth(SPACING_TEXT))
      return true;

   //Now get the initial index so we know how to calculate the stuff

    int i=0;
    Uint8 fh=game::GRIDSIZE;
    int fxh= (int)(  (game::screen->h *0.5) / fh );
    if (Active>fxh)
    {
        i=Active-fxh+1;
    }

    fxh=fh;
    int j=i;
    while (y>fxh)
    {
        j++;
        fxh+=fh;
    }
    if (j<FileN)
    {
        if (Active==j)
            return ! ActiveIsValid;
        Active=j;
        LoadActiveFileInfo();
    }

return true;





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
    SDL_Event event;
    while(loop)
    {
        while (SDL_PollEvent(&event)) switch (event.type)
        {

         case (SDL_MOUSEBUTTONUP):
            loop=MouseUpEvent(event.button.x,event.button.y);
            break;


         case (SDL_KEYDOWN): //Key IS Down!
             loop=KeyDownEvent(event);
             break;
         case (SDL_KEYUP): //Key release
              if (event.key.keysym.sym!=SDLK_ESCAPE )
              {
                 loop=KeyUpEvent(event);
                 break;
              }

         case(SDL_QUIT):
             CurrentFileName = "";
             delete[] FoundFile;
             return;

         default:
            ;
        }
        Draw();
        SDL_Delay(100);
    }
    CurrentFileName = FoundFile[Active];
    delete[] FoundFile;
    FoundFile=NULL;

}



const char* GetLevelFile()
{
    SDL_WM_SetCaption("Xye - Select a level file",0);
    //strcpy(res,"dummy.xye");
    Show();
    
    
    if(runeditor)
    {
        string commandline=options::ExecutablePath;
        commandline+=" --edit editortest.xye ";
        commandline+=options::Dir;

        Command::executeParallel(commandline);

        CurrentFileName = "";
    }
    else if(editfile)
    {
        string commandline=options::ExecutablePath;
        commandline+=" --edit ";
        commandline+=CurrentFileName.substr(editor::myLevelsPath.size() );
        commandline+=" ";
        commandline+=options::Dir;
        

        Command::executeParallel(commandline);
        
        
        CurrentFileName="";
    }

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

void AssignLevelFile( const char * path)
{
    CurrentFileName = path;
}


}
