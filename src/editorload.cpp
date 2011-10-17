#include "xyedit.h"
#include "xye.h"
#include "tinyxml/xye_tinyxml.h"
#include "kye_script.h"
#include "xye_script.h"
#include<iostream>
#include<algorithm>
#include<map>
using std::map;
using std::cout;

DefaultColorData editorload_colors[TOTAL_EDITOR_COLOR_OPTIONS];
boardelement editorload_objects[XYE_HORZ][XYE_VERT];
int editorload_xyex;
int editorload_xyey;

int editorload_portal_x[5][2];
int editorload_portal_y[5][2];
int editorload_defaultwall = 0;

string editor::loadError;

int getElementPosition_lastx = 400;
int getElementPosition_lasty = 400;

vector< pair<int,int> > errorPositions;

bool isObjectTypeGround(editorobjecttype ed)
{
    if  (ed==EDOT_NONE || ed==EDOT_ERROR ) {
        return false;
    }
    switch(ed) {
    case EDOT_ONEDIRECTION:
    case EDOT_PORTAL:
    case EDOT_COLORSYSTEM:
    case EDOT_FIREPAD:
    case EDOT_HINT:
        return true;    
        
    }
    
    return false;
}
bool isObjectTypeTop(editorobjecttype ed)
{
    return  (ed!=EDOT_NONE && ed!=EDOT_ERROR && !isObjectTypeGround(ed) );
}


bool getTopElementPosition(TiXmlElement *el, int &x , int &y, bool allowSamePos=false)
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
    if(y==400) {
        y = getElementPosition_lasty;
    } else {
        getElementPosition_lasty = y;
    }

    if((x<0) || (x>=XYE_HORZ) || (y<0) || (y>=XYE_VERT) )
    {
        cout<<"Wrong coordinates "<<x<<","<<y<<" found in a tag: <"<<el->Value()<<">\n";
        return false;
    }
    if( (!allowSamePos) && isObjectTypeGround(editorload_objects[x][y].type) )
    {
        errorPositions.push_back( make_pair(x,y) );
        cout<<"Unable to load two objects in same position: "<<x<<","<<y<<" , tag: <"<<el->Value()<<">\n";
        //return false;
    }
    return true;
}
bool getGroundElementPosition(TiXmlElement *el, int &x , int &y, bool allowSamePos=false)
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
    if(y==400) {
        y = getElementPosition_lasty;
    } else {
        getElementPosition_lasty = y;
    }

    if((x<0) || (x>=XYE_HORZ) || (y<0) || (y>=XYE_VERT) )
    {
        cout<<"Wrong coordinates "<<x<<","<<y<<" found in a tag: <"<<el->Value()<<">\n";
        return false;
    }
    if( (!allowSamePos) && isObjectTypeTop(editorload_objects[x][y].type) )
    {
        errorPositions.push_back( make_pair(x,y) );
        cout<<"Unable to load two objects in same position: "<<x<<","<<y<<" , tag: <"<<el->Value()<<">\n";
        //return false;
    }
    return true;
}

bool editor_LoadWall(TiXmlElement* el, bool round = false)
{
    int x,y; if(!getTopElementPosition(el,x,y)) return false;
    int t=editorload_defaultwall;
    el->QueryIntAttribute("type",&t);
    if( t<0 || t>5 ) {
        //fix variations...
        if (t != 6) cout<<"Found a unknown wall type: "<<t<<" set to 0."<<endl;
        t = 0;
    }
        
    int test=0;
    el->QueryIntAttribute("round1",&test); round=round || test;
    el->QueryIntAttribute("round3",&test); round=round || test;
    el->QueryIntAttribute("round7",&test); round=round || test;
    el->QueryIntAttribute("round9",&test); round=round || test;

    int x2 = 400;
    int y2 = 400;
    el->QueryIntAttribute("x2", &x2);
    el->QueryIntAttribute("y2", &y2);
    if (x2 == 400) {
        x2 = x;
    }
    if (y2 == 400) {
        y2 = y;
    }
    for (int i=x; i<=x2; i++) {
        for (int j=y; j<=y2; j++) {
            boardelement &o=editorload_objects[i][j];
            if ( (o.type != EDOT_NONE) && (o.type != EDOT_WALL) ) {
                o.type = EDOT_ERROR;
            } else {
                o.type = EDOT_WALL;
            }
            o.variation = t;
            o.round = round;

        }
    }
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
    
    if (v.length() >= 1) {
        switch(v[0]) {
        case 'R': case 'r':
            return EDCO_RED;
        case 'G': case 'g':
            return EDCO_GREEN;
        case 'B': case 'b':
            return EDCO_BLUE;            
        }
    }
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

    if (v.size() >= 1) {
        if(v[0]=='U') return EDITORDIRECTION_UP;
        else if(v[0]=='R') return EDITORDIRECTION_RIGHT;
        else if(v[0]=='L') return EDITORDIRECTION_LEFT;
    }
 return EDITORDIRECTION_DOWN;
}


bool editor_LoadGem(TiXmlElement* el)
{
    int x,y; if(!getTopElementPosition(el,x,y)) return false;
    editorcolor col=getElementColor(el);

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_GEM;
    o.color = col;
    return true;
}

bool editor_LoadStar(TiXmlElement* el)
{
    int x,y; if(!getTopElementPosition(el,x,y)) return false;

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_GEM;
    o.color = EDCO_WHITE;
    return true;
}

bool editor_LoadGenRC(TiXmlElement* el,editorobjecttype type, int variation=0, editorcolor* oldcolor=NULL)
{
    int t= errorPositions.size();
    int x,y; 
    
    if (isObjectTypeTop(type)) {
        if(!getTopElementPosition(el,x,y)) return false;
    } else{
        if(!getGroundElementPosition(el,x,y)) return false;
    }
    
    int round=false; el->QueryIntAttribute("round",&round);
    editorcolor col=getElementColor(el);
    boardelement &o=editorload_objects[x][y];
    if (oldcolor != NULL) {
        *oldcolor = o.color;
    }
    o.type=type;
    o.color = col;
    o.variation=variation;
    o.round=round;

    return true;
}

bool editor_LoadGen(TiXmlElement * el, editorobjecttype type, int variation=0)
{
    int x,y;
    if (isObjectTypeTop(type)) {
        if(!getTopElementPosition(el,x,y)) return false;
    } else{
        if(!getGroundElementPosition(el,x,y)) return false;
    }

    boardelement &o=editorload_objects[x][y];
    o.type=type;
    o.variation=variation;
    return true;
}



bool editor_LoadGenR(TiXmlElement * el, editorobjecttype type, int variation=0, editorcolor color = EDCO_YELLOW)
{
    int x,y;
    if (isObjectTypeTop(type)) {
        if(!getTopElementPosition(el,x,y)) return false;
    } else{
        if(!getGroundElementPosition(el,x,y)) return false;
    }

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
    int x,y;
    if (isObjectTypeTop(type)) {
        if(!getTopElementPosition(el,x,y)) return false;
    } else{
        if(!getGroundElementPosition(el,x,y)) return false;
    }

    int direction = getElementDirection(el);
    boardelement &o=editorload_objects[x][y];
    o.type=type;
    o.direction=direction;
    o.variation=variation;
    return true;
}


bool editor_LoadGenDOpposite(TiXmlElement * el, editorobjecttype type, int variation=0)
{
    int x,y;
    if (isObjectTypeTop(type)) {
        if(!getTopElementPosition(el,x,y)) return false;
    } else{
        if(!getGroundElementPosition(el,x,y)) return false;
    }

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
    int x,y;
    if (isObjectTypeTop(type)) {
        if(!getTopElementPosition(el,x,y)) return false;
    } else{
        if(!getGroundElementPosition(el,x,y)) return false;
    }

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
    int x,y;
    if (isObjectTypeTop(type)) {
        if(!getTopElementPosition(el,x,y)) return false;
    } else{
        if(!getGroundElementPosition(el,x,y)) return false;
    }


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
    int x,y; if(!getTopElementPosition(el,x,y)) return false;

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
    int x,y; if(!getTopElementPosition(el,x,y)) return false;

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
    int x,y; if(!getTopElementPosition(el,x,y)) return false;

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

void AssignHiddenWayVarDirFromFlags( Uint32 flags , int &variation, int &direction)
{
    for (variation = 2; variation <= 7; variation++) {
        for (direction = 0; direction < 4; direction++) {
            if (flags == getHiddenWayFlagsByVariationAndDir(variation, direction)) {
                return ;
            }
        }
    }
}

bool editor_LoadHiddenWay(TiXmlElement * el)
{
    int x,y; if(!getGroundElementPosition(el,x,y)) return false;


    const char * ptr=el->Attribute("ent");
    if(! ptr)
    {
        ptr="2468";
        cout << "Notice: using default entrances for hiddenway element.\n";
    }
    string v=ptr;
    Uint32 flags = 0;
    
    for (int i=0; i<v.size(); i++) {
        char ch = v[i];
        if ( (ch>='2') && (ch<='8') ) {
            flags |= ( 1 << ( ch-'0') );
        } else {
            cout << "Invalid/cannot understand entry point attributes for a <hiddenway> tag.\n";
            return false;
        }
    }

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_ONEDIRECTION;
    AssignHiddenWayVarDirFromFlags( flags , o.variation, o.direction);

    return true;
}


bool loadPortalIssue = false;
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
        loadPortalIssue = true;
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
        loadPortalIssue = true;
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
    int x,y; if(!getTopElementPosition(el,x,y)) return false;

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
    int x,y; if(!getTopElementPosition(el,x,y)) return false;

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
    int x,y;
    if (isObjectTypeTop(type)) {
        if(!getTopElementPosition(el,x,y)) return false;
    } else{
        if(!getGroundElementPosition(el,x,y)) return false;
    }

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
    int x,y; if(!getTopElementPosition(el,x,y)) return false;
    int grow=0; el->QueryIntAttribute("grow",&grow);

    boardelement &o=editorload_objects[x][y];
    o.direction = getElementDirection(el);
    o.type=EDOT_RATTLERHEAD;
    o.variation=grow;


    return true;
}


bool editor_LoadEarth(TiXmlElement* el)
{
    int x,y; if(!getTopElementPosition(el,x,y)) return false;
    int round=false; el->QueryIntAttribute("round",&round);

    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_EARTH;
    o.round = (int)round;
    return true;
}

bool editor_LoadColorDoor(TiXmlElement* el, int closedvariation)
{
    int x,y; if(!getGroundElementPosition(el,x,y)) return false;
    int open=0; el->QueryIntAttribute("open",&open);

    boardelement &o=editorload_objects[x][y];
    o.color=getElementColor(el);
    o.variation = closedvariation+open;
    o.type=EDOT_COLORSYSTEM;
    return true;
}

bool editor_LoadFactory(TiXmlElement* el)
{
    int x,y,kind=0; if(!getTopElementPosition(el,x,y)) return false;
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
    int x = errorPositions.size();
    editorcolor col;
    if( ! editor_LoadGenRC(el,EDOT_BLOCK, 0, &col)) {
        return false;
    }
    if (x != errorPositions.size() ){
        errorPositions.resize( std::max<int>(0, (int)errorPositions.size()-1) );
        //handle special block above marked area case.
        int x, y;
        if(! getTopElementPosition(el, x , y, true) ) return false;
        boardelement &o1=editorload_objects[x][y];
        o1.type = EDOT_COLORSYSTEM;
        if (o1.color != col ) {
            cout<<"The editor cannot yet load a block on top of a marked area of a different color."; 
            errorPositions.push_back( make_pair(x,y) );
            //return false;
        }
        o1.variation = 6;
        
    }
    return true;
}

bool editor_LoadHint(TiXmlElement* el)
{
    int x,y;
    if(!getGroundElementPosition(el,x,y)) return false;

    boardelement &o=editorload_objects[x][y];
    o.type = EDOT_HINT;
    const char * ch = el->GetText();
    if (ch != NULL) {
        o.hint = ch;
    } else {
        o.hint = "";
    }
    return true;
}


bool editor_LoadWildCardBlock(TiXmlElement* el)
{
    int x = errorPositions.size();
    if( ! editor_LoadGenR(el,EDOT_BLOCK,0, EDCO_WILD    ) ) {
        return false;
    }
    if (x != errorPositions.size() ){
        errorPositions.resize( std::max<int>(0, (int)errorPositions.size()-1) );
        //handle special block above marked aread case.
        int x, y;
        if(! getTopElementPosition(el, x , y, true) ) return false;
        boardelement &o1=editorload_objects[x][y];
        /*if((o1.type != EDOT_COLORSYSTEM) || (o1.variation!=4))
            return false;*/
        o1.type = EDOT_COLORSYSTEM;
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
        else if (v=="roundwall")     { if (! editor_LoadWall(ch,true)) return false;}
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

        else if (v=="pit")     { if (! editor_LoadGen(ch,EDOT_HAZARD,2)) return false;}
        else if (v=="firepad")     { if (! editor_LoadGen(ch,EDOT_FIREPAD,0)) return false;}
        else if (v=="force") { if (! editor_LoadGenD(ch,EDOT_ONEDIRECTION,1)) return false; }
        else if (v=="oneway") { if (! editor_LoadGenDOpposite(ch,EDOT_ONEDIRECTION,0)) return false; }
        else if (v=="hiddenway") { if (! editor_LoadHiddenWay(ch)) return false; }

        else if (v=="blockdoor") { if (! editor_LoadColorDoor(ch, 0)) return false; }
        else if (v=="blocktrap") { if (! editor_LoadColorDoor(ch, 2)) return false; }
        else if (v=="marked") { if (! editor_LoadGenC(ch,EDOT_COLORSYSTEM,4)) return false; }
        else if (v=="hint") { if (! editor_LoadHint(ch)) return false; }

        else
        {
            cout << "Editor-incompatible object: "<<v<<"\n";
            if (! editor_LoadGen(ch, EDOT_ERROR) ) {
                return false;
            }
        }

        ch=ch->NextSiblingElement();
    }
 return true;
}

bool editor_LoadXye(TiXmlElement* el)
{
    int x,y;
    
    
    int lives=1;
    el->QueryIntAttribute("lives",&lives);
    el->QueryIntAttribute("x",&editorload_xyex);
    el->QueryIntAttribute("y",&editorload_xyey);
    x = editorload_xyex = max(0, min(XYE_HORZ-1, editorload_xyex) );
    y = editorload_xyey = max(0, min(XYE_VERT-1, editorload_xyey) );


    boardelement &o=editorload_objects[x][y];
    o.type=EDOT_XYE;
    o.variation= lives-1;

 return true;
}



void loadKyeChar( char ch, boardelement & o , int x)
{
    o.type = EDOT_NONE;
    o.variation = 0;
    o.color = EDCO_YELLOW;
    o.direction = 0;
    o.round = false;
    
    int timer = -1;
    
    switch(ch) {
    case 'K':
        o.type = EDOT_XYE;
        o.variation = 3;
        break;
    case 'e':
        o.type = EDOT_EARTH;
        break;
    case '5':
        o.type = EDOT_WALL;
        o.variation = editorload_defaultwall;
        break;
    case('1'): case('2'): case('3'): case('4'):
    case('6'): case('7'): case('8'): case('9'):
        o.type = EDOT_WALL;
        o.round = true;
        o.variation = editorload_defaultwall;
        break;
    case 'b':
        o.type = EDOT_BLOCK;
        o.color = EDCO_YELLOW;
        break;
    case 'B':
        o.type = EDOT_BLOCK;
        o.color = EDCO_YELLOW;
        o.round = true;
        break;
    case '*':
        o.type = EDOT_GEM;
        o.color = EDCO_BLUE;
        break;
    case 'E':
        o.type = EDOT_BEAST;
        o.variation = (int)BT_GNASHER;
        break;
    case 'C':
        o.type = EDOT_BEAST;
        o.variation = (int)BT_BLOB;
        break;
    case '~':
        o.type = EDOT_BEAST;
        o.variation = (int)BT_VIRUS;
        break;
    case '[':
        o.type = EDOT_BEAST;
        o.variation = (int)BT_SPIKE;
        break;
    case 'T':
        o.type = EDOT_BEAST;
        o.variation = (int)BT_TWISTER;
        break;
    case '/':
        o.type = EDOT_BEAST;
        o.variation = (int)BT_DARD;
        break;
    case 's':
        o.type = EDOT_MAGNET;
        o.direction = EDITORDIRECTION_UP;
        break;
    case 'S':
        o.type = EDOT_MAGNET;
        o.direction = EDITORDIRECTION_RIGHT;
        break;
        
    case ('}'): timer = (3); break;
    case ('|'): timer = (4); break;
    case ('{'): timer = (5); break;
    case ('z'): timer = (6); break;
    case ('y'): timer = (7); break;
    case ('x'): timer = (8); break;
    case ('w'): timer = (9); break;
        
    case 'H':
        o.type = EDOT_HAZARD;
        break;
        
    case 'l': case'r': case'u': case'd':
        o.type = EDOT_SPECIALBLOCKS;
        o.variation = 0;
        o.color = EDCO_YELLOW;
        switch (ch) {
            case 'r': o.direction=EDITORDIRECTION_RIGHT; break;
            case 'l': o.direction=EDITORDIRECTION_LEFT; break;
            case 'u': o.direction=EDITORDIRECTION_UP; break;
            case 'd': o.direction=EDITORDIRECTION_DOWN; break;
        }
        break;
    case '<': case'>': case'^': case'v':
        o.type = EDOT_SPECIALBLOCKS;
        o.variation = 0;
        o.color = EDCO_YELLOW;
        o.round = true;
        switch (ch) {
            case '<': o.direction=EDITORDIRECTION_LEFT; break;
            case '>': o.direction=EDITORDIRECTION_RIGHT; break;
            case '^': o.direction=EDITORDIRECTION_UP; break;
            case 'v': o.direction=EDITORDIRECTION_DOWN; break;
        }
        break;
    case 'L': case'R': case'U': case'D':
        o.type = EDOT_PUSHER;
        o.color = EDCO_YELLOW;
        switch (ch) {
            case 'R': o.direction=EDITORDIRECTION_RIGHT; break;
            case 'L': o.direction=EDITORDIRECTION_LEFT; break;
            case 'U': o.direction=EDITORDIRECTION_UP; break;
            case 'D': o.direction=EDITORDIRECTION_DOWN; break;
        }
        break;
    case 'A': case 'F':
        o.round = ( ch == 'F' );
        o.color = EDCO_YELLOW;
        o.type = EDOT_ARROWMAKER;
        switch (x%4)
        {
            case 0: o.direction = EDITORDIRECTION_RIGHT ; break;
            case 1: o.direction = EDITORDIRECTION_UP ; break;
            case 2: o.direction = EDITORDIRECTION_LEFT ; break;
            case 3: o.direction = EDITORDIRECTION_DOWN ; break;
        }
        break;
    case 'a': case 'c':
        o.type = EDOT_TURNER;
        o.color = EDCO_YELLOW;
        o.variation = (ch == 'c');
        break;
    case 'f': case'g': case'h': case'i':
        o.type = EDOT_ONEDIRECTION;
        o.variation = 0;
        switch (ch) {
            case 'g': o.direction=EDITORDIRECTION_LEFT; break;
            case 'f': o.direction=EDITORDIRECTION_RIGHT; break;
            case 'h': o.direction=EDITORDIRECTION_DOWN; break;
            case 'i': o.direction=EDITORDIRECTION_UP; break;
        }
        break;
    case 'P': case 'p':    //Skye's sticky
        o.type = EDOT_MAGNET;
        o.variation = 1;
        o.direction = ( (ch == 'P') ? EDITORDIRECTION_RIGHT : EDITORDIRECTION_UP );
        break;
        
    case '!': //Skye's bomb
        o.type = EDOT_SPECIALBLOCKS;
        o.variation = 5;
        o.color = EDCO_RED;
        break;
        
    case 'O': //Skye's pit
        o.type = EDOT_HAZARD;
        o.variation = 2;
        break;

    case '(': case')': case'_': case'\'':  //Kye 3.0's teleport
        o.type = EDOT_TELEPORT;
        o.variation = 0;
        switch (ch) {
            case ')': o.direction=EDITORDIRECTION_LEFT; break;
            case '(': o.direction=EDITORDIRECTION_RIGHT; break;
            case '\'': o.direction=EDITORDIRECTION_DOWN; break;
            case '_': o.direction=EDITORDIRECTION_UP; break;
        }
        break;
        
    case 'o': //Xye mine
        o.type = EDOT_HAZARD;
        o.variation = 1;
        break;
        
    case '$':  //Xye emerald
        o.type = EDOT_GEM;
        o.color = EDCO_GREEN;
        break;
    
    case '@':  //Xye  green gem block
        o.type = EDOT_GEMBLOCK;
        o.color = EDCO_GREEN;
        break;
        
    case '#': //Xye yellow marked area
        o.type = EDOT_COLORSYSTEM;
        o.color = EDCO_YELLOW;
        o.variation = 4;
        break;
    case '%': //Xye yellow door
        o.type = EDOT_COLORSYSTEM;
        o.color = EDCO_YELLOW;
        o.variation = 0;
        break;
    case '=': //Xye yellow door
        o.type = EDOT_COLORSYSTEM;
        o.color = EDCO_YELLOW;
        o.variation = 1;
        break;
    case 'Q': case 'q':    //Xye's antimagnet
        o.type = EDOT_MAGNET;
        o.variation = 2;
        o.direction = ( (ch == 'Q') ? EDITORDIRECTION_RIGHT : EDITORDIRECTION_UP );
        break;
    case '.': case ',':
        o.type  = EDOT_SPECIALBLOCKS;
        o.color = EDCO_YELLOW;
        o.round = (ch == ',');
        o.variation = 4;
        break;
    case ':': case ';':              //In hindsight, I am not really sure why I added 
                                     // so many extensions to Kye's format
        o.type  = EDOT_SPECIALBLOCKS;
        o.variation = 5;
        o.color = EDCO_BLUE;
        o.round = (ch == ';');
        break;
        
    case '-': case '+':
        o.type  = EDOT_SPECIALBLOCKS;
        o.variation = ((ch=='-')?2:3);
        o.color = EDCO_YELLOW;
        break;
        
    case '?':
        o.type = EDOT_BOT;
        break;
        
    case 'M':
        o.type = EDOT_WALL;
        o.variation = 4;
        break;
        
    case '"': //do nothing
        break;

    }
    
    if (timer != -1) {
        o.type = EDOT_NUMBER;
        o.color = EDCO_YELLOW;
        o.variation = timer;
    }
}
map<string, SDL_Color> editorload_palette;
bool editor_LoadPalette(TiXmlElement* el)
{
    editorload_palette.clear();
    TiXmlElement * ch=el->FirstChildElement();

    while(ch!=NULL) {
        
        string v=ch->Value();
        if (v=="color") {
            int r,g,b;
            ch->QueryIntAttribute("red", &r);
            ch->QueryIntAttribute("green", &g);
            ch->QueryIntAttribute("blue", &b);
            SDL_Color c;
            c.r = r;
            c.b = b;
            c.g = g;
            const char* nam = ch->Attribute("id");
            if (nam == NULL) {
                return false;
            }
            editorload_palette[nam] = c;
        }
        ch=ch->NextSiblingElement();
    }
    
    return true;
}

bool editor_LoadDefault(TiXmlElement* el, DefaultColorData& cd)
{
    const char* id = el->Attribute("color");
    if (id == NULL) {
        return true;
    }
    string key = id;
    if (editorload_palette.find(key) == editorload_palette.end() ) {
        return true;
    }
    SDL_Color c = editorload_palette[key];
    cd.useDefault =false;
    cd.color =c;
    return true;    
}

bool editor_LoadDefaults(TiXmlElement* el)
{
    TiXmlElement * ch=el->FirstChildElement();

    while(ch!=NULL) {
        
        string v=ch->Value();
        if (v=="wall") {
            if (! editor_LoadDefault(ch, editorload_colors[EDITOR_COLOR_WALLS]) ) {
                return false;
            }
            int x = -1;
            ch->QueryIntAttribute("type", &x);
            if (x!=-1) {
                editorload_defaultwall = x;
            }
        }
        if (v=="earth") {
            if (! editor_LoadDefault(ch, editorload_colors[EDITOR_COLOR_EARTH]) ) {
                return false;
            }
        }
        if (v=="oneway" || v=="trick" || v=="hiddenway" ) {
            if (! editor_LoadDefault(ch, editorload_colors[EDITOR_COLOR_DOORS]) ) {
                return false;
            }            
        }
        if (v=="force") {
            if (! editor_LoadDefault(ch, editorload_colors[EDITOR_COLOR_FORCE]) ) {
                return false;
            }
        }

        ch=ch->NextSiblingElement();
    }
    
    return true;
}

bool editor_LoadFloor(TiXmlElement* el)
{
    TiXmlElement * ch=el->FirstChildElement();

    while(ch!=NULL) {
        
        string v=ch->Value();
        if (v=="area") {
            int x1=400,x2=400,y1=400,y2=400;
            ch->QueryIntAttribute("x1",&x1);
            ch->QueryIntAttribute("y1",&y1);
            if (x1==400) {
                ch->QueryIntAttribute("x",&x1);
                ch->QueryIntAttribute("y",&y1);
            }

            ch->QueryIntAttribute("x2",&x2);
            ch->QueryIntAttribute("y2",&y2);
            if (x1 != 0 || x2 != 29 || y1 != 0 || y2 != 19 ) {
                return true;
            }

            if (! editor_LoadDefault(ch, editorload_colors[EDITOR_COLOR_FLOOR]) ) {
                return false;
            }

        } else {
            return false;
        }
        ch=ch->NextSiblingElement();
    }
    
    return true;
}


void editorload_loadKyeLevel(const KyeLevel& klv)
{
    editor::board->hint = klv.lhint;
    editor::board->title = klv.name;
    editor::board->bye = klv.bye;
    editor::board->solution = "";
    editor::board->xye_x = 0;
    editor::board->xye_y = 0;
    for (int i=0; i<TOTAL_EDITOR_COLOR_OPTIONS; i++) {
        DefaultColorData &cd = editor::board->colors[i];
        cd.useDefault = true;
    }

    
    for (int i=0;i<XYE_HORZ;i++)for (int j=0;j<XYE_VERT;j++)
    {
        loadKyeChar( klv.data[i][j], editor::board->objects[i][XYE_VERT - j - 1], i );
        if ( klv.data[i][j] == 'K' ) {
            editor::board->xye_x = i;
            editor::board->xye_y = XYE_VERT - j - 1;
        }
    }
    for (int i=0; i<5; i++) {
        for (int j=0; j<2; j++) {
            editor::board->portal_x[i][j] = -1,
            editor::board->portal_y[i][j] = -1;
        }
    }
}


bool editor::load_kye()
{
    vector<KyeLevel> levels;
    string s = KyeLevelPack::LoadForEditor(filename.c_str(), levels);
    if (s != "") {
        loadError = s;
        return false;
    }
    editorboard::ResetLevels(true);
    editorboard::filetitle = filename;
    editorboard::description = "";
    editorboard::author = "";
    for (int k=0; k<levels.size(); k++) {
        editorload_loadKyeLevel(levels[k]);
        editorboard::SaveAtLevelNumber(editor::board, k);
    }
    editorboard::LoadLevelNumber(editor::board, 0);
    updateCountRelated();
    
    return true;
}

//-------------
bool editor::load_KyeFormat(TiXmlElement* el)
{
    KyeLevel ky;
    if ( ! LoadKyeFormatTag(el, &ky) ) {
        return false;
    }
    editorload_loadKyeLevel(ky);
    for (int i=0;i<XYE_HORZ;i++) for (int j=0;j<XYE_VERT;j++)
    {
        editorload_objects[i][j] = editor::board->objects[i][XYE_VERT-j-1];
    }
    for (int i=0; i<5; i++) {
        for (int j=0; j<2; j++) {
            editorload_portal_x[i][j] = editor::board->portal_x[i][j],
            editorload_portal_y[i][j] = XYE_VERT-editor::board->portal_y[i][j]-1;
        }
    }
    editorload_xyex = editor::board->xye_x;
    editorload_xyey = XYE_VERT - editor::board->xye_y - 1;

    return true;
}

bool editor::appendLevels(const string file)
{
    loadPortalIssue = false;
    loadError = "";
    cout<<"Attempt to append file: "<<file<<endl;
    TiXmlDocument  fil(file.c_str());
    
    int x = editorboard::CurrentLevelNumber();
    int n = editorboard::CountLevels();
    int oldn = n;
    if (fil.LoadFile())
    {
        TiXmlElement* pack, *el, *level;
        pack=fil.FirstChildElement("pack");
        if (pack==NULL)
        {
            cout<<"Not a valid .xye file (missing <pack>)\n";
            loadError="The level file is invalid (missing <pack> tag)";
            return false;
        }

        
        level=pack->FirstChildElement("level");
        bool colorWarn = false;
        bool errorsWarn = false;
        int tn = 0;
        while(level!=NULL)
        {
            string ltitle=filename_name,lhint="",lbye="",lsolution="";

            int i,j;
            editorload_xyex=-1;

            editorload_defaultwall = 0;
            for (int i=0; i<TOTAL_EDITOR_COLOR_OPTIONS; i++) {
                editorload_colors[i].useDefault = true;
            }

            for (int i=0; i<5; i++) {
                for (int j=0; j<2;j++) {
                    editorload_portal_x[i][j] = editorload_portal_y[i][j] = -1;
                }
            }
    
            for (i=0;i<XYE_HORZ;i++)for (j=0;j<XYE_VERT;j++) editorload_objects[i][j].type=EDOT_NONE;
    
            loadError="Found tags and/or attributes that are not recognized by the current version.";
            el=level->FirstChildElement();
    
    
            while (el!=NULL)
            {
                string v=el->Value();
                if (v=="objects" ||v=="ground" || v=="normal")
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
                else if (v=="title" || v=="name")
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
                else if (v=="kyeformat") {
                    if ( ! load_KyeFormat(el) ) {
                        loadError = "There were issues while loading a <kyeformat> tag.";
                        return false;
                    }
                } else {
                    if ( v!="palette" && v!="floor" && v!="default") {
                        cout << "Editor-incompatible <level> child: "<<v<<"\n";
                        loadError="Found a tag <"+v+"> that is incompatible with the level editor. Had to stop loading, sorry.";
                        return false;
                    } else {
                        if (v=="palette") {
                            if ( ! editor_LoadPalette(el) ) {
                                return "There were issues while loading a <palette> tag";
                            }
                        }
                        if (v=="default") {
                            if ( ! editor_LoadDefaults(el) ) {
                                return "There were issues while loading a <default> tag";
                            }
                        }
                        //colorWarn = true;
                        if (v=="floor") {
                            if (! editor_LoadFloor(el) ) {
                                return "There were issues while loading a <floor> tag.";
                            }
                        }
                        
                    }
                }
                el=el->NextSiblingElement();
            }
    
            if(editorload_xyex==-1)
            {
                cout << "Notice: Unable to find xye in the level file.\n";
            }
            cout << "Level loaded successfully.\n";

            for (int i=0; i<TOTAL_EDITOR_COLOR_OPTIONS; i++) {
                editor::board->colors[i] = editorload_colors[i];
            }
            
            for (i=0;i<XYE_HORZ;i++)for (j=0;j<XYE_VERT;j++)
            {
                editor::board->objects[i][XYE_VERT-j-1]=editorload_objects[i][j];
            }
            for (int i=0; i<5; i++) {
                for (int j=0; j<2; j++) {
                    editor::board->portal_x[i][j] = editorload_portal_x[i][j],
                    editor::board->portal_y[i][j] = XYE_VERT-editorload_portal_y[i][j]-1;
                }
            }
            if (errorPositions.size() != 0) {
                errorsWarn = true;
                for (int i=0; i<errorPositions.size(); i++) {
                    pair<int,int> p = errorPositions[i];
                    editor::board->objects[p.first][XYE_VERT-p.second-1].type = EDOT_ERROR;
                }
                errorPositions.resize(0);
            }
    
            editor::board->xye_x = editorload_xyex;
            editor::board->xye_y = XYE_VERT-editorload_xyey-1;
            editor::board->hint = lhint;
            editor::board->title = ltitle;
            editor::board->bye = lbye;
            editor::board->solution = lsolution;
            
            editorboard::SaveAtLevelNumber(editor::board, n);

            n++;
            tn++;
            level=level->NextSiblingElement("level");
        }
        if(tn==0)
        {
            cout<<"Not a valid .xye file (missing <level>)\n";
            loadError="The level file is invalid (missing <level> tag)";
            return false;
        }
        cout<<tn<<" levels found.\n";
        loadError = "";
        if (colorWarn) {
            loadError += "Color information is not supported by this version of the editor. Colors were reset to default values. ";
        }
        if (errorsWarn )  {
            loadError += "There were issues when loading some of the objects, possibly related to features that the editor does not yet support. ";
        }
        if (loadPortalIssue) {
            loadError += "Some portal objects used features that are not compatible with the editor. ";
            
        }
        cout << "File loaded successfully.\n";
        editorboard::LoadLevelNumber(editor::board, oldn);
        updateCountRelated();
    } else {
        loadError = "Not a valid XML file.";
        return false;
    }
 return true;
}


bool editor::load()
{
    loadPortalIssue = false;
    loadError = "";
    {
        int len = filename.length();
        if (len >= 4) {
            string ext = filename.substr(len-4);
            if( (ext==".kye" || ext==".KYE") ) {
                return editor::load_kye();
            }
        }
    }
    
    editorboard::ResetLevels(true);
    TiXmlDocument  fil(filename.c_str());
    if (fil.LoadFile())
    {
        string lauthor="xyedit",ldescription="Generated by xyedit";
        string lname=filename_name;
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
        el=pack->FirstChildElement("name");
        if (el!=NULL)
        {
            const char* gt=el->GetText();
            lname = (gt!=NULL ? gt : "");
        }

        editorboard::description = ldescription;
        editorboard::author = lauthor;
        editorboard::filetitle = lname;
        
        if ( ! appendLevels(filename) ) {
            return false;
        }


        return true;


    }
    else
    {
        cout<<"Unable to load level file.\n";
        cout<<"\t"<<fil.ErrorDesc()<<"\n";

        if (fil.ErrorId()==2)
        {
            if(filename_name=="editortest.xye") {
                editorboard::ResetLevels();
                board->makeDefaultLevel();
                updateCountRelated();
                return true;
            }
            loadError="Xyedit was unable to open the level file.";

        }
        else
            loadError="There are errors in the XML structure of the file: "+string(fil.ErrorDesc());
        return false;
    }
    return true;
}
