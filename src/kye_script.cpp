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
#include "xye_script.h"
#include "kye_script.h"

/** Class KyeLevelPack start **/
KyeLevel* KyeLevelPack::First;
KyeLevel* KyeLevelPack::Final;
KyeLevel* KyeLevelPack::CurrentLevel;
unsigned int KyeLevelPack::tn;

void getline_xplt(std::ifstream &a, std::string &l)
{
    getline(a,l);
    int L=l.length();
    if ((L>1) && (l[L-1]=='\r'))
    {
        l = l.substr(0,L-1);
    }
}

void KyeLevelPack::Init()
{
    First=Final=NULL;
}

void KyeLevelPack::Clean()
{
    KyeLevel* q;
    while (First)
    {
        q=First;
        First=First->Next;
        delete q;
    }
    First=Final=NULL;
}

void KyeLevelPack::Load(const char* filename, unsigned int ln)
{
    std::string line;

    std::ifstream fl ;

    Clean();
    fl.open(filename,std::ios::in);
    if (! fl.is_open()) {LevelPack::Error("Unable to load level file (.kye) (stream error)"); return; }
    if (fl.eof()) {LevelPack::Error("Level File is empty"); return; }

    //read stuff !
    getline_xplt (fl,line);
    int N,i,j,k;


    if (! TryS2I(line,N))
        {LevelPack::Error("Not a kye file, first line not a number!");return;}
    LevelPack::n=N;


    if (N<=0) {LevelPack::Error("Not really a kye file (Number of levels too low <=0)");return;}
    if (fl.eof()) {LevelPack::Error("Unexpected end of file");return;}

    //now we know the file is *supposed* to have N files;

    KyeLevel* current,*ql;
    First=current=new KyeLevel();
    current->Prev=NULL;
    tn=0;
    while(N>0)
    {
        //Try to read N levels.

        getline_xplt (fl,current->name);
        if (fl.eof()) {LevelPack::Error("Unexpected end of file");return;}
        getline_xplt (fl,current->lhint);
        if (fl.eof()) {LevelPack::Error("Unexpected end of file");return;}
        getline_xplt (fl,current->bye);
        if (fl.eof()) {LevelPack::Error("Unexpected end of file");return;}

        //Now it should get interesting, we will read 20 lines that form the level
        //and parse them into the 2d array.

        for (j=19;j>=0;j--)
        {
            getline_xplt (fl,line);
            if ((j>0) && fl.eof()) {LevelPack::Error("Unexpected end of file");return;}
            k=line.length();
            k= (k>30)?30:k;
            for (i=0;i<k;i++)
                current->data[i][j]=line[i];

            for (i=k;i<30;i++)
                current->data[i][j]=' '; //just in case.

        }
        Final=current;
        N--;
        if (N>0)
        {
            if (fl.eof()) {LevelPack::Error("Invalid Kye file (missing level?)");return;}
            ql=new KyeLevel();
            ql->Prev=current;
            current->Next=ql;
            current=ql;
        }
        else
            current->Next=NULL;
        tn++;
    }

    fl.close();


    //The result is supposed to be a double linked list with all the levels in sequence or a fiasco.
    //There is always a chance a normal text document could be loaded.

    //It should now be *easy* to load a Kye level
    LoadNthLevel(ln);


}

const char* KyeLevelPack::ReadData(const char* path,unsigned int &n )
{
    std::ifstream fl ;
    fl.open(path,std::ios::in);
    if (! fl.is_open()) return ("Unable to open file");
    if (fl.eof())
    {
        fl.close();
        return ("The file is empty");
    }
    std::string line;
    getline_xplt (fl,line);
    int tm=0;
    if (! TryS2I(line,tm))
    {
        fl.close();
        return ("First line is not a number");
    }
    else if (tm<0)
    {
        fl.close();
        return ("Negative number on line 1");
    }
    else n=(unsigned int)(tm);



    //count the lines and verify they match the number of levels.
    unsigned int c=0;
    while (! fl.eof())
    {
        getline_xplt (fl,line);
        c++;
    }

    // A level requires 3 lines + other 20 lines = 23
    c=(unsigned int)(c / 23);
    if (c<n)
    {
        fl.close();
        return "Not enough lines to match level number";
    }



    fl.close();
return NULL;

}

void KyeLevelPack::LoadNthLevel(unsigned int n)
{
    LevelPack::OpenFileLn=n;
    //This is just double linked list iteration.
    if (n==1)
        (CurrentLevel=First)->Load();
    else
    {
        KyeLevel* tm=First;
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

void KyeLevelPack::Restart()
{
    CurrentLevel->Load();
}

void KyeLevelPack::Next()
{
    LevelPack::OpenFileLn++;
    KyeLevel* k=CurrentLevel->Next;
    if (! k)
    {
        (CurrentLevel=First)->Load();
        LevelPack::OpenFileLn=1;
    }
    else (CurrentLevel=k)->Load();
}

void KyeLevelPack::Last()
{
    LevelPack::OpenFileLn--;
    KyeLevel* k=CurrentLevel->Prev;
    if (! k)
    {
        (CurrentLevel=Final)->Load();
        LevelPack::OpenFileLn=tn;
    }
    else (CurrentLevel=k)->Load();
}

bool KyeLevelPack::HasNext()
{
    return (CurrentLevel->Next != NULL);

}

bool KyeLevelPack::HasLast()
{
    return (CurrentLevel->Prev != NULL);
}


/** Class KyeLevelPack end **/

void LoadKyeWall(char t, unsigned  char i, unsigned char j)
{
    wall* wl=new wall(game::Square(i,j));
    wl->SetRoundCorners(
     (t=='7') || (t=='4') || (t=='8'),
     (t=='1') || (t=='2') || (t=='4'),
     (t=='3') || (t=='2') || (t=='6'),
     (t=='9') || (t=='6') || (t=='8'));
}

void LoadXyeWall_R(unsigned char i, unsigned  char j)
{
    wall* wl=new wall(game::Square(i,j),4);
}

void LoadKyeTimer(unsigned  char x, unsigned char y,unsigned char tm)
{
    number* t=new number(game::Square(x,y),B_YELLOW,tm,false);
}

void LoadKyeEarth(unsigned  char x, unsigned char y)
{
    earth* e=new earth(game::Square(x,y));
}

void LoadKyeGem(unsigned char x,unsigned char y)
{
    gem* g=new gem(game::Square(x,y),B_BLUE);
}

void LoadXyeEmerald(unsigned char x,unsigned char y)
{
    gem* g=new gem(game::Square(x,y),B_GREEN);
}

void LoadXyeGemBlock(unsigned char x,unsigned char y)
{
    gemblock* g=new gemblock(game::Square(x,y),B_GREEN);
}

void LoadXyeBlockDoor(unsigned char x,unsigned char y,bool trap)
{
    blockdoor* g=new blockdoor(game::Square(x,y),trap,true,B_YELLOW);
}


void LoadXyeMarked(unsigned char x,unsigned char y)
{
    marked* g=new marked(game::Square(x,y),B_YELLOW);
}


void LoadKyeBlock(unsigned  char x, unsigned char y,bool round, bool fromxye)
{
    block* b=new block(game::Square(x,y), /*(fromxye?B_YELLOW: B_GREEN)*/ B_YELLOW,round);
    
    
    
}

void LoadKyeBeast(unsigned char x, unsigned char y, btype B)
{
    beast* b=new beast(game::Square(x,y),B,D_UP);
}

void LoadKyeSticky(unsigned char x, unsigned char y, bool horz)
{
    magnetic* m=new magnetic(game::Square(x,y),T_MAGNET,horz);
}

void LoadSKyeSticky(unsigned char x, unsigned char y, bool horz)
{
    magnetic* m=new magnetic(game::Square(x,y),T_STICKY,horz);
}

void LoadXyeAntiMagnet(unsigned char x, unsigned char y, bool horz)
{
    magnetic* m=new magnetic(game::Square(x,y),T_ANTIMAGNET,horz);
}

void LoadSKyeBomb(unsigned char x, unsigned char y)
{
    surprise* m=new surprise(game::Square(x,y),B_RED,false);
}

void LoadSKyePit(unsigned char x, unsigned char y)
{
    pit* m=new pit(game::Square(x,y));
}

void LoadKyeBlacky(unsigned char x, unsigned char y)
{
    dangerous* b=new dangerous(game::Square(x,y),OT_BLACKHOLE);
}

void LoadXyeMine(unsigned char x, unsigned char y)
{
    dangerous* b=new dangerous(game::Square(x,y),OT_MINE);
}


void LoadKyeArrow(unsigned char x, unsigned char y, bool round, edir d)
{
    arrow *a=new arrow(game::Square(x,y),B_YELLOW,d,round);
}

void LoadKye3Teleport(unsigned char x, unsigned char y, edir d)
{
    teleport *a=new teleport(game::Square(x,y),d);
}


void LoadKyeBouncer(unsigned char x, unsigned char y, edir d)
{
    impacter *i=new impacter(game::Square(x,y),B_YELLOW,d);
}

void LoadKyeClockerAclocker(unsigned char x, unsigned char y,bool clockwise)
{
    turner *t=new turner(game::Square(x,y),B_YELLOW,clockwise,false);
}

void LoadKyeAuto(unsigned char x, unsigned char y,bool round)
{
    autoarrow* bc=new autoarrow(game::SquareN(x,y),B_YELLOW,autoarrow::GetDefaultEdirByColumn(x),round);

}

void LoadKyeOneWay(unsigned char x, unsigned char y,edir dr)
{
    bool u=false,r=false,d=false,l=false;
    switch(dr)
    {
        case(D_LEFT): l=true; break;
        case(D_RIGHT): r=true; break;
        case(D_DOWN): d=true; break;
        default: u=true;
   }

   tdoor* td= new tdoor(game::SquareN(x,y), (l || r)? td_HORZ:td_VERT  ,u,r,d,l);

}

void LoadXyeDotBlock(unsigned char x, unsigned char y,bool round)
{
    lowdensity* l=new lowdensity(game::SquareN(x,y),B_YELLOW,round);
}

void LoadXyeSurprise(unsigned char x, unsigned char y,bool round)
{
    surprise* l=new surprise(game::SquareN(x,y),B_BLUE,round);
}

void LoadXyeToggle(unsigned char x, unsigned char y,bool plus)
{
    toggle* l=new toggle(game::SquareN(x,y),B_YELLOW,false,! plus);
}

void LoadXyeBot(unsigned char x, unsigned char y)
{
    roboxye* l=new roboxye(game::SquareN(x,y));
}

/** Class KyeLevel start **/
void KyeLevel::SetGameCaption()
{
    int L=name.length();
    string title = "Xye - "+name;
    LevelPack::CurrentLevelTitle=title.c_str();
    SDL_WM_SetCaption(title.c_str(),0);

}
void KyeLevel::Load()
{

    if (! FromXyeLevel)
    {
        SetGameCaption();
        LevelPack::SetLevelBye( bye.c_str() );
        LevelPack::Solution="";
    }

    FoundKye=false;

    //We have a complete 2D array full with characters, let's convert it to a level.

    unsigned char kx,ky,i,j;
    for (j=0;j<20;j++) for (i=0;i<30;i++) switch(data[i][j])
    {
        case('1'): case('2'):case('3'):case('4'):case('5'):case('6'):case('7'):case('8'):case('9'):
            LoadKyeWall(data[i][j],i,j);
            break;
        case('e'): LoadKyeEarth(i,j); break;
        case('b'): LoadKyeBlock(i,j,false, FromXyeLevel); break;
        case('B'): LoadKyeBlock(i,j,true, FromXyeLevel); break;
        case('*'): LoadKyeGem(i,j); break;

        case ('E'): LoadKyeBeast(i,j,BT_GNASHER); break;
        case ('C'): LoadKyeBeast(i,j,BT_BLOB); break;
        case ('~'): LoadKyeBeast(i,j,BT_VIRUS); break;
        case ('['): LoadKyeBeast(i,j,BT_SPIKE); break;
        case ('T'): LoadKyeBeast(i,j,BT_TWISTER); break;
        case ('/'): LoadKyeBeast(i,j,BT_DARD); break;


        case ('s'): LoadKyeSticky(i,j,false); break;
        case ('S'): LoadKyeSticky(i,j,true); break;

        case ('}'): LoadKyeTimer(i,j,3); break;
        case ('|'): LoadKyeTimer(i,j,4); break;
        case ('{'): LoadKyeTimer(i,j,5); break;
        case ('z'): LoadKyeTimer(i,j,6); break;
        case ('y'): LoadKyeTimer(i,j,7); break;
        case ('x'): LoadKyeTimer(i,j,8); break;
        case ('w'): LoadKyeTimer(i,j,9); break;
        case ('H'): LoadKyeBlacky(i,j); break;

        //Non-round Arrows:
        case ('l'): LoadKyeArrow(i,j,false,D_LEFT); break;
        case ('r'): LoadKyeArrow(i,j,false,D_RIGHT); break;
        case ('u'): LoadKyeArrow(i,j,false,D_UP); break;
        case ('d'): LoadKyeArrow(i,j,false,D_DOWN); break;

        //Round Arrows:
        case ('<'): LoadKyeArrow(i,j,true,D_LEFT); break;
        case ('>'): LoadKyeArrow(i,j,true,D_RIGHT); break;
        case ('^'): LoadKyeArrow(i,j,true,D_UP); break;
        case ('v'): LoadKyeArrow(i,j,true,D_DOWN); break;

        //Bouncers:
        case ('L'): LoadKyeBouncer(i,j,D_LEFT); break;
        case ('R'): LoadKyeBouncer(i,j,D_RIGHT); break;
        case ('U'): LoadKyeBouncer(i,j,D_UP); break;
        case ('D'): LoadKyeBouncer(i,j,D_DOWN); break;

        //Auto slider/rocky:
        case ('A'): LoadKyeAuto(i,j,false); break;
        case ('F'): LoadKyeAuto(i,j,true); break;


        case ('a'): LoadKyeClockerAclocker(i,j,true); break;
        case ('c'): LoadKyeClockerAclocker(i,j,false); break;

        case ('h'): LoadKyeOneWay(i,j,D_UP); break;
        case ('g'): LoadKyeOneWay(i,j,D_RIGHT); break;
        case ('f'): LoadKyeOneWay(i,j,D_LEFT); break;
        case ('i'): LoadKyeOneWay(i,j,D_DOWN); break;

        //SKye Additions:
        case ('P'): LoadSKyeSticky(i,j,true); break;
        case ('p'): LoadSKyeSticky(i,j,false); break;
        case ('!'): LoadSKyeBomb(i,j); break;
        case ('O'): LoadSKyePit(i,j); break;

        //Kye 3.0 additions:
        case ('('): LoadKye3Teleport(i,j,D_RIGHT); break;
        case (')'): LoadKye3Teleport(i,j,D_LEFT); break;
        case ('_'): LoadKye3Teleport(i,j,D_UP); break;
        case ('\''): LoadKye3Teleport(i,j,D_DOWN); break;

        //Xye additions:
        case('o'): LoadXyeMine(i,j); break;
        case('$'): LoadXyeEmerald(i,j); break;
        case('@'): LoadXyeGemBlock(i,j); break;
        case('#'): LoadXyeMarked(i,j); break;
        case('%'): LoadXyeBlockDoor(i,j,false); break;
        case('='): LoadXyeBlockDoor(i,j,true); break;

        case ('Q'): LoadXyeAntiMagnet(i,j,true); break;
        case ('q'): LoadXyeAntiMagnet(i,j,false); break;

        case ('.'): LoadXyeDotBlock(i,j,false); break;
        case (','): LoadXyeDotBlock(i,j,true); break;

        case (':'): LoadXyeSurprise(i,j,false); break;
        case(';'): LoadXyeSurprise(i,j,true); break;

        case ('-'): LoadXyeToggle(i,j,false); break;
        case ('+'): LoadXyeToggle(i,j,true); break;

        case ('?'): LoadXyeBot(i,j); break;
        case ('M'): LoadXyeWall_R(i,j); break;

        case('K'):
            FoundKye=true;
            kx=i;
            ky=j;
    }

    char rx,ry;

    if (FoundKye || ! FromXyeLevel)
        game::XYE= new xye(game::Square(kx,ky));

    if ((! FromXyeLevel) && (lhint!=""))
    {
        hint::SetGlobalHint(lhint);
    }


}

/** Class KyeLevel end **/
