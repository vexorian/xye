/*
 Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/

const int MAX_FILENAMES_TO_REMEMBER = 100;


#include "options.h"
#include "gen.h"


#include<iostream>
#include<fstream>
#include<map>
#include<algorithm>
#include<string>
using std::string;

//for simplicity's sake I copied it, hopefully we won't ever change this enum
enum blockcolor
{
    B_YELLOW=0,
    B_RED=1,
    B_BLUE=2,
    B_GREEN=3
};


bool options::enundo=false;
bool options::bini=false;

/*TiXmlDocument* options::cnf;
TiXmlDocument* options::skin;
TiXmlElement* options::ele;
TiXmlElement* options::skinele;
*/

string options::Dir;



string options::LevelFile;
char* options::Texture;
char* options::LuminosityTexture;
char* options::Font;
char* options::FontBold;

int options::FontSize=0;
int options::FontBoldSize=0;

unsigned char options::r,options::g,options::b;
int options::GridSize;
unsigned int options::lvnum;
string options::ExecutablePath;

SDL_Color options::OneWayDoorColor;
SDL_Color options::WallColor      [XYE_WALL_VARIATIONS];
SDL_Color options::WallSpriteColor[XYE_WALL_VARIATIONS];

SDL_Color options::BFColor[4];
SDL_Color options::BKColor[4];


SDL_Color options::LevelMenu_info;
SDL_Color options::LevelMenu_selected;
SDL_Color options::LevelMenu_selectederror;
SDL_Color options::LevelMenu_menu;
SDL_Color options::LevelMenu_menutext;
SDL_Color options::LevelMenu_selectedtext;
SDL_Color options::LevelMenu_infotext;
bool options::xyeDirectionSprites = false;


// Used for saving level file numbers and recent level files...
int MemTime = 0;
typedef std::map< std::pair<int, string>, int> memmap;
std::map< std::pair<int, string>, int> MemFileLevelNumber;
std::map< string, int> MemFileLevelTime;




bool options::Error(const char* msg)
{
    fprintf(stderr,"%s",msg);
    throw msg;
}

TiXmlElement* options::GetOptionsElement(TiXmlDocument* cnf)
{
    TiXmlElement* ele;
    if (cnf->LoadFile())
    {
        ele=cnf->FirstChildElement("options");
        return(ele);
    }
 return NULL;
}

string* options::fixpath(string& path,bool dohomecheck)
{
    char* fx=fixpath( path.c_str(),dohomecheck);
    string* s=new string(fx);
    delete [] fx;
    return s;

}
//
char* options::fixpath(const char * path,bool dohomecheck)
{
    const char* home=getenv("HOME");
    int L1= strlen(path);
    #ifndef _WIN32
    if (dohomecheck && home)
    {
        char* homeloc=new char[strlen(home)+strlen("/.xye/")+L1+1];
        strcpy(homeloc,home);
        strcat(homeloc,"/.xye/");
        strcat(homeloc,path);
        if (access(homeloc,/*R_OK*/0)==0)
        {
            printf("found %s\n",homeloc);
            return homeloc;
        }
        delete[] homeloc;
    }
    #endif
    int L2=Dir.length();

    char* r= new char[L1+L2+1];
    int i;
    for (i=0;i<L2;i++) r[i]=Dir[i];
    for (i=0;i<L1;i++) r[i+L2]=path[i];
    r[L2+L1]='\0';
    return r;

}


void options::Default()
{
    LevelFile = "#browse";
}

TiXmlDocument* options::defaultxyeconf(const char* path,TiXmlElement *&options)
{
    std::ofstream file;
    file.open (path,std::ios::trunc | std::ios::out );
    if (!file.is_open()) return NULL; //ouch just halt.

    file << "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
<!--xye config file-->\n\
<options levelfile='#browse#' skinfile='default.xml' red='0' green='255' blue='0' />";

    file.close();
    TiXmlDocument* r=new TiXmlDocument(path);
    if (options=options::GetOptionsElement(r))
        return r;
    delete r;
    return NULL;
}



TiXmlDocument* options::getxyeconf(TiXmlElement *&options  )
{
    printf("looking for valid xyeconf.xml:\n");
    const char* home=NULL;
    
    TiXmlDocument* r;
    if (home=getenv("HOME"))
    {
        string loc = string(home)+"/.xye/xyeconf.xml";
        //string loc="./xyeconf.xml";
        r= new TiXmlDocument(loc.c_str());
        if (options=options::GetOptionsElement(r))
        {
            printf("found %s\n",loc.c_str() );
            return (r);
        }
        delete r;


        r=defaultxyeconf(loc.c_str(),options);
        if (r)
        {
            printf("Generated default xyeconf.xml");
            return r;
        }


    }



    r= new TiXmlDocument("./xyeconf.xml");
    if (options=options::GetOptionsElement(r))
    {
        printf("found ./xyeconf.xml\n");
        return (r);
    }
    delete r;

    r=defaultxyeconf("./xyeconf.xml",options);
    if(r)
    {
        printf("generated default ./xyeconf.xml\n");
        return(r);
    }


 return NULL;


}


void options::Init()
{


    if (bini) return;




    enundo=false;
    r=b=0;
    g=255;
    bini=true;
    GridSize=20;
    bool b1,b2,b3,b4;
    b1=b2=b3=b4=true;
    bini=true;
    TiXmlElement* ele;
    TiXmlDocument* cnf = options::getxyeconf(ele);

    if (!ele)
    {
         Default();
         delete cnf;
         return;
    }

    const char * tm;
    int i=1;


    ele->QueryIntAttribute("levelnumber",&i);
    lvnum=i;
    i=255;

    if (tm=ele->Attribute("red")) r=atoi(tm);
    if (tm=ele->Attribute("green")) g=atoi(tm);
    if (tm=ele->Attribute("blue")) b=atoi(tm);

    if (tm=ele->Attribute("undo"))
    {
        printf("NOTE: Undo is enabled!\n");
        enundo=true;
    }

    tm=ele->Attribute("levelfile");

    LevelFile = tm;

    const char * sknfile = ele->Attribute("skinfile");
    char *skin;

    if (! sknfile)
    {
         //12345678901234567
         //"res/default.xml";
         skin = fixpath("res/default.xml",true);
         printf("No skin file selection found in xyeconf.xml, will use default.xml");
    }
    else
    {
         char* ttt = new char[strlen(sknfile)+5];
         strcpy(ttt,"res/");
         strcat(ttt,sknfile);
         skin=fixpath(ttt,true);
         delete [] ttt;
    }

    TiXmlDocument skinxml(skin);
    delete[] skin;

    if (skinxml.LoadFile())
    {
         ele=skinxml.FirstChild("xyeskin")->ToElement();
         if (ele)
         {
             LoadColors(ele);
             b4=false;
             if (tm=ele->Attribute("sprites"))
             {
                 b1=false;
                 Texture=new char[5+strlen(tm)];
                 strcpy(Texture,"res/");
                 strcat(Texture,tm);

             }
             LuminosityTexture=NULL;
             if (tm=ele->Attribute("luminosity"))
             {
                 b1=false;
                 LuminosityTexture=new char[5+strlen(tm)];
                 strcpy(LuminosityTexture,"res/");
                 strcat(LuminosityTexture,tm);

             }


             if (tm=ele->Attribute("xyedirections"))
             {
                 xyeDirectionSprites = ( (strlen(tm) >= 1) && ((tm[0]=='y') || (tm[0]=='Y') ));
             }


             if (tm=ele->Attribute("fontfile"))
             {
                 b2=false;
                 Font=new char[5+strlen(tm)];
                 strcpy(Font,"res/");
                 strcat(Font,tm);
             }

             if (tm=ele->Attribute("boldfontfile"))
             {
                 b3=false;
                 FontBold=new char[5+strlen(tm)];
                 strcpy(FontBold,"res/");
                 strcat(FontBold,tm);
             }
             if (tm=ele->Attribute("truetypesize"))
             {
                 sscanf(tm,"%d",&FontSize);
                 FontBoldSize=FontSize;
                 printf("Fontsize is %d\n",FontSize);
             }


             ele->QueryIntAttribute("size",&GridSize);


         }
     }

    //Default colors:
    if(b4)
        LoadColors(ele);


    //Set default values for skin stuff:
    if (b1)
    {
        //123456789012345
        //res/xye_big.png
        Texture=new char[16];
        strcpy(Texture,"res/xye_big.png");
    }

    if (b2)
    {
        //12345678901
        //res/fon.nmp
        Font=new char[12];
        strcpy(Font,"res/fon.bmp");
    }

    if (b3)
    {
        //123456789012345
        //res/fonbold.nmp
        FontBold=new char[16];
        strcpy(FontBold,"res/fonbold.bmp");
    }

//fix the paths of all the options that are file locations:

char* tem;
tem = fixpath(Font,true);delete[] Font;Font=tem;
tem = fixpath(FontBold,true);delete[] FontBold;FontBold=tem;
tem = fixpath(Texture,true);delete[] Texture;Texture=tem;
if( LuminosityTexture!=NULL)
{ tem = fixpath(LuminosityTexture,true);delete[] LuminosityTexture;LuminosityTexture=tem; }

if (LevelFile != "#browse#")
{
   char* tem2=new char[strlen("levels/")+LevelFile.length()+1];
   strcpy(tem2,"levels/");
   strcat(tem2,LevelFile.c_str());
   tem = fixpath(tem2,true);LevelFile=tem;
   delete[] tem2;
}


    tem = options::fixpath("./", true);
    homefolder=tem;
    if( (homefolder.length() >= 3) && (homefolder.substr( homefolder.length()-3, 3)=="/./") )
        homefolder.resize( homefolder.length() - 2);
    delete[] tem;

    LoadLevelFile();
    delete cnf;

}

bool TryColorOptions(TiXmlElement* skn, SDL_Color* c,char type,char bc)
{
    if (!skn) return false;
    TiXmlElement* tem=skn->FirstChildElement("color");
    const char* e1,*e2;
    char alt= bc-'A'+'a';
    while (tem)
    {
        e1=tem->Attribute("bc");
        if (e1 && ((*e1==bc) || (*e1==alt)) )
        {
            e2=tem->Attribute("type");
            if (e2 && (*e2==type))
            {
                string quo;
                int i=0;
                quo=tem->Attribute("red");TryS2I(quo,i);c->r=i;
                quo=tem->Attribute("green");TryS2I(quo,i);c->g=i;
                quo=tem->Attribute("blue");TryS2I(quo,i);c->b=i;


                return true;
            }
        }

        tem=tem->NextSiblingElement("color");
    }
    return false;
}

bool TryMiscColorOptions(TiXmlElement* skn)
{
    TiXmlElement* tem=skn->FirstChildElement("color");
    while (tem!=NULL)
    {
        int variation = -1;
        tem->QueryIntAttribute("variation", &variation);
        const char* e1 = tem->Attribute("type");
        SDL_Color * c=NULL;
        int n = 1;
        
        if( (e1!=NULL) && (string(e1)=="WALL") )
        {
            c=options::WallColor;
            if(variation==-1) n=XYE_WALL_VARIATIONS;
            else c+=variation;
        }
        else if( (e1!=NULL) && (string(e1)=="WALL_SPRITE") )
        {
            c=options::WallSpriteColor;
            if(variation==-1) n=XYE_WALL_VARIATIONS;
            else c+=variation;
        }
        else if ( (e1!=NULL) && (string(e1)=="ONEWAYDOOR" ) )
        {
            c=&options::OneWayDoorColor;
            n=1;
        }
        if(c!=NULL)
        {
            for (int j=0; j<n; j++)
            {
                
                string quo;
                int i=0;
                quo=tem->Attribute("red");TryS2I(quo,i);c[j].r=i;
                quo=tem->Attribute("green");TryS2I(quo,i);c[j].g=i;
                quo=tem->Attribute("blue");TryS2I(quo,i);c[j].b=i;
                c[j].unused=255;
            }
        }
        tem=tem->NextSiblingElement("color");
    }
    return false;
}
        


void TryLoadLevelMenuColor(TiXmlElement* levelmenu, const char* name, SDL_Color & c, Uint8 dR, Uint8 dG, Uint8 dB)
{
    if(levelmenu==NULL)
    {
        c.r=dR;
        c.g=dG;
        c.b=dB;
    }
    else
    {
        TiXmlElement* el=levelmenu->FirstChildElement(name);
        if(el!=NULL)
        {
            string quo;
            int i;
            quo=el->Attribute("red");TryS2I(quo,i);c.r=i;
            quo=el->Attribute("green");TryS2I(quo,i);c.g=i;
            quo=el->Attribute("blue");TryS2I(quo,i);c.b=i;
        }
        else
        {
            c.r=dR;
            c.g=dG;
            c.b=dB;
        }
    }
    c.unused=255;
}

void LoadMenuColors(TiXmlElement* levelmenu)
{
    TiXmlElement* el=levelmenu->FirstChildElement("levelmenu");
    TryLoadLevelMenuColor(el,"info"         ,options::LevelMenu_info         ,239,235,231);
    TryLoadLevelMenuColor(el,"selected"     ,options::LevelMenu_selected     ,250,211,150);
    TryLoadLevelMenuColor(el,"selectederror",options::LevelMenu_selectederror,255,  0,  0);
    TryLoadLevelMenuColor(el,"menu"         ,options::LevelMenu_menu         ,255,255,255);
    TryLoadLevelMenuColor(el,"menutext"     ,options::LevelMenu_menutext     ,  0,  0,  0);
    TryLoadLevelMenuColor(el,"selectedtext" ,options::LevelMenu_selectedtext ,  0,  0,  0);
    TryLoadLevelMenuColor(el,"infotext"     ,options::LevelMenu_infotext     ,  0,  0,  0);
}

void options::LoadColors(TiXmlElement* skn)
{
    LoadMenuColors(skn);
    int i;
    char cname[4];
    cname[B_YELLOW]='Y';
    cname[B_BLUE]='B';
    cname[B_GREEN]='G';
    cname[B_RED]='R';
    for (int i=0; i<XYE_WALL_VARIATIONS; i++)
    {
        WallSpriteColor[i].r  =WallSpriteColor[i].g =WallSpriteColor[i].b = 192;
        WallSpriteColor[i].unused = 255;
        WallColor[i].r =WallColor[i].g =WallColor[i].b = WallColor[i].unused = 255;
    }
    OneWayDoorColor.r = 255, OneWayDoorColor.g = OneWayDoorColor.b = OneWayDoorColor.unused = 0 ;
    TryMiscColorOptions(skn);
    for (i=0;i<4;i++)
    {
       
        if (! TryColorOptions(skn, options::BKColor+i, 'B', cname[i]   ))
        switch(i)
        {
            case(B_YELLOW):
                options::BKColor[i].r=255;
                options::BKColor[i].g=255;
                options::BKColor[i].b=0;
                break;

            case(B_RED):
                options::BKColor[i].r=255;
                options::BKColor[i].g=0;
                options::BKColor[i].b=0;
                break;

            case(B_BLUE):
                options::BKColor[i].r=0;
                options::BKColor[i].g=0;
                options::BKColor[i].b=255;
                break;

            default: //green
                options::BKColor[i].r=0;
                options::BKColor[i].g=170;
                options::BKColor[i].b=0;
        }
        options::BKColor[i].unused=255;
        if (! TryColorOptions(skn, options::BFColor+i, 'F', cname[i]   ))
        switch(i)
        {
            case(B_YELLOW):
                options::BFColor[i].r=255;
                options::BFColor[i].g=0;
                options::BFColor[i].b=0;
                break;


            case(B_RED):
                options::BFColor[i].r=255;
                options::BFColor[i].g=255;
                options::BFColor[i].b=0;
                break;

            case(B_BLUE):
                //
                options::BFColor[i].r=0;
                options::BFColor[i].g=255;
                options::BFColor[i].b=255;
                break;

            default: //Green
                //
                options::BFColor[i].r=255;
                options::BFColor[i].g=255;
                options::BFColor[i].b=255;
                break;

        }
        options::BFColor[i].unused=255;

    }

}

void options::Clean()
{
    if (bini)
    {
        PerformLevelFileSave();
        delete [] Texture;
        delete [] LuminosityTexture;
        delete [] Font;
        delete [] FontBold;

        bini=false;
        
        MemTime = 0;
        MemFileLevelNumber.clear();
        MemFileLevelTime.clear();

    }
    
}

string options::GetDir()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return Dir;
}


const char* options::GetLevelFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    if(LevelFile == "#browse#") return NULL;
    return (LevelFile.c_str() );
}


const char* options::GetSpriteFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (Texture);
}

const char* options::GetLuminositySpriteFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (LuminosityTexture);
}


const char* options::GetFontFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (Font);
}

const char* options::GetFontBoldFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (FontBold);
}

int options::GetFontSize()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (FontSize);
}

int options::GetFontBoldSize()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (FontBoldSize);
}

int options::GetGridSize()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return GridSize;
}

string options::homefolder;
const string& options::GetHomeFolder()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return homefolder;
}



unsigned char options::Red()
{
    return(r);
}
unsigned char options::Green()
{
    return(g);
}
unsigned char options::Blue()
{
    return(b);
}

bool options_saveignored = false;

void options::IgnoreLevelSave()
{
    options_saveignored = true;
}





void options::SaveLevelFile(const char* filename, int levelNumber)
{
    if(filename == NULL) return;
    
    MemTime++;
    string s = filename;
    if( MemFileLevelTime.count(s) == 1)
    {
        int t = MemFileLevelTime[s];
        MemFileLevelTime[filename]  = MemTime;
        MemFileLevelNumber.erase( MemFileLevelNumber.find( make_pair(t, s) ) );
        MemFileLevelNumber[ make_pair(MemTime, s) ] =levelNumber;
    }
    else 
    {
        if( MemFileLevelTime.size() >= MAX_FILENAMES_TO_REMEMBER )
        {
            //remove the one with the lowest time
            std::pair<int,string> oldest = MemFileLevelNumber.begin()->first;
            MemFileLevelTime.erase( MemFileLevelTime.find(oldest.second) );
            MemFileLevelNumber.erase(MemFileLevelNumber.begin());
        }
        MemFileLevelTime[s] = MemTime;
        MemFileLevelNumber[ make_pair(MemTime,s) ] = levelNumber;
        
    }
   
    LevelFile = filename;
    lvnum = levelNumber;
}


unsigned int options::GetLevelNumber(const char* filename)
{
    if(filename==NULL) return 1;
    string s=filename;
    if( MemFileLevelTime.count(s))
    {
        int t = MemFileLevelTime[s];
        return MemFileLevelNumber[ make_pair(t, s) ];
    }
    return 1;
}

void options::PerformLevelFileSave()
{
    if(options_saveignored) return;
    std::ofstream file;
    string path = GetHomeFolder()+"lastlevel.conf";
    file.open (path.c_str(),std::ios::trunc | std::ios::out );
    if (!file.is_open()) return ; //ouch just halt.
    
    for ( memmap::iterator q = MemFileLevelNumber.begin(); q!=MemFileLevelNumber.end(); q++)
    {
        file<< q->first.second  << std::endl<< q->second <<std::endl;
    }
    
    
    file.close();
}


void options::LoadLevelFile()
{
    MemTime = 0;
    MemFileLevelNumber.clear();
    MemFileLevelTime.clear();
    
    std::ifstream file;
    string path = GetHomeFolder()+"lastlevel.conf";
    file.open (path.c_str(), std::ios::in );
    if (!file.is_open())
    {
        LevelFile = "#browse#";
        lvnum = 0;
        return ; //ouch just halt.
    }
    std::cout<<file.eof()<<std::endl;
    string LevelFile;
    int lvnum;
    while( !file.eof()  )
    {
        getline(file,LevelFile);
        if(LevelFile=="") break;
        file>>lvnum;
        SaveLevelFile(LevelFile.c_str(), lvnum);
        getline(file,LevelFile);
    }
}
