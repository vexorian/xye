/*
Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/
#ifndef WINDOWINCLUDED


#include "vxsdl.h"
#ifndef FONTINCLUDED
   #include "font.h"
#endif
#include<string>
#define WINDOWINCLUDED

using namespace std;

class control
{
 public:
    int x;
    int y;
    int w;
    int h;

    int depth;

     virtual void onMouseMove(int x,int y)=0;
     virtual void onMouseOut()=0;
     virtual void onMouseDown(int x,int y)=0;
     virtual void onMouseUp(int x,int y)=0;
     virtual void onMouseRightUp(int x,int y)=0;

     virtual void draw(SDL_Surface* target)=0;
     virtual void loop()=0;
     
     virtual ~control() {}

};


#define MAXCONTROLS 100
#define MAXSUBS 5

typedef void (*keysimFunction)(SDLKey keysim, Uint16 unicode);
class window;
typedef void (*voidFunction)(window* wind);
class window
{
  private:
     static window * CurrentInstance;
     static bool SDLactive;
     SDL_Surface* surface;

      
     
     
     control* controls [MAXCONTROLS];
     int controln;
     int sub;
     control* subcontrols[MAXSUBS][MAXCONTROLS];
     int subcontroln[MAXSUBS];
     
     keysimFunction subOnKeyDown[MAXSUBS];
     keysimFunction subOnKeyUp[MAXSUBS];
     
     int curcontrol;
     

     void loop();
     bool mouse_pressed;
     unsigned int mouse_x;
     unsigned int mouse_y;
     bool TriggeredLoop,InActive;

     static Uint32 timer(Uint32 interval, void *param);

     void draw();


     void handleMouseMove(int x, int y);
     void handleMouseDown(int x, int y);
     void handleMouseUp(int x, int y);
     void handleMouseRightUp(int x, int y);
     void drawControls();
     void loopControls();
     
     

     bool halt;
     
     window();
     
     
     void init(int width,int height, const char* caption);
     voidFunction transition;
     void reset();


 public:

    static window* create(int width , int height,const char* caption);
    void Resize(int width, int height);
    void SetCaption(const char* caption);
    void SetCaption(const string caption);
    ~window();
    SDL_Surface* getDrawingSurface();
    int Width;
    int Height;
    void Error(const char* msg);
    void (*onExitAttempt)();
    keysimFunction onKeyDown;
    keysimFunction onKeyUp;
    void (*beforeDraw)();
    
    void beginSub();
    void endSub();
    void stop();
    
    
     void deleteControls();
     void addControl(control *c);
    
    
    void loop(double fps);
    
    static bool InitSDL();
    static void QuitSDL();
    void Close();
    void SetTransition( voidFunction tra);
};

class rectangle: public control
{
 public:
    Uint8 r,g,b;
    rectangle(int sx, int sy, int sw, int sh, Uint8 red, Uint8 green, Uint8 blue);
    rectangle(int sx, int sy, int sw, int sh, const SDL_Color & c);

    void draw(SDL_Surface* target);
    void loop() {}
    inline void onMouseMove(int px,int py){}
    inline void onMouseOut() {}
    inline void onMouseDown(int px,int py) {}
    inline void onMouseUp(int px,int py) {}
    inline void onMouseRightUp(int px,int py) {}
};

class textblock: public control
{
 public:
    Font* FontRes;
    
    string text;
    textblock(int sx, int sy, int sw, int sh, Font* fontToUse);
    

    void draw(SDL_Surface* target);
    void loop() {}
    inline void onMouseMove(int px,int py){}
    inline void onMouseOut() {}
    inline void onMouseDown(int px,int py) {}
    inline void onMouseUp(int px,int py) {}
    inline void onMouseRightUp(int px,int py) {}
};

class buttondata
{
    public:
        virtual ~buttondata() {};
};

class buttontooltip : public control
{
 public:
    buttontooltip();
    int maxx, maxy, minx, miny;
    
    int tx;
    int ty;
    string text;
    bool enabled;
    
    
    void draw(SDL_Surface* target);
    void onMouseMove(int x,int y) {};
    void onMouseOut() {};
    void onMouseDown(int x,int y) {};
    void onMouseUp(int x,int y) {};
    void onMouseRightUp(int x,int y) {};
    
    void loop() {};
    
    
    bool * drawnSwitch;
};

class button : public control
{
    private:
        int flashperiod;
        int iconx, icony;
        int toolx, tooly;
        int mouseInside;

    public:
        buttondata* data;
    
        string text;
        bool click;

        bool Visible;
        bool Enabled;
        bool ToggleButton;
        button(int sx, int sy, int sw, int sh);
        ~button();
        void draw(SDL_Surface* target);
        void loop() {}
        
        void (*onModeEnd) ();
        void Icon(int ix, int iy);
        
        
        void (*onClick)(const buttondata* data);
        void (*onPress)(const buttondata* data);
        void (*onRelease)(const buttondata* data);
        
        void onMouseMove(int px,int py);
        void onMouseOut();
        void onMouseDown(int px,int py);
        void onMouseUp(int px,int py);
        void resetToggle();        
        inline void onMouseRightUp(int px,int py) {}

        void flash();
        
        static Font* FontResource;
        static Uint8 Size;
        static SDL_Surface* SourceSurface;
        static Uint8 PressedTextureY;
        static Uint8 NormalTextureY;
        static Uint8 ShortTextureX;
        static Uint8 LongTextureX;
        
        static Sint16 recommendedWidth(const char* s);
        static Sint16 recommendedWidth(const string s) { return recommendedWidth(s.c_str()); }
        
        static SDL_Color ToolTipColor;
      
        string toolTip;
        buttontooltip* toolTipControl; 
};



#endif
