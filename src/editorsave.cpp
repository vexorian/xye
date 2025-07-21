#include "xyedit.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "xye.h"
using std::pair;
using std::max_element;



void saveColor(std::ofstream &file,boardelement &o,bool allownocolor=false)
{
    if(allownocolor && (o.color==EDCO_WHITE)) file << "nocolor='1' ";
    else switch(o.color)
    {
        case EDCO_BLUE: file<<"bc='B' "; break;
        case EDCO_YELLOW: file<<"bc='Y' "; break;
        case EDCO_RED: file<<"bc='R' "; break;
        case EDCO_PURPLE: file<<"bc='P' "; break;
        default: file<<"bc='G' "; break;
    }

}


void saveDirection(std::ofstream &file,boardelement &o)
{
    switch(o.direction)
    {
        case EDITORDIRECTION_UP: file<<"dir='U' "; break;
        case EDITORDIRECTION_DOWN: file<<"dir='D' "; break;
        case EDITORDIRECTION_RIGHT: file<<"dir='R' "; break;
        default: file<<"dir='L' "; break;
    }

}

void saveOppositeDirection(std::ofstream &file,boardelement &o)
{
    switch(o.direction)
    {
        case EDITORDIRECTION_UP: file<<"dir='D' "; break;
        case EDITORDIRECTION_DOWN: file<<"dir='U' "; break;
        case EDITORDIRECTION_RIGHT: file<<"dir='L' "; break;
        default: file<<"dir='R' "; break;
    }

}


void saveRound(std::ofstream &file,boardelement &o)
{
    if(o.round) file << "round='1' ";
}

int savePosition_lastx = -1;
void resetSavedPosition()
{
    savePosition_lastx = -1;
}
void savePosition(std::ofstream &file, int x ,int y)
{
    if(x!=savePosition_lastx) {
        file << "x='"<<x<<"' y='"<<y<<"' ";
        savePosition_lastx = x;
    } else {
        file << "y='"<<y<<"' ";
    }

}

void saveLargeBlock(std::ofstream &file, boardelement &o, int x ,int y)
{
    file <<"<largeblockpart ";
    savePosition(file,x,y);
    saveColor(file,o, true);
    Uint8 flags = getLargeBlockFlagsByVarDir(o.variation, o.direction);
    const char* dirs = "-U-R-D-L";
    file<<"sharededges = '";
    for (int i=1; i<9; i++)
        if(flags&(1<<i))
            file<<dirs[i];
    file<<"' />";


}

void saveColorFactory(std::ofstream &file, boardelement &o, int x ,int y)
{
    file <<"<factory ";
    savePosition(file,x,y);
    switch(o.direction)
    {
        case EDITORDIRECTION_UP: file<<"dir='U' swdir='D' "; break;
        case EDITORDIRECTION_DOWN: file<<"dir='D' swdir='U' "; break;
        case EDITORDIRECTION_RIGHT: file<<"dir='R' swdir='L' "; break;
        default: file<<"dir='L' swdir='R' "; break;
    }

    saveColor(file,o, false);
    saveRound(file,o);
    file<<"kind='";
    switch(o.variation)
    {
        case 0: case 1: case 2: file<<o.variation; break;
        case 3: file<<4; break;
        case 4: file<<8; break;
    }

    file<<"' />";


}

void saveDangerFactory(std::ofstream &file, boardelement &o, int x ,int y)
{
    file <<"<factory ";
    savePosition(file,x,y);
    switch(o.direction)
    {
        case EDITORDIRECTION_UP: file<<"dir='U' swdir='D' "; break;
        case EDITORDIRECTION_DOWN: file<<"dir='D' swdir='U' "; break;
        case EDITORDIRECTION_RIGHT: file<<"dir='R' swdir='L' "; break;
        default: file<<"dir='L' swdir='R' "; break;
    }

    file<<"kind='";
    switch(o.variation)
    {
        case 14:
            file<<7; break;
        case 15:
            file<<6; break;
        case 16:
            file<<3; break;

        default:
            file<<5<<"' beastkind='"<<o.variation; break;
    }

    file<<"' />";


}

int defaultWallVariation = 0;
void saveNormalObject(std::ofstream &file, boardelement &o, int x, int y)
{
    switch(o.type)
    {
        case EDOT_NONE : return; //nothing to do
        case EDOT_BLOCK:
            switch(o.color)
            {
              case EDCO_METAL:
                file<<"\t\t<metalblock ";
                savePosition(file,x,y);
                saveRound(file,o);
                file <<"/>\n";
                break;
              case EDCO_WILD:
                file<<"\t\t<wild ";
                savePosition(file,x,y);
                saveRound(file,o);
                file <<"/>\n";
                break;

              default:
                file<<"\t\t<block ";
                savePosition(file,x,y);
                saveColor(file,o,true);
                saveRound(file,o);
                file <<"/>\n";
            }
            break;
        case EDOT_GEM:
            if(o.color == EDCO_PURPLE)
            {
                file<<"\t\t<star ";
                savePosition(file,x,y);
                file << "/>\n";
            }
            else
            {
                file<<"\t\t<gem ";
                savePosition(file,x,y);
                saveColor(file,o);
                file <<"/>\n";
            }
            break;

        case EDOT_WALL:
            file<<"\t\t<wall ";
            savePosition(file,x,y);
            if(o.round)
            {
                //update memory in case the board wasn't drawn (which may happen when there are multiple levels)
                editor::board->updateWallMem(x,XYE_VERT-y-1);
                if(o.r1mem) file << "round1='1' ";
                if(o.r7mem) file << "round7='1' ";
                if(o.r3mem) file << "round3='1' ";
                if(o.r9mem) file << "round9='1' ";
            }


            if(o.variation != defaultWallVariation) file << "type='"<<o.variation<<"' ";

            file <<"/>\n";
            break;

        case EDOT_TURNER:
            file<<"\t\t<";
            if(o.variation) file<<"aclocker ";
            else file << "clocker ";
            savePosition(file,x,y);
            saveRound(file,o);
            saveColor(file,o,true);
            file<<"/>\n";
            break;



        case EDOT_GEMBLOCK:
            file<<"\t\t<gemblock ";
            savePosition(file,x,y);
            saveColor(file,o);
            file <<"/>\n";
            break;

        case EDOT_EARTH:
            file << "\t\t<earth ";
            savePosition(file,x,y);
            saveRound(file,o);
            file<<"/>\n";
            break;

        case EDOT_MAGNET:
            file << "\t\t<magnet ";
            savePosition(file,x,y);

            if ((o.direction==EDITORDIRECTION_LEFT) ||  (o.direction==EDITORDIRECTION_RIGHT) )
            {
                file << "horz='1' ";
            }

            file << "kind='"<<o.variation<<"' ";
            file<<"/>\n";
            break;

        case EDOT_PUSHER:
            file << "\t\t<pusher ";
            savePosition(file,x,y);
            saveDirection(file,o);
            saveColor(file,o);

            file<<"/>\n";
            break;

        case EDOT_ARROWMAKER:
            file<<"\t\t<";
            if(o.variation==0) file << "auto ";
            else if(o.variation==2) file << "sniper ";
            else if(o.variation==1) file << "filler ";
            savePosition(file,x,y);
            saveDirection(file,o);
            saveColor(file,o);
            saveRound(file,o);

            file<<"/>\n";
            break;

        case EDOT_HAZARD: if(o.variation==2) break;
            file<<"\t\t<";
            if(o.variation==0) file << "blacky ";
            else if(o.variation==1) file << "mine ";
            savePosition(file,x,y);
            file<<"/>\n";
            break;

        case EDOT_BEAST:
            file<<"\t\t<beast ";
            savePosition(file,x,y);
            saveDirection(file,o);
            file << "kind='"<<o.variation<<"' ";

            file<<"/>\n";
            break;

        case EDOT_TELEPORT:
            file<<"\t\t<teleport ";
            savePosition(file,x,y);
            saveDirection(file,o);
            file <<"/>\n";break;

        case EDOT_COLORSYSTEM:
            if(o.variation==5) //window block:
            {
                file<<"\t\t<window ";
                savePosition(file,x,y);
                saveColor(file,o);
                file<<"/>\n";
            }
            else if(o.variation==6) //block on top of area
            {
                file<<"\t\t<block ";
                savePosition(file,x,y);
                saveColor(file,o);
                file<<"/>\n";
            } else if (o.variation==7) { //wildcard block on top of area
                file<<"\t\t<wild ";
                savePosition(file,x,y);
                file<<"/>\n";
            }
            break;

        case EDOT_BOT:
            file<<"\t\t<bot ";
            savePosition(file,x,y);
            file << "/>\n";
            break;

        case EDOT_KEYSYSTEM:
            file<<"\t\t<";
            if(o.variation) file<<"lock ";
            else file<<"key ";
            savePosition(file,x,y);
            saveColor(file,o);
            file << "/>\n";
            break;

        case EDOT_NUMBER:
            file<<"\t\t<timer ";
            savePosition(file,x,y);
            saveRound(file,o);
            saveColor(file,o);
            file << "val='"<<o.variation<<"' ";
            file << "/>\n";
            break;


        case EDOT_SPECIALBLOCKS:
            file<<"\t\t<";

            if (o.variation==0)
            {
                file<<"arrow ";
                saveDirection(file,o);
            }
            else if (o.variation==1)
            {
                file<<"scroll ";
                saveDirection(file,o);

            }
            else if(o.variation<=3)
            {
                if( o.variation==2) file << "toggle off='1' ";
                else file << "toggle ";
            }
            else if (o.variation==4) file << "lblock ";
            else file << "surprise ";
            savePosition(file,x,y);
            saveColor(file,o);
            saveRound(file,o);
            file << "/>\n";
            break;
        case EDOT_RATTLERHEAD:
            file<<"\t\t<rattler ";
            savePosition(file,x,y);
            saveDirection(file,o);
            if (o.variation) file << "grow='"<<o.variation<<"' ";

            file << "/>\n";
            break;

        case EDOT_FOOD:
            file<<"\t\t<rfood ";
            savePosition(file,x,y);
            file << "/>\n";
            break;

        case EDOT_LARGEBLOCK:
            file<<"\t\t";
            saveLargeBlock(file, o, x,y);
            file<<"\n";
            break;
        case EDOT_COLORFACTORY:
            file<<"\t\t";
            saveColorFactory(file,o,x,y);
            file<<"\n";
            break;
        case EDOT_DANGERFACTORY:
            file<<"\t\t";
            saveDangerFactory(file,o,x,y);
            file<<"\n";
            break;

        case EDOT_PORTAL:
            // Do nothing.
            break;
    }
}

void savePortals(std::ofstream &file, editorboard *board)
{
    for (int i=0; i<XYE_OBJECT_COLORS+1; i++)
        if( board->portal_x[i][0] != -1)
        {
            int x = board->portal_x[i][0];
            int y = board->portal_y[i][0];
            int tx = board->portal_x[i][1];
            int ty = board->portal_y[i][1];
            if(tx<0) tx=0;
            if(ty<0) ty=0;
            file<<"\t\t<portal ";
            savePosition(file, x,XYE_VERT-y-1);
            int ni = i;
            if ( i == 5 ) {
                ni = 4;
            } else if (i==4) {
                ni = 5;
            }

            file<<"defcolor='"<<ni<<"' ";
            file<<"targetx='"<<tx<<"' targety='"<<(XYE_VERT-ty-1)<<"' ";
            file << "/>\n";
            if(board->objects[tx][ty].variation == 1)
            {
                file<<"\t\t<portal ";
                savePosition(file, tx,XYE_VERT-ty-1);
                file<<"defcolor='"<<ni<<"' ";
                file<<"targetx='"<<x<<"' targety='"<<(XYE_VERT-y-1)<<"' ";
                file << "/>\n";

            }
        }

}

void saveGroundObject(std::ofstream &file,boardelement &o, int x, int y)
{
    switch(o.type)
    {
        case EDOT_NONE : return; //nothing to do

        case EDOT_HAZARD: if(o.variation!=2) break;
            file<<"\t\t<pit ";
            savePosition(file,x,y);
            file<<"/>\n";
            break;

        case EDOT_FIREPAD:
            file<<"\t\t<firepad ";
            savePosition(file,x,y);
            file<<"/>\n";
            break;


        case EDOT_ONEDIRECTION:
            if (o.variation >= 2) {
                file<<"\t\t<hiddenway ";
                Uint32 flags = getHiddenWayFlagsByVariationAndDir(o.variation, o.direction);
                string ent = "";
                for (int i=2; i<=8; i+=2) {
                    if ( flags&(1<<i) ) {
                        ent += ('0'+(char)i);
                    }
                }
                file<<"ent='"<<ent<<"' ";
            } else if(o.variation) {//ground arrow
                file<<"\t\t<force ";
                saveDirection(file,o);
            } else {
                file<<"\t\t<oneway ";
                saveOppositeDirection(file,o);
            }
            savePosition(file,x,y);

            file<<"/>\n";
            break;


        case EDOT_COLORSYSTEM:
            if(o.variation==5) break;
            file << "\t\t<";

            switch(o.variation)
            {
                case 0 /*closed door*/: file<<"blockdoor ";break;
                case 1 /*open door*/: file<<"blockdoor open='1' ";break;
                case 2 /*closed trap*/: file<<"blocktrap ";break;
                case 3 /*open trap*/: file<<"blocktrap open='1' ";break;
                case 4 /*marked area*/: file<<"marked ";break;
                case 6 /*block above area*/: file<<"marked "; break;
                case 7 /*wildcard above area*/: file<<"marked "; break;

            }
            savePosition(file,x,y);
            saveColor(file,o);
            file<<"/>\n";
            break;

        case EDOT_HINT:
            file << "\t\t<hint ";
            savePosition(file,x,y);
            file<<">"<<StripXML(o.hint)<<"</hint>";
            break;

    }
}

bool editor::save()
{
    if (save(filename, false))
    {
         SavedFile=true;
         return true;
    }
    dialogs::makeMessageDialog(editorwindow, string("Unable to save the level file, cannot rewrite ")+string(filename)+".","Ok",onDialogClickDoNothing);
    return false;
}




void saveDefault( std::ofstream & file, string tag, editorboard*board, int i)
{
     DefaultColorData &cd = board->colors[i];
     if (! cd.useDefault) {
         file<<"        <"<<tag<<" ";
         file<<" color='"<<(i+1)<<"' />" <<endl;
     }
}

void saveWallDefault( std::ofstream & file, editorboard*board)
{
    const int MAX_WALL_VARIATIONS = 6;
    pair<int, int> variationCount[MAX_WALL_VARIATIONS];
    for (int i=0; i<MAX_WALL_VARIATIONS; i++) {
        variationCount[i] = make_pair(0, i);
    }
    for (int i=0; i<XYE_HORZ; i++) {
        for (int j=0; j<XYE_VERT; j++) {
            if (board->objects[i][j].type == EDOT_WALL ) {
                variationCount[board->objects[i][j].variation].first++;
            }
        }
    }
    defaultWallVariation = max_element(variationCount, variationCount + MAX_WALL_VARIATIONS)->second;
    file<<"        <wall type='"<< defaultWallVariation <<"' ";
    DefaultColorData &cd = board->colors[EDITOR_COLOR_WALLS];
    if (! cd.useDefault ) {
        file<<" color='"<<(EDITOR_COLOR_WALLS+1)<<"' ";
    }

    file << " />" <<endl;

}


void saveColorStuff( std::ofstream & file, editorboard*board)
{
    bool doit = false;
    for (int i=0; i<TOTAL_EDITOR_COLOR_OPTIONS; i++) {
        doit |= (! board->colors[i].useDefault );
    }
    if (doit) {
        //first save the palette.
        file<<"    <palette>"<<endl;
        for (int i=0; i<TOTAL_EDITOR_COLOR_OPTIONS; i++) {
            DefaultColorData &cd = board->colors[i];
            if ( ! cd.useDefault ) {
                file << "        <color id='"<<(i+1)<<"'";
                file << " red='"<<(int)cd.color.r<<"'";
                file << " green='"<<(int)cd.color.g<<"'";
                file << " blue='"<<(int)cd.color.b<<"'";
                file << " />"<<endl;
            }

        }
        file<<"    </palette>"<<endl;
    }

    //now save the defaults...
    file<<"    <default>"<<endl;

    //saveDefault(file, "wall", board, EDITOR_COLOR_WALLS);
    saveWallDefault(file, board);
    saveDefault(file, "earth", board, EDITOR_COLOR_EARTH);
    saveDefault(file, "oneway", board, EDITOR_COLOR_DOORS);
    saveDefault(file, "force", board, EDITOR_COLOR_FORCE);

    file<<"    </default>"<<endl;

    if (! board->colors[EDITOR_COLOR_FLOOR].useDefault) {
        file <<"    <floor><area color = '"<<(EDITOR_COLOR_FLOOR+1)<<"' ";
        file << "x1='0' x2='29' y1='0' y2='19' /></floor>"<<endl;
    }


}
bool editor::save(const string &target, bool onlyOneLevel)
{
    std::ofstream file;
    file.open (target.c_str(),std::ios::trunc | std::ios::out );
    if (!file.is_open()) return false; //ouch just halt.

    int oldcur = editorboard::CurrentLevelNumber();
    int first = 0;
    if (onlyOneLevel) {
        first = oldcur;
    }
    editorboard::SaveCopy(board);
    editorboard::LoadLevelNumber(board, first);

    file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    file << "<pack>\n<name>"<< StripXML(editorboard::filetitle) <<"</name><author>"<< StripXML(editorboard::author) <<"</author>\n<description>"<< StripXML(editorboard::description) <<"</description>\n";

    for (int i=first; i<editorboard::CountLevels(); i++) {
        editorboard::LoadLevelNumber(board, i);
        file << "\n<level>\n";
        file << "<title>"<< StripXML(board->title) <<"</title>\n";

        if(board->hint!="")
        {
            file << "<hint>"<< StripXML(board->hint) <<"</hint>\n";
        }
        if(board->solution!="")
        {
            file << "<solution>"<< StripXML(board->solution) <<"</solution>\n";
        }

        if(board->bye!="")
        {
            file << "<bye>"<< StripXML(board->bye) <<"</bye>\n";
        }
        saveColorStuff(file, board);

        int j;
        file << "\t<ground>\n";
        resetSavedPosition();
        for (i=0;i<XYE_HORZ;i++) for (j=0;j<XYE_VERT;j++) saveGroundObject(file,editor::board->objects[i][j],i,XYE_VERT-j-1);
        file << "\t</ground>\n";
        file << "\t<objects>\n";
        resetSavedPosition();
        for (i=0;i<XYE_HORZ;i++) for (j=0;j<XYE_VERT;j++)
        {
            saveNormalObject(file,editor::board->objects[i][j],i,XYE_VERT-j-1);
        }
        savePortals( file, editor::board);


        file << "\t</objects>\n";


        if(editor::board->xye_x>=0)
        {
            resetSavedPosition();
            file << "\t<xye x='"<<editor::board->xye_x<<"' y='"<<(XYE_VERT-editor::board->xye_y-1)<<"' lives='"<<(editor::board->objects[editor::board->xye_x][editor::board->xye_y].variation+1)<<"' />\n";
        }

        file << "</level>\n";

        if (onlyOneLevel) break;
    }

    file << "</pack>\n";
    editorboard::LoadLevelNumber(board, oldcur);





    file.close();
 return true;

}
