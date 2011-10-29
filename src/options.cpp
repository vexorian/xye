/*
 Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/
#include "options.h"
#include "gen.h"
#include "xye.h"

#include<iostream>
#include<fstream>
#include<map>
#include<algorithm>
#include<string>
using std::string;

namespace options
{

struct parsedSkinFile;
void LoadColors(parsedSkinFile & ps);
void LoadLevelFile();
void LoadSkinFile(const char* file);

// Public variables:
    bool  xyeDirectionSprites;
    string Dir;
   
    SDL_Color OneWayDoorColor;
    SDL_Color ForceArrowColor;
    SDL_Color EarthColor;
    SDL_Color FloorColor;
    SDL_Color WallColor      [XYE_WALL_VARIATIONS];
    SDL_Color WallSpriteColor[XYE_WALL_VARIATIONS];
    
    SDL_Color HintColor;
    
    SDL_Color BFColor[5];
    SDL_Color BKColor[5];
    
    SDL_Color LevelMenu_info;
    SDL_Color LevelMenu_selected;
    SDL_Color LevelMenu_selectederror;
    SDL_Color LevelMenu_menu;
    SDL_Color LevelMenu_menutext;
    SDL_Color LevelMenu_selectedtext;   
    SDL_Color LevelMenu_infotext;
    
    string ExecutablePath;


const int MAX_FILENAMES_TO_REMEMBER = 100;
string currentSkinFile;



//for simplicity's sake I copied it, hopefully we won't ever change this enum
enum blockcolor
{
    B_YELLOW=0,
    B_RED=1,
    B_BLUE=2,
    B_GREEN=3
};

string homefolder;
bool haspickedtheme=false;
bool enundo=false;
bool bini=false;

/*TiXmlDocument* cnf;
TiXmlDocument* skin;
TiXmlElement* ele;
TiXmlElement* skinele;
*/





string LevelFile;
char* Texture;
string WindowIcon;
char* LuminosityTexture;
char* Font;
char* FontBold;

int FontSize=0;
int FontBoldSize=0;

unsigned char r,g,b;
int GridSize;
unsigned int lvnum;



// Used for saving level file numbers and recent level files...
int MemTime = 0;
typedef std::map< std::pair<int, string>, int> memmap;
std::map< std::pair<int, string>, int> MemFileLevelNumber;
std::map< string, int> MemFileLevelTime;


std::map< std::pair<string,int> , string > MemSavedGame;

//--------
// skin parser
struct parsedSkinFile
{
    string sprites;
    string windowicon;
    string lum;
    string font;
    string boldFont;
    string author;
    string description;
    string title;
    bool   directionSprites;
    int    ttfSize;
    int    gridSize;
    
    SDL_Color OneWayDoorColor;
    SDL_Color ForceArrowColor;
    SDL_Color EarthColor;
    SDL_Color FloorColor;
    SDL_Color WallColor      [XYE_WALL_VARIATIONS];
    SDL_Color WallSpriteColor[XYE_WALL_VARIATIONS];
    
    SDL_Color BFColor[5];
    SDL_Color BKColor[5];
    
    SDL_Color HintColor;
    
    SDL_Color LevelMenu_info;
    SDL_Color LevelMenu_selected;
    SDL_Color LevelMenu_selectederror;
    SDL_Color LevelMenu_menu;
    SDL_Color LevelMenu_menutext;
    SDL_Color LevelMenu_selectedtext;   
    SDL_Color LevelMenu_infotext;

};

bool tryParseColorOptions(TiXmlElement* skn, SDL_Color* c,char type,char bc)
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

string parseSkinMenuColors(TiXmlElement* ele, parsedSkinFile & ps) {
    TiXmlElement* el=ele->FirstChildElement("levelmenu");
    TryLoadLevelMenuColor(el,"info"         ,ps.LevelMenu_info         ,239,235,231);
    TryLoadLevelMenuColor(el,"selected"     ,ps.LevelMenu_selected     ,250,211,150);
    TryLoadLevelMenuColor(el,"selectederror",ps.LevelMenu_selectederror,255,  0,  0);
    TryLoadLevelMenuColor(el,"menu"         ,ps.LevelMenu_menu         ,255,255,255);
    TryLoadLevelMenuColor(el,"menutext"     ,ps.LevelMenu_menutext     ,  0,  0,  0);
    TryLoadLevelMenuColor(el,"selectedtext" ,ps.LevelMenu_selectedtext ,  0,  0,  0);
    TryLoadLevelMenuColor(el,"infotext"     ,ps.LevelMenu_infotext     ,  0,  0,  0);

    return "";
}

string parseMiscColorOptions(TiXmlElement* skn, parsedSkinFile & ps) {
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
            c=ps.WallColor;
            if(variation==-1) n=XYE_WALL_VARIATIONS;
            else c+=variation;
        }
        else if( (e1!=NULL) && (string(e1)=="WALL_SPRITE") )
        {
            c=ps.WallSpriteColor;
            if(variation==-1) n=XYE_WALL_VARIATIONS;
            else c+=variation;
        }
        else if ( (e1!=NULL) && (string(e1)=="ONEWAYDOOR" ) )
        {
            c=&ps.OneWayDoorColor;
            n=1;
        }
        else if ( (e1!=NULL) && (string(e1)=="FORCEARROW" ) )
        {
            c=&ps.ForceArrowColor;
            n=1;
        }
        else if ( (e1!=NULL) && (string(e1)=="EARTH" ) )
        {
            c=&ps.EarthColor;
            n=1;
        }
        else if ( (e1!=NULL) && (string(e1)=="FLOOR" ) )
        {
            c=&ps.FloorColor;
            n=1;
        }
        else if ( (e1!=NULL) && (string(e1)=="HINT" ) )
        {
            c=&ps.HintColor;
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
    return "";

}


void parseSkinInformation(TiXmlElement* ele, parsedSkinFile & ps) {
    TiXmlElement* tem=ele->FirstChildElement("author");
    if (tem!=NULL) {
        if (tem->GetText() != NULL) {
            ps.author = tem->GetText();
        }
    }
    tem=ele->FirstChildElement("description");
    if (tem!=NULL) {
        if (tem->GetText() != NULL) {
            ps.description = tem->GetText();
        }
    }
    tem=ele->FirstChildElement("title");
    if (tem!=NULL) {
        if (tem->GetText() != NULL) {
            ps.title = tem->GetText();
        }
    }

}

string parseSkinColors(TiXmlElement* ele, parsedSkinFile & ps) {
    string tm = parseSkinMenuColors(ele, ps);
    if(tm != "") {
        return tm;
    }
    int i;
    char cname[5];
    cname[B_YELLOW]='Y';
    cname[B_BLUE]='B';
    cname[B_GREEN]='G';
    cname[B_RED]='R';
    cname[B_PURPLE]='P';

    //defaults:
    for (int i=0; i<XYE_WALL_VARIATIONS; i++)
    {
        ps.WallSpriteColor[i].r = ps.WallSpriteColor[i].g = ps.WallSpriteColor[i].b = 192;
        ps.WallSpriteColor[i].unused = 255;
        ps.WallColor[i].r = ps.WallColor[i].g = ps.WallColor[i].b = ps.WallColor[i].unused = 255;
    }
    ps.OneWayDoorColor.r = 255, ps.OneWayDoorColor.g = ps.OneWayDoorColor.b = ps.OneWayDoorColor.unused = 0 ;
    ps.ForceArrowColor.r = 100, ps.ForceArrowColor.g = 200, ps.ForceArrowColor.b = 200, ps.ForceArrowColor.unused = 0 ;
    ps.EarthColor.r = ps.EarthColor.g = 255, ps.EarthColor.b = ps.EarthColor.unused = 0 ;
    ps.FloorColor.r = ps.FloorColor.g = ps.FloorColor.b = 255, ps.FloorColor.unused = 0 ;
    ps.HintColor.r = ps.HintColor.g = 255, ps.HintColor.b = 200, ps.HintColor.unused = 0; 
    tm = parseMiscColorOptions(ele,ps);
    if(tm != "") {
        return "";
    }
    for (i=0;i<5;i++)
    {
        if (! tryParseColorOptions(ele, ps.BKColor+i, 'B', cname[i] ) )
        switch(i)
        {
            case(B_YELLOW):
                ps.BKColor[i].r=255;
                ps.BKColor[i].g=255;
                ps.BKColor[i].b=0;
                break;

            case(B_RED):
                ps.BKColor[i].r=255;
                ps.BKColor[i].g=0;
                ps.BKColor[i].b=0;
                break;

            case(B_BLUE):
                ps.BKColor[i].r=0;
                ps.BKColor[i].g=0;
                ps.BKColor[i].b=255;
                break;
            case(B_PURPLE):
                ps.BKColor[i].r = 128;
                ps.BKColor[i].g = 0;
                ps.BKColor[i].b = 128;
                break;

            default: //green
                ps.BKColor[i].r=0;
                ps.BKColor[i].g=170;
                ps.BKColor[i].b=0;
        }
        ps.BKColor[i].unused=255;
        if (! tryParseColorOptions(ele, ps.BFColor+i, 'F', cname[i]   ))
        switch(i)
        {
            case(B_YELLOW):
                ps.BFColor[i].r=255;
                ps.BFColor[i].g=0;
                ps.BFColor[i].b=0;
                break;


            case(B_RED):
                ps.BFColor[i].r=255;
                ps.BFColor[i].g=255;
                ps.BFColor[i].b=0;
                break;

            case(B_BLUE):
                //
                ps.BFColor[i].r=0;
                ps.BFColor[i].g=255;
                ps.BFColor[i].b=255;
                break;

            case(B_PURPLE):
                ps.BFColor[i].r = 255;
                ps.BFColor[i].g = 255;
                ps.BFColor[i].b = 255;
                break;
                
            default: //Green
                //
                ps.BFColor[i].r=255;
                ps.BFColor[i].g=255;
                ps.BFColor[i].b=255;
                break;

        }
        ps.BFColor[i].unused=255;

    }

    return "";
}

string parseSkinFile(const char*filename, parsedSkinFile & ps)
{
    bool correct = false;
    TiXmlDocument skinxml(filename);
    if ( ! skinxml.LoadFile()) {
        return "Not a valid XML file";
    }
    TiXmlElement* ele=skinxml.FirstChild("xyeskin")->ToElement();
    if ( ele == NULL ) {
        return "Not a valid Xye skin file.";
    }
    string err = parseSkinColors(ele, ps);
    if ( err != "") {
        return err;
    }
    
    parseSkinInformation(ele,ps);
    
    const char* tm;
    if (tm=ele->Attribute("sprites")) {
        ps.sprites = fixpath(string("res/")+string(tm),true);
        if  ( ! DoesFileExist(ps.sprites) ) {
            return "Missing sprites file: "+ps.sprites;
        }
    } else {
        return "No sprites file specified in skin XML.";
    }
    if (tm=ele->Attribute("icon")) {
        ps.windowicon = fixpath(string("res/")+string(tm),true);
        if  ( ! DoesFileExist(ps.windowicon) ) {
            cout<< "Missing window icon file: "+ps.windowicon<<endl;
            ps.windowicon = "";
        }
    } else {
        cout<< "WRN: No window icon file specified in skin XML."<<endl;
        ps.windowicon = "";
    }

    if (tm=ele->Attribute("luminosity")) {
        ps.lum = fixpath(string("res/")+string(tm),true);
        if  ( ! DoesFileExist(ps.lum) ) {
            return "Missing sprites luminosity file: "+ps.lum;
        }
    }
    ps.directionSprites = false;
    if (tm=ele->Attribute("xyedirections"))
    {
        ps.directionSprites = ( (strlen(tm) >= 1) && ((tm[0]=='y') || (tm[0]=='Y') ));
    }
    if (tm=ele->Attribute("fontfile")) {
        ps.font = fixpath(string("res/")+string(tm),true);
        if (! DoesFileExist(ps.font) ) {
            return "Missing font file: "+ps.font;
        }
    } else {
        return "No font file specified in skin XML.";
    }
    if (tm=ele->Attribute("boldfontfile")) {
        ps.boldFont = fixpath(string("res/")+string(tm),true);
        if (! DoesFileExist(ps.boldFont.c_str()) ) {
            return "Missing bold font file: "+ps.boldFont;
        }
    } else {
        ps.boldFont = ps.font;
    }
    ps.ttfSize = 0;
    if (tm=ele->Attribute("truetypesize")) {
        sscanf(tm,"%d",&ps.ttfSize);
    }
    if (tm=ele->Attribute("size")) {
        if(sscanf(tm,"%d",&ps.gridSize)!=1) {
            return "Invalid grid size.";
        }
    } else {
        return "Unspecified grid size.";
    }
    return "";
}

//---




bool Error(const char* msg)
{
    fprintf(stderr,"%s",msg);
    throw msg;
}

TiXmlElement* GetOptionsElement(TiXmlDocument* cnf)
{
    TiXmlElement* ele;
    if (cnf->LoadFile())
    {
        ele=cnf->FirstChildElement("options");
        return(ele);
    }
 return NULL;
}

string fixpath(const string path, bool dohomecheck)
{
    char* fx=fixpath( path.c_str(),dohomecheck);
    string s=fx;
    delete [] fx;
    return s;
}

//
char* fixpath(const char * path,bool dohomecheck)
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
        if (DoesFileExist(homeloc))
        {
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


void Default()
{
    LevelFile = "#browse";
}

TiXmlDocument* defaultxyeconf(const char* path,TiXmlElement *&options)
{
    std::ofstream file;
    file.open (path,std::ios::trunc | std::ios::out );
    if (!file.is_open()) return NULL; //ouch just halt.

    file << "<?xml version='1.0' encoding='ISO-8859-1'?>\n"
"<!--xye config file-->\n"
"<options levelfile='#browse#' skinfile='default.xml' red='102' green='255' blue='102' />";

    file.close();
    TiXmlDocument* r=new TiXmlDocument(path);
    if (options=GetOptionsElement(r))
        return r;
    delete r;
    return NULL;
}



TiXmlDocument* getxyeconf(TiXmlElement *&options  )
{
    printf("looking for valid xyeconf.xml:\n");
    const char* home=NULL;
    
    TiXmlDocument* r;
    if (home=getenv("HOME"))
    {
        string loc = string(home)+"/.xye/xyeconf.xml";
        //string loc="./xyeconf.xml";
        r= new TiXmlDocument(loc.c_str());
        if (options=GetOptionsElement(r))
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
    if (options=GetOptionsElement(r))
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


void Init()
{


    if (bini) return;


    haspickedtheme = false;
    enundo=true;
    r=b=102;
    g=255;
    bini=true;
    GridSize=20;
    bini=true;
    TiXmlElement* ele;
    TiXmlDocument* cnf = getxyeconf(ele);

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
        if( tm[0] == 'N' || tm[0] == 'n' ) {
            printf("NOTE: Undo is disabled!\n");
            enundo=false;
            
        }
    }

    if (tm=ele->Attribute("pickedtheme"))
    {
        if( tm[0] == 'Y' || tm[0] == 'y' ) {
            haspickedtheme=true;
            
        }
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
         printf("No skin file selection found in xyeconf.xml, will use default.xml.\n");
    }
    else
    {
         char* ttt = new char[strlen(sknfile)+5];
         strcpy(ttt,"res/");
         strcat(ttt,sknfile);
         skin=fixpath(ttt,true);
         delete [] ttt;
    }

    LoadSkinFile(skin);
    delete[] skin;


//fix the paths of all the options that are file locations:

char* tem;

if (LevelFile != "#browse#")
{
   char* tem2=new char[strlen("levels/")+LevelFile.length()+1];
   strcpy(tem2,"levels/");
   strcat(tem2,LevelFile.c_str());
   tem = fixpath(tem2,true);LevelFile=tem;
   delete[] tem2;
}


    tem = fixpath("./", true);
    homefolder=tem;
    if( (homefolder.length() >= 3) && (homefolder.substr( homefolder.length()-3, 3)=="/./") )
        homefolder.resize( homefolder.length() - 2);
    delete[] tem;

    LoadLevelFile();
    delete cnf;

}

void LoadMenuColors(parsedSkinFile & ps)
{
    LevelMenu_info          = ps.LevelMenu_info;
    LevelMenu_selected      = ps.LevelMenu_selected;
    LevelMenu_selectederror = ps.LevelMenu_selectederror;
    LevelMenu_menu          = ps.LevelMenu_menu;
    LevelMenu_menutext      = ps.LevelMenu_menutext;
    LevelMenu_selectedtext  = ps.LevelMenu_selectedtext;
    LevelMenu_infotext      = ps.LevelMenu_infotext;
}

void LoadColors(parsedSkinFile & ps)
{
    LoadMenuColors(ps);
    for (int i=0; i<5; i++) {
        BFColor[i] = ps.BFColor[i];
        BKColor[i] = ps.BKColor[i];
    }
    for (int i=0; i<XYE_WALL_VARIATIONS; i++) {
        WallSpriteColor[i] = ps.WallSpriteColor[i];
        WallColor[i] = ps.WallColor[i];
    }
    OneWayDoorColor = ps.OneWayDoorColor;
    ForceArrowColor = ps.ForceArrowColor;
    EarthColor = ps.EarthColor;
    FloorColor = ps.FloorColor;
    HintColor = ps.HintColor;
}


void PerformLevelFileSave();
void Clean()
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
        MemSavedGame.clear();

    }
    
}

string GetDir()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return Dir;
}


const char* GetLevelFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    if(LevelFile == "#browse#") return NULL;
    return (LevelFile.c_str() );
}


const char* GetSpriteFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (Texture);
}

const string GetWindowIconFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (WindowIcon);
}


const char* GetLuminositySpriteFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (LuminosityTexture);
}


const char* GetFontFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (Font);
}

const char* GetFontBoldFile()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (FontBold);
}

int GetFontSize()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (FontSize);
}

int GetFontBoldSize()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return (FontBoldSize);
}

int GetGridSize()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return GridSize;
}

const string& GetHomeFolder()
{
    ! bini? Error("Attempt to call unitialized options"):0;
    return homefolder;
}



unsigned char Red()
{
    return(r);
}
unsigned char Green()
{
    return(g);
}
unsigned char Blue()
{
    return(b);
}

bool options_saveignored = false;

void IgnoreLevelSave()
{
    options_saveignored = true;
}





void SaveLevelFile(string filename, int levelNumber)
{
    if(filename == "") return;
    
    MemTime++;
    string &s = filename;
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

void SaveLevelGame(string filename, int levelNumber, string moves)
{
    if ( moves == string(moves.length(), '0') ) {
        ForgetLevelGame(filename,levelNumber);
    } else {
        pair<string,int> p = make_pair(filename, levelNumber);
        MemSavedGame[p] = moves;
    }
}

string LoadLevelGame(string filename, int levelNumber)
{
    pair<string,int> p = make_pair(filename, levelNumber);
    map<pair<string,int>,string>::iterator q = MemSavedGame.find(p);
    if ( q == MemSavedGame.end() ) {
        return "";
    } else {
        return q->second;
    }
}
void ForgetLevelGame(string filename, int levelNumber)
{
    pair<string,int> p = make_pair(filename, levelNumber);
    map<pair<string,int>,string>::iterator q = MemSavedGame.find(p);
    if ( q != MemSavedGame.end() ) {
        MemSavedGame.erase(q);
    }
    
}


unsigned int GetLevelNumber(const char* filename)
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

void PerformLevelFileSave()
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

    path = GetHomeFolder()+"savedgames.conf";
    file.open (path.c_str(),std::ios::trunc | std::ios::out );
    if (!file.is_open()) return ; //ouch just halt.
    
    for ( map< pair<string,int>, string >::iterator q = MemSavedGame.begin(); q!=MemSavedGame.end(); q++)
    {
        file<< q->first.first<< std::endl << q->first.second << " "<<q->second <<std::endl;
    }
    
    
    file.close();


}



string GetSkinFile()
{
    return StripPath(currentSkinFile);
}
void SaveConfigFile()
{
    std::ofstream file;
    string path = GetHomeFolder()+"xyeconf.xml";
    file.open (path.c_str(),std::ios::trunc | std::ios::out );
    if (!file.is_open()) return ; //ouch just halt.
    file<<"<?xml version='1.0' encoding='ISO-8859-1'?>"<<endl;
    file<<"<!--xye config file-->"<<endl;
    file<<"<options levelfile='#browse#' ";
    file<<"skinfile='"<<GetSkinFile()<<"' ";
    file<<"red='"<<(int)Red()<<"' green='"<<(int)Green()<<"' blue='"<<(int)Blue()<<"' ";
    if(! UndoEnabled()) {
        file<<"undo='NO' ";
    }
    if ( HasConsciouslyChosenTheme() ) {
        file<<"pickedtheme='YES' ";
    }
    
    file<<"/>"<<endl;

    file.close();

}


void LoadLevelFile()
{
    MemTime = 0;
    MemFileLevelNumber.clear();
    MemFileLevelTime.clear();

   
    std::ifstream file;

    // ............. Load saved games
    string path = GetHomeFolder()+"savedgames.conf";
    file.open (path.c_str(), std::ios::in );
    if ( file.is_open())
    {
        string LevelFile;
        int lvnum;
        string savedgame, empty;
        
        while( !file.eof()  )
        {
            getline(file,LevelFile);
            if(LevelFile=="") break;
            file>>lvnum;
            file>>savedgame;
            
            SaveLevelGame(LevelFile.c_str(), lvnum, savedgame);
            getline(file,LevelFile);
        }
    }
    file.close();


    path = GetHomeFolder()+"lastlevel.conf";
    file.open (path.c_str(), std::ios::in );
    if (!file.is_open())
    {
        printf("Could not open lastlevel.conf\n");
        LevelFile = "#browse#";
        lvnum = 0;
        return ; //ouch just halt.
    }
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



bool UndoEnabled() {
    return enundo;
}

bool HasConsciouslyChosenTheme() {
    return haspickedtheme;
}

struct previewMaker {
    int w,h;
    int sz;
    parsedSkinFile* ps;
    Drawer* D;
    LuminositySprites ls;
    
    SDL_Surface * result;
    previewMaker(parsedSkinFile& ps, int w, int h) {
        this->w = w;
        this->h = h;
        this->ps = &ps;
        this->sz = ps.gridSize;
        ls.sprites = IMG_Load( ps.sprites.c_str() );
        if(ps.lum == "") {
            ls.luminosity = NULL;
        } else {
            ls.luminosity = IMG_Load( ps.lum.c_str() );
        }

        D = new Drawer(ls,0,0,sz,sz);
        result = SDL_CreateRGBSurface(0,w,h,32,SDL_ENDIAN32MASKS);
        SDL_FillRect(result, 0, 0, w, h, 0xFFFFFFFF);
        
        draw(0,0,          1,0, Red(), Green(), Blue());
        
        //blocks
        draw(1,0,2,0, ps.BKColor[0]);
        draw(1,0,3,0, ps.BKColor[1]);
        draw(1,0,4,0, ps.BKColor[2]);
        draw(1,0,5,0, ps.BKColor[3]);
        
        //arrows
        draw(1,0,9,0, ps.BKColor[0]);
        draw(4,9,9,0, ps.BFColor[0]);
        draw(1,0,9,1, ps.BKColor[1]);
        draw(4,10,9,1, ps.BFColor[1]);
        draw(2,0,9,2, ps.BKColor[2]);
        draw(5,12,9,2, ps.BFColor[2]);
        draw(2,0,9,3, ps.BKColor[3]);
        draw(5,11,9,3, ps.BFColor[3]);
        
        //magnets
        draw(6,12,7,1);
        
        //wildcard
        draw(1,2,7,2);
        

        //gems
        draw(2,3+rand()%2,1,1);
        draw(3,3+rand()%2,4,1);
        draw(4,3+rand()%2,3,1);
        draw(5,3+rand()%2,2,1);
        draw(9,12+rand()%2,5,1);
        
        //monsters
        draw(16, rand()%2, 1,2);
        draw(19, rand()%2, 2,2);
        draw(19, 6+rand()%3, 3,2);
        draw(19, 9+rand()%3, 4,2);
        draw(10,12+rand()%2, 5,2);
        //robot
        draw(3,rand()%3, 1,3);
        //blackie
        draw(0,3, 2,3);
        draw(0,4, 2,3);
        
        //portal
        draw(8,rand()%3, 3,3, ps.BKColor[(int)(rand()%4)] );
        
        { //turner
            int d = rand()%4;
            draw(1,0,7,3, ps.BKColor[d]);
            draw(3,10,7,3, ps.BFColor[d]);
        }
        
        //pusher
        int d=rand()%2;
        draw(4+d,5, 6,0, ps.BKColor[0]);
        draw(4+d,7, 6,0, ps.BFColor[0]);
        d=rand()%2;
        draw(4+d,6, 7,0, ps.BKColor[2]);
        draw(4+d,8, 7,0, ps.BFColor[2]);
        
        //wall time ! 
        sz /= 2;
        draw(18,0, 0,0, ps.WallColor[0]);
        draw(19,0, 1,0, ps.WallColor[0]);
        for (int i=0; i<6; i++) {
            draw(24,i%2==0, 0,1+i, ps.WallColor[0]);
            draw(25,i%2==0, 1,1+i, ps.WallColor[0]);
        }
        draw(18,1, 0,7, ps.WallColor[0]);
        draw(19,1, 1,7, ps.WallColor[0]);
        //--
        draw(18,4, 12,2, ps.WallColor[2]);
        draw(19,4, 13,2, ps.WallColor[2]);
        for (int i=0; i<4; i++) {
            draw(24,4+(i%2==0), 12,3+i, ps.WallColor[2]);
            draw(25,4+(i%2==0), 13,3+i, ps.WallColor[2]);
        }
        draw(18,5, 12,7, ps.WallColor[2]);
        draw(19,5, 13,7, ps.WallColor[2]);
        //--
        draw(18,10, 16,0, ps.WallColor[5]);
        draw(19,10, 17,0, ps.WallColor[5]);
        for (int i=0; i<6; i++) {
            draw(24,10+(i%2==0), 16,1+i, ps.WallColor[5]);
            draw(25,10+(i%2==0), 17,1+i, ps.WallColor[5]);
        }
        draw(18,11, 16,7, ps.WallColor[5]);
        draw(19,11, 17,7, ps.WallColor[5]);

    }
    ~previewMaker() {
        SDL_FreeSurface(ls.sprites);
        if(ls.luminosity != NULL) {
            SDL_FreeSurface(ls.luminosity);
        }
        delete D;
    }
    
    void draw(int sx, int sy, int tx, int ty, SDL_Color col) {
        D->ChangeRect(sx*sz, sy*sz, sz, sz);
        D->SetColors(col,255);
        D->Draw(result, tx*sz, ty*sz);
    }
    void draw(int sx, int sy, int tx, int ty, Uint8 r, Uint8 g, Uint8 b) {
        SDL_Color col;
        col.unused = 255;
        col.r = r, col.g = g, col.b = b;
        draw(sx,sy,tx,ty, col);
    }
    void draw(int sx, int sy, int tx, int ty) {
        SDL_Color col;
        col.unused = col.r = col.g = col.b = 255;
        draw(sx,sy,tx,ty, col);
    }

};

SDL_Surface* makeSkinPreview(parsedSkinFile & ps, int w, int h) {
    previewMaker pr(ps,w,h);
    return pr.result;    
}

bool GetSkinInformation(const char* file, SkinInformation & si)
{
    si.title = "";
    si.author = "";
    si.description = "";
    si.title = "";
    if(si.preview != NULL) {
        SDL_FreeSurface(si.preview);
    }
    si.preview = NULL;
    parsedSkinFile ps;
    string error = parseSkinFile(file, ps);
    if(error!="") {
        si.description = error;
        return false;
    }
    //Make preview...
    haspickedtheme = true;
    si.preview= makeSkinPreview(ps, si.pw, si.ph);
    si.dimx = ps.gridSize * 30;
    si.dimy = ps.gridSize * 22;
    si.author = ps.author;
    si.description = ps.description;
    si.title = ps.title;;
    return true;
}

void LoadSkinFile(const char* file) {
    parsedSkinFile ps;
    string tms = parseSkinFile(file, ps);
    if(tms=="") {
        currentSkinFile = file;
        LoadColors(ps);
        
        WindowIcon = ps.windowicon;
        
        Texture = new char[ps.sprites.length()+1];
        strcpy(Texture, ps.sprites.c_str());
        if(ps.lum == "") {
            LuminosityTexture = NULL;
        } else {
            LuminosityTexture = new char[ps.lum.length()+1];
            strcpy(LuminosityTexture, ps.lum.c_str());
        }
        FontBoldSize = FontSize = ps.ttfSize;
        xyeDirectionSprites = ps.directionSprites;
        Font = new char[ps.font.length()+1];
        strcpy(Font, ps.font.c_str());
        FontBold = new char[ps.boldFont.length()+1];
        strcpy(FontBold, ps.boldFont.c_str());
        GridSize = ps.gridSize;
    } else {
        printf("%s",(tms+"\n").c_str());
    }
}


void ChangeSkinFile(const char* file)
{
    LoadSkinFile(file);
    game::RefreshGraphics();
    haspickedtheme = true;
    options::SaveConfigFile();
}

}
