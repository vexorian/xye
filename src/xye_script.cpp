/*
Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/
//Todo rename this to xye_xml or something like that

#include "xye.h"
#include "options.h"
#include "xye_script.h"
#include "kye_script.h"
#include "xsb_level.h"
#include "gen.h"
#include "tinyxml/xye_tinyxml.h"
#include<string>


/** Class LevelPack begin**/
unsigned int LevelPack::n;
string LevelPack::LevelError;
string LevelPack::Author;
string LevelPack::Name;
string LevelPack::Desc;
string LevelPack::Solution;
string LevelPack::CurrentLevelBye;
string LevelPack::OpenFile;
unsigned int LevelPack::OpenFileLn;
bool LevelPack::defmode;
bool LevelPack::kyemode;
bool LevelPack::xsbmode;
bool LevelPack::FromEditor=false;


TiXmlDocument* LevelPack::Doc =NULL;
TiXmlElement* LevelPack::pack =NULL ;
TiXmlElement* LevelPack::CurrentLevel =NULL;
TiXmlElement* LevelPack::FirstLevel =NULL;

string LevelPack::CurrentLevelTitle;

void LevelPack::Init()
{
    XsbLevelPack::Init();
    KyeLevelPack::Init();
}

void LevelPack::Clean()
{
    XsbLevelPack::Clean();
    KyeLevelPack::Clean();
}

bool LevelPack::AllowUndo()
{
    return (xsbmode);
}





void LevelPack::LoadNthLevel(unsigned int n)
{
    if(defmode)
    {
        LevelPack::Default();
        return;
    }
    if (kyemode)
    {
        KyeLevelPack::LoadNthLevel(n);
        return;
    }
    if (xsbmode)
    {
        XsbLevelPack::LoadNthLevel(n);
        return;
    }

    CurrentLevel=FirstLevel=pack->FirstChildElement("level");
    if (! FirstLevel) { LevelPack::Error("can't find any level"); return; }
    TiXmlElement* tm=FirstLevel;
    int i=1;
    while ((i<n) && (CurrentLevel))
    {
        CurrentLevel=CurrentLevel->NextSiblingElement("level");
        i++;
    }
    if (! CurrentLevel)
    {
        CurrentLevel=FirstLevel;
        OpenFileLn=1;
    }
    else OpenFileLn=n;
    LoadLevel(CurrentLevel);

}

void LevelPack::LoadFirstLevel()
{
    if (kyemode)
        KyeLevelPack::LoadNthLevel(1);
    else if (xsbmode)
        XsbLevelPack::LoadNthLevel(1);
    else
        LoadNthLevel(1);
}



void LevelPack::Restart()
{
    if (kyemode)
        KyeLevelPack::Restart();
    else if (xsbmode)
        XsbLevelPack::Restart();
    else if (defmode)
        LevelPack::Default();
    else
        LoadLevel(CurrentLevel);

}

bool LevelPack::HasSolution()
{
    return(Solution!="");
}


void  LevelPack::LoadInformation()
{
    LevelError = "";
    int a,b,c;
    a=0;

    //Number of levels (count)
    TiXmlElement* pEChild=pack->FirstChildElement("level");
    n=0;
    while(pEChild)
    {
        n++;
        pEChild=pEChild->NextSiblingElement("level");
    }


    if (!n) {LevelPack::Error("The file is a valid Xye XML level file. But it contains no <level> tags."); return;}

    //Pack name:
    pEChild= pack->FirstChildElement("name");
    if ( (pEChild != NULL) && (pEChild->GetText()!=NULL) )
        Name= pEChild->GetText();
    else
        Name= "Unknown";

    //Pack author:
    pEChild= pack->FirstChildElement("author");
    if ( (pEChild != NULL) && (pEChild->GetText()!=NULL) )
        Author= pEChild->GetText();
    else
        Author= "Unknown";

    //Pack Desc:
    pEChild= pack->FirstChildElement("description");
    if ( (pEChild!=NULL) && (pEChild->GetText()!=NULL ) )
        Desc= pEChild->GetText();
    else
        Desc= "Unknown";


}


bool LevelPack::GetFileData(const char* filename, string &au, string &ds, string &ti, unsigned int &leveln)
{

    int L=strlen(filename);
    int i=0;


    if ((L>4) && doublematch<char>(filename[L-1],'e','E') && doublematch<char>(filename[L-2],'y','Y') && doublematch<char>(filename[L-3],'k','K') && (filename[L-4]=='.'))
    {

        const char* err=KyeLevelPack::ReadData(filename,leveln );

        i=L-1;
        while((i>=0) && (filename[i]!='/')) i--;
        i++;
        ti="";
        while ((i<L) && (filename[i]!='.'))
           ti+=filename[i++];


        if (err)
        {
            leveln=0;
            ds="Invalid .kye file ";
            ds+="(";
            ds+=err;
            ds+=")";

            return false;
        }
        au="";
        ds=".kye level file";
        return true;
    }

    if ((L>4) && (filename[L-4]=='.')
          && (  (strcmp(filename+L-3,"xsb") == 0) || (strcmp(filename+L-3,"XSB") == 0) 
                || (strcmp(filename+L-3,"slc") == 0) || (strcmp(filename+L-3,"SLC") == 0)
             )
       )
       
    {
        ti = "";
        const char* err=XsbLevelPack::ReadData(filename,leveln , au, ds, ti);

        if( ti=="")
        {
            i=L-1;
            while((i>=0) && (filename[i]!='/')) i--;
            i++;
            ti=(filename+i);
        }


        if (err)
        {
            leveln=0;
            ds="Invalid sokoban level file ";
            ds+="(";
            ds+=err;
            ds+=")";

            return false;
        }
        
        return true;
    }



    char* tm,*tm2;
/*   au="!!";
   ds="...";
   ti="!!!";
return true;*/
    //TiXmlDocument fil(tm2);
    TiXmlDocument  fil(filename);


    TiXmlElement* pack, *el;
    bool val=false;
    if (fil.LoadFile())
    {
        pack=fil.FirstChildElement("pack");
        if (pack!=NULL)
        {

            el= pack->FirstChildElement("name");

            if ( (el!=NULL) && (el->GetText()!=NULL) )
                ti= el->GetText();
            else
                ti="Just another Xye level file";

            el= pack->FirstChildElement("author");
            if ( (el!=NULL) && (el->GetText()!=NULL) )
                au= el->GetText();
            else
                au="Unknown";
            el= pack->FirstChildElement("description");

            if ( (el!=NULL) && (el->GetText()!=NULL) )
                ds= el->GetText();
            else
                ds="(No description)";
            val=true;

            leveln=0;
            el = pack->FirstChildElement("level");
            while (el)
            {
                leveln++;
                el=el->NextSiblingElement("level");
            }
            
            if( leveln==0) val=false;


        }
        else if (pack=fil.FirstChildElement("xyereplay"))
            {
                leveln=0;
                au="";
                ds="Level:";
                ds+=pack->Attribute("levelfile");
                ds+=",#";
                ds+=pack->Attribute("leveln");


                ti="Replay";
                val=true;
            }
    }

    if (! val)
    {

        au="Invalid File";
        ds="This file is not a valid Xye level file";
        int L=strlen(fil.ErrorDesc());
        char* err=new char[L+40];
        sprintf(err,"\n\n(Error: %s at line: %d col: %d)",fil.ErrorDesc(),fil.ErrorRow(),fil.ErrorCol());
        ds+=err;
        delete[] err;
        ti="Invalid File";
    }


return(val);
}



void LevelPack::Load(const char *filename, unsigned int ln, const string replay)
{
    LevelError = "";
    OpenFile=filename;
    OpenFileLn=ln;
    if (Doc!=NULL)
    {

        pack=NULL;
        CurrentLevel=NULL;
        delete Doc;
        Doc=NULL;
    }
    int L=strlen(filename);






    if ((L>4) && doublematch<char>(filename[L-1],'e','E') && doublematch<char>(filename[L-2],'y','Y') && doublematch<char>(filename[L-3],'k','K') && (filename[L-4]=='.'))
    {
        LevelPack::Author="";
        LevelPack::Name="";
        LevelPack::Desc="";
        xsbmode=!(kyemode=true);
        KyeLevelPack::Load(filename,ln);
        if (replay!="") {
            game::PlayRecording(replay);
        }

        return;
    }

    if ((L>4) && (filename[L-4]=='.') &&
        ((strcmp(filename+L-3,"xsb") ==0)||(strcmp(filename+L-3,"XSB") ==0)||(strcmp(filename+L-3,"SLC") ==0)||(strcmp(filename+L-3,"slc") ==0))
       )
    {
        LevelPack::Author="";
        LevelPack::Name="";
        LevelPack::Desc="";
        kyemode=!(xsbmode=true);
        XsbLevelPack::Load(filename,ln);
        if (replay!="") {
            game::PlayRecording(replay);
        }

        return;
    }
    kyemode=xsbmode=defmode=false;
    Doc= new TiXmlDocument(filename);

    if (Doc->LoadFile())
    {

        pack=Doc->FirstChildElement("pack");

        if (pack==NULL)
        {
            pack=Doc->FirstChildElement("xyereplay");
            if (pack) //It is a replay file!
            {
                const char * bf=pack->Attribute("levelfile");
                
                const char* tm=bf;
                int tmx=1;
                pack->QueryIntAttribute("leveln",&tmx);
                ln=tmx;
                if (pack=pack->FirstChildElement("moves"))
                {
                    bf=pack->GetText();
                    if(bf!=NULL)
                    {
                        LevelPack::Load(tm,ln,bf);
                    }
                    else
                    {
                        LevelPack::Load(tm, ln, "");
                    }
                    return;
                }
                else { LevelPack::Error("replay file has no replay data"); return; }

            }
            else
            {
                fprintf(stderr,"Can't find pack element!");
                LevelPack::Error("Can't find pack element!");
                return;
            }
        }
        LoadInformation();
        LoadNthLevel(ln);
        if (replay!="") {
            game::PlayRecording(replay);
        }

    }
    else
    {
        string s=string("Invalid / Missing Level xml file: "+string(filename)+" ["+string(Doc->ErrorDesc())+"]")  ;
        fprintf(stderr,"%s", s.c_str() );
        LevelPack::Error(s.c_str());
    }


}

void LevelPack::Next()
{
    if (defmode)
    {
        LevelPack::Default();
        return;
    }

    if (kyemode)
    {
        KyeLevelPack::Next();
        return;
    }
    if (xsbmode)
    {
        XsbLevelPack::Next();
        return;
    }
    TiXmlElement* nx= (CurrentLevel->NextSiblingElement("level"));
    if (nx)
    {
        CurrentLevel=nx;
        OpenFileLn++;
    }
    else
    {
        CurrentLevel=FirstLevel;
        OpenFileLn=1;
    }
    LoadLevel(CurrentLevel);
}

void LevelPack::Last()
{
    if (defmode)
    {
        LevelPack::Default();
        return;
    }

    if (kyemode)
    {
        KyeLevelPack::Last();
        return;
    }
    if (xsbmode)
    {
        XsbLevelPack::Last();
        return;
    }
    TiXmlNode * nd=CurrentLevel->PreviousSibling("level");
    while ((nd) && (! nd->ToElement() )) nd=nd->PreviousSibling("level");

    if (nd)
    {
        CurrentLevel=nd->ToElement();
        OpenFileLn--;
    }
    else
    {

        TiXmlElement * el=FirstLevel, *ls;
        OpenFileLn=1;
        while ( (ls=el) &&  (el=el->NextSiblingElement("level")) ) OpenFileLn++;;
        CurrentLevel=ls;
    }

    LoadLevel(CurrentLevel);

}

bool LevelPack::HasPrevious()
{
    if(defmode) return false;
    if (kyemode)
        return KyeLevelPack::HasLast();
    if (xsbmode)
        return XsbLevelPack::HasLast();
    TiXmlNode * nd=CurrentLevel->PreviousSibling("level");
    while ((nd) && (! nd->ToElement() )) nd=nd->PreviousSibling("level");
    return (nd != NULL);
}
bool LevelPack::HasNext()
{
    if(defmode) return false;
    if (kyemode)
        return KyeLevelPack::HasNext();
    if (xsbmode)
        return XsbLevelPack::HasNext();
    return ( CurrentLevel->NextSiblingElement("level") != NULL);
}

void LevelPack::SetLevelBye(const char* bye)
{
    const char* add="";
    if(FromEditor)
    {
        add="Press [backspace] to return to the editor, [enter] to play again.";
    }
    else if (HasNext())
    {
        add="Press [N] or [+] to go to the next level";
    }
    else if (HasPrevious())
    {
        add="You beat the last level. use [P] or [-] to go back to a previous level or [backspace] to browse for more levels.";
    }
    else
        add="You beat the only level of this level pack, use [backspace] to browse for more levels";

    if ( (! bye) || (strlen(bye) == 0) )
        CurrentLevelBye="";
    else
    {
        CurrentLevelBye=bye;
        CurrentLevelBye+=" - ";
    }
    CurrentLevelBye+=add;

}


/** Class LevelPack end**/






char TempCharA[30];

int LastX;
int LastY;



/* Load Xye*/
void Load_Xye(TiXmlElement* el,bool KyeLoaded)
{
    int lives=4;
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        el->QueryIntAttribute("lives",&lives);

    if (! KyeLoaded) game::XYE= new xye(game::SquareN(LastX,LastY));
    lives=(lives<=0)?1:lives;
    game::XYE->SetLives(lives);
}

/* Load Wall*/
void Load_Wall(TiXmlElement* el, bool defround)
{
    int cid=0,r1=defround,r3=defround,r7=defround,r9=defround,t=-1;
    int x2=0,y2=0,i,j;
    bool round;

    square* sq;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);

        el->QueryIntAttribute("x2",&x2);
        el->QueryIntAttribute("y2",&y2);

        //for compat
        el->QueryIntAttribute("round1",&r1);
        el->QueryIntAttribute("round3",&r3);
        el->QueryIntAttribute("round7",&r7);
        el->QueryIntAttribute("round9",&r9);

        //round attrib
        const char* rnd=el->Attribute("round");
        if (rnd)
         for (int i=0; ((rnd[i]!=0) && (i<4)) ;i++)
           switch (rnd[i])
           {
               case('1'): r1=true; break;
               case('3'): r3=true; break;
               case('7'): r7=true; break;
               default : r9=true;
           }




        el->QueryIntAttribute("type",&t);
        el->QueryIntAttribute("color",&cid);



    //normalize:
    LastX=LastX%XYE_HORZ;x2=x2%XYE_HORZ;
    LastY=LastY%XYE_VERT;y2=y2%XYE_VERT;




    if (LastX>x2) x2=LastX;
    if (LastY>y2) y2=LastY;


    round= (r1 || r3 || r7 || r9);
    Uint8 R,G,B;
    palette_mode pm = PM_MULTIPLY;
    if (cid) palette::GetColor(cid,R,G,B,pm);


    for (i=LastX;i<=x2;i++) for (j=LastY;j<=y2;j++)
    {
        sq=game::Square(i,j);
        wall* wl;
        if (t>=0) wl=new wall(sq,t);
        else wl=new wall(sq);
        if (round) wl->SetRoundCorners(
            (r7 && (i==LastX) && (j==y2)),
            (r1 && (i==LastX) && (j==LastY)),
            (r3 && (i==x2) && (j==LastY)),
            (r9 && (i==x2) && (j==y2))
        );
        if (cid) wl->ChangeColor(R,G,B, (pm==PM_MULTIPLY) );

    }


}

/* Load Gem*/
void Load_Gem(TiXmlElement* el)
{
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el,B_BLUE);

    gem* gm=new gem(game::SquareN(LastX,LastY),c);

}

/* Load Star*/
void Load_Star(TiXmlElement* el)
{
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);

    star* gm=new star(game::SquareN(LastX,LastY) );

}



/* Load Number*/
void Load_Number(TiXmlElement* el)
{
    int v=0,round=0;
    unsigned char cv;
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        el->QueryIntAttribute("val",&v);
        cv = (unsigned char)( (v<0)?0:(v>9)?9:v);
        blockcolor c=GetElementBlockColor(el,B_YELLOW);
        el->QueryIntAttribute("round",&round);

    number* gm=new number(game::SquareN(LastX,LastY),c,cv,round);
}


/* Load Robot*/
void Load_Robot(TiXmlElement* el)
{
    int v=0;
    unsigned char cv;
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);

    roboxye* rb=new roboxye(game::SquareN(LastX,LastY));
}







/* Load BlackHole*/
void Load_Blackhole(TiXmlElement* el)
{
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);


    dangerous* dn=new dangerous(game::SquareN(LastX,LastY),OT_BLACKHOLE);

}

/* Load Mine*/
void Load_Mine(TiXmlElement* el)
{
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);


    dangerous* dn=new dangerous(game::SquareN(LastX,LastY),OT_MINE);

}


/* Load Block*/
void Load_Block(TiXmlElement* el)
{
    int colorless=0,round=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        el->QueryIntAttribute("nocolor",&colorless);
        el->QueryIntAttribute("round",&round);

    block* dn=new block(game::SquareN(LastX,LastY),c, round);
    if (colorless) dn->colorless=true;

}

/* Load LargeBlock*/
void Load_LargeBlock(TiXmlElement* el)
{
    int colorless=0;
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        el->QueryIntAttribute("nocolor",&colorless);
    const char* ptr = el->Attribute("sharededges");
    bool up=false, right=false, down=false, left=false;
    if(ptr)
    {
        for (int i=0; ptr[i]!='\0'; i++)
            switch(ptr[i])
            {
                case 'U': up=true; break;
                case 'R': right=true; break;
                case 'D': down=true; break;
                case 'L': left=true; break;
            }
    }
    if( !(up|down|left|right))
    {  //don't bother
       block* dn=new block(game::SquareN(LastX,LastY),c, false);
       if (colorless) dn->colorless=true;
    }
    else
    {
        largeblock* bc=new largeblock(game::SquareN(LastX,LastY),c, up,right,down,left);
        if (colorless) bc->colorless=true;
    }
    

}

/* Load WindowBlock*/
void Load_WindowBlock(TiXmlElement* el)
{

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);

    windowblock* bc=new windowblock(game::SquareN(LastX,LastY),c);


}


/* Load Rattler Food*/
void Load_RFood(TiXmlElement* el)
{

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);

    rfood* rf=new rfood(game::SquareN(LastX,LastY));


}


/* Load Rattler*/
void Load_Rattler(TiXmlElement* el)
{
    int grow=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        el->QueryIntAttribute("grow",&grow);
        edir d=GetElementDir(el, D_DOWN );

    rattler* rt=new rattler(game::SquareN(LastX,LastY),d,grow);
    TiXmlElement* nd=el->FirstChildElement("body");
    while (nd)
    {
        nd->QueryIntAttribute("x",&LastX);
        nd->QueryIntAttribute("y",&LastY);
        rt->Node(game::SquareN(LastX,LastY));
        nd= nd->NextSiblingElement("body");
    }

}


/* Load Lock*/
void Load_Lock(TiXmlElement* el)
{
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
    lock* bc=new lock(game::SquareN(LastX,LastY),c);
}

/* Load Key*/
void Load_Key(TiXmlElement* el)
{
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
    key* bc=new key(game::SquareN(LastX,LastY),c);
}






/* Load Low Density*/
void Load_LowDensity(TiXmlElement* el)
{
    int round=0;
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        el->QueryIntAttribute("round",&round);
    lowdensity* bc=new lowdensity(game::SquareN(LastX,LastY),c,round);
    if (el->Attribute("active") ) bc->Activate(GetElementDir(el,D_DOWN,"active"));
}


/* Load Surprise*/
void Load_Surprise(TiXmlElement* el)
{
    int round=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        el->QueryIntAttribute("round",&round);

    surprise* bc=new surprise(game::SquareN(LastX,LastY),c,round);


}


/* Load turner*/
void Load_Turner(TiXmlElement* el,unsigned int aclock)
{
    int colorless=0,round=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        el->QueryIntAttribute("nocolor",&colorless);
        el->QueryIntAttribute("round",&round);

    turner* bc=new turner(game::SquareN(LastX,LastY),c,!(aclock),round);
    if (colorless) bc->colorless=true;


}


/* Load GemBlock*/
void Load_GemBlock(TiXmlElement* el)
{

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);

    gemblock* bc=new gemblock(game::SquareN(LastX,LastY),c);


}

/* Load WildCard*/
void Load_WildCard(TiXmlElement* el)
{

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        int r=0;
        el->QueryIntAttribute("round",&r);

    wildcard* wd=new wildcard(game::SquareN(LastX,LastY),(bool)(r) );


}

/* Load MetalBlock*/
void Load_MetalBlock(TiXmlElement* el)
{

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        int r=0;
        el->QueryIntAttribute("round",&r);

    metalblock* mb=new metalblock(game::SquareN(LastX,LastY),(bool)(r) );


}


/* Load Earth*/
void Load_Earth(TiXmlElement* el)
{
    int c=0;
    int r=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        el->QueryIntAttribute("color",&c);
        el->QueryIntAttribute("round",&r);

    earth* et=new earth(game::SquareN(LastX,LastY));

    if (c)
    {
        Uint8 r,g,b;
        palette::GetColor(c,r,g,b);
        et->ChangeColor(r,g,b);
    }
    if (r) et->SetRound(r);


}


/* Load Auto*/
void Load_Auto(TiXmlElement* el)
{
    int round=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        edir d=GetElementDir(el, autoarrow::GetDefaultEdirByColumn(LastX) );

        el->QueryIntAttribute("round",&round);

    autoarrow* bc=new autoarrow(game::SquareN(LastX,LastY),c,d,round);



}


/* Load Factory*/
void Load_Factory(TiXmlElement* el)
{
    int round=0,colorless=0,kind=0,limit=1000000;
    int ib=0;
        el->QueryIntAttribute("beastkind",&ib);
    otype rs;
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        edir d=GetElementDir(el);
        edir sd=GetElementDir(el, D_UP, "swdir" );
        if (d==sd) sd=(edir)((int)(d)+1);

        el->QueryIntAttribute("round",&round);
        el->QueryIntAttribute("nocolor",&colorless);

        el->QueryIntAttribute("kind",&kind);
        if (el->Attribute("limit")) el->QueryIntAttribute("limit",&limit);

        switch(kind)
        {
            case(1):
                rs=OT_ARROW; break;
            case(2):
                rs=OT_LOWDENSITY; break;
            case(3):
                rs=OT_MINE; break;
            case(4):
                rs=OT_PUSHER; break;
            case(5):
                rs=OT_BEAST; break;
            case(6):
                rs=OT_RATTLERFOOD; break;
            case(7):
                rs=OT_RATTLER; break;
            case(8):
                rs=OT_GEM; break;

            default:
             rs=OT_BLOCK;
        }

    factory* f=new factory(game::SquareN(LastX,LastY),rs,c,d,sd,round,colorless,(btype)(ib%BEASTN));
    f->limit=limit;



}


/* Load Filler*/
void Load_Filler(TiXmlElement* el)
{
    int round=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        edir d=GetElementDir(el, D_DOWN );

        el->QueryIntAttribute("round",&round);

    filler* bc=new filler(game::SquareN(LastX,LastY),c,d,round);



}

/* Load Snipper*/
void Load_Sniper(TiXmlElement* el)
{
    int round=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);

        el->QueryIntAttribute("round",&round);

    sniper* bc=new sniper(game::SquareN(LastX,LastY),c,round);



}


/* Load Arrow*/
void Load_Arrow(TiXmlElement* el)
{
    int round=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        edir d=GetElementDir(el, D_DOWN );

        el->QueryIntAttribute("round",&round);

    arrow* bc=new arrow(game::SquareN(LastX,LastY),c,d,round);



}

/* Load ScrollBlock*/
void Load_ScrollBlock(TiXmlElement* el)
{
    int round=0,nocolor=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        edir d=GetElementDir(el, D_DOWN );

        el->QueryIntAttribute("round",&round);
        el->QueryIntAttribute("nocolor",&nocolor);


    scrollblock* bc=new scrollblock(game::SquareN(LastX,LastY),c,round,d);
    bc->colorless=nocolor;



}


/* Load Teleport*/
void Load_Teleport(TiXmlElement* el)
{
    int round=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        edir d=GetElementDir(el, D_DOWN );


    teleport* tp=new teleport(game::SquareN(LastX,LastY),d);



}








/* Load Toggle*/
void Load_Toggle(TiXmlElement* el)
{
    int round=0;
    int off=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        el->QueryIntAttribute("off",&off);

        el->QueryIntAttribute("round",&round);

    toggle* bc=new toggle(game::SquareN(LastX,LastY),c,round, ! off);



}




/* Load Pusher*/
void Load_Pusher(TiXmlElement* el)
{

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        edir d=GetElementDir(el, D_DOWN );
    impacter* bc=new impacter(game::SquareN(LastX,LastY), c,d);



}

/* Load Beast*/
void Load_Beast(TiXmlElement* el)
{

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        int ib=0;

        el->QueryIntAttribute("kind",&ib);

        edir d=GetElementDir(el, D_DOWN );
    beast* bc=new beast(game::SquareN(LastX,LastY), btype(ib),d);



}



/* Load Magnet*/
void Load_Magnet(TiXmlElement* el)
{
    int kind=0;
    int horz=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);

        el->QueryIntAttribute("horz",&horz);
        el->QueryIntAttribute("kind",&kind);



    magnetic* mg=new magnetic(game::SquareN(LastX,LastY), (mgtype)(kind), horz  );



}



/* Load BlockDoor */
void Load_BlockDoor(TiXmlElement* el, unsigned int AsTrap)
{
    int open=0,round=0;

        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);
        el->QueryIntAttribute("open",&open);

    blockdoor* bc=new blockdoor(game::SquareN(LastX,LastY),(AsTrap!=0), (! open),c);
}

/* Load Marked Area */
void Load_Marked(TiXmlElement* el)
{
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        blockcolor c=GetElementBlockColor(el);

    marked* bc=new marked(game::SquareN(LastX,LastY), c);
}

/* Load Fire Pad*/
void Load_FirePad(TiXmlElement* el)
{
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);

    firepad* fp=new firepad(game::SquareN(LastX,LastY));
}

/* Load Pit*/
void Load_Pit(TiXmlElement* el)
{
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);

    pit* fp=new pit(game::SquareN(LastX,LastY));
}


/* Load Hint */
void Load_Hint(TiXmlElement* el, bool warn)
{
    int c1=0,c2;
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);
        /*el->QueryIntAttribute("color",&c1);*/
    const char* tx = el->GetText ();
    string text = ( (tx!=NULL) ? tx : "") ;
    hint* hn=new hint(game::SquareN(LastX,LastY), text, warn);
    /*if (c1!=0)
    {
        el->QueryIntAttribute("markcolor",&c2);
        hn->ChangeColor(palette::GetColor(c1),palette::GetColor(c2));
    }*/
}
#include<iostream>

/* Load Portal */
void Load_Portal(TiXmlElement* el)
{
    int tx=0,ty=0,c=0;
    int defcolor=-1;
        el->QueryIntAttribute("x",&LastX);
        el->QueryIntAttribute("y",&LastY);

        el->QueryIntAttribute("targetx",&tx);
        el->QueryIntAttribute("targety",&ty);
        el->QueryIntAttribute("color",&c);

    Uint8 R,G,B;
    R=G=B=255;
    el->QueryIntAttribute("defcolor",&defcolor);
    if( (defcolor>=0) && (defcolor<4))
    {
            SDL_Color cc = options::BKColor[defcolor];
            R=cc.r;
            G=cc.g;
            B=cc.b;
    }
    else if (c) palette::GetColor(c,R,G,B);

    portal* pt= new portal(game::SquareN(LastX,LastY),R,G,B,tx,ty);
}

/* Load TrickDoor */
void Load_TrickDoor(TiXmlElement* el, int opt)
{
   int c=0;
   el->QueryIntAttribute("x",&LastX);
   el->QueryIntAttribute("y",&LastY);
   el->QueryIntAttribute("color",&c);

   //tdoor(square* sq,tdtype t,bool up, bool right, bool down, bool left);

   bool u=false,r=false,d=false,l=false;

   tdtype tt;

   if ( (! opt) || (opt==2) ) //Classical one way doors from kye or Force arrow
   {
       edir dr=GetElementDir(el);
       if (opt==2) dr=Opposite(dr);
       switch(dr)
       {
            case(D_LEFT): l=true; break;
            case(D_RIGHT): r=true; break;
            case(D_DOWN): d=true; break;
            default: u=true;
       }
       if (!opt) tt= (l || r)? td_HORZ:td_VERT;
       else switch(dr)
       {
            case(D_LEFT): tt=td_FORCEARROW_LEFT; break;
            case(D_RIGHT): tt=td_FORCEARROW_RIGHT; break;
            case(D_DOWN): tt=td_FORCEARROW_DOWN; break;
            default: tt=td_FORCEARROW_UP; break;
       }




   }
   else //magic squares or whatever
   {
       tt=td_SUPER;
       const char * ent=el->Attribute("ent");
       if (ent)
         for (int i=0; ((ent[i]!=0) && (i<4)) ;i++)
           switch (ent[i])
           {
               case('4'): l=true; break;
               case('6'): r=true; break;
               case('2'): d=true; break;
               default : u=true;
           }



   }

   tdoor* td= new tdoor(game::SquareN(LastX,LastY),tt,u,r,d,l);
   Uint8 red,green,blue;


   if (c)
   {
        palette::GetColor(c,red,green,blue);
        td->ChangeColor(red,green,blue);
   }




}



//==================================================================================================
// Really helpful things:
//
blockcolor GetElementBlockColor(TiXmlElement* el, blockcolor def)
{
    char x='\0';
    const char* at=el->Attribute ("bc");
    if (at!=NULL) x=at[0];
    switch(x)
    {
        case('B'): case('b'): return (B_BLUE);
        case('G'): case('g'): return (B_GREEN);
        case('R'): case('r'): return (B_RED);
        case('Y'): case('y'): return (B_YELLOW);
    }

    return (def);
}

edir GetElementDir(TiXmlElement* el, edir def,const char * tag)
{
    char x='\0';
    const char* at=el->Attribute (tag);
    if (at!=NULL) x=at[0];
    switch(x)
    {
        case('U'): case('u'): return (D_UP);
        case('D'): case('d'): return (D_DOWN);
        case('L'): case('l'): return (D_LEFT);
        case('R'): case('r'): return (D_RIGHT);
    }

    return (def);

}



otype GetOTFromXmlElement(TiXmlElement* x, unsigned int *extra)
{

    strcpy(TempCharA,x->Value());

    if (strcmp(TempCharA,"wall")==0) { *extra=0; return OT_WALL; }
    else if (strcmp(TempCharA,"roundwall")==0) { *extra=1; return OT_WALL; }
    else if (strcmp(TempCharA,"block")==0) return OT_BLOCK;
    else if (strcmp(TempCharA,"window")==0) return OT_WINDOW;
    else if (strcmp(TempCharA,"rattler")==0) return OT_RATTLER;
    else if (strcmp(TempCharA,"rfood")==0) return OT_RATTLERFOOD;

    else if (strcmp(TempCharA,"toggle")==0) return OT_TOGGLE;
    else if (strcmp(TempCharA,"auto")==0) return OT_AUTO;
    else if (strcmp(TempCharA,"factory")==0) return OT_FACTORY;
    else if (strcmp(TempCharA,"surprise")==0) return OT_SURPRISE;
    else if (strcmp(TempCharA,"filler")==0) return OT_FILLER;
    else if (strcmp(TempCharA,"sniper")==0) return OT_SNIPER;
    else if (strcmp(TempCharA,"earth")==0) return OT_EARTH;
    else if (strcmp(TempCharA,"gemblock")==0) return OT_GEMBLOCK;
    else if (strcmp(TempCharA,"lock")==0) return OT_LOCK;
    else if (strcmp(TempCharA,"key")==0) return OT_KEY;
    else if (strcmp(TempCharA,"wild")==0) return OT_WILDCARD;
    else if (strcmp(TempCharA,"star")==0) return OT_STAR;
    else if (strcmp(TempCharA,"metalblock")==0) return OT_METALBLOCK;
    else if (strcmp(TempCharA,"arrow")==0) return OT_ARROW;
    else if (strcmp(TempCharA,"scroll")==0) return OT_SCROLLBLOCK;
    else if (strcmp(TempCharA,"magnet")==0) return OT_MAGNETIC;
    else if (strcmp(TempCharA,"pusher")==0) return OT_PUSHER;
    else if (strcmp(TempCharA,"teleport")==0) return OT_TELEPORT;
    else if (strcmp(TempCharA,"beast")==0) return OT_BEAST;
    else if (strcmp(TempCharA,"blacky")==0) return OT_BLACKHOLE;
    else if (strcmp(TempCharA,"mine")==0) return OT_MINE;
    else if (strcmp(TempCharA,"timer")==0) return OT_NUMBER;
    else if (strcmp(TempCharA,"bot")==0) return OT_ROBOXYE;
    else if (strcmp(TempCharA,"lblock")==0) return OT_LOWDENSITY;
    else if (strcmp(TempCharA,"largeblockpart")==0) return OT_LARGEBLOCK;
    else if (strcmp(TempCharA,"clocker")==0) { *extra=0; return OT_TURNER; }
    else if (strcmp(TempCharA,"aclocker")==0) { *extra=1; return OT_TURNER; }

     //ground:
    else if (strcmp(TempCharA,"blockdoor")==0) { *extra=0; return OT_BLOCKDOOR; }
    else if (strcmp(TempCharA,"blocktrap")==0) { *extra=1; return OT_BLOCKDOOR; }
    else if (strcmp(TempCharA,"hint")==0) return OT_HINT;
    else if (strcmp(TempCharA,"warning")==0) return OT_WARNING;
    else if (strcmp(TempCharA,"portal")==0) return OT_PORTAL;
    else if (strcmp(TempCharA,"firepad")==0) return OT_FIREPAD;
    else if (strcmp(TempCharA,"pit")==0) return OT_PIT;
    else if (strcmp(TempCharA,"oneway")==0) { *extra=0; return OT_TRICKDOOR; }
    else if (strcmp(TempCharA,"hiddenway")==0) { *extra=1; return OT_TRICKDOOR; }
    else if (strcmp(TempCharA,"force")==0) { *extra=2; return OT_TRICKDOOR; }
    else if (strcmp(TempCharA,"marked")==0) { *extra=0; return OT_MARKEDAREA; }
    

    return OT_GEM; //Default "gem"
}



//========================================================================================
// Let's load the objects!:
//
void LoadObjects(TiXmlElement* normal)
{
    TiXmlElement* pEChild= normal->FirstChildElement();
    unsigned int x=0;
    while (pEChild)
    {
        switch(GetOTFromXmlElement(pEChild,&x))
        {
            case(OT_WALL): Load_Wall(pEChild,x); break;
            case(OT_GEM): Load_Gem(pEChild); break;
            case(OT_STAR): Load_Star(pEChild); break;
            case(OT_AUTO): Load_Auto(pEChild); break;
            case(OT_FACTORY): Load_Factory(pEChild); break;
            case(OT_SURPRISE): Load_Surprise(pEChild); break;
            case(OT_FILLER): Load_Filler(pEChild); break;
            case(OT_SNIPER): Load_Sniper(pEChild); break;
            case(OT_EARTH): Load_Earth(pEChild); break;
            case(OT_GEMBLOCK): Load_GemBlock(pEChild); break;
            case(OT_WILDCARD): Load_WildCard(pEChild); break;
            case(OT_METALBLOCK): Load_MetalBlock(pEChild); break;
            case(OT_ARROW): Load_Arrow(pEChild); break;
            case(OT_SCROLLBLOCK): Load_ScrollBlock(pEChild); break;

            case(OT_MAGNETIC): Load_Magnet(pEChild); break;
            case(OT_PUSHER): Load_Pusher(pEChild); break;
            case(OT_BEAST): Load_Beast(pEChild); break;
            case(OT_LOWDENSITY): Load_LowDensity(pEChild); break;
            case(OT_BLACKHOLE): Load_Blackhole(pEChild); break;
            case(OT_MINE): Load_Mine(pEChild); break;
            case(OT_NUMBER): Load_Number(pEChild); break;
            case(OT_TELEPORT): Load_Teleport(pEChild); break;
            case(OT_TURNER): Load_Turner(pEChild,x); break;
            case(OT_ROBOXYE): Load_Robot(pEChild); break;
            case(OT_TOGGLE): Load_Toggle(pEChild); break;
            case(OT_LOCK): Load_Lock(pEChild); break;
            case(OT_KEY): Load_Key(pEChild); break;
            case(OT_RATTLER): Load_Rattler(pEChild); break;
            case(OT_RATTLERFOOD):Load_RFood(pEChild); break;
            case(OT_WINDOW):Load_WindowBlock(pEChild); break;
            case(OT_BLOCK): Load_Block(pEChild); break;
            case(OT_BLOCKDOOR): Load_BlockDoor(pEChild,x); break;
            case(OT_MARKEDAREA): Load_Marked(pEChild); break;
            case(OT_HINT): Load_Hint(pEChild,false); break;
            case(OT_WARNING): Load_Hint(pEChild,true); break;
            case(OT_LARGEBLOCK): Load_LargeBlock(pEChild); break;
            case(OT_PORTAL): Load_Portal(pEChild); break;
            case(OT_FIREPAD): Load_FirePad(pEChild); break;
            case(OT_PIT): Load_Pit(pEChild); break;
            case(OT_TRICKDOOR): Load_TrickDoor(pEChild,x);
        }
        pEChild= pEChild->NextSiblingElement(); //Next normal object
    }



}

//===========================================================================================
void LoadPalette(TiXmlElement* pal)
{
    TiXmlElement* pEChild= pal->FirstChildElement("color");

    int id=0;
    int r,g,b;

    while (pEChild)
    {

        //Load a color
        id=r=g=b=0;

        pEChild->QueryIntAttribute("id",&id);
        pEChild->QueryIntAttribute("red",&r);
        pEChild->QueryIntAttribute("green",&g);
        pEChild->QueryIntAttribute("blue",&b);
        const char* mode = pEChild->Attribute("mode");
        if( (mode!=NULL) && (strcmp(mode,"MULTIPLY")==0) )
        {
            palette::SetColor(id,r%256,g%256,b%256, PM_MULTIPLY);
        }
        else
        {
            palette::SetColor(id,r%256,g%256,b%256, PM_RECOLOR);
        }

        


        pEChild= pEChild->NextSiblingElement("color"); //Next color element
    }

}


//===========================================================================================
void LoadDefaults_Wall(TiXmlElement* el)
{
    int cid=0;
    int t=0;
        el->QueryIntAttribute("color",&cid);
        el->QueryIntAttribute("type",&t);

    if (cid)
    {
        SDL_Color tm;
        palette_mode pm;
        palette::GetColor(cid,tm.r, tm.g, tm.b, pm);
        wall::SetDefaultColor(tm, (pm==PM_MULTIPLY) );
    }


    if (t) wall::SetDefaultType(t);

}

void LoadDefaults_Tdoor(TiXmlElement* el)
{
    int cid=0;
    int t=0;
        el->QueryIntAttribute("color",&cid);

    if (cid)
    {
        Uint8 r,g,b;
        palette::GetColor(cid,r,g,b);
        tdoor::ChangeDefaultColor(r,g,b);
    }

}


void LoadDefaults_ForceArrow(TiXmlElement* el)
{
    int cid=0;
    int t=0;
        el->QueryIntAttribute("color",&cid);

    if (cid)
    {
        Uint8 r,g,b;
        palette::GetColor(cid,r,g,b);
        tdoor::ChangeForceArrowDefaultColor(r,g,b);
    }

}

void LoadDefaults_Earth(TiXmlElement* el)
{
    int cid=0;
    int t=0;
        el->QueryIntAttribute("color",&cid);

    if (cid)
    {
        Uint8 r,g,b;
        palette::GetColor(cid,r,g,b);
        earth::SetDefaultColor(r,g,b);
    }

}

//===========================================================================================
void LoadDefaults(TiXmlElement* def)
{
    TiXmlElement* pEChild= def->FirstChildElement();



    while (pEChild)
    {

        strcpy(TempCharA,pEChild->Value());
        if (strcmp(TempCharA,"wall")==0) LoadDefaults_Wall(pEChild);
        else if (strcmp(TempCharA,"trick")==0) LoadDefaults_Tdoor(pEChild);
        else if (strcmp(TempCharA,"force")==0) LoadDefaults_ForceArrow(pEChild);
        else if (strcmp(TempCharA,"earth")==0) LoadDefaults_Earth(pEChild);

        pEChild= pEChild->NextSiblingElement(); //Next element
    }

}






//===========================================================================================
// Let's load the floor decoration!
//

void LoadFloor(TiXmlElement* floor)
{
    int i,j;
    int x2=0,y2=0;
    int cid=0;
    unsigned int c=0;
    int skn=0;
    TiXmlElement* area= floor->FirstChildElement("area");
    square* sq;
    Uint8 R,G,B;

    while (area)
    {
        x2=y2=cid=skn=0;
            area->QueryIntAttribute("x",&LastX);
            area->QueryIntAttribute("y",&LastY);
            area->QueryIntAttribute("x2",&x2);
            area->QueryIntAttribute("y2",&y2);
            area->QueryIntAttribute("color",&cid);
            area->QueryIntAttribute("skin",&skn);

        //Normalize:
        LastX = LastX % XYE_HORZ;
        LastY = LastY % XYE_VERT;
        x2= x2 % XYE_HORZ;
        y2= y2 % XYE_VERT;

        if (x2<LastX) x2=LastX;
        if (y2<LastY) y2=LastY;

        //Load color from palette:
        if (cid)
            palette::GetColor(cid,R,G,B);


        //Change squares:
        for (i=LastX;i<=x2;i++) for (j=LastY;j<=y2;j++)
        {
            sq=game::Square(i,j);
            if (skn) sq->gs=GROUND_2;
            if (cid)
            {
                sq->R=R;
                sq->G=G;
                sq->B=B;
            }
        }

        //Next
        area= area->NextSiblingElement("area");
    }


}

//==========================
// Default level, in case there were errors:
//
void LevelPack::Default(const char* msg)
{
    defmode=true;
    n=1; //one level
    Name = "Level File could not be open";
    Author = "Unknown";
    Desc = "An error happened while attempting to load the file";
    
    LevelPack::CurrentLevelTitle= "Xye - Could not open level file";
    SDL_WM_SetCaption(LevelPack::CurrentLevelTitle.c_str(),0);
    
    const char* err[5]=
    {"###.##..##...##..##.",
     "#...#.#.#.#.#..#.#.#",
     "##..##..##..#..#.##.",
     "#...#.#.#.#.#..#.#.#",
     "###.#.#.#.#..##..#.#"};
    for (int i=0;i<5; i++)
        for (int j=0;j<20; j++)
        {
            if(err[i][j]=='#') {
                block* b =  new block(game::Square(5+j,12-i) , B_RED, false);
            }
        }
    
    
    
    
    LevelPack::SetLevelBye("Sorry");
    hint::SetGlobalHint(Desc.c_str());    
    LevelPack::Solution="";
    
    palette::Clear();
    if(msg!=NULL) 
        LevelError = msg;
    

    
    
    game::XYE= new xye(game::Square(0,0));
}


void LevelPack::Error(const char * msg)
{
    Default(msg);
}

//===========================================================================================
// Let's load a level
//
void LoadLevel(TiXmlElement* level)
{
TiXmlElement* pEChild;

    LastX=0;
    LastY=0;

    //Part 1: Load the level's title:
    pEChild= level->FirstChildElement("title");
    if (pEChild != NULL)
    {
        const char *cntd=pEChild->GetText();
        if (cntd != NULL)
        {
            LevelPack::CurrentLevelTitle= "Xye - "+string(cntd);
        }
        else
        {
            LevelPack::CurrentLevelTitle= "Xye";
        }
        SDL_WM_SetCaption(LevelPack::CurrentLevelTitle.c_str(),0);

    }
    else
        SDL_WM_SetCaption("Xye",0);


    //Part 2: Load the level's bye message if any:
    LevelPack::SetLevelBye("");
    pEChild= level->FirstChildElement("bye");
    if (pEChild)
    {
        const char *cntd=pEChild->GetText();
        if (cntd != NULL)
            LevelPack::SetLevelBye(cntd);
    }

    //Part 3: Load the level's hint message if any:
    pEChild= level->FirstChildElement("hint");
    if (pEChild)
    {
        const char *cntd=pEChild->GetText();
        if (cntd!=NULL)
            hint::SetGlobalHint(cntd);
    }


    // Load the level's solution if any
    pEChild= level->FirstChildElement("solution");
    LevelPack::Solution="";
    if (pEChild)
    {
        const char *cntd=pEChild->GetText();
        if (cntd!=NULL)
            LevelPack::Solution=cntd;
    }


    //First of all, load the palette, this makes writting the xml file easier so you don't set the same
    //color N times, and you just set it once, it also makes the xml files smaller.

    //And people can just change the color value of multiple elements without effort.
    palette::Clear();
    pEChild= level->FirstChildElement("palette");
    while (pEChild)
    {
        LoadPalette(pEChild);

        pEChild= pEChild->NextSiblingElement("palette");
    }

    //Load default values, these are really helpful:
    pEChild= level->FirstChildElement("default");
    while (pEChild)
    {
        LoadDefaults(pEChild);

        pEChild= pEChild->NextSiblingElement("default");
    }



    //Now load the floor:

    pEChild= level->FirstChildElement("floor");
    while (pEChild)
    {

        LoadFloor(pEChild);

        pEChild= pEChild->NextSiblingElement("floor");
    }

    bool KyeLoaded=false;
    //kyeformat allows to embed .kye levels in .xye files
    if (pEChild= ( level->FirstChildElement("kyeformat")))
    {
        KyeLoaded=LoadKyeFormat(pEChild);
    }



    //Yep, multiple ground and normal tags might be present and they work correctly
    //Ground objects are loaded first so if a normal object is created where a ground object is
    //supposed to be it already triggers its events

    //The separation between ground objects and 'normal' objects was an old idea that turned out
    // to be extremely lame as for the level format.

    //<compatibility>
    pEChild= ( level->FirstChildElement("ground"));
    while (pEChild)
    {
        LoadObjects(pEChild);
        pEChild= ( pEChild->NextSiblingElement("ground"));
    }
    pEChild= ( level->FirstChildElement("normal"));
    while (pEChild)
    {
        LoadObjects(pEChild);
        pEChild= ( pEChild->NextSiblingElement("normal"));
    }
    //</compatibility>
    pEChild= ( level->FirstChildElement("objects"));
    while (pEChild)
    {
        LoadObjects(pEChild);
        pEChild= ( pEChild->NextSiblingElement("objects"));
    }


    //There can only be one Xye
    pEChild= ( level->FirstChildElement("xye"));
    if (pEChild)
        Load_Xye(pEChild,KyeLoaded);
    else if (! KyeLoaded)
        game::XYE= new xye(game::Square(0,0));



}

//returns true if kye was found:
int NextLine(const char* c, int i)
{
    if (i<0) return -1;
    while (c[i]!='\n') if (c[i]=='\0') return -1;
    else i++;
    if (c[i+1]=='\r') return i+2;
    return i+1;

}


bool LoadKyeFormat(TiXmlElement* kf)
{

    KyeLevel K(true);
    const char* tx=kf->GetText();
    if(tx==NULL) tx="";
    
    int offset=0,m;
    kf->QueryIntAttribute("offset",&offset);

    int L=strlen(tx),k=NextLine(tx,0); //ignore first line;

    int i=0,j=19;
    for (i=0;i<offset;i++) k=NextLine(tx,k);
    i=0;

    if ((k==-1) || (tx[k]==0)) return false;

    //we have to convert the text into kye level data:

    while ((k<L) && (j>=0))
    {
        for (m=0;m<offset;m++) k++;
        while ((i<30) && (tx[k]!=13))
        {
            K.data[i][j]=tx[k];
            i++;
            k++;
        }
        while (i<30)
        {
            K.data[i][j]=' ';
            i++;
        }
        j--;i=0;
        if (tx[k]=='\n')
        {
            k++;
            if (tx[k]=='\r') k++;
        }
    }
    while (j>=0)
    {
        i=0;
        while (i<30)
        {
            K.data[i][j]=' ';
            i++;
        }
        j--;
    }
    K.Load();
    return (K.FoundKye);




}



/******* The palette psuedo class ***/
//
// This is a hash table that takes integers instead of strings, to make a super dynamic array that
// gets kind of slow with big sizes but works for any index in the integer range.
//
colorentry* palette::ls[PALETTE_BASE_SIZE];
bool palette::ready=false;

void palette::Clear()
{
    int i;
    colorentry* tm,*c;
    for (i=0;i<PALETTE_BASE_SIZE;i++)
        if (ready)
        {
            tm=ls[i];
            while (tm)
            {
                c=tm;
                tm=tm->next;
                delete (c);
            }
            ls[i]=NULL;
        }
        else
        {
            ls[i]=NULL;
            ready=true;
        }

}

//
colorentry* palette::GetEntry(int id, bool create)
{
    unsigned int indx=id % PALETTE_BASE_SIZE;
    colorentry* tm=ls[indx], *last=NULL;
    while (tm)
    {
        if (tm->id==id) return (tm);
        last=tm;
        tm=tm->next;
    }
    if (create)
    {
        tm=new colorentry;
        if (last)
            last->next=tm;
        else
            ls[indx]=tm;
        tm->next=NULL;
        tm->color=0xFFFFFFFF;
        tm->id=id;
        return(tm);
    }
 return (NULL);
}

void palette::SetColor(int id, unsigned int color, palette_mode pm)
{
    colorentry *ce=GetEntry(id,true);
    ce->color=color;
    ce->R=255;
    ce->G=255;
    ce->B=255;
    ce->pm = pm;

}

void palette::SetColor(int id, int r,int  g,int b, palette_mode pm)
{
    colorentry *ce=GetEntry(id,true);
    ce->color=  (0xFF000000) | (r << 16) | (g << 8) | (b)  ;
    ce->R=r;
    ce->G=g;
    ce->B=b;
    ce->pm = pm;
}

unsigned int palette::GetColor(int id)
{

    colorentry *ce=GetEntry(id,false);
    if (ce) return ce->color;
    return 0xFFFFFFFF;
}

void palette::GetColor(int id, Uint8 &R, Uint8 &G,Uint8 &B)
{
    colorentry *ce=GetEntry(id,false);
    if (ce)
    {
        R=ce->R;
        G=ce->G;
        B=ce->B;
    }
    else
    {
        R=G=B=255;
    }

}

void palette::GetColor(int id, Uint8 &R, Uint8 &G,Uint8 &B, palette_mode& pm)
{
    colorentry *ce=GetEntry(id,false);
    if (ce)
    {
        R=ce->R;
        G=ce->G;
        B=ce->B;
        pm = ce->pm;
    }
    else
    {
        R=G=B=255;
        pm = PM_MULTIPLY;
    }

}
















