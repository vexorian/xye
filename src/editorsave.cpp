#include "xyedit.h"
#include <iostream>
#include <fstream>



void saveColor(std::ofstream &file,boardelement &o,bool allownocolor=false)
{
    if(allownocolor && (o.color==EDCO_WHITE)) file << "nocolor='1'";
    else switch(o.color)
    {
        case EDCO_BLUE: file<<"bc='B' "; break;
        case EDCO_YELLOW: file<<"bc='Y' "; break;
        case EDCO_RED: file<<"bc='R' "; break;
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

void savePosition(std::ofstream &file, int x ,int y)
{
    file << "x='"<<x<<"' y='"<<y<<"' ";
}




void saveNormalObject(std::ofstream &file, boardelement &o, int x, int y)
{
    switch(o.type)
    {
        case EDOT_NONE : return; //nothing to do
        case EDOT_BLOCK:
            file<<"\t\t<block ";
            savePosition(file,x,y);
            saveColor(file,o,true);
            saveRound(file,o);
            file <<"/>\n";
            break;
        case EDOT_GEM:
            file<<"\t\t<gem ";
            savePosition(file,x,y);
            saveColor(file,o);
            file <<"/>\n";
            break;

        case EDOT_WALL:
            file<<"\t\t<wall ";
            savePosition(file,x,y);
            if(o.round)
            {
                if(o.r1mem) file << "round1='1' ";
                if(o.r7mem) file << "round7='1' ";
                if(o.r3mem) file << "round3='1' ";
                if(o.r9mem) file << "round9='1' ";
            }

            if(o.variation) file << "type='"<<o.variation<<"' ";

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
            break;

        case EDOT_BOT:
            file<<"\t\t<bot ";
            savePosition(file,x,y);
            file << "/>\n";
            break;


        case EDOT_WILDCARD:
            file<<"\t\t<wild ";
            savePosition(file,x,y);
            saveRound(file,o);
            file << "/>\n";
            break;

        case EDOT_METAL:
            file<<"\t\t<metalblock ";
            savePosition(file,x,y);
            saveRound(file,o);
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
        
        case EDOT_STAR:
            file<<"\t\t<star ";
            savePosition(file,x,y);
            file << "/>\n";
            break;
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
            if(o.variation) //ground arrow
            {
                file<<"\t\t<force ";
                saveDirection(file,o);
            }
            else
            {
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

            }
            savePosition(file,x,y);
            saveColor(file,o);
            file<<"/>\n";
            break;

    }
}

bool editor::save()
{
    if (save(filename))
    {
         SavedFile=true;
         return true;
    }
    dialogs::makeMessageDialog(editorwindow, string("Unable to save the level file, cannot rewrite ")+string(filename)+".","Ok",onDialogClickDoNothing);
    return false;
}


string  stripXML(const string s)
{
    string r="";
    
    int k=0;
    for (int i=0;i<s.length();i++)
    {
        switch(s[i])
        {
            case '<':
                r+=s.substr(k,i-k);r+="&lt;";k=i+1;break;
            case '>':
                r+=s.substr(k,i-k);r+="&gt;";k=i+1;break;
            case '&':
                r+=s.substr(k,i-k);r+="&amp;";k=i+1;break;
            case '"':
                r+=s.substr(k,i-k);r+="&quot;";k=i+1;break;    
                
            case '\'':
                r+=s.substr(k,i-k);r+="&apos;";k=i+1;break;    
            
        }
    }
    if(k<s.length()) r+=s.substr(k,s.length()-k);
    
    return r;
}

bool editor::save(const string &target)
{
    std::ofstream file;
    file.open (target.c_str(),std::ios::trunc | std::ios::out );
    if (!file.is_open()) return false; //ouch just halt.

    file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    file << "<pack>\n<name>"<< stripXML(board->title) <<"</name><author>"<< stripXML(board->author) <<"</author>\n<description>"<< stripXML(board->description) <<"</description>\n";
    file << "\n<level>\n";
    file << "<title>"<< stripXML(board->title) <<"</title>\n";
    
    if(board->hint!="")
    {
        file << "<hint>"<< stripXML(board->hint) <<"</hint>\n";
    }
    if(board->bye!="")
    {
        file << "<bye>"<< stripXML(board->bye) <<"</bye>\n";
    }

    int i,j;
    file << "\t<ground>\n";
    for (i=0;i<XYE_HORZ;i++) for (j=0;j<XYE_VERT;j++) saveGroundObject(file,editor::board->objects[i][j],i,XYE_VERT-j-1);
    file << "\t</ground>\n";
    file << "\t<objects>\n";
    for (i=0;i<XYE_HORZ;i++) for (j=0;j<XYE_VERT;j++)
    {
        saveNormalObject(file,editor::board->objects[i][j],i,XYE_VERT-j-1);
    }
    file << "\t</objects>\n";


    if(editor::board->xye_x>=0)
    {
        file << "\t<xye x='"<<editor::board->xye_x<<"' y='"<<(XYE_VERT-editor::board->xye_y-1)<<"' lives='"<<(editor::board->objects[editor::board->xye_x][editor::board->xye_y].variation+1)<<"' />\n";
    }

    file << "</level>\n";
    file << "</pack>\n";





    file.close();
 return true;

}
