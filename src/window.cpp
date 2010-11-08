/*
Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/

#include "xyedit.h"
#include "xye.h"

#include <stdio.h>
#include "xye_script.h"
#include "options.h"
#include<stdlib.h>



/* **************************** Window class! ****************/
window* window::CurrentInstance=NULL;
bool window::SDLactive=false;


window::window() {};

void onExitAttemptDoNothing() {};
void onKeyEventDoNothing(SDLKey keysim, Uint16 unicode) {};
void beforeDrawDoNothing() {}

bool window::InitSDL()
{
    if(window::SDLactive)
    {
        fprintf(stderr,"Unable to init SDL because it has already been initialized\n");
        return false;
    }

    if (SDL_Init(SDL_INIT_TIMER)==-1)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return false;
    }
    #ifndef NOTRUETYPE
        TTF_Init();
    #endif   

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    window::SDLactive=true;
    return true;
}



void window::QuitSDL()
{
    if(!window::SDLactive) return;    
    window::SDLactive=false;
    #ifndef NOTRUETYPE
        TTF_Quit();
    #endif    
    SDL_Quit();    
}

void window::Close()
{
    if( onExitAttempt != NULL ) onExitAttempt();
}

void window::endSub()
{
    if(sub==0) fprintf(stderr,"Warning: attempt to close sub window when none is open\n");
    else
    {
        deleteControls();
        sub--;
        controln=subcontroln[sub];
        for (int i=0;i<controln;i++) controls[i]=subcontrols[sub][i];
        onKeyUp=subOnKeyUp[sub];
        onKeyDown=subOnKeyDown[sub];
        
    }
}

void window::beginSub()
{
    if(sub==MAXSUBS) fprintf(stderr,"Warning: ignored request to begin a sub window");
    else
    {
        for (int i=0;i<controln;i++) subcontrols[sub][i]=controls[i];
        subcontroln[sub]=controln;
        
        subOnKeyUp[sub] = onKeyUp;
        subOnKeyDown[sub] = onKeyDown;
        
        onKeyUp=onKeyEventDoNothing;
        onKeyDown=onKeyEventDoNothing;
        
        controln=0;
        sub++;

    }
}

void window::init(int width, int height, const char * caption)
{
    sub=0;
    onExitAttempt=onExitAttemptDoNothing;
    onKeyDown=onKeyEventDoNothing;
    onKeyUp=onKeyEventDoNothing;
    beforeDraw=beforeDrawDoNothing;
    curcontrol=-1;
    mouse_pressed=false;
    
    halt=false;

    
    
    controln=0;
    Width=width;
    
    Height=height;
    SDL_Init(SDL_INIT_VIDEO);
    surface=SDL_SetVideoMode(width,height, 32, 0);

    transition = NULL;

    SDL_WM_SetCaption(caption,0);
   

}

void window::Resize(int width, int height)
{
    if((Width==width) && (Height==height)) return;
    Width = width;
    Height = height;
    surface=SDL_SetVideoMode(width,height, 32, 0);

}

void window::SetCaption(const char* caption)
{
    SDL_WM_SetCaption(caption,0);
}
void window::SetCaption(const string caption)
{
    SDL_WM_SetCaption(caption.c_str(),0);
}


window* window::create(int width, int height, const char * caption)
{
    //all right, singleton pattern is tricky, only reason this is a 
    // singleton is because SDL does not allow multiple windows yet.
    
    
    if(CurrentInstance!=NULL) return CurrentInstance;

    if(!window::SDLactive)
    {
        fprintf(stderr,"SDL is not initialized, cannot create a window\n");
        return NULL;
    }

    
    CurrentInstance = new window();
    CurrentInstance->init(width,height,caption);
    return CurrentInstance;
     
}


void window::reset()
{
    beforeDraw=NULL;
    deleteControls();
    while (sub>0)
    {
        endSub();
    }

}

window::~window()
{
    CurrentInstance=NULL;
    reset();
    
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

SDL_Surface* window::getDrawingSurface()
{
    return surface;
}

void window::draw()
{
    if(beforeDraw!=NULL)
        beforeDraw();
    drawControls();
    SDL_Flip(surface);
}



void window::Error(const char* msg)
{
    fprintf(stderr,"%s", msg);
    fprintf(stderr,"\n");
    throw (msg);
}


Uint32 window::timer(Uint32 interval, void *param)
{
    
    if (!CurrentInstance->TriggeredLoop)
    {
        SDL_Event event;
        SDL_UserEvent userevent;

        userevent.type = SDL_USEREVENT;
        userevent.code = 0; //game loop
        userevent.data1 = NULL;
        userevent.data2 = NULL;

        event.type = SDL_USEREVENT;
        event.user = userevent;
        CurrentInstance->TriggeredLoop=true;
        SDL_PushEvent(&event);
    }
    return(interval);
}

void window::stop()
{
    halt=true;
}

void window::SetTransition( voidFunction tra )
{
    transition = tra;
}


using namespace std;
void window::loop(double fps)
{
    bool &done=halt;
    bool browse=false;
    Uint32 per=(Uint32)((1.0 / fps)*1000); //calculates period of time based on FPS;

    SDL_Event event;
    Uint8 st;

    TriggeredLoop=InActive=false;
    SDL_TimerID tim = SDL_AddTimer( per, window::timer,0);
    while (!done)
    {
        int t=0;
        
        // message processing loop
        while (SDL_PollEvent(&event) && (!done))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
                case SDL_QUIT:
                    onExitAttempt();
                    break;

                // check for keypresses
                case (SDL_KEYDOWN): //Key IS Down!
                    onKeyDown(event.key.keysym.sym,event.key.keysym.unicode );
                    break;
                case (SDL_KEYUP): //Key IS Down!
                    onKeyUp(event.key.keysym.sym,event.key.keysym.unicode);
                    break;

            case (SDL_MOUSEBUTTONDOWN):
                if(event.button.button==SDL_BUTTON_LEFT)
                {
                    mouse_x=event.button.x;mouse_y=event.button.y;
                    window::handleMouseDown(mouse_x,mouse_y);
                    mouse_pressed=true;
                }
                break;

            case (SDL_MOUSEBUTTONUP):
                if(event.button.button==SDL_BUTTON_LEFT)
                {
                    mouse_pressed=false;
                    window::handleMouseUp(mouse_x,mouse_y);
                }
                else if(event.button.button==SDL_BUTTON_RIGHT)
                {
                    window::handleMouseRightUp(mouse_x,mouse_y);
                }

                break;
            case(SDL_MOUSEMOTION) :
                mouse_x=event.motion.x;mouse_y=event.motion.y;
                window::handleMouseMove(mouse_x,mouse_y);


                break;

            case (SDL_USEREVENT): //the only user event is the normal loop:
                if(transition==NULL)
                {
                    st=SDL_GetAppState();
                    if ( st & (SDL_APPMOUSEFOCUS | SDL_APPINPUTFOCUS) )
                    {
                        loopControls();
                        draw();
                    }
                }
                TriggeredLoop=false;


                //if (
                break;

                
            } // end switch



        }
        if(transition!=NULL)
        {
            reset();
            transition(this);
            transition = NULL;
        }

        if  (!done) SDL_Delay((InActive?1000:10));
        
        
        
        //if (!done) SDL_Delay(1);
    }

    //assert(save());
    SDL_RemoveTimer(tim);
    return;
}



void window::deleteControls()
{
    for (int i=0;i<controln;i++)
    {
        if(i==curcontrol) 
            controls[curcontrol]->onMouseOut();
            
        delete controls[i];
    }
    curcontrol = -1;
    controln=0;
}

void window::addControl(control *c)
{


    if(curcontrol!=-1)
    {
        controls[curcontrol]->onMouseOut();
        curcontrol=-1;
    }
    if(controln==MAXCONTROLS)
    {
        window::Error("Too many GUI controls!!");
    }
    int i=0;

    while((i<controln) && (  (controls[i]->depth) < (c->depth) )) i++;

    int k=controln;
    while (k>i)
    {
        controls[k]=controls[k-1];
        k--;
    }

    controls[i]=c;
    controln++;


}


void window::handleMouseMove(int x, int y)
{
    for (int i=controln-1;i>=0;i--)
    {
        control* c=controls[i];
        if (( x>= c->x) && ( x<= c->x+c->w) && ( y>= c->y) && ( y<= c->y + c->h) )
        {
            if((i!=curcontrol) && (curcontrol!=-1)) controls[curcontrol]->onMouseOut();
            curcontrol=i;
            c->onMouseMove(x-c->x,y-c->y);
            if(mouse_pressed) mouse_pressed=false; //c->onMouseDown(x-c->x,y-c->y);
            return;
        }
    }
    if (curcontrol!=-1) controls[curcontrol]->onMouseOut();
}


void window::handleMouseDown(int x, int y)
{
    for (int i=controln-1;i>=0;i--)
    {
        control* c=controls[i];
        
        
        if (( x>= c->x) && ( x<= c->x+c->w) && ( y>= c->y) && ( y<= c->y+c->h) )
        {
            c->onMouseDown(x-c->x,y-c->y);
            return;
        }
    }

}
void window::handleMouseUp(int x, int y)
{
    for (int i=controln-1;i>=0;i--)
    {
        control* c=controls[i];
        if (( x>= c->x) && ( x<= c->x+c->w) && ( y>= c->y) && ( y<= c->y+c->h) )
        {
            c->onMouseUp(x-c->x,y-c->y);
            return;
        }
    }
}


void window::handleMouseRightUp(int x, int y)
{
    for (int i=controln-1;i>=0;i--)
    {
        control* c=controls[i];
        if (( x>= c->x) && ( x<= c->x+c->w) && ( y>= c->y) && ( y<= c->y+c->h) )
        {
            c->onMouseRightUp(x-c->x,y-c->y);
            return;
        }
    }
}

void window::drawControls()
{
    for (int j=0;j<sub;j++) for (int i=0;i<subcontroln[j];i++) subcontrols[j][i]->draw(surface);
    for (int i=0;i<controln;i++) controls[i]->draw(surface);
}

void window::loopControls()
{
    for (int i=0;i<controln;i++) controls[i]->loop();
}
/*** The rectangle control!! **/
rectangle::rectangle(int sx, int sy, int sw, int sh, Uint8 red, Uint8 green, Uint8 blue)
{
    x=sx;y=sy;w=sw;h=sh;
    r=red;
    g=green;
    b=blue;
    depth=0;
}

rectangle::rectangle(int sx, int sy, int sw, int sh, const SDL_Color & c)
{
    x=sx;y=sy;w=sw;h=sh;
    r=c.r;
    g=c.g;
    b=c.b;
    depth=0;
}

void rectangle::draw(SDL_Surface* target)
{
    SDL_FillRect(target, x,y,w,h, SDL_MapRGB(target->format,r,g,b ) );
}


/*** The textblock control!! **/
textblock::textblock(int sx, int sy, int sw, int sh, Font* fontToUse)
{
    x=sx;y=sy;w=sw;h=sh;
    FontRes=fontToUse;
    depth=0;
    text ="textblock";
}

void textblock::draw(SDL_Surface* target)
{
    FontRes->WriteWrap(target,x,y,w,h,text.c_str() );
}


/*** The button control **/
Font* button:: FontResource=NULL;
SDL_Surface* button::SourceSurface;
Uint8 button::Size=20;
Uint8 button::PressedTextureY=18; //18
Uint8 button::NormalTextureY=17; //17
Uint8 button::ShortTextureX=6;
Uint8 button::LongTextureX=7;


button::button(int sx, int sy, int sw, int sh)
{
    Visible=Enabled=true;
    data=NULL;
    x=sx;y=sy;w=sw;h=sh;
    depth=0;
    text="Button";
    onClick=onPress=onRelease=NULL;
    click=false;
    flashperiod=0;
    iconx = icony = -1;
    ToggleButton = false;
}

void button::Icon(int ix, int iy)
{
    text = "";
    iconx=ix, icony=iy;
}

void button::draw(SDL_Surface* target)
{
    const Uint8 disablealpha = 128;//64;
    if(!Visible) return;
    bool pressed = (click || !Enabled);
    int sz=button::Size;
    int x=this->x;
    int y=this->y;
    if(flashperiod)
    {
        switch(flashperiod&3)
        {
            case 0: x--;y++; break;
            case 2: x++;y--; break;

        }
        flashperiod--;
    }

    if(w>sz)
    {
        int ty=NormalTextureY;
        int tx=LongTextureX;
        if(pressed) ty=PressedTextureY;
        DaVinci A(button::SourceSurface,tx*sz,ty*sz,  std::max(std::min(w-sz ,sz),0)  ,sz);
        if(! Enabled)
            A.SetColors(255,255,255,disablealpha);

        A.Draw(target,x,y);

        
        for (int i=x+sz;i<x+w-sz;i+=sz)
        {
            A.ChangeRect( (tx+1)*sz,ty*sz, std::min(x+w-sz-i ,sz)   ,sz);
            A.Draw(target,i,y);
        }

        A.ChangeRect((tx+2)*sz,ty*sz,sz,sz);
        A.Draw(target,x+w-sz,y);

    }
    else if (w==sz)
    {
        int ty=NormalTextureY;
        int tx=ShortTextureX;
        if(pressed) ty=PressedTextureY;
        DaVinci C(button::SourceSurface,tx*sz,ty*sz,sz,sz);
        if(! Enabled)
            C.SetColors(255,255,255,disablealpha);

        C.Draw(target,x,y);
    }
    int o=0;
    if(pressed) o=1;

    if(button::FontResource!=NULL)
    {
        button::FontResource->Write(target,o+x+(w-button::FontResource->TextWidth(text.c_str()))/2 ,o+y+(sz-button::FontResource->Height())/2,text.c_str());
    }
    if ( (iconx!=-1) )
    {
        DaVinci I(button::SourceSurface, iconx*sz, icony*sz, sz,sz);
        if(! Enabled)
            I.SetColors(0,0,0,64);
        I.Draw(target,x+o +(w-sz)/2,y+o);
    }
}

void button::onMouseOut()
{
    if( click && ! ToggleButton)
    {
        if(onRelease!=NULL) onRelease(data);
        click=false;
    }
}
void button::onMouseDown(int px,int py)
{
    if( ToggleButton && click) {
        if(onRelease!=NULL) onRelease(data);
        click = false;
    } else {
        if( (!Visible) || (!Enabled) ) return;
        if (onPress!=NULL) onPress(data);
        click=true;
    }
}

void button::onMouseUp(int px,int py)
{
    if(ToggleButton) return;
    if(onRelease!=NULL) onRelease(data);
    if(onClick && click && Visible) onClick(data);
}

void button::flash()
{
    flashperiod=4;
}

button::~button()
{
    if(data!=NULL) delete data;
}

Sint16 button::recommendedWidth(const char* s)
{
    Sint16 w=FontResource->TextWidth(s) + Size;
    if(w<Size*3) return Size*3;
    return w;
}
