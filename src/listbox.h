#pragma once

#include "vxsdl.h"
#include "window.h"
#include <string>

class listbox: public control
{
    public:
        SDL_Color BackgroundColor;
        SDL_Color SelectedColor;
        SDL_Color InvalidColor;
        SDL_Color BarColor;
        Font*     NormalFont;
        Font*     SelectedFont;


    static listbox * makeNew(int x, int y, int w, int h);
    virtual string getSelectedValue()=0;
    virtual int    getSelectedIndex()=0;

    virtual ~listbox(){};


    virtual void   addItem(const string tag, const string value)=0;

    bool (*onSelect)(listbox* thelist); //returns true or false if selection is invalid

    void (*onItemDoubleClick)(listbox* thelist);



    virtual void   selectItem(int index)=0;
    virtual void   selectItem(string value)=0;

    virtual void   onKeyUp(SDLKey keysim, Uint16 unicode) = 0;

};
