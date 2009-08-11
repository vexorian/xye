/*
Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/

#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<fstream>

#include "levels.h"
#include "xyedit.h"
#include "xye.h"
#include "xye_script.h"
#include "options.h"
#include "dialogs.h"
#include "command.h"


#define sz editor::GRIDSIZE //typing game::GRIDSIZE is an annoyance



int editor::Width,editor::Height, editor::GRIDSIZE;

window* editor::editorwindow;
editorbuttons *editor::buttons;
editorboard *  editor::board;

unsigned int editor::tic4;
unsigned int editor::subtic4;

string editor::filename;
string editor::filename_name;
string editor::filename_path;

bool editor::SavedFile=true;
bool editor::ExitPrompt=false;


button * editor::savebutton;
SDL_Surface * editor::sprites;
Font* editor::FontRes;


void editor::onExitWithoutSavingClick(bool yes)
{
    ExitPrompt=false;
    if(yes) cancel();
}

void editor::onBrowseWithoutSavingClick(bool yes)
{
    if(yes)
        editor::editorwindow->SetTransition(LevelBrowser::StartSection);
}


void editor::onExitAttempt()
{
    if(! SavedFile)
    {
        if(!ExitPrompt)
        {
            ExitPrompt=true;
            dialogs::makeYesNoDialog(editorwindow,"Are you sure you want to close the editor before saving the level?","Yes","No",editor::onExitWithoutSavingClick);
        }
    }
    else
         cancel();
};

void editor::onQuitClick(const buttondata* data)
{
    onExitAttempt();
}

void editor::beforeDraw()
{
    subtic4++; if (subtic4==10)
    {
        subtic4=0;
        tic4++; if (tic4==4) tic4=0;
    }
}


void testSubWindowDo(bool okclicked, const string text, inputDialogData * dat)
{
    if(okclicked) printf(": %s\n",text.c_str());
    else printf("<--Cancel-->\n");

}

editor::setText_state_enum editor::setText_state;

editorobjecttype editor::SelectedType()
{
    return buttons->SelectedObjectType;
}

void editor::continueSetText(bool okclicked, const string text, inputDialogData * dat)
{
    if(! okclicked ) return;
    SavedFile=false;
    switch(setText_state)
    {
        case ASSIGN_TITLE:
            board->title=text;
            setText_state = ASSIGN_DESCRIPTION;
            dialogs::makeTextInputDialog(editorwindow,"Enter description",board->description, 3, "Ok", "Cancel", continueSetText,NULL);
            break;
        case ASSIGN_DESCRIPTION:
            board->description=text;
            setText_state = ASSIGN_AUTHOR;
            dialogs::makeTextInputDialog(editorwindow,"Enter level author(s)",board->author, 1, "Ok", "Cancel", continueSetText,NULL);
            break;

        case ASSIGN_AUTHOR:
            board->author=text;
            setText_state = ASSIGN_HINT;
            dialogs::makeTextInputDialog(editorwindow,"Enter level hint (Leave empty in case you do not want to add a hint)",board->hint, 3, "Ok", "Cancel", continueSetText,NULL);
            break;

        case ASSIGN_HINT:
            board->hint=text;
            setText_state = ASSIGN_BYE;
            dialogs::makeTextInputDialog(editorwindow,"Enter level end message (You can leave it empty)",board->bye, 3, "Ok", "Cancel", continueSetText,NULL);
            break;

        case ASSIGN_BYE:
            board->bye = text;


    }
}
void editor::beginSetText(const buttondata* data)
{
    setText_state=ASSIGN_TITLE;
    dialogs::makeTextInputDialog(editorwindow,"Enter title",board->title, 1, "Ok", "Cancel", continueSetText,NULL);

}

string stripDotXyeExtension(const string s)
{
    int l=s.length();
    if ( (l<4) || (s[l-1]!='e') || (s[l-2]!='y') || (s[l-3]!='x') || (s[l-4]!='.') ) return s+string(".xye");
    return s;

}

void editor::onClearConfirmation(bool yes)
{
    if(yes)
    {
        SavedFile=false;
        board->makeDefaultLevel();
    }
}

void editor::onClearClick(const buttondata* data)
{
    dialogs::makeYesNoDialog(editorwindow,"Clearing will restore the entire level to the default layout, are you sure you want to clear the level?","Yes","No",editor::onClearConfirmation);
}

string editor::myLevelsPath;

void editor::saveAs(bool okclicked, const string text, inputDialogData * dat)
{
    if(okclicked)
    {
        string tname=stripDotXyeExtension(text);
        string tfilename=myLevelsPath+tname;
        std::ifstream file;
        file.open (tfilename.c_str(), std::ios::in );
        if(file.is_open() && (filename!=tfilename) )
        {
            dialogs::makeMessageDialog(editorwindow, string("File ")+string(tfilename)+string(" already exists, unable to replace it."),"Ok",onDialogClickDoNothing);
            file.close();
            return;
        }

        filename=tfilename;
        filename_name=tname;
        filename_path=myLevelsPath;
        if (save())
        {
            dialogs::makeMessageDialog(editorwindow, string(filename)+string(" saved succesfully."),"Ok",onDialogClickDoNothing);
        }
    }
}

void editor::onSaveAsClick(const buttondata* data)
{
    dialogs::makeTextInputDialog(editorwindow,"Enter a new level file name","", 1, "Ok", "Cancel", saveAs,NULL);
}


void editor::onBrowseClick(const buttondata* data)
{
    if(! SavedFile)
    {
        dialogs::makeYesNoDialog(editorwindow,"Are you sure you want to close the editor before saving the level?","Yes","No",editor::onBrowseWithoutSavingClick);
    }
    else 
        editor::editorwindow->SetTransition(LevelBrowser::StartSection);
}


void editor::cancel()
{
    printf("Closing the editor without saving\n");
    editorwindow->stop();
}

void editor::ResumeSectionAndQuit(window* wind)
{
    ResumeSection(wind);
    editor::onQuitClick(NULL);
}

void editor::ResumeSection(window* wind)
{


    tic4=0;

    Width=6+XYE_HORZ*GRIDSIZE;
    Height=7+2+(GRIDSIZE+2)*4+ (XYE_VERT*GRIDSIZE+6) + GRIDSIZE + 3;
    editorwindow = wind;
    editorwindow->Resize(Width, Height);
    editorwindow->onExitAttempt=editor::onExitAttempt;


    SDL_WM_SetCaption("Xye - Editor",0);




    SDL_Color c;
    //Setup widgets settings:


    editorwindow->beforeDraw = editor::beforeDraw;
    editorwindow->onKeyDown= editor::onKeyDown;
    editorwindow->onKeyUp= editor::onKeyUp;

    editorwindow->addControl(new rectangle(0,0,Width,Height, 0,0,0 ) );




   control* tmcntrl=new rectangle(3,3,Width-6,sz, options::LevelMenu_info );
    tmcntrl->depth=1;
    editorwindow->addControl(tmcntrl);

    int bx=3;
    int bw;

    button* tmbut;

    bw=button::recommendedWidth("Set level text");
    tmbut= new button(bx,3,bw,button::Size);
    tmbut->text="Set level text";
    tmbut->onClick = beginSetText;
    tmbut->depth=20;
    editorwindow->addControl(tmbut);
    bx+=bw+1;


    bw=button::recommendedWidth("Clear");
    tmbut= new button(bx,3,bw,button::Size);
    tmbut->text="Clear";
    tmbut->onClick = onClearClick;
    tmbut->depth=20;
    editorwindow->addControl(tmbut);
    bx+=bw+1;

    bw=button::recommendedWidth("Test");
    tmbut= new button(bx,3,bw,button::Size);
    tmbut->text="Test";
    tmbut->onClick = editor::test;
    tmbut->depth=20;
    editorwindow->addControl(tmbut);
    bx+=bw+1;

    bw=button::recommendedWidth("Save");
    tmbut= new button(bx,3,bw,button::Size);
    tmbut->text="Save";
    tmbut->onClick = editor::buttonSave;
    tmbut->depth=20;
    savebutton=tmbut;
    editorwindow->addControl(tmbut);
    bx+=bw+1;

    bw=button::recommendedWidth("Save as");
    tmbut= new button(bx,3,bw,button::Size);
    tmbut->text="Save as";
    tmbut->onClick = editor::onSaveAsClick;
    tmbut->depth=20;
    savebutton=tmbut;
    editorwindow->addControl(tmbut);
    bx+=bw+1;

    bw=button::recommendedWidth("Browse");
    tmbut= new button(bx,3,bw,button::Size);
    tmbut->text="Browse";
    tmbut->onClick = editor::onBrowseClick;
    tmbut->depth=20;
    savebutton=tmbut;
    editorwindow->addControl(tmbut);
    bx+=bw+1;


    bw=button::recommendedWidth("Quit");
    tmbut= new button(Width-6 -bw,3,bw,button::Size);
    tmbut->text="Quit";
    tmbut->onClick = editor::onQuitClick;
    tmbut->depth=21;
    editorwindow->addControl(tmbut);
    bx+=bw+1;




    tmcntrl=new rectangle(3, XYE_VERT*sz + sz+9 , Width-6, 4+(sz+2)*4 , 255,255,255 );
    tmcntrl->depth=2;
    editorwindow->addControl(tmcntrl);


    board   = new editorboard(3,6+sz);
    editorboard::LoadCopy(board);
    
    buttons = new editorbuttons(5, XYE_VERT*sz + 11+sz , Width-10, 2+(sz+2)*4 );

    board->depth = 3;
    buttons->depth = 3;

    editorwindow->addControl(buttons);
    editorwindow->addControl(board);

}

void editor::StartSection(window* wind)
{ 
    ResumeSection(wind);
    if (!load())
    {
        dialogs::makeMessageDialog(editorwindow, editor::loadError,"Ok",onDialogClickDoNothing);
    }

    editorwindow = wind;
}

void editor::SetFile(const string &path, const string &file)
{
    filename_path=path;
    filename_name=file;
    filename=path+file;
}



void editor::Error(const char* msg)
{
    fprintf(stderr,"%s", msg);
    fprintf(stderr,"\n");
    throw (msg);
}

bool editorEscapePressed=false;
void editor::onKeyDown(SDLKey keysim, Uint16 unicode)
{
    switch(keysim)
    {
        case (SDLK_ESCAPE): //ESC
            editorEscapePressed=true;
            break;
    };

}

void editor::onKeyUp(SDLKey keysim, Uint16 unicode)
{
    switch(keysim)
    {
        case (SDLK_ESCAPE): //ESC
            if(editorEscapePressed)
            {
                editorEscapePressed=false;
                onExitAttempt();
            }
            break;
    };
}
void editor::test()
{
    string nfilename = filename+"~";
    if (! save(nfilename) )
    {
        dialogs::makeMessageDialog(editorwindow, string("Unable to test the level because xyedit cannot rewrite ")+string(filename+"~")+".","Ok",onDialogClickDoNothing);
        return;
    }
    //SDL_Surface* screen = editorwindow->getDrawingSurface();
    //SDL_FillRect(screen,0,0,editor::Width,editor::Height,SDL_MapRGB(screen->format,0,0,0));
    //SDL_Flip(screen);
    editorboard::SaveCopy(editor::board);
    game::TestLevel(nfilename.c_str(), 1);

    return;
    /*
    SDL_WM_SetCaption("Xye - Editor (Testing)",0);
    SDL_Surface* screen = editorwindow->getDrawingSurface();
    SDL_FillRect(screen,0,0,editor::Width,editor::Height,SDL_MapRGB(screen->format,0,0,0));
    SDL_Flip(screen);

    string commandline=options::ExecutablePath;
    commandline+=" --playlevel ";
    commandline+=editor::filename_name;
    commandline+="~";
    commandline+=" ";
    commandline+=options::Dir;

    Command::executeWaitForProcess(commandline);
    SDL_WM_SetCaption("Xye - Editor",0);*/
}


/*********** Plenty of object handling ***/





/*********** editorbuttons control! ***/
void editorbuttons::setInfo(const string & msg)
{
    text=msg;
    SelectedObjectType= EDOT_NONE;
    Eraser=true;

}

editorbuttons::editorbuttons(int sx, int sy, int sw, int sh)
{
    depth=0;
    x=sx;y=sy;w=sw;h=sh;
    mousex=mousey=0;
    clicked=mouse=false;

    for (int i=0;i<EDITORBUTTONS_COUNTX;i++)
        for (int j=0;j<EDITORBUTTONS_COUNTY;j++)
        {
            singleobject &o=buttons[i][j];
            o.content=CONTENT_NOCONTENT;
            o.selected=false;
            o.flash=false;
            o.color=EDCO_YELLOW;
            o.variation=0;
            o.round=false;
        }

    hover=NULL;
    clickedempty=NULL;
    clickedobject=NULL;
    selection=NULL;

    direction=0;
    text="Welcome to xyedit!";

    int bp=-1;
    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_XYE;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_GEM;
    buttons[bp][1].color=EDCO_BLUE;


    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_WALL;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_BLOCK;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_SPECIALBLOCKS;


    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_EARTH;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_GEMBLOCK;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_MAGNET;


    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_PUSHER;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_ARROWMAKER;


    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_HAZARD;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_ONEDIRECTION;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_BEAST;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_TELEPORT;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_TURNER;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_COLORSYSTEM;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_BOT;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_KEYSYSTEM;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_NUMBER;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_FIREPAD;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_RATTLERHEAD;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_FOOD;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_LARGEBLOCK;
    buttons[bp][1].variation = 4;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_PORTAL;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_COLORFACTORY;

    buttons[++bp][1].content=CONTENT_CHANGEOBJECT;
    buttons[bp][1].type=EDOT_DANGERFACTORY;

    SelectedObjectType= EDOT_NONE;
    Eraser=true;

}

singleobject * editorbuttons::getbuttonbyxy(int x, int y)
{

    x-=2; if (x<0) return NULL;
    int bx=x/(sz+2);



    if (bx>=EDITORBUTTONS_COUNTX) return NULL;
    y-=2; if (y<0) return NULL;

    int by=y/(sz+2);

    if (by>=EDITORBUTTONS_COUNTY) return NULL;

    lastclickedx=bx;
    lastclickedy=by;

 return &buttons[bx][by];
}

int editorbuttons::detectRotation(int x0, int y0, int x1, int y1)
{
    if((x0==x1)&&(y0==y1)) return 0;
    singleobject * a=getbuttonbyxy(x0,y0);
    singleobject * b=getbuttonbyxy(x1,y1);
    if ((a!=b) || (a==NULL)) return 0;
    x0= (x0-2)%(sz+2);if(x0>sz) x0=sz;
    x1= (x1-2)%(sz+2);if(x1>sz) x1=sz;
    y0= (y0-2)%(sz+2);if(y0>sz) y0=sz;
    y1= (y1-2)%(sz+2);if(y1>sz) y1=sz;

    int sz2=sz>>1;

    int q0,q1;

    if (x0>sz2)
    {
        if( y0>sz2) q0=3;
        else        q0=0;
    }
    else
    {
        if( y0>sz2) q0=2;
        else        q0=1;
    }
    if (x1>sz2)
    {
        if( y1>sz2) q1=3;
        else        q1=0;
    }
    else
    {
        if( y1>sz2) q1=2;
        else        q1=1;
    }
    if(q1==q0) return 0;
    if(q1== (q0+1)%4  ) return 1;
    return -1;
}

void editorbuttons::onMouseMove(int px,int py)
{
    singleobject * obj;
    obj=getbuttonbyxy(px,py);

    if((hover!=obj) && (hover!=NULL)) hover->flash=false;
    hover=obj;
    if (hover) hover->flash=true;

    if(clicked)
    {
        //Drag and drop, handle rotation...
        int rot=detectRotation(mousex,mousey,px,py);
        direction+=rot+4;
        direction%=4;
    }
    mousex=px;
    mousey=py;
    mouse=true;
}

void editorbuttons::onMouseOut()
{
    if(hover!=NULL)
    {
        hover->flash=false;
        hover=NULL;
    }
    mouse=false;
    clicked=false;
}

void editorbuttons::onMouseDown(int px,int py)
{
    clicked=true;
}

void ifnotnulldeselect(singleobject*s)
{
    if(s!=NULL) s->selected=false;
}
void ifnotnullselect(singleobject*s)
{
    if(s!=NULL) s->selected=true;
}



void editorbuttons::onMouseUp(int px,int py)
{
    clicked=false;
    singleobject* target=getbuttonbyxy(px,py);
    Eraser=false;
    if(target)
    {
        switch(target->content)
        {
            case CONTENT_NOCONTENT:
                if(clickedempty==target) break;
                ifnotnulldeselect(clickedempty);
                ifnotnulldeselect(selection);
                target->selected=true;
                clickedempty=target;
                text="Eraser tool";
                Eraser=true;
                break;

            case CONTENT_CHANGEOBJECT:

                ifnotnulldeselect(clickedempty); clickedempty=NULL;
                ifnotnulldeselect(clickedobject);
                ifnotnulldeselect(selection);


                switchToObject(target->type,target->color,target->round,target->variation);
                target->selected=true;
                selection=clickedobject=target;
                break;

            case CONTENT_RECOLOR:
                ifnotnulldeselect(clickedempty); clickedempty=NULL;
                ifnotnulldeselect(selection);
                switchToObject(SelectedObjectType,target->color, SelectedRound, SelectedVariation);
                target->selected=true;

                clickedobject->color=target->color;
                selection=target;
                break;


            case CONTENT_VARIATION:
                ifnotnulldeselect(clickedempty); clickedempty=NULL;
                ifnotnulldeselect(selection);
                switchToObject(SelectedObjectType,SelectedColor, SelectedRound, target->variation);
                target->selected=true;

                clickedobject->variation=target->variation;
                selection=target;
                break;

            case CONTENT_MAKEROUND:
                ifnotnulldeselect(clickedempty); clickedempty=NULL;
                ifnotnulldeselect(selection);
                switchToObject(SelectedObjectType,SelectedColor, target->round, SelectedVariation);
                target->selected=true;

                clickedobject->round=target->round;
                selection=target;


        }
    }
}

void editorbuttons::drawbutton(SDL_Surface* target,singleobject &o, int x, int y)
{
    if(o.selected || o.flash)
    {

        if(o.flash)
        {
            if(clicked) SDL_FillRect(target,x-2,y-2,editor::GRIDSIZE+4,editor::GRIDSIZE+4,SDL_MapRGB(target->format,200,200,255));
            else       SDL_FillRect(target,x-2,y-2,editor::GRIDSIZE+4,editor::GRIDSIZE+4,SDL_MapRGB(target->format,200,200,200));
        }
        else      SDL_FillRect(target,x-2,y-2,editor::GRIDSIZE+4,editor::GRIDSIZE+4,SDL_MapRGB(target->format,0,0,0));
        SDL_FillRect(target,x,y,editor::GRIDSIZE,editor::GRIDSIZE,SDL_MapRGB(target->format,255,255,255));

    }
    switch(o.content)
    {
        case CONTENT_NOCONTENT:
            //SDL_FillRect(target,x,y,editor::GRIDSIZE,editor::GRIDSIZE,SDL_MapRGB(target->format,0,0,120));
            break;
        case CONTENT_CHANGEOBJECT:
            drawObjectBySpecs( target,x,y,o.type,o.color, o.round, o.variation, direction);
            break;

        case CONTENT_RECOLOR:
            drawObjectBySpecs( target,x,y, SelectedObjectType, o.color, SelectedRound, SelectedVariation, direction);
            break;
        case CONTENT_VARIATION:
            drawObjectBySpecs( target,x,y, SelectedObjectType, SelectedColor, SelectedRound, o.variation, direction);
            break;
        case CONTENT_MAKEROUND:
            drawObjectBySpecs( target,x,y, SelectedObjectType, SelectedColor, o.round, SelectedVariation, direction);
            break;


    }
}

void editorbuttons::draw(SDL_Surface* target)
{

    SDL_FillRect(target,x,y,w,h,SDL_MapRGB(target->format,255,255,255));

    int i,j;
    for (i=0;i<EDITORBUTTONS_COUNTX;i++) for (j=0;j<EDITORBUTTONS_COUNTY;j++)
    {
        singleobject &o=buttons[i][j];

/*
        if ((clickedobject==&o) && (! o.selected) && (clickedempty==NULL) ) continue;

        if ((o.content==CONTENT_VARIATION) && (o.variation==SelectedVariation) && (! o.selected)) continue;
        if ((o.content==CONTENT_RECOLOR) && (o.color==SelectedColor) && (! o.selected)) continue;
        if ((o.content==CONTENT_MAKEROUND) && (SelectedRound==o.round) && (! o.selected)) continue;
*/
        drawbutton(target, o,2+x+i*(editor::GRIDSIZE+2),2+y+j*(editor::GRIDSIZE+2));

    }

    const char* c = text.c_str();
    int tw= editor::FontRes->TextWidth(c);
    int ty=2+y+EDITORBUTTONS_COUNTY*(sz+2)+(sz-editor::FontRes->Height())/2+2;
    int tx=(w-tw)/2;

    editor::FontRes->Write(target,x+tx,ty,c);

}

/**** editorbuttons object specific: ***/

string gettextRC(const char * base, editorcolor color, bool round)
{
    string res="";
    switch (color)
    {
        case EDCO_BLUE: res="Blue "; break;
        case EDCO_RED: res="Red "; break;
        case EDCO_GREEN: res="Green "; break;
        case EDCO_YELLOW: res="Yellow "; break;
        case EDCO_WHITE: res="White "; break;
        case EDCO_METAL: res="Metal "; break;
        case EDCO_WILD:  res="Wildcard "; break;
    }
    if(round) res+="round ";
    res+=base;
    if(color == EDCO_METAL) res+=" (resists fire)";
    return res;
}

const char* GetMonsterName(int variation)
{
    switch ((btype)(variation))
            {
                case BT_GNASHER: return "Gnasher";
                case BT_BLOB: return "Blob"; break;
                case BT_TWISTER: return "Twister"; break;
                case BT_SPIKE:  return "Spike"; break;
                case BT_STATIC : return "Inertia"; break;
                case BT_PATIENCE: return "Patience"; break;
                case BT_TIGER: return "Tiger"; break;
                case BT_DARD:     return "Dart"; break;
                case BT_RANGER:   return "Ranger"; break;
                case BT_SPINNER:  return "Spinner (clockwise)"; break;
                case BT_ASPINNER:  return "Spinner (anti-clockwise)"; break;
                case BT_BLOBBOSS:  return "Blob mind"; break;
                case BT_WARD:  return "Ward"; break;
                case BT_VIRUS:  return "Virus"; break;

            }
    return "?";
}
void editorbuttons::updateText( editorobjecttype ot, editorcolor color, bool round, int variation)
{
    switch(ot)
    {
        case EDOT_BOT: text="Bot";break;
        case EDOT_FOOD: text="Snake food";break;
        case EDOT_RATTLERHEAD:
            text="Snake";
            if(variation>0)
            {
                char lengthtex[20];
                sprintf(lengthtex," (Length: %d) ",variation);
                text+=lengthtex;
            }




            break;
        case EDOT_FIREPAD: text="Fire pad";break;


        case EDOT_XYE:
            text="Xye";
            if( variation!=0)
            {
                char livetex[8];
                sprintf(livetex,"%d",variation);
                text += " (+";
                text += livetex;
                if(variation==1) text += " extra life)";
                else text+=" extra lives)";
            }
            break;

        case EDOT_COLORSYSTEM:
            switch(variation)
            {
                case 0: text="Color door (closed)"; break;
                case 1: text="Color door (open)"; break;
                case 2: text="Color trapdoor (closed)"; break;
                case 3: text="Color trapdoor (open)"; break;
                case 4: text="Color marked area"; break;
                case 5: text="Color requirement-block"; break;
                case 6: text="Color marked area (with block)"; break;

            }
            break;

        case EDOT_KEYSYSTEM:
            if(variation) text="Lock";
            else text="Key";
            break;

        case EDOT_BLOCK:
            text=gettextRC("block",color,round); break;

        case EDOT_NUMBER:

            switch(color)
            {
                case EDCO_RED: text="Exploding number-block"; break;
                case EDCO_GREEN: text="Security timer block"; break;
                case EDCO_BLUE: text="Slow timer block"; break;
                case EDCO_YELLOW: text="Timer block"; break;

            }

            break;

        case EDOT_WALL:
            if(round)
            {
                if(variation==6) text="Wall (auto round, inside decoration)";
                else if (variation>3) text="Wall (auto-round, fire-resistant)";
                else text="Wall (auto-round)";
            }
            else
            {
                if(variation==6) text="Wall (inside decoration)";
                else if (variation>3) text="Wall (fire-resistant)";
                else text="Wall";
            }
            break;
        case EDOT_GEM:

            switch (color)
            {
                case EDCO_GREEN: text="Emerald (green gem)"; break;
                case EDCO_YELLOW: text="Topaz (yellow gem)"; break;
                case EDCO_BLUE: text="Diamond (blue gem)"; break;
                case EDCO_RED: text="Ruby (red gem)";
                default: text="Star (optional gem)";
            }
            break;

        case EDOT_EARTH:
            if(round) text="Round soft block";
            else  text="Soft block"; break;
        case EDOT_GEMBLOCK: text="Gem requirement block"; break;
        case EDOT_MAGNET:

            switch(variation)
            {
                case 0: text="Magnet"; break;
                case 2: text="Sticky block"; break;
                case 1: text="Inverse magnet"; break;

            }
            break;

        case EDOT_LARGEBLOCK:
            switch(variation)
            {
                case 0: text="Large block (1-0-0-0)"; break;
                case 1: text="Large block (1-1-0-0)"; break;
                case 2: text="Large block (1-0-1-0)"; break;
                case 3: text="Large block (1-1-1-0)"; break;
                case 4: text="Large block (1-1-1-1)"; break;
            }

            break;

        case EDOT_PORTAL:

            if(variation==0) text=gettextRC("portal (primary)",color,false);
            else if(variation==1) text=gettextRC("portal (secondary)",color,false);
            else text=gettextRC("portal (exit)",color,false);
           break;

        case EDOT_COLORFACTORY:
            switch(variation)
            {
                case 0: text=gettextRC("block factory",color,round); break;
                case 1: text=gettextRC("arrow block factory",color,round); break;
                case 2: text=gettextRC("dot-block factory",color,round); break;
                case 3: text=gettextRC("pusher factory",color,false); break;
                case 4: text=gettextRC("gem factory",color,false); break;
            }
            break;
        case EDOT_DANGERFACTORY:
            switch(variation)
            {
                case 14: text="Rattler factory"; break;
                case 15: text="Rattler food factory"; break;
                case 16: text="Land mine factory"; break;

                default: text=string(GetMonsterName(variation))+" factory";
            }
            break;


        case EDOT_PUSHER:
            switch(color)
            {
                case EDCO_YELLOW: text="Bouncer pusher"; break;
                case EDCO_RED: text="Persistent pusher"; break;
                case EDCO_BLUE: text="Clock-wise pusher"; break;
                case EDCO_GREEN: text="Anticlock-wise pusher"; break;

            }
            break;

        case EDOT_ARROWMAKER:

            if(variation==0) text=gettextRC("shooter clock",color ,round);
            else if (variation==1) text=gettextRC("filler",color ,round);
            else if (variation==2) text=gettextRC("sniper",color ,round);
           break;

        case EDOT_HAZARD:
            switch(variation)
            {
                case 0: text="Black hole"; break;
                case 1: text="Land mine"; break;
                case 2: text="Pit"; break;
            }

            break;
        case EDOT_ONEDIRECTION:
            if(!variation) switch(direction)
            {
                case EDITORDIRECTION_DOWN : text="One-way door (up->down)"; break;
                case EDITORDIRECTION_UP : text="One-way door (down->up)"; break;
                case EDITORDIRECTION_LEFT : text="One-way door (right->left)"; break;
                case EDITORDIRECTION_RIGHT : text="One-way door (left->right)"; break;
            }
            else text="Force arrow";

            break;

        case EDOT_TELEPORT: text="Teleport"; break;

        case EDOT_TURNER:
            if(variation) text=gettextRC("turning block (anticlock-wise)",color ,round);
            else text=gettextRC("turning block (clock-wise)",color ,round);
            break;

        case EDOT_SPECIALBLOCKS:
            switch(variation)
            {
                case 0: text=gettextRC("arrow-block",color,round);break;
                case 1: text=gettextRC("scroll-block",color,round);break;
                case 2: text=gettextRC("toggle-block (on)",color,round);break;
                case 3: text=gettextRC("toggle-block (off)",color,round);break;
                case 4: text=gettextRC("dot-block",color,round);break;
                case 5: text=gettextRC("surprise! block",color,round);break;
            }
            break;

        case EDOT_BEAST:
            text = GetMonsterName(variation);

            break;
        default:
            text = "unknown";
    }
}

void editorbuttons::extendButtons( editorobjecttype ot, editorcolor color, bool round, int variation)
{
    bool roundchoice=false;
    int maxvariations=0;
    int colorchoice = 0;

    switch(ot)
    {
        case EDOT_GEM: colorchoice=2; break;

        case EDOT_COLORSYSTEM: colorchoice=1; maxvariations=7; break;
        case EDOT_RATTLERHEAD: maxvariations=10; break;

        case EDOT_NUMBER: colorchoice=1; roundchoice=1; maxvariations=10; break;

        case EDOT_BLOCK: colorchoice=4; roundchoice=true; break;
        case EDOT_TURNER: colorchoice=2; roundchoice=true; maxvariations=2; break;


        case EDOT_XYE: maxvariations=4; break;
        case EDOT_WALL: maxvariations=7; roundchoice=true; break;
        case EDOT_MAGNET: maxvariations=3; break;

        case EDOT_EARTH: roundchoice=true; break;

        case EDOT_KEYSYSTEM: colorchoice=1; maxvariations=2; break;
        case EDOT_SPECIALBLOCKS: roundchoice=true; colorchoice=1; maxvariations=6; break;
        case EDOT_GEMBLOCK: colorchoice=1; break;




        case EDOT_ARROWMAKER: colorchoice=1; roundchoice=true; maxvariations=3; break;


        case EDOT_PUSHER: colorchoice=1; break;

        case EDOT_HAZARD: maxvariations=3; break;
        case EDOT_ONEDIRECTION: maxvariations=2; break;
        case EDOT_BEAST: maxvariations=14; break;

        case EDOT_LARGEBLOCK: maxvariations=5; colorchoice=2;  break;
        case EDOT_PORTAL: maxvariations=3; colorchoice=2;  break;
        case EDOT_COLORFACTORY: maxvariations=5; colorchoice=1; roundchoice=1;  break;
        case EDOT_DANGERFACTORY: maxvariations=17;  break;

        //default : //EDOT_TELEPORT,EDOT_BOT,EDOT_FIREPAD, EDOT_FOOD
    }

    int roundstart=0;
    int colorstart=3;
    int variationstart=0;
    int colorcount=0;
    if(colorchoice == 4) colorcount=7;
    else if(colorchoice == 3) colorcount=6;
    else if (colorchoice == 2) colorcount=5;
    else if (colorchoice == 1) colorcount=4;

    if( roundchoice && colorchoice )
    {
        roundstart=lastclickedx - 2;
        if(roundstart<0) roundstart=0;
        colorstart=roundstart + 3;

        if (colorstart+colorcount>=EDITORBUTTONS_COUNTX)
        {
            colorstart=EDITORBUTTONS_COUNTX-colorcount-1;
            roundstart=colorstart-3;
        }
    }
    else if (roundchoice)
    {
        roundstart=lastclickedx;
        if (roundstart+1>=EDITORBUTTONS_COUNTX) roundstart=EDITORBUTTONS_COUNTX-2;
    }
    else if (colorchoice)
    {
        colorstart=lastclickedx;
        if (colorstart+colorcount>=EDITORBUTTONS_COUNTX)
            colorstart=EDITORBUTTONS_COUNTX-colorcount-1;
    }

    if(maxvariations)
    {
        variationstart=lastclickedx;
        if (variationstart+maxvariations-1>=EDITORBUTTONS_COUNTX)
            variationstart=EDITORBUTTONS_COUNTX-maxvariations;
    }

    if(roundchoice)
    {

        for (int i=0;i<2;i++)
        {
            singleobject &o=buttons[roundstart+i][0];
            o.content= CONTENT_MAKEROUND;
            o.round=(bool)(i);
            /*if ((bool)(i)== round)
            {
                o.selected=true;
                clickedround=&o;
            }*/
        }
    }

    for (int i=0;i<colorcount;i++)
    {
        singleobject &o=buttons[i+colorstart][0];
        o.content= CONTENT_RECOLOR;
        o.color=(editorcolor)(i);
    }

    if(maxvariations>0)
    {
        for (int i=0;i<maxvariations;i++)
        {
            singleobject &o=buttons[variationstart+i][2];
            o.content=CONTENT_VARIATION;
            o.variation=i;
            /*if (i==variation)
            {
                o.selected=true;
                clickedvariation=&o;
            }*/
        }
    }

}

void editorbuttons::switchToObject( editorobjecttype ot, editorcolor color, bool round, int variation)
{
    updateText(ot,color,round,variation);

    if(SelectedObjectType!=ot)
    {
        for (int i=0;i<EDITORBUTTONS_COUNTX;i++)
        {
            singleobject &o=buttons[i][0];
            o.content=CONTENT_NOCONTENT;
            o.selected=false;

            singleobject &p=buttons[i][2];
            p.content=CONTENT_NOCONTENT;
            p.selected=false;
        }

        extendButtons(ot,color,round,variation);
    }


    SelectedColor = color;
    SelectedObjectType=ot;
    SelectedRound=round;
    SelectedVariation=variation;
}

/*** editorboard control **/

void makewall(boardelement &o)
{
    o.type=EDOT_WALL;
    o.variation=0;
    o.round=false;
}

editorboard editorboard::copy(0,0);

void editorboard::assign(editorboard* other)
{
    for (int i=0; i<XYE_HORZ; i++)
        for (int j=0; j<XYE_VERT; j++)
            objects[i][j] = other->objects[i][j];

    xye_x = other->xye_x;
    xye_y = other->xye_y;
    for (int i=0; i<5; i++)
        for (int j=0; j<2; j++)
        {
            portal_x[i][j] = other->portal_x[i][j];
            portal_y[i][j] = other->portal_y[i][j];
        }
    title=other->title;
    description=other->description;
    hint=other->hint;
    bye=other->bye;
    author=other->author;
    
}

void editorboard::SaveCopy(editorboard* ed)
{
    copy.assign(ed);
}
void editorboard::LoadCopy(editorboard* ed)
{
    ed->assign(&copy);
}

void editorboard::makeDefaultLevel()
{
    int i,j;
    bool already = true;
    for (i=0;i<XYE_HORZ;i++)for (j=0;j<XYE_VERT;j++)
    {
        if (objects[i][j].type != EDOT_NONE)
        {
            if(objects[i][j].type==EDOT_WALL)
            {
                already = already && ( (i==0) || (j==0) || (i==XYE_HORZ-1) || (j==XYE_VERT-1));
            }
            else already = false;
        }
        else if( (i==0) || (j==0) || (i==XYE_HORZ-1) || (j==XYE_VERT-1))
            already=false;

        objects[i][j].type=EDOT_NONE;
    }
    if( ! already)
    {
        //fill border walls
        for (j=0;j<XYE_VERT;j++)
        {
            makewall(objects[0][j]);
            makewall(objects[XYE_HORZ-1][j]);
        }
        for (j=0;j<XYE_HORZ;j++)
        {
            makewall(objects[j][0]);
            makewall(objects[j][XYE_VERT-1]);
        }
    }
    xye_x=-1;
    xye_y=-1;
    for (int i=0; i<5; i++)
        for (int j=0; j<2; j++)
            portal_x[i][j]= portal_y[i][j]=-1;
}

editorboard::editorboard(int sx, int sy)
{
    author="xyedit";
    description="Generated by xyedit";
    title=editor::filename_name;
    hint="";

    depth=0;
    x=sx;y=sy;
    w=sz*XYE_HORZ;
    h=sz*XYE_VERT;
    mousex=mousey=0;
    clicked=mouse=false;

    makeDefaultLevel();


}
void editorboard::onMouseMove(int px,int py)
{
    if(clicked)
    {
        int bi=px/sz,bj=py/sz;
        applyFromButtons(bi,bj);

    }
    mousex=px;
    mousey=py;
    mouse=true;
}

void editorboard::onMouseOut()
{
    mouse=false;
    clicked=false;
}

void editorboard::onMouseDown(int px,int py)
{
    int bi=px/sz,bj=py/sz;
    applyFromButtons(bi,bj);
    clicked=true;
}

void editorboard::onMouseUp(int px,int py)
{
    clicked=false;
}


void editorboard::onMouseRightUp(int px,int py)
{
    int bi=px/sz,bj=py/sz;


    bool er=editor::buttons->Eraser;
    editor::buttons->Eraser=true;
    applyFromButtons(bi,bj);
    editor::buttons->Eraser=er;

}


void editorboard::drawRoundWall(SDL_Surface*target,int ox,int oy, int x, int y, int variation)
{
    bool r7,r9,r1,r3;
    r7=r9=r1=r3=true;

    if( (ox>0) && (objects[ox-1][oy].type==EDOT_WALL) ) r7=r1=false;
    if( (ox<XYE_HORZ-1) && (objects[ox+1][oy].type==EDOT_WALL) ) r9=r3=false;

    if( (oy>0) && (objects[ox][oy-1].type==EDOT_WALL) ) r9=r7=false;
    if( (oy<XYE_VERT-1) && (objects[ox][oy+1].type==EDOT_WALL) ) r1=r3=false;

    boardelement &o=objects[ox][oy];
    o.r1mem=(Uint8)(r1);
    o.r7mem=(Uint8)(r7);
    o.r9mem=(Uint8)(r9);
    o.r3mem=(Uint8)(r3);


    Sint16 sz2=sz/2;
    Sint16 tx,ty;
    ty=variation+1;
    if(variation==6) ty=0;

    tx=9*sz;
    if(r7) tx+=sz;

    DaVinci D(editor::sprites,tx,ty*sz,sz2,sz2);
    D.Draw(target,x,y);

    tx=9*sz+(sz2);
    if(r9) tx+=sz;
    D.ChangeRect(tx,ty*sz,sz2,sz2);
    D.Draw(target,x+sz2,y);


    tx=9*sz;
    if(r1) tx+=sz;
    D.ChangeRect(tx,ty*sz+sz2,sz2,sz2);
    D.Draw(target,x,y+sz2);

    tx=9*sz+sz2;
    if(r3) tx+=sz;
    D.ChangeRect(tx,ty*sz+sz2,sz2,sz2);
    D.Draw(target,x+(sz2),y+sz2);


}


void editorboard::draw(SDL_Surface* target)
{
    int i,j;
    SDL_FillRect(target,x,y,w,h,SDL_MapRGB(target->format,255,255,255));
    for (i=0;i<XYE_HORZ;i++)for (j=0;j<XYE_VERT;j++)
    {
        boardelement &o=objects[i][j];
        if(o.type!=EDOT_NONE)
        {
            if (( o.type==EDOT_WALL) && (o.round))
                drawRoundWall(target,i,j,x+i*sz,y+j*sz,o.variation);
            else if ( o.type == EDOT_LARGEBLOCK )
                drawLargeBlockInBoard(target,i,j,x+i*sz,y+j*sz,o.color, o.variation, o.direction);
            else
                drawObjectBySpecs(target,x+i*sz,y+j*sz, o.type, o.color, o.round, o.variation, o.direction);
        }

    }
}

void editorboard::enforceUniquePortals(int x, int y, int variation, editorcolor color)
{
    int cid = (int)(color);
    int use = 0;

    if(variation > 0 ) use = 1;


    int tx= portal_x[cid][use], ty=portal_y[cid][use];
    if(tx!=-1)
    {
        objects[tx][ty].type=EDOT_NONE;
    }
    portal_x[cid][use]=x;
    portal_y[cid][use]=y;



}

void editorboard::applyFromButtons(int x, int y)
{
    if((x<0) || (y<0) || (x>=XYE_HORZ) || (y>=XYE_VERT)) return ;


    editor::SavedFile=false;
    boardelement &o=objects[x][y];
    if((x==xye_x) && (y==xye_y))
    {
        xye_x=-1;
        xye_y=-1;
    }
    for (int i=0; i<5; i++)
        for (int j=0; j<2; j++)
            if((x==portal_x[i][j]) && (y==portal_y[i][j]))
            {
                portal_x[i][j] = portal_y[i][j] = -1;
            }



    if (editor::buttons->Eraser)
    {
        o.type=EDOT_NONE;
        return;
    }

    switch(editor::buttons->SelectedObjectType)
    {
        case EDOT_XYE:
            if(xye_x!=-1) objects[xye_x][xye_y].type=EDOT_NONE;
                xye_x=x;
                xye_y=y;
            break;
        case EDOT_PORTAL:
            enforceUniquePortals(x,y, editor::buttons->SelectedVariation, editor::buttons->SelectedColor);
            break;
    }


    o.type=editor::buttons->SelectedObjectType;
    o.color=editor::buttons->SelectedColor;
    o.variation=editor::buttons->SelectedVariation;
    o.round=editor::buttons->SelectedRound;
    o.direction=editor::buttons->direction;
    o.parentx = o.parenty = -1;
}

/****** object drawing, Very painful ****/

void drawXye( SDL_Surface * target, int x, int y, int variation)
{
    DaVinci D(editor::sprites,0,0,sz,sz);
    D.SetColors( &game::PlayerColor);
    D.Draw(target,x,y);
    if(variation)
    {
        char vartext[3];
        vartext[0]='+';
        vartext[1]=(char)variation+'0';
        vartext[2]='\0';
        editor::FontRes->Write(target,x,y,vartext);
    }
}


void drawRattlerHead( SDL_Surface * target, int x, int y, int variation, int direction)
{
    Uint8 tx,ty;


    switch(direction)
    {
        case EDITORDIRECTION_LEFT: tx=12,ty=10; break;
        case EDITORDIRECTION_UP: tx=13,ty=10; break;
        case EDITORDIRECTION_RIGHT: tx=14,ty=10; break;
        case EDITORDIRECTION_DOWN: tx=15,ty=10; break;
    }
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    if(variation)
    {
        D.SetColors(255,255,255,70);
    }
    D.Draw(target,x,y);

    if(variation)
    {
        char vartext[3];
        vartext[0]='+';
        vartext[1]=(char)variation+'0';
        vartext[2]='\0';
        editor::FontRes->Write(target,x,y,vartext);
    }
}



void drawGem( SDL_Surface * target, int x, int y, editorcolor color)
{
    Uint8 tx,ty;
    ty=3;
    switch(color)
    {
        case(EDCO_BLUE): tx=2; break;
        case(EDCO_RED): tx=3; break;
        case(EDCO_GREEN): tx=4; break;
        case(EDCO_YELLOW): tx=5; break;
        default:
           ty = 13, tx=9;
    }
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    D.Draw(target,x,y);
}

void drawWall( SDL_Surface * target, int x, int y, bool round, int variation)
{
    Uint8 tx,ty;
    ty=variation+1;
    if(ty==7) ty=0;
    if(round) tx=10;
    else tx=9;
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
    D.Draw(target,x,y);
}

void drawBlock( SDL_Surface * target, int x, int y, bool round, editorcolor color)
{
    Uint8 tx,ty=0;
    if(round) tx=2;
    else tx=1;
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
    if( color==EDCO_WILD)
    {
        ty = 2, tx = 1+round;
        D.ChangeRect(tx*sz,ty*sz,sz,sz);
    }
    else if(color==EDCO_METAL)
    {
        ty = 8, tx = 9+round;
        D.ChangeRect(tx*sz,ty*sz,sz,sz);
    }
    else if(color!=EDCO_WHITE)
    {
        D.SetColors(&options::BKColor[color],255);
    }
    D.Draw(target,x,y);
}

void drawPortal( SDL_Surface * target, int x, int y, editorcolor color, int variation)
{
    Uint8 tx=8,ty=0;
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    Uint8 alpha = 255;
    if(variation>0) alpha = 127;
    if(color!=EDCO_WHITE)
    {
        D.SetColors(&options::BKColor[color],alpha);
    }
    else if(alpha!=255) D.SetColors(255,255,255,alpha);
    D.Draw(target,x,y);
    if(variation==2)
    {
        tx=4, ty=8;
        D.ChangeRect(tx*sz,ty*sz,sz,sz);
        D.SetColors(0,0,0,alpha);
        D.Draw(target,x,y);
    }
}

void drawMetalBlock( SDL_Surface * target, int x, int y, bool round)
{
    Uint8 tx,ty=8;
    if(round) tx=10;
    else tx=9;
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
    D.Draw(target,x,y);
}

void drawGemBlock( SDL_Surface * target, int x, int y, editorcolor color)
{
    Uint8 tx=6,ty=9;
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    D.SetColors(&options::BKColor[color],255);
    D.Draw(target,x,y);

    tx=8;
    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.SetColors(&options::BFColor[color],255);
    D.Draw(target,x,y);
}

void drawColorSystem( SDL_Surface * target, int x, int y, editorcolor color, int variation)
{
    Uint8 tx,ty;

    switch (variation)
    {
        case 0: /*closed door*/ tx=7,ty=5; break;
        case 1: /*open door*/ tx=7,ty=7; break;
        case 2: /*closed trap*/ tx=8,ty=5; break;
        case 3: /*open trap*/ tx=8,ty=7; break;
        case 4: /*marked*/ tx=6,ty=5; break;
        case 5: /*window*/ tx=9,ty=7; break;
        case 6:
            drawBlock(target,x,y,false,color);
            tx=6,ty=5;
            break;

    }

    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    D.SetColors(&options::BKColor[color],255);
    D.Draw(target,x,y);

}



void drawKeySystem( SDL_Surface * target, int x, int y, editorcolor color, int variation)
{
    Uint8 tx,ty;

    switch (variation)
    {
        case 0: /*key*/ tx=6,ty=4; break;
        case 1: /*lock*/ tx=7,ty=4; break;

    }

    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    D.SetColors(&options::BKColor[color],255);
    D.Draw(target,x,y);

}



void drawEarth( SDL_Surface * target, int x, int y, bool round)
{
    Uint8 tx,ty=1;
    if(round) tx=2;
    else tx=1;
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
    D.SetColors(&options::BKColor[0],255);
    D.Draw(target,x,y);
}



void drawNumber( SDL_Surface * target, int x, int y, editorcolor color, bool round, int variation)
{
    drawBlock(target,x,y,round,color);


    Uint8 tx,ty;
    if(variation==0) tx=0,ty=9;
    else if (variation<=3) tx=variation-1,ty=10;
    else if (variation<=6) tx=variation-4,ty=11;
    else tx=variation-7,ty=12;

    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    D.SetColors(&options::BFColor[color],255);
    D.Draw(target,x,y);
}


void drawSpecialBlocks( SDL_Surface * target, int x, int y, bool round, editorcolor color, int variation, int direction)
{
    drawBlock(target,x,y,round,color);


    Uint8 tx,ty;
    if (variation==0)
    {
        switch(direction)
        {
            case(EDITORDIRECTION_RIGHT): tx=4; ty=9; break;
            case(EDITORDIRECTION_LEFT): tx=5; ty=9; break;
            case(EDITORDIRECTION_UP): tx=5; ty=10; break;
            default: tx=4; ty=10;
        }
    }
    else if (variation==1)
    {
        switch(direction)
        {
            case(EDITORDIRECTION_RIGHT): tx=4; ty=16; break;
            case(EDITORDIRECTION_LEFT): tx=5; ty=16; break;
            case(EDITORDIRECTION_UP): tx=5; ty=17; break;
            default: tx=4; ty=17;
        }

    }
    else if(variation==2) tx=3,ty=8;
    else if (variation==3) tx=3,ty=9;
    else if (variation==4) tx=3,ty=12;
    else tx=3,ty=7;

    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    D.SetColors(&options::BFColor[color],255);
    D.Draw(target,x,y);
}

void drawTurner( SDL_Surface * target, int x, int y, bool round, editorcolor color, int variation)
{
    drawBlock(target,x,y,round,color);


    Uint8 tx=3,ty;
    if(variation) ty=10;
    else ty=11;

    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    if(color!=EDCO_WHITE) D.SetColors(&options::BFColor[color],255);
    else D.SetColors(0,0,0,255);
    D.Draw(target,x,y);
}

/*
 *   0 | 1 | 2
 *   7 -   - 3
 *   6 | 5 | 4
 *
 * */
void drawLargeBlockByFlags( SDL_Surface * target, int x, int y, editorcolor color, Uint8 flags, bool doalpha=false)
{
    Uint8 tx,ty;
    Uint8 sz2 = sz>>1;

    //flags = 0b1101;


    Uint8 up = (flags>>1)&1;
    Uint8 right = (flags>>3)&1;
    Uint8 down = (flags>>5)&1;
    Uint8 left = (flags>>7)&1;

    Uint8 upleft = (flags>>0)&1;
    Uint8 upright = (flags>>2)&1;
    Uint8 downright = (flags>>4)&1;
    Uint8 downleft = (flags>>6)&1;

    //top left corner:
    Uint8 var = 0;
    if( up&&left&&upleft) var=4;
    else if( up&&left) var = 3;
    else if(up) var = 2;
    else if(left) var = 1;

    tx = 10;
    ty = var + 15;
    DaVinci D(editor::sprites, tx*sz,  ty*sz,sz2,sz2);
    Uint8 alpha = 255, white=255;
    if( doalpha) alpha = 128;
    if(editor::buttons->SelectedObjectType == EDOT_LARGEBLOCK) white = 220;

    if(color!=EDCO_WHITE) D.SetColors(&options::BKColor[color],alpha);
    else D.SetColors(white,white,white,alpha);
    D.Draw(target,x,y);

    //top right corner:
    var = 0;
    if( up&&right&&upright) var=4;
    else if( up&&right) var = 3;
    else if(up) var = 2;
    else if(right) var = 1;

    tx = 10;
    ty = var + 15;
    D.ChangeRect(tx*sz+sz2, ty*sz, sz2,sz2);
    D.Draw(target,x+sz2,y);

    //bottom left corner:
    var = 0;
    if( down&&left&&downleft) var=4;
    else if( down&&left) var = 3;
    else if(down) var = 2;
    else if(left) var = 1;

    tx = 10;
    ty = var + 15;
    D.ChangeRect(tx*sz,  ty*sz + sz2,sz2,sz2);
    D.Draw(target,x,y+sz2);

    //bottom right corner:
    var = 0;
    if( down&&right&& downright) var=4;
    else if( down&&right) var = 3;
    else if(down) var = 2;
    else if(right) var = 1;

    tx = 10;
    ty = var + 15;
    D.ChangeRect(tx*sz+sz2, ty*sz +sz2, sz2,sz2);
    D.Draw(target,x+sz2,y+sz2);

}

Uint8 getLargeBlockFlagsByVarDir( int variation, int direction)
{
    /*     |0|
     *    -   -
     *    3   1
     *    -|2|  */
    Uint8 flags = 0;

    switch(variation)
    {
        case 0: flags = /*0b0001*/ 1; break;
        case 1: flags = /*0b0011*/ 3; break;
        case 2: flags = /*0b0101*/ 5; break;
        case 3: flags = /*0b1011*/11; break;
        case 4: flags = /*0b1111*/15; break;
    }
    switch(direction)
    {
        case EDITORDIRECTION_DOWN: //bit 0 to 2
            flags = ( ((flags<<2)&/*0b1111*/15) | (flags>>2) );
            break;
        case EDITORDIRECTION_UP: //bit 0 to 0
            break;
        case EDITORDIRECTION_LEFT: //bit 0 to 3
            flags = ( ((flags<<3)&/*0b1111*/15) | (flags>>1) );
            break;
        case EDITORDIRECTION_RIGHT: //bit 0 to 1
            flags = ( ((flags<<1)&/*0b1111*/15) | (flags>>3) );
            break;
    }
    Uint8 up = flags&1;
    Uint8 right = (flags>>1)&1;
    Uint8 down = (flags>>2)&1;
    Uint8 left = (flags>>3)&1;
    Uint8 nflags = (up<<1)|(right<<3)|(down<<5)|(left<<7);
    return nflags;
}

void drawLargeBlock( SDL_Surface * target, int x, int y, editorcolor color, int variation, int direction)
{
    Uint8 nflags = getLargeBlockFlagsByVarDir(variation, direction);
    drawLargeBlockByFlags( target, x,y, color, nflags);
}

Uint8 largeBlockDFS[XYE_HORZ][XYE_VERT] = {};

void dfsLargeBlocks(int x, int y, editorcolor color, int px, int py, editorboard* eb)
{

    if(largeBlockDFS[x][y] != editor::tic4)
    {

        largeBlockDFS[x][y] = editor::tic4;
        boardelement &o=eb->objects[x][y];
        Uint8 nflags = getLargeBlockFlagsByVarDir(o.variation, o.direction);


        o.parentx = px,
        o.parenty = py;
        int dx[4] = { 0, 1, 0, -1};
        int dy[4] = { -1, 0, 1, 0};
        int df[4] = { 1, 3, 5, 7};
        int dop[4] = { 5, 7, 1, 3};
        for (int r=0; r<4; r++)
        {
            int nx = dx[r]+x, ny = dy[r]+y;
            if(nx<0) nx = XYE_HORZ-1;
            if(ny<0) ny = XYE_VERT-1;
            if(ny>=XYE_VERT) ny = 0;
            if(nx>=XYE_HORZ) nx = 0;

            boardelement &o2 = eb->objects[nx][ny];
            Uint8 nflags2 = getLargeBlockFlagsByVarDir(o2.variation, o2.direction);

            if( ((nflags &( 1<<df[r]) ) && (nflags2 &( 1<<dop[r]) ) ) && (o2.type==EDOT_LARGEBLOCK ) && (o2.color==color )  )
            {
                dfsLargeBlocks(nx,ny, color,px,py, eb);
            }
        }

    }
}

void editorboard::drawLargeBlockInBoard(SDL_Surface * target, int ox,int oy, int x, int y, editorcolor color, int variation, int direction)
{
    Uint8 flags = getLargeBlockFlagsByVarDir(variation, direction);
    Uint8 nflags = 0;
    {
        dfsLargeBlocks(ox,oy, color, ox,oy, this);
        boardelement &o = objects[ox][oy];
        int dy[8] = {-1,-1,-1, 0, 1,1, 1, 0};
        int dx[8] = {-1, 0, 1, 1, 1,  0,-1, -1};
        for (int r=0; r<8; r++)
        {
            int nx = dx[r]+ox, ny = dy[r]+oy;
            if(nx<0) nx = XYE_HORZ-1;
            if(ny<0) ny = XYE_VERT-1;
            if(ny>=XYE_VERT) ny = 0;
            if(nx>=XYE_HORZ) nx = 0;
            boardelement &o2 = objects[nx][ny];
            if( (o2.type==EDOT_LARGEBLOCK) && (o2.parentx == o.parentx) && (o.parenty==o2.parenty)) {
                nflags|=(1<<r);
            }
        }

    }
    bool doalpha = false;
    if( /*( editor::tic4 > 1) &&*/ ( editor::buttons->SelectedObjectType == EDOT_LARGEBLOCK) )
    {

        if ( flags != (nflags-(nflags&/*0b01010101*/85) ) )
        {
            doalpha = true;
            if(editor::tic4<2)
            {
                drawLargeBlockByFlags( target, x,y, color, nflags , doalpha);
                return;
            }

        //    nflags |= flags;
        }
        nflags = flags|(nflags&/*0b01010101*/85);
    }
    drawLargeBlockByFlags( target, x,y, color, nflags , doalpha);
}

void drawMagnet( SDL_Surface * target, int x, int y, int variation, int direction)
{
    Uint8 tx,ty;

    bool vert=((direction==EDITORDIRECTION_DOWN)||(direction==EDITORDIRECTION_UP));
    if (vert) ty=12;
    else ty=13;

    tx=6+variation;


    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);

    D.Draw(target,x,y);
}

void drawPusher( SDL_Surface * target, int x, int y, editorcolor color, int direction)
{
    Uint8 tx,ty;

    switch(direction)
    {
        case EDITORDIRECTION_RIGHT :tx=4;ty=5; break;
        case EDITORDIRECTION_LEFT :tx=5;ty=5; break;
        case EDITORDIRECTION_DOWN :tx=4;ty=6; break;
        case EDITORDIRECTION_UP :tx=5;ty=6; break;
    }
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
    D.SetColors(&options::BKColor[color],255);
    D.Draw(target,x,y);

    ty+=2;
    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.SetColors(&options::BFColor[color],255);
    D.Draw(target,x,y);
}

void drawArrowMaker( SDL_Surface * target, int x, int y, bool round, editorcolor color, int direction, int variation)
{
    Uint8 dx,dy,tx,ty;

    switch(direction)
    {
        case EDITORDIRECTION_RIGHT :dx=0;dy=0; break;
        case EDITORDIRECTION_LEFT :dx=1;dy=0; break;
        case EDITORDIRECTION_DOWN :dx=0;dy=1; break;
        case EDITORDIRECTION_UP :dx=1;dy=1; break;
    }

    switch(variation)
    {
        case 0: tx=2;ty=7; break;
        case 1: tx=1;ty=15; break;
        case 2: tx=3;ty=15; break;
    }
    if(round)tx--;
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
    D.SetColors(&options::BKColor[color],255);
    D.Draw(target,x,y);

    switch(variation)
    {
        case 0: tx=1+dx;ty=8+dy; break;
        case 1: tx=dx;ty=16+dy; break;
        case 2: tx=2+dx;ty=16+dy; break;
    }


    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.SetColors(&options::BFColor[color],255);
    D.Draw(target,x,y);


}

void drawHazard( SDL_Surface * target, int x, int y, int variation)
{
    Uint8 tx,ty;
    if(variation==0)
    {
        DaVinci Black(editor::sprites,0,sz*4,sz,sz);
        Black.Draw(target,x,y);
    }

    switch(variation)
    {
        case 0: tx=0;ty=3; break;
        case 1: tx=0;ty=7; break;
        case 2: tx=4;ty=19; break;
    }
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
    D.Draw(target,x,y);
}


void drawTeleport( SDL_Surface * target, int x, int y, int direction)
{
    Uint8 tx,ty;
    switch(direction)
    {
        case EDITORDIRECTION_RIGHT: tx=4; break;
        case EDITORDIRECTION_DOWN: tx=5; break;
        case EDITORDIRECTION_LEFT: tx=6; break;
        default: /*up*/            tx=7;
    }
    ty=2;
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
    D.Draw(target,x,y);
}


void drawBot( SDL_Surface * target, int x, int y)
{
    DaVinci D(editor::sprites,3*sz,1*sz,sz,sz);
    D.Draw(target,x,y);
}

void drawFirePad( SDL_Surface * target, int x, int y)
{
    DaVinci D(editor::sprites,2*sz,5*sz,sz,sz);
    D.Draw(target,x,y);
}

void drawFood( SDL_Surface * target, int x, int y)
{
    DaVinci D(editor::sprites,11*sz,13*sz,sz,sz);
    D.Draw(target,x,y);
}

void drawWildCard( SDL_Surface * target, int x, int y, bool round)
{
    Uint8 tx;
    if(round)tx=2;
    else tx=1;
    DaVinci D(editor::sprites,tx*sz,2*sz,sz,sz);
    D.Draw(target,x,y);
}

void drawOneDir( SDL_Surface * target, int x, int y, int direction, int variation)
{
    Uint8 tx,ty;
    if(variation) //ground arrow
    {
        switch(direction)
        {
            case EDITORDIRECTION_DOWN:  tx=9; ty=11; break;
            case EDITORDIRECTION_UP:  tx=10; ty=11; break;
            case EDITORDIRECTION_RIGHT:  tx=9; ty=10; break;
            default:  tx=10; ty=10; break;
        }
        DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
        D.SetColors(100,200,200,255);
        D.Draw(target,x,y);
    }
    else //door
    {
        ty=10;
        if( (direction==EDITORDIRECTION_DOWN) || (direction==EDITORDIRECTION_UP) ) tx=7;
        else tx=8;

        DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
        D.SetColors(255,0,0,255);
        D.Draw(target,x,y);

        switch(direction)
        {
            case EDITORDIRECTION_DOWN:  tx=4; ty=10; break;
            case EDITORDIRECTION_UP:  tx=5; ty=10; break;
            case EDITORDIRECTION_RIGHT:  tx=4; ty=11; break;
            default:  tx=5; ty=11; break;
        }
        D.ChangeRect(tx*sz,ty*sz,sz,sz);
        D.SetColors(0,0,0,120);
        D.Draw(target,x,y);
    }
}

void drawBeast( SDL_Surface * target, int x, int y, int direction, int variation, bool noDirectionAid=false)
{
    Uint8 tx,ty;

    switch((btype)variation)
    {
        case(BT_TWISTER):  tx=12; ty=0; break;
        case(BT_SPIKE):    tx=14; ty=0; break;
        case(BT_VIRUS):    tx=13; ty=0; break;
        case(BT_BLOB):     tx=15; ty=0; break;
        case(BT_BLOBBOSS): tx=15; ty=6; break;
        case(BT_PATIENCE): tx=15; ty=3; break;
        case(BT_TIGER): tx=10; ty=14; break;

        case(BT_STATIC):   tx=11; ty=15; break;

        case(BT_DARD):
            switch(direction)
            {
                case(EDITORDIRECTION_UP): tx=14; break;
                case(EDITORDIRECTION_LEFT): tx=13; break;
                case(EDITORDIRECTION_DOWN): tx=12; break;
                default: tx=11;
            }
            ty= 2;
            break;
        case(BT_WARD):
            switch(direction)
            {
                case(EDITORDIRECTION_UP): tx=14; break;
                case(EDITORDIRECTION_LEFT): tx=13; break;
                case(EDITORDIRECTION_DOWN): tx=12; break;
                default: tx=11;
            }
            ty= 4;
            break;

        case(BT_RANGER):
            switch(direction)
            {
                case(EDITORDIRECTION_UP): tx=15; break;
                case(EDITORDIRECTION_LEFT): tx=14; break;
                case(EDITORDIRECTION_DOWN): tx=13; break;
                default: tx=12;
            }
            ty= 18;
            break;


        case(BT_SPINNER):case(BT_ASPINNER):
            tx=11;
            if (variation==(int)BT_ASPINNER) ty=11;
            else ty=9;
            break;


        default: //gnasher
            tx=11;
            ty=0;


    }
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);


    if(( (variation==(int)(BT_SPINNER) ) ||  (variation==(int)(BT_ASPINNER) ) ) && (editor::tic4<2) && !noDirectionAid)
    {
        D.SetColors(255,255,255,128);
        D.Draw(target,x,y);

        switch(direction)
        {
              case(EDITORDIRECTION_RIGHT): tx=4; ty=11; break;
               case(EDITORDIRECTION_LEFT): tx=5; ty=11; break;
              case(EDITORDIRECTION_DOWN): tx=4; ty=12; break;
              default: tx=5; ty=12;
        }
        D.ChangeRect(tx*sz,ty*sz,sz,sz);

        D.SetColors(0,0,0,255);
        D.Draw(target,x,y);
    }
    else D.Draw(target,x,y);

}


void drawFactoryTop( SDL_Surface * target, int x, int y, int direction, bool dotrans)
{
    Uint8 tx, ty=14;
    switch(direction)
    {
        case(EDITORDIRECTION_RIGHT): tx=6; break;
        case(EDITORDIRECTION_DOWN): tx=7; break;
        case(EDITORDIRECTION_LEFT): tx=8; break;
        default: tx=9;
    }
    DaVinci D(editor::sprites,tx*sz,ty*sz,sz,sz);
    if( dotrans )
    {
        Uint8 alpha = 255;
        if((editor::tic4<2) ) alpha = 120;

        D.SetColors(255,255,255,alpha);
    }
    D.Draw(target,x,y);
    tx = ((tx-6)+2)%4+6;
    ty ++;
    D.ChangeRect(tx*sz,ty*sz,sz,sz);
    D.Draw(target,x,y);
}

void drawColorFactory( SDL_Surface * target, int x, int y, bool round, editorcolor color, int variation, int direction)
{
    switch(variation)
    {
        case 0: drawBlock(target, x,y, round, color); break;
        case 1: drawSpecialBlocks(target,x,y,round, color, 0, direction); break;
        case 2: drawSpecialBlocks(target,x,y,round, color, 4, direction); break;
        case 3: drawPusher(target,x,y,color,direction); break;
        case 4: drawGem(target,x,y,color);

    }
    drawFactoryTop(target,x,y, direction, (editor::SelectedType() == EDOT_COLORFACTORY)||(editor::SelectedType() == EDOT_DANGERFACTORY) );
}
void drawDangerFactory( SDL_Surface * target, int x, int y, int variation, int direction)
{
    switch(variation)
    {
        case 14: drawRattlerHead(target,x,y,0,direction); break;
        case 15: drawFood(target,x,y); break;
        case 16: drawHazard(target,x,y,1); break;
        default : drawBeast(target,x,y,direction,variation, true);

    }
    drawFactoryTop(target,x,y, direction, (editor::SelectedType() == EDOT_COLORFACTORY)||(editor::SelectedType() == EDOT_DANGERFACTORY) );
}

void drawObjectBySpecs( SDL_Surface * target, int x, int y, editorobjecttype ot, editorcolor color, bool round, int variation, int direction)
{
    switch(ot)
    {
        case EDOT_XYE: drawXye(target,x,y,variation); break;
        case EDOT_RATTLERHEAD: drawRattlerHead(target,x,y,variation,direction); break;
        case EDOT_GEM: drawGem(target,x,y,color); break;
        case EDOT_WALL: drawWall(target,x,y,round,variation); break;
        case EDOT_BLOCK: drawBlock(target,x,y,round,color); break;
        case EDOT_LARGEBLOCK: drawLargeBlock(target,x,y,color,variation, direction); break;
        case EDOT_PORTAL: drawPortal(target,x,y,color,variation); break;
        case EDOT_COLORFACTORY: drawColorFactory(target,x,y,round, color,variation, direction); break;
        case EDOT_DANGERFACTORY: drawDangerFactory(target,x,y,variation, direction); break;
        case EDOT_TURNER: drawTurner(target,x,y,round,color,variation); break;


        case EDOT_NUMBER: drawNumber(target,x,y,color,round,variation); break;
        case EDOT_SPECIALBLOCKS: drawSpecialBlocks(target,x,y,round,color,variation,direction); break;

        case EDOT_EARTH: drawEarth(target,x,y,round); break;
        case EDOT_GEMBLOCK: drawGemBlock(target,x,y,color); break;
        case EDOT_MAGNET: drawMagnet(target,x,y,variation,direction); break;
        case EDOT_PUSHER: drawPusher(target,x,y,color,direction); break;
        case EDOT_ARROWMAKER: drawArrowMaker(target,x,y,round,color,direction,variation); break;
        case EDOT_HAZARD: drawHazard(target,x,y,variation); break;
        case EDOT_ONEDIRECTION: drawOneDir(target,x,y,direction,variation); break;
        case EDOT_BEAST: drawBeast(target,x,y,direction,variation); break;
        case EDOT_TELEPORT: drawTeleport(target,x,y,direction); break;
        case EDOT_BOT: drawBot(target,x,y); break;
        case EDOT_FOOD: drawFood(target,x,y); break;
        case EDOT_FIREPAD: drawFirePad(target,x,y); break;



        case EDOT_COLORSYSTEM: drawColorSystem(target,x,y,color,variation); break;
        case EDOT_KEYSYSTEM: drawKeySystem(target,x,y,color,variation); break;
    }
}




