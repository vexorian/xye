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
#include "gen.h"
#include<iostream>
#include<fstream>
#include<string>
#include<algorithm>
#include "xye_script.h"
#include "xsb_level.h"

/** Class XsbLevelPack start **/
class XsbLevel
{
public:
    char data[XYE_HORZ][XYE_VERT];
    char w;
    char h;
    string name;
    XsbLevel* Prev;
    XsbLevel* Next;
    XsbLevel() { Prev=Next=NULL; levelnum=1; }

    void SetGameCaption();
    void Load();
    unsigned int levelnum;
    static int gm;
    static int tx;
    static int ty;
    static blockcolor bc;
};


XsbLevel* XsbLevelPack::First=NULL;
XsbLevel* XsbLevelPack::Final=NULL;
XsbLevel* XsbLevelPack::CurrentLevel;
unsigned int XsbLevelPack::tn;

void getline_xplt2(std::ifstream &a, std::string &l)
{
    getline(a,l);
    int L=l.length();
    if ((L>1) && (l[L-1]=='\r'))
    {
        l = l.substr(0,L-1);
    }
}



/*
@ - sokoban
+ - sokoban on target
# - wall
$ - box
. - target
* - box on target
*/

bool IsValidXsbLine(std::string &s)
{
    int L=s.length(),i;
    if (! L) return false;
    char c;
    for (i=0;i<L;i++)
    {
        c=s[i];


        if ((c!='@') && (c!='+') && (c!='#') && (c!='$') && (c!='.') && (c!=' ') && (c!='*'))
          return false;
    }
    return true;
}
unsigned int RealSokoLineLength(std::string &s)
{
    unsigned int L=s.length();
    while (s[L-1]==' ') L--;
    return L;
}

bool IsSLC(const char* path)
{
    int L= strlen(path);
    return ((L>=4) && (strcmp(path+L-4,".slc") ==0 ));
}

int SLC_CountValidLevels(TiXmlElement* levels)
{
    int n=0;
    TiXmlElement* el = levels->FirstChildElement("Level");
    while (el != NULL)
    {
        int w = -1, h = -1;
        el->QueryIntAttribute("Width", &w);
        el->QueryIntAttribute("Height", &h);
        if( h > w) std::swap(w,h);
        if ( (w>=1) && (w<=XYE_HORZ) && (h<=XYE_VERT) && (h>=1) )
            n++;
        el=el->NextSiblingElement("Level");
    }
    
    
    return n;
}

string GetSokobanLevelName(const char* filename, int ln)
{
    string name = GetFileNameNoExtension(filename);
    int len = name.length();
    
    char buf[len+10];
    sprintf(buf, "%s %d", name.c_str(), ln);
    return string(buf);
}

void XsbLevelPack::LoadSLC(const char* filename, unsigned int ln)
{
    TiXmlDocument  fil(filename);
    TiXmlElement* pack, *el;
    tn=0;
    fil.SetCondenseWhiteSpace(false);
    if (fil.LoadFile())
    {
        pack=fil.FirstChildElement("SokobanLevels");
        if (pack!=NULL)
        {
            el=pack->FirstChildElement("LevelCollection");
            if(el == NULL) {LevelPack::Error("Unable to find a <LevelCollection> tag.");return;}
            
            tn = SLC_CountValidLevels(el);
            
            int temlevel =0 ;
            for ( el= el->FirstChildElement("Level"); el != NULL; el = el->NextSiblingElement("Level") )
            {
                int w = -1, h = -1;
                el->QueryIntAttribute("Width", &w);
                el->QueryIntAttribute("Height", &h);
                bool swapped = (h>XYE_VERT);
                int L= w;
                if(swapped)
                {
                    L = h;
                    std::swap(w,h);
                }
                if((w<0) || (h<0) || (w>XYE_HORZ) || (h>XYE_VERT) ) continue;
                
                XsbLevel* cur;
                if( First == NULL )
                {
                    cur = First = Final = new XsbLevel();
                    First->Next = First->Prev =  NULL;
                }
                else
                {
                    cur = new XsbLevel();
                    Final->Next = cur;
                    cur->Prev = Final;
                    Final = cur;
                }
                cur->w = w, cur->h = h;
                TiXmlElement* line;
                int linenum = 0;
                for(int i=0; i<XYE_HORZ; i++)
                    for(int j=0; j<XYE_VERT; j++)
                       cur->data[i][j]='#';
                       
                cur->name = el->Attribute("Id");
                if(cur->name=="")
                    cur->name = GetSokobanLevelName( filename, temlevel+1);

                for (line = el->FirstChildElement("L"); line != NULL; line = line->NextSiblingElement("L") )
                {
                    const char* gt =line->GetText(); 
                    string row = ( (gt!=NULL) ? gt : "");
                    
                    
                    for(int i=0; i<row.length(); i++)
                        if(swapped)
                            cur->data[linenum][i]= row[i];
                        else
                            cur->data[i][linenum]= row[i];
                    
                    linenum ++;
                }
                cur->levelnum = temlevel+1;
                temlevel++;
                
            }

        }
        else return LevelPack::Error("Unable to load Sokoban XML file");

    }
    else return LevelPack::Error("Unable to load Sokoban XML file");
    LevelPack::n = tn;
    LoadNthLevel(ln);
}


const char* XsbLevelPack::ReadDataSLC(const char* path,unsigned int &n, string&author, string &description, string&title)
{
    TiXmlDocument  fil(path);
    TiXmlElement* pack, *el;
    author = "";
    description = "Sokoban levels in SLC format";
    string email, url;
    n=0;
    if (fil.LoadFile())
    {
        pack=fil.FirstChildElement("SokobanLevels");
        if (pack!=NULL)
        {
            el= pack->FirstChildElement("Description");
            if( (el != NULL) && (el->GetText()!=NULL) ) description = el->GetText();
            el= pack->FirstChildElement("Title");
            if( (el != NULL) && (el->GetText()!=NULL) ) title = el->GetText();

            el= pack->FirstChildElement("Email");
            if( (el != NULL) && (el->GetText()!=NULL) ) email = el->GetText();
            el= pack->FirstChildElement("Url");
            if( (el != NULL) && (el->GetText()!=NULL) ) url = el->GetText();
            el=pack->FirstChildElement("LevelCollection");
            if(el == NULL) return "Unable to find a <LevelCollection> tag.";
            
            n = SLC_CountValidLevels(el);
            author = el->Attribute("Copyright");

        }
        else return "Not a sokoban XML file";
    }
    else return "Not a sokoban XML file";
    if(email!="") author+=" <"+email+">";
    if(url!="") description+="\n\n"+url;


    return NULL;
}

const char* XsbLevelPack::ReadData(const char* path,unsigned int &n, string&author, string &description, string & title)
{
    if( IsSLC(path))
    {
        return ReadDataSLC(path, n, author, description, title);
    }
    
    n=0;
    std::ifstream fl ;
    fl.open(path,std::ios::in);
    if (! fl.is_open()) return ("Unable to open file");
    if (fl.eof())
    {
        fl.close();
        return ("The file is empty");
    }
    std::string line;
    unsigned int L;

    unsigned char cw,ch,aux;
    while (! fl.eof())
    {
        do
        {
            getline_xplt2 (fl,line);
        }
        while (! IsValidXsbLine(line)  && ! fl.eof());
        if (fl.eof()) break;
        cw=0;
        ch=0;
        while (IsValidXsbLine(line) && ! fl.eof())
        {
            L=RealSokoLineLength(line);
            ch++;
            cw=(L>cw)?L:cw;
            getline_xplt2 (fl,line);
        }
        if (cw<ch)
        {
            aux=ch;
            ch=cw;
            cw=ch;
        }
        if ((cw<=XYE_HORZ) && (cw>=1) && (ch>=1) && (ch<=XYE_VERT))
            n++;
    }
    fl.close();
    if (!n)
        return "Could not find compatible xsb levels";

    author = "";
    description = "This file contains Sokoban levels in standard (text) format.";
return NULL;

}
void XsbLevelPack::Init()
{
    First=Final=NULL;
}

void XsbLevelPack::Clean()
{
    XsbLevel* q;
    while (First)
    {
        q=First;
        First=First->Next;
        delete q;
    }
    First=Final=NULL;
}



void XsbLevelPack::Load(const char* filename, unsigned int ln)
{
    Clean();
    if( IsSLC(filename))
    {
        return LoadSLC(filename, ln);
    }
    
    std::string line;
    std::ifstream fl ;
    fl.open(filename,std::ios::in);
    if (! fl.is_open()) return LevelPack::Error("Unable to load level file (.Xsb) (stream error)");
    if (fl.eof()) return LevelPack::Error("Level File is empty");
    std::string buf;
    unsigned char ch,cw,i,j;
    unsigned int k,L;
    bool foundsokoban=false;
    char c;
    tn=0;
    XsbLevel* current;
    while (!fl.eof())
    {
        //Non-necessary things:
        do
        {
            getline_xplt2 (fl,line);
        } while ( (! IsValidXsbLine(line)) &&(! fl.eof())   );
        if (fl.eof()) break;
        buf="";
        ch=0;
        cw=0;
        while (IsValidXsbLine(line))
        {
            ch++;
            L=RealSokoLineLength(line);
            cw=(L<cw)?cw:L;
            line.erase(L);
            buf+=line;
            buf+=';'; //separator
            if (fl.eof())
            {
                line="";
                break;
            }
            getline_xplt2 (fl,line);

        }
        if (((cw>XYE_HORZ) && (ch>cw)) || ((ch>XYE_VERT) && (cw>ch)) || (ch>XYE_HORZ) || (cw>XYE_VERT) || (ch==0) || (cw==0))
            continue; //doh

        current=new XsbLevel();
        tn++;
        for (i=0;i<30;i++) for (j=0;j<20;j++) current->data[i][j]=' ';
        i=0;j=0;
        L=buf.length();
        foundsokoban=false;
        for (k=0;k<L;k++)
        {
           c=buf[k];
           if (c==';')
           {
               j++;
               i=0;
           }
           else
           {
               foundsokoban=(foundsokoban || (c=='@')  || (c=='+'));
                if (cw>ch)
                    current->data[i][j]=c;
                else
                    current->data[j][i]=c;
                i++;
           }
        }
        if (!foundsokoban) //not a valid level, skip.
        {
            tn--;
            delete current;
            continue;
        }

        if (cw>ch)
        {
            current->w=cw;
            current->h=ch;
        }
        else
        {
            current->w=ch;
            current->h=cw;
        }


        current->name= GetSokobanLevelName(filename,tn);

        current->levelnum=tn;

        //our current level is done, add it
        if (First==NULL)
            First=Final=current;
        else
        {
            current->Prev=Final;
            Final->Next=current;
            Final=current;
        }


        //now the line contains a non-valid line
    }
if (! First) return LevelPack::Error("No compatible levels found (XSB file)");
LevelPack::n=tn;
    fl.close();


    //The result is supposed to be a double linked list with all the levels in sequence or a fiasco.
    //There is always a chance a normal text document could be loaded.

    //It should now be *easy* to load a Xsb level
    LoadNthLevel(ln);


}



void XsbLevelPack::LoadNthLevel(unsigned int n)
{
    LevelPack::OpenFileLn=n;
    //This is just double linked list iteration.
    if (n==1)
        (CurrentLevel=First)->Load();
    else
    {
        XsbLevel* tm=First;
        while (n>1)
            if (tm=tm->Next)
               n--;
            else
                n=0;
        if (! tm)
        {
            (CurrentLevel=First)->Load();
            LevelPack::OpenFileLn=1;
        }
        else (CurrentLevel=tm)->Load();

    }
}

void XsbLevelPack::Restart()
{
    CurrentLevel->Load();
}

void XsbLevelPack::Next()
{
    LevelPack::OpenFileLn++;
    XsbLevel* k=CurrentLevel->Next;
    if (! k)
    {
        (CurrentLevel=First)->Load();
        LevelPack::OpenFileLn=1;
    }
    else (CurrentLevel=k)->Load();
}

void XsbLevelPack::Last()
{
    LevelPack::OpenFileLn--;
    XsbLevel* k=CurrentLevel->Prev;
    if (! k)
    {
        (CurrentLevel=Final)->Load();
        LevelPack::OpenFileLn=tn;
    }
    else (CurrentLevel=k)->Load();
}

bool XsbLevelPack::HasNext()
{
    return (CurrentLevel->Next);

}

bool XsbLevelPack::HasLast()
{
    return (CurrentLevel->Prev);
}


/** Class XsbLevelPack end **/

void LoadXsbWall(unsigned  char i, unsigned char j,bool dark=false)
{
    wall* wl=new wall(game::Square(i,j));
    if (dark)
    wl->ChangeColor(0,0,0);

}

void LoadXsbMarked(unsigned char x,unsigned char y,blockcolor bc)
{
    marked* g=new marked(game::Square(x,y),bc);
}

void LoadXsbBlock(unsigned  char x, unsigned char y,blockcolor bc)
{
    block* b=new block(game::Square(x,y),bc,false);
}


bool MarkedPresentAt(unsigned char x, unsigned char y)
{

    gobj* gobject=game::Square(x,y)->gobject;

    if (!gobject) return false;
    return (gobject->GetType()==OT_MARKEDAREA);

}

bool BlockedEntrance(unsigned char x, unsigned char y)
{

    if (game::Square(x,y)->gobject) return true;
    obj* object;
    if (object=game::Square(x,y)->object ) return (object->GetType()==OT_WALL);
return false;

}


bool WhiteSpace(unsigned char x,unsigned char y)
{
    square* sq=game::Square(x,y);
    if (sq->gobject) return false;
    obj* object;
    if (object=sq->object)
    {
        return object->GetType()!=OT_WALL;
    }
 return true;
}



bool EnsurePath(unsigned char x,unsigned char y,int*mem,bool nowall,blockcolor bc,bool &bywall)
{
    bywall=false;

    if ((x>=XYE_HORZ) || (y>=XYE_VERT)) return false;

    //explosion* ex=new explosion(game::Square(x,y));
    int tx=XsbLevel::tx;
    int ty=XsbLevel::ty;
    if ((x==tx) && (y==ty))
        return 1;
    int memv=mem[y*XYE_HORZ+x];
    if (memv<2)
        return memv;
    if (memv>1000) //for safety, this should never happen though
    {
        return false;
    }

    mem[y*XYE_HORZ+x]=false;
    square* sq=game::Square(x,y);
    if (MarkedPresentAt(x,y)) return (false);
    obj* object;
    bool wallrep=false;
    if (object=sq->object)
    {
        if (object->GetType()==OT_WALL)
        {

            if (nowall)
            {
                mem[y*XYE_HORZ+x]=memv+1;
                bywall=true;
                return false;
            }
            wallrep=true;
            object->Kill();
            blockdoor* g=new blockdoor(sq,false,true,bc);

        }
    }

    /*if (EnsurePath(x+1,y,mem,true,bc)) return (mem[y*XYE_HORZ+x]=1);
    if (EnsurePath(x,y+1,mem,true,bc)) return (mem[y*XYE_HORZ+x]=1);
    if  (EnsurePath(x-1,y,mem,true,bc)) return (mem[y*XYE_HORZ+x]=1);
    if  (EnsurePath(x,y-1,mem,true,bc)) return (mem[y*XYE_HORZ+x]=1);


    if (EnsurePath(x+1,y,mem,false,bc)) return (mem[y*XYE_HORZ+x]=1);
    if (EnsurePath(x,y+1,mem,false,bc)) return (mem[y*XYE_HORZ+x]=1);
    if  (EnsurePath(x-1,y,mem,false,bc)) return (mem[y*XYE_HORZ+x]=1);
    if  (EnsurePath(x,y-1,mem,false,bc)) return (mem[y*XYE_HORZ+x]=1);*/


    int tx2,ty2,tx3,ty3,tx4,ty4,dx,dy;

    dx=tx-x;
    dy=ty-y;
    dx=(dx<0)?-dx:dx;
    dy=(dy<0)?-dy:dy;
    if (dx<dy)
    {
        tx2=(tx>x)?x+1:x-1;
        ty2=y;

        tx3=(tx>x)?x-1:x+1;
        ty3=y;

        tx4=x;
        ty4=(ty>y)?y-1:y+1;


        tx=x;
        ty=(ty>y)?y+1:y-1;
    }
    else
    {
        tx2=x;
        ty2=(ty>y)?y+1:y-1;

        tx3=x;
        ty3=(ty>y)?y-1:y+1;

        ty4=y;
        tx4=(tx>x)?x-1:x+1;

        ty=y;
        tx=(tx>x)?x+1:x-1;
    }

    bool wallcheck;
    if (EnsurePath(tx,ty,mem,true,bc,bywall)) return (mem[y*XYE_HORZ+x]=1);
    wallcheck=bywall;
    if (EnsurePath(tx2,ty2,mem,true,bc,bywall)) return (mem[y*XYE_HORZ+x]=1);
    wallcheck=wallcheck || bywall;

    if (EnsurePath(tx3,ty3,mem,true,bc,bywall)) return (mem[y*XYE_HORZ+x]=1);
    wallcheck=wallcheck || bywall;

    if (EnsurePath(tx4,ty4,mem,true,bc,bywall)) return (mem[y*XYE_HORZ+x]=1);
    wallcheck=wallcheck || bywall;

    bywall=wallcheck;
    if (!nowall)
    {
        if  (EnsurePath(tx,ty,mem,false,bc,bywall)) return (mem[y*XYE_HORZ+x]=1);
        if  (EnsurePath(tx2,ty2,mem,false,bc,bywall)) return (mem[y*XYE_HORZ+x]=1);
        if  (EnsurePath(tx3,ty3,mem,false,bc,bywall)) return (mem[y*XYE_HORZ+x]=1);
        if  (EnsurePath(tx4,ty4,mem,false,bc,bywall)) return (mem[y*XYE_HORZ+x]=1);
    }
    else if (wallcheck)
        mem[y*XYE_HORZ+x]=memv+1;
    //else
    //    printf("??");

    /*if  (EnsurePath(tx4,ty4,mem,false,bc)) return (mem[y*XYE_HORZ+x]=1);*/


    if (wallrep)
    {
        wall* wl=new wall(game::Square(x,y),0);
    }

return false;

}

bool FromXyeDFS(int* mem, unsigned char x, unsigned char y)
{
    static const unsigned char dx[4] = {0,0,-1,1}, dy[4] = {-1,1,0,0};
    int &res =  mem[y*XYE_HORZ+x];
    if( res==0)
    {
        res = 1;
        for (int t=0; t<4; t++)
        {
            unsigned char nx = x+dx[t], ny=y+dy[t];
            if ( (nx<XYE_HORZ) && (ny<XYE_VERT)
                 && (( game::Square(x,y)->object == NULL) || ( game::Square(x,y)->object->GetType() == OT_BLOCK))
               )
            {
                FromXyeDFS(mem,nx,ny);
            }
        }
        
    }
    return ( (res==2) ? true: false);
}


bool FindAGoodWall(int i, int j,bool rec=true)
{
    if ((i==0) || (j==0) || (i>=XYE_HORZ) || (j>=XYE_VERT))
       return false;

    square* sq=game::SquareN(i,j);
    obj* object;
    if (object=sq->object)
    {
        if (object->GetType()==OT_WALL)
        {
            object->Kill();
            XsbLevel::tx=(i<0)?XYE_HORZ-1:(i>=XYE_HORZ)?0:i;
            XsbLevel::ty=(j<0)?XYE_VERT-1:(j>=XYE_VERT)?0:j;
            blockdoor *bd= new blockdoor(sq,false,true,XsbLevel::bc);
            gem* gm=new gem(sq,XsbLevel::bc);
            return true;
        }
    }
if (rec)
{
    if (FindAGoodWall(i,j+1,false)) return true;
    if (FindAGoodWall(i+1,j,false)) return true;
    if (FindAGoodWall(i,j-1,false)) return true;
    if (FindAGoodWall(i-1,j,false)) return true;

    if (FindAGoodWall(i,j+1)) return true;
    if (FindAGoodWall(i+1,j)) return true;
    if (FindAGoodWall(i,j-1)) return true;
    if (FindAGoodWall(i-1,j)) return true;

}
XsbLevel::tx=0;
XsbLevel::ty=0;

return false;
}




/** Class XsbLevel start **/
int XsbLevel::tx;
int XsbLevel::ty;
blockcolor XsbLevel::bc;


void XsbLevel::SetGameCaption()
{
    int L=name.length();
    string title = "Xye - "+name;
    LevelPack::CurrentLevelTitle=title.c_str();
    SDL_WM_SetCaption(title.c_str(),0);

}
void XsbLevel::Load()
{

        SetGameCaption();
        hint::SetGlobalHint("To get the gem, place a block above each of the marked areas.");
        LevelPack::SetLevelBye("");
        LevelPack::Solution="";


    //We have a complete 2D array full with characters, let's convert it to a level.
    unsigned int kx=0,ky=0,i,j;
    unsigned char ox=(unsigned char)((XYE_HORZ-w)/2);
    unsigned char oy=XYE_VERT-(unsigned char)((XYE_VERT-h)/2)-1;


    bc=(blockcolor)((levelnum-1)%4);

/*
    for (i=0;i<XYE_HORZ;i++)
    {
        for (j=oy+2;j<XYE_VERT;j++) LoadXsbWall(i,j,false);
        for (j=0;j<oy+1-h;j++) LoadXsbWall(i,j,false);
    }


    for (j=0;j<XYE_VERT;j++)
    {
        for (i=0;i<ox-1;i++) LoadXsbWall(i,j,false);
        for (i=ox+w;i<XYE_HORZ;i++) LoadXsbWall(i,j,false);
    }
*/
    for (j=0;j<h;j++) for (i=0;i<w;i++) switch(data[i][j])
    {

/*
@ - sokoban
+ - sokoban on target
# - wall
$ - box
. - target
* - box on target
*/

        case('#'): LoadXsbWall(ox+i,oy-j); break;

        case('.'):LoadXsbMarked(ox+i,oy-j,bc); break;

        case('*'):
             LoadXsbMarked(ox+i,oy-j,bc);
        case('$'): LoadXsbBlock(ox+i,oy-j,bc); break;

        case('+'):
            LoadXsbMarked(ox+i,oy-j,bc);
        case('@'):
            kx=ox+i;
            ky=oy-j;
    }

    //XsbLevel::tx=ox-1;
    //XsbLevel::ty=oy+1;

    int * mem=new int[XYE_HORZ*XYE_VERT];
    memset(mem, 0, sizeof(int)*XYE_HORZ*XYE_VERT);;
    FromXyeDFS(mem, kx, ky);
    for (j=0;j<XYE_VERT;j++)
        for (i=0;i<XYE_HORZ;i++)
            if( mem[j*XYE_HORZ+i] == 0 )
            {
                square * sq = game::Square(i,j);
                obj* object = sq->object;
                if(object==NULL)
                {
                    wall* wl = new wall(sq);
                }
            }
    delete[] mem;

    FindAGoodWall(kx,ky);

    //This is where the hard part begins, we have to make the level have gems
    mem=new int[XYE_HORZ*XYE_VERT];

    for (j=0;j<XYE_HORZ*XYE_VERT;j++)
       mem[j]=2;
    bool bywall=false;
    for (j=1;j<XYE_VERT-1;j++)
       for (i=1;i<XYE_HORZ-1;i++)
           if ( WhiteSpace(i,j) &&
               ( (MarkedPresentAt(i,j+1) && !BlockedEntrance(i,j-1) )
                ||(MarkedPresentAt(i+1,j) && !BlockedEntrance(i-1,j) )
                ||(MarkedPresentAt(i-1,j) && !BlockedEntrance(i+1,j) )
                ||(MarkedPresentAt(i,j-1) && !BlockedEntrance(i,j+1) )
                )

              )
            {
                EnsurePath(i,j,mem,false,bc,bywall);
            }

    delete [] mem;

    /// Now  change walls to "decoration" walls accordingly...
    for (j=0;j<XYE_VERT;j++)
        for (i=0;i<XYE_HORZ;i++)
        {

            square * sq = game::Square(i,j);
            obj* object = sq->object;
            
            if( (object!=NULL) && (object->GetType() == OT_WALL) )
            {
                unsigned int dx[8]={0,0,1,-1, 1,1,-1,-1};
                unsigned int dy[8]={1,-1, 0,0,1,-1,1,-1};
                bool sorrounded = true;
                for (int t=0; t<8; t++)
                    if ( (i+dx[t]< XYE_HORZ) && (j+dy[t]< XYE_VERT) )
                    {
                        obj* obj2 = game::Square(i+dx[t], j+dy[t])->object;
                        if( (obj2 == NULL) || (obj2->GetType() != OT_WALL) )
                            sorrounded = false;
                    }
                if( sorrounded)
                {
                    wall* wl = static_cast<wall*>(object);
                    wl->ChangeKind(6);
                }
            }
        }


    game::XYE= new xye(game::Square(kx,ky));

/*    ox--;
    oy++;
    int q,r;
    gem* gm=new gem(game::Square(ox,oy),B_BLUE);
    for (q=-1;q<2;q++)
    for (r=-1;r<2;r++)
        if (q || r)
        {
            square* sq2=game::SquareN(ox+q,oy+r);
            blockdoor* bd=new blockdoor(sq2,false,true,bc);

        }
*/




}

/** Class XsbLevel end **/
