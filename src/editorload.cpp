#include "xyedit.h"
#include "tinyxml/xye_tinyxml.h"
#include<iostream>
using std::cout;

boardelement editorload_objects[XYE_HORZ][XYE_VERT];
int editorload_xyex;
int editorload_xyey;

int editorload_portal_x[5][2];
int editorload_portal_y[5][2];

string editor::loadError;

int getElementPosition_lastx = 400;

bool getElementPosition(TiXmlElement *el, int &x , int &y, bool allowSamePos=false)
{
    x=400;
    y=400;
    el->QueryIntAttribute("x",&x);
    if(x==400) {
        x = getElementPosition_lastx;
    } else {
        getElementPosition_lastx = x;
    }
    el->QueryIntAttribute("y",&y);

    if((x<0) || (x>=XYE_HORZ) || (y<0) || (y>=XYE_VERT) )
    {
        cout<<"Wrong coordinates "<<x<<","<<y<<" found in a tag: <"<<el->Value()<<">\n";
        return false;
    }
    if((!allowSamePos) && (editorload_objects[x][y].type!=EDOT_NONE) )
    {
        cout<<"Unable to load two objects in same position: "<<x<<","<<y<<" , tag: <"<<el->Value()<<">\n";
        return false;
    }
    return true;
}


bool editor_LoadWall(TiXmlElement* el)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    int t=0;
    el->QueryIntAttribute("type",&t);
    bool round=false;
    int test=0;
    el->QueryIntAttribute("round1",&test); round=round || test;
    el->QueryIntAttribute("round3",&test); round=round || test;
    el->QueryIntAttribute("round7",&test); round=round || test;
    el->QueryIntAttribute("round9",&test); round=round || test;

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_WALL;
    o.variation = t;
    o.round = round;
    return true;
}


editorcolor getElementColor(TiXmlElement* el)
{
    int t=0;
    el->QueryIntAttribute("nocolor",&t);
    if (t) return EDCO_WHITE;

    const char * ptr=el->Attribute("bc");
    if(! ptr)
    {
        ptr="Y";
        cout << "Notice: using default color for a <"<<el->Value()<<"> element\n";
    }

    string v=ptr;

    if(v=="R") return EDCO_RED;
    else if (v=="G") return EDCO_GREEN;
    else if (v=="B") return EDCO_BLUE;
 return EDCO_YELLOW;
}

int getElementDirection(TiXmlElement* el)
{
    const char * ptr=el->Attribute("dir");
    if(! ptr)
    {
        ptr="D";
        cout << "Notice: using default direction for a <"<<el->Value()<<"> element\n";
    }
    string v=ptr;


    if(v=="U") return EDITORDIRECTION_UP;
    else if(v=="R") return EDITORDIRECTION_RIGHT;
    else if(v=="L") return EDITORDIRECTION_LEFT;
 return EDITORDIRECTION_DOWN;
}


bool editor_LoadGem(TiXmlElement* el)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    editorcolor col=getElementColor(el);

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_GEM;
    o.color = col;
    return true;
}

bool editor_LoadStar(TiXmlElement* el)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_GEM;
    o.color = EDCO_WHITE;
    return true;
}

bool editor_LoadGenRC(TiXmlElement* el,editorobjecttype type, int variation=0)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    int round=false; el->QueryIntAttribute("round",&round);
    editorcolor col=getElementColor(el);

    boardelement &o=editorload_objects[x][y];
    o.type=type;
    o.color = col;
    o.variation=variation;
    o.round=round;
    return true;
}

bool editor_LoadGen(TiXmlElement * el, editorobjecttype type, int variation=0)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    boardelement &o=editorload_objects[x][y];
    o.type=type;
    o.variation=variation;
    return true;
}



bool editor_LoadGenR(TiXmlElement * el, editorobjecttype type, int variation=0, editorcolor color = EDCO_YELLOW)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    int round=false; el->QueryIntAttribute("round",&round);
    boardelement &o=editorload_objects[x][y];
    o.type=type;
    o.color = color;
    o.variation=variation;
    o.round=round;
    return true;
}

bool editor_LoadGenD(TiXmlElement * el, editorobjecttype type, int variation=0)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    int direction = getElementDirection(el);
    boardelement &o=editorload_objects[x][y];
    o.type=type;
    o.direction=direction;
    o.variation=variation;
    return true;
}


bool editor_LoadGenDOpposite(TiXmlElement * el, editorobjecttype type, int variation=0)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    int direction = getElementDirection(el);
    boardelement &o=editorload_objects[x][y];
    o.type=type;

    switch(direction)
    {
        case EDITORDIRECTION_UP: o.direction=EDITORDIRECTION_DOWN; break;
        case EDITORDIRECTION_LEFT: o.direction=EDITORDIRECTION_RIGHT; break;
        case EDITORDIRECTION_RIGHT: o.direction=EDITORDIRECTION_LEFT; break;
        default: o.direction=EDITORDIRECTION_UP; break;
    }

    o.variation=variation;
    return true;
}

bool editor_LoadGenCDR(TiXmlElement * el, editorobjecttype type, int variation=0)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    int round=false; el->QueryIntAttribute("round",&round);
    int direction = getElementDirection(el);
    editorcolor col=getElementColor(el);

    boardelement &o=editorload_objects[x][y];
    o.type=type;
    o.color = col;
    o.variation=variation;
    o.round=round;
    o.direction= direction;
    return true;
}

bool editor_LoadGenCD(TiXmlElement * el, editorobjecttype type)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;

    int direction = getElementDirection(el);
    editorcolor col=getElementColor(el);

    boardelement &o=editorload_objects[x][y];
    o.type=type;
    o.color = col;

    o.direction= direction;
    return true;
}


bool editor_LoadTimer(TiXmlElement * el)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;

    editorcolor col=getElementColor(el);
    int val=0; el->QueryIntAttribute("val",&val);
    int round=0; el->QueryIntAttribute("round",&round);

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_NUMBER;
    o.color=col;
    o.round=round;
    o.variation=val;

    return true;
}


bool editor_LoadToggle(TiXmlElement * el)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;

    editorcolor col=getElementColor(el);
    int off=0; el->QueryIntAttribute("off",&off);
    int round=0; el->QueryIntAttribute("round",&round);


    boardelement &o=editorload_objects[x][y];
    o.round=round;
    o.type=EDOT_SPECIALBLOCKS;
    o.color=col;
    o.variation=2+!off;

    return true;
}

void AssignLargeBLockVarDirFromFlags( Uint8 flags , int &variation, int &direction)
{
    variation = 4;
    direction = EDITORDIRECTION_UP;
    //I can't think of something better, sorry.
    switch(flags)
    {
        case 1:
            variation = 0;
            direction = EDITORDIRECTION_UP;
            break;
        case 2:
            variation = 0;
            direction = EDITORDIRECTION_RIGHT;
            break;
        case 4:
            variation = 0;
            direction = EDITORDIRECTION_DOWN;
            break;
        case 8:
            variation = 0;
            direction = EDITORDIRECTION_LEFT;
            break;

        case 3:
            variation = 1;
            direction = EDITORDIRECTION_UP;
            break;
        case /*0b0110*/ 6:
            variation = 1;
            direction = EDITORDIRECTION_RIGHT;
            break;
        case /*0b1100*/ 12:
            variation = 1;
            direction = EDITORDIRECTION_DOWN;
            break;
        case /*0b1001*/ 9:
            variation = 1;
            direction = EDITORDIRECTION_LEFT;
            break;

        case /*0b0101*/ 5:
            variation = 2;
            direction = EDITORDIRECTION_UP;
            break;
        case /*0b1010*/ 10:
            variation = 2;
            direction = EDITORDIRECTION_RIGHT;
            break;

        case /*0b1110*/ 14:
            variation = 3;
            direction = EDITORDIRECTION_DOWN;
            break;
        case /*0b1101*/ 13:
            variation = 3;
            direction = EDITORDIRECTION_LEFT;
            break;
        case /*0b1011*/ 11:
            variation = 3;
            direction = EDITORDIRECTION_UP;
            break;
        case/* 0b0111*/ 7:
            variation = 3;
            direction = EDITORDIRECTION_RIGHT;
            break;


        case /*0b1111*/ 15:
            variation = 4;
            direction = EDITORDIRECTION_UP;
            break;

    }
}

bool editor_LoadLargeBlock(TiXmlElement * el)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;

    editorcolor col=getElementColor(el);

    const char * ptr=el->Attribute("sharededges");
    if(! ptr)
    {
        ptr="UDRL";
        cout << "Notice: using default edges for a large block part element\n";
    }
    string v=ptr;
    Uint8 flags = 0;

    const char * dirs = "URDL";
    for (int i=0; i<v.length(); i++)
        for (int j=0; j<4; j++)
            if(dirs[j] == v[i])
                flags |= (1<<j);

    boardelement &o=editorload_objects[x][y];
    o.round=false;
    o.type=EDOT_LARGEBLOCK;
    o.color=col;
    AssignLargeBLockVarDirFromFlags( flags , o.variation, o.direction);

    return true;
}

bool editor_LoadPortal(TiXmlElement * el)
{
    int &x = getElementPosition_lastx;
    int y=-1,tx=-1,ty=-1;
    el->QueryIntAttribute("x", &x);
    el->QueryIntAttribute("y", &y);

    el->QueryIntAttribute("targetx", &tx);
    el->QueryIntAttribute("targety", &ty);
    if( (tx<0) || (tx>=XYE_HORZ) || (ty<0) || (ty>=XYE_VERT) )
    {
        tx=ty=0;
        cout<< "Notice: A <portal> tag had strange target point data, ignored"<<endl;
    }
    if( (x<0) || (x>=XYE_HORZ) || (y<0) || (y>=XYE_VERT) )
    {
        cout <<"Bad <portal> position "<<endl;
        return false;
    }


    int defcolor=-1;
    el->QueryIntAttribute("defcolor", &defcolor);
    if ( (defcolor<0) || (defcolor>=5) )
    {
        defcolor  = 0;
        cout<< "Notice: A <portal> tag had a strange/missing defcolor value. This could indicate that the level was not made by this version of the editor."<<endl;
    }
    if( editorload_portal_x[defcolor][0] == -1)
    {
        boardelement &o=editorload_objects[x][y];
        o.type = EDOT_PORTAL;
        o.variation = 0;
        o.color=(editorcolor)(defcolor);

        boardelement &o2=editorload_objects[tx][ty];
        o2.type = EDOT_PORTAL;
        o2.variation = 2;
        o2.color=(editorcolor)(defcolor);

        editorload_portal_x[defcolor][0] = x;
        editorload_portal_y[defcolor][0] = y;
        editorload_portal_x[defcolor][1] = tx;
        editorload_portal_y[defcolor][1] = ty;

    }
    else //secondary!?
    {
        boardelement &o=editorload_objects[x][y];
        o.type = EDOT_PORTAL;
        o.variation = 1;
        o.color=(editorcolor)(defcolor);
        editorload_portal_x[defcolor][1] = x;
        editorload_portal_y[defcolor][1] = y;


    }


    return true;
}



bool editor_LoadBeast(TiXmlElement * el)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;

    int direction = getElementDirection(el);
    int kind=0; el->QueryIntAttribute("kind",&kind);

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_BEAST;
    o.variation=kind;


    o.direction= direction;
    return true;
}


bool editor_LoadMagnet(TiXmlElement * el)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;

    int variation=0; el->QueryIntAttribute("kind",&variation);
    int direction;

    int horz=0; el->QueryIntAttribute("horz",&horz);
    if (horz) direction = EDITORDIRECTION_LEFT;
    else direction = EDITORDIRECTION_UP;


    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_MAGNET;
    o.variation=variation;
    o.direction= direction;
    return true;
}

bool editor_LoadGenC(TiXmlElement * el, editorobjecttype type, int variation=0)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    editorcolor col=getElementColor(el);

    boardelement &o=editorload_objects[x][y];
    o.type=type;
    o.variation=variation;
    o.color = col;
    return true;
}


bool editor_LoadRattler(TiXmlElement* el)
{
    if(el->FirstChildElement()!=NULL)
    {
        cout << "Editor is unable to edit this file, found <rattler> with body elements.\n";
        return false;
    }
    int x,y; if(!getElementPosition(el,x,y)) return false;
    int grow=0; el->QueryIntAttribute("grow",&grow);

    boardelement &o=editorload_objects[x][y];
    o.direction = getElementDirection(el);
    o.type=EDOT_RATTLERHEAD;
    o.variation=grow;


    return true;
}


bool editor_LoadEarth(TiXmlElement* el)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    int round=false; el->QueryIntAttribute("round",&round);

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_EARTH;
    o.round = (int)round;
    return true;
}

bool editor_LoadColorDoor(TiXmlElement* el, int closedvariation)
{
    int x,y; if(!getElementPosition(el,x,y)) return false;
    int open=0; el->QueryIntAttribute("open",&open);

    boardelement &o=editorload_objects[x][y];
    o.color=getElementColor(el);
    o.variation = closedvariation+open;
    o.type=EDOT_COLORSYSTEM;
    return true;
}

bool editor_LoadFactory(TiXmlElement* el)
{
    int x,y,kind=0; if(!getElementPosition(el,x,y)) return false;
    el->QueryIntAttribute("kind",&kind);
    int round=false; el->QueryIntAttribute("round",&round);
    boardelement &o=editorload_objects[x][y];
    o.direction = getElementDirection(el);
    o.color = getElementColor(el);
    o.round = round;
    switch (kind)
    {
        case 0: case 1: case 2:
            o.variation = kind;
            o.type = EDOT_COLORFACTORY;
            break;
        case 4:
            o.variation = 3;
            o.type = EDOT_COLORFACTORY;
            break;
        case 8:
            o.variation = 4;
            o.type = EDOT_COLORFACTORY;
            break;
        case 7:
            o.variation = 14;
            o.type = EDOT_DANGERFACTORY;
            break;
        case 6:
            o.variation = 15;
            o.type = EDOT_DANGERFACTORY;
            break;
        case 3:
            o.variation = 16;
            o.type = EDOT_DANGERFACTORY;
            break;
        default:
            el->QueryIntAttribute("beastkind", &o.variation);
            o.type = EDOT_DANGERFACTORY;
    }
    return true;

}

bool editor_LoadBlock(TiXmlElement* el)
{
    if(! editor_LoadGenRC(el,EDOT_BLOCK))
    {
        //handle special block above marked aread case.
        int x, y;
        if(! getElementPosition(el, x , y, true) ) return false;
        boardelement &o1=editorload_objects[x][y];
        if((o1.type != EDOT_COLORSYSTEM) || (o1.variation!=4))
            return false;
        o1.variation = 6;
        
    }
    return true;
}

bool editor_LoadWildCardBlock(TiXmlElement* el)
{
    if(! editor_LoadGenR(el,EDOT_BLOCK,0, EDCO_WILD    ))
    {
        //handle special block above marked aread case.
        int x, y;
        if(! getElementPosition(el, x , y, true) ) return false;
        boardelement &o1=editorload_objects[x][y];
        if((o1.type != EDOT_COLORSYSTEM) || (o1.variation!=4))
            return false;
        o1.variation = 7;
        
    }
    return true;
}


bool editor_LoadObjects(TiXmlElement* el)
{
    TiXmlElement * ch=el->FirstChildElement();

    while(ch!=NULL)
    {
        string v=ch->Value();
        if (v=="wall")     { if (! editor_LoadWall(ch)) return false;}
        else if (v=="gem")    { if (! editor_LoadGem(ch)) return false;}
        else if (v=="block") { if (! editor_LoadBlock(ch) ) return false;}

        else if (v=="clocker") { if (! editor_LoadGenRC(ch,EDOT_TURNER)) return false;}
        else if (v=="aclocker") { if (! editor_LoadGenRC(ch,EDOT_TURNER,1)) return false;}

        else if (v=="arrow") { if (! editor_LoadGenCDR(ch,EDOT_SPECIALBLOCKS,0)) return false; }
        else if (v=="scroll") { if (! editor_LoadGenCDR(ch,EDOT_SPECIALBLOCKS,1)) return false; }

        else if (v=="auto") { if (! editor_LoadGenCDR(ch,EDOT_ARROWMAKER,0)) return false; }
        else if (v=="sniper") { if (! editor_LoadGenCDR(ch,EDOT_ARROWMAKER,2)) return false; }
        else if (v=="filler") { if (! editor_LoadGenCDR(ch,EDOT_ARROWMAKER,1)) return false; }

        else if (v=="pusher") { if (! editor_LoadGenCD(ch,EDOT_PUSHER)) return false; }
        else if (v=="earth") { if (! editor_LoadEarth(ch)) return false;}
        else if (v=="gemblock") { if (! editor_LoadGenC(ch,EDOT_GEMBLOCK)) return false; }
        else if (v=="magnet") { if (! editor_LoadMagnet(ch)) return false; }
        else if (v=="blacky") { if (! editor_LoadGen(ch,EDOT_HAZARD,0)) return false; }
        else if (v=="mine") { if (! editor_LoadGen(ch,EDOT_HAZARD,1)) return false; }
        else if (v=="beast") { if (! editor_LoadBeast(ch)) return false; }

        else if (v=="teleport") { if (! editor_LoadGenD(ch,EDOT_TELEPORT)) return false; }

        else if (v=="bot")  { if (! editor_LoadGen(ch,EDOT_BOT,0)) return false; }
        else if (v=="star")  { if (! editor_LoadStar(ch)) return false;}
        else if (v=="rfood")  { if (! editor_LoadGen(ch,EDOT_FOOD,0)) return false; }
        else if (v=="wild")  { if (! editor_LoadWildCardBlock(ch) ) return false; }
        else if (v=="metalblock")  { if (! editor_LoadGenR(ch,EDOT_BLOCK,0 , EDCO_METAL )) return false; }
        else if (v=="window")  { if (! editor_LoadGenC(ch,EDOT_COLORSYSTEM,5)) return false; }
        else if (v=="lock")  { if (! editor_LoadGenC(ch,EDOT_KEYSYSTEM,1)) return false; }
        else if (v=="key")  { if (! editor_LoadGenC(ch,EDOT_KEYSYSTEM,0)) return false; }

        else if (v=="timer")  { if (! editor_LoadTimer(ch)) return false; }

        else if (v=="toggle") { if (! editor_LoadToggle(ch)) return false; }

        else if (v=="lblock") { if (! editor_LoadGenRC(ch,EDOT_SPECIALBLOCKS,4)) return false;}
        else if (v=="surprise") { if (! editor_LoadGenRC(ch,EDOT_SPECIALBLOCKS,5)) return false;}

        else if (v=="rattler") { if (! editor_LoadRattler(ch)) return false; }

        else if (v=="largeblockpart") { if (! editor_LoadLargeBlock(ch)) return false; }
        else if (v=="portal")         { if (! editor_LoadPortal(ch)) return false; }
        else if (v=="factory")         { if (! editor_LoadFactory(ch)) return false; }





        else
        {
            cout << "Editor-incompatible object: "<<v<<"\n";
            return false;
        }

        ch=ch->NextSiblingElement();
    }
 return true;
}
bool editor_LoadGround(TiXmlElement* el)
{
    TiXmlElement * ch=el->FirstChildElement();

    while(ch!=NULL)
    {
        string v=ch->Value();
        if (v=="pit")     { if (! editor_LoadGen(ch,EDOT_HAZARD,2)) return false;}
        else if (v=="firepad")     { if (! editor_LoadGen(ch,EDOT_FIREPAD,0)) return false;}
        else if (v=="force") { if (! editor_LoadGenD(ch,EDOT_ONEDIRECTION,1)) return false; }
        else if (v=="oneway") { if (! editor_LoadGenDOpposite(ch,EDOT_ONEDIRECTION,0)) return false; }

        else if (v=="blockdoor") { if (! editor_LoadColorDoor(ch, 0)) return false; }
        else if (v=="blocktrap") { if (! editor_LoadColorDoor(ch, 2)) return false; }
        else if (v=="marked") { if (! editor_LoadGenC(ch,EDOT_COLORSYSTEM,4)) return false; }

        else
        {
            cout << "Editor-incompatible object: "<<v<<"\n";
            return false;
        }

        ch=ch->NextSiblingElement();
    }
 return true;
}


bool editor_LoadXye(TiXmlElement* el)
{
    int x,y; if (!getElementPosition(el,x,y)) return false;
    int lives=1;
    el->QueryIntAttribute("lives",&lives);


    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_XYE;
    o.variation= lives-1;

    editorload_xyex=x;
    editorload_xyey=y;
 return true;
}


bool editor::load()
{
    TiXmlDocument  fil(filename.c_str());
    if (fil.LoadFile())
    {
        string lauthor="xyedit",ldescription="Generated by xyedit";
        string ltitle=filename_name,lhint="",lbye="",lsolution="";
        cout<<"Correctly loaded XML file.\n";

        TiXmlElement* pack, *el, *level;



        pack=fil.FirstChildElement("pack");
        if (pack==NULL)
        {
            cout<<"Not a valid .xye file (missing <pack>)\n";
            loadError="The level file is invalid (missing <pack> tag)";
            return false;
        }

        el=pack->FirstChildElement("author");
        if (el!=NULL)
        {
            const char* gt=el->GetText();
            lauthor= (gt!=NULL ? gt : "");
            
        }

        el=pack->FirstChildElement("description");
        if (el!=NULL)
        {
            const char* gt=el->GetText();
            ldescription= (gt!=NULL ? gt : "");
        }

        int n=0;
        level=el=pack->FirstChildElement("level");
        while(el!=NULL)
        {
            n++;
            el=el->NextSiblingElement("level");
        }
        if(n==0)
        {
            cout<<"Not a valid .xye file (missing <level>)\n";
            loadError="The level file is invalid (missing <level> tag)";
            return false;
        }
        if (n>1)
        {
            cout<<"Unable to edit files with multiple levels.\n";
            loadError="The level file contains multiple levels, this version of the editor does not support multiple levels.";
            return false;
        }
        cout<<n<<" level found.\n";

        int i,j;
        editorload_xyex=-1;

        for (int i=0; i<5; i++)
            for (int j=0; j<2;j++)
                editorload_portal_x[i][j] = editorload_portal_y[i][j] = -1;

        for (i=0;i<XYE_HORZ;i++)for (j=0;j<XYE_VERT;j++) editorload_objects[i][j].type=EDOT_NONE;

        loadError="Found tags and/or attributes that are not recognized by the current version.";
        el=level->FirstChildElement();


        while (el!=NULL)
        {
            string v=el->Value();
            if (v=="ground")
            {
                if(!editor_LoadGround(el))
                    return false;
            }
            else if (v=="objects")
            {
                if(!editor_LoadObjects(el))
                    return false;
            }
            else if (v=="xye")
            {
                if(! editor_LoadXye(el))
                {
                    loadError="Invalid <xye> tag in the level file.";
                    return false;
                }
            }
            else if (v=="title")
            {
                const char* gt=el->GetText();
                ltitle= ( (gt!=NULL) ? gt : "");
            }
            else if (v=="hint")
            {
                const char* gt=el->GetText();
                lhint= ( (gt!=NULL) ? gt : "");
            }
            else if (v=="bye")
            {
                const char* gt=el->GetText();
                lbye= ( (gt!=NULL) ? gt : "");
            }
            else if (v=="solution")
            {
                const char* gt=el->GetText();
                lsolution= ( (gt!=NULL) ? gt : "");
            }
            else
            {
                cout << "Editor-incompatible <pack> child: "<<v<<"\n";
                loadError="Found a tag "+v+" that is incompatible with the level editor.";
                return false;
            }
            el=el->NextSiblingElement();
        }

        if(editorload_xyex==-1)
        {
            cout << "Notice: Unable to find xye in the level file.\n";
        }
        cout << "File loaded succesfully.\n";

        for (i=0;i<XYE_HORZ;i++)for (j=0;j<XYE_VERT;j++)
        {
            editor::board->objects[i][XYE_VERT-j-1]=editorload_objects[i][j];
        }
        for (int i=0; i<5; i++)
            for (int j=0; j<2; j++)
                editor::board->portal_x[i][j] = editorload_portal_x[i][j],
                editor::board->portal_y[i][j] = XYE_VERT-editorload_portal_y[i][j]-1;

        editor::board->xye_x = editorload_xyex;
        editor::board->xye_y = XYE_VERT-editorload_xyey-1;
        editor::board->hint = lhint;
        editor::board->title = ltitle;
        editor::board->description = ldescription;
        editor::board->author = lauthor;
        editor::board->bye = lbye;
        editor::board->solution = lsolution;


        return true;


    }
    else
    {
        cout<<"Unable to load level file.\n";
        cout<<"\t"<<fil.ErrorDesc()<<"\n";

        if (fil.ErrorId()==2)
        {
            if(filename_name=="editortest.xye") return true;
            loadError="Xyedit was unable to open the level file.";

        }
        else
            loadError="There are errors in the XML structure of the file: "+string(fil.ErrorDesc());
        return false;
    }
    return true;
}
