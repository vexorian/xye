/*
this is a placeholder
*/

#include "listbox.h"
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;


namespace listbox_private
{
    
    const int PADDING_LEFT = 2;

    class list: public listbox
    {
    public:  //not-so-public because no other file sees this class.
        
        
        vector<string> tags;
        vector<string> values;
        
        bool selectedValid;
        int selectedIndex;
        int viewIndex;
        
        int barWidth;
        
        list(int x, int y, int w, int h)
        {
            this->x = x; this->y = y; this->w = w; this->h = h;
            onSelect = NULL;
            onItemDoubleClick = NULL;
            NormalFont = NULL;
            SelectedFont = NULL;
            viewIndex = 0;
            selectedIndex = -1;
            barWidth = 10;
            clicked = false;
            barClicked = false;
            selectedValid = true;
            maxLines = -1;
        }
        
        void handleSelectEvent(bool centerIt = false)
        {
            if (maxLines < 0) {
                updateMaxLines();
            }
            if (selectedIndex == -1) {
                return;
            }
            if ( centerIt) {
                int s = tags.size();
                if ( s > maxLines ) {
                    viewIndex = selectedIndex - maxLines/2;
                    // {viewIndex == selectedIndex - maxLines/2 }
                    // {viewIndex + maxLines/2 == selectedIndex}
                    
                    viewIndex = std::max( std::min(s - maxLines, viewIndex) , 0);
                } else {
                    viewIndex = 0;
                }
                
            } else {
                //  update the viewIndex so that the
                // newly selected item is visible:
                if ( selectedIndex < viewIndex ) {
                    viewIndex = selectedIndex;
                }
                if ( selectedIndex >= viewIndex + maxLines ) {
                    // viewIndex + maxLines - 1 = selectedIndex
                    // viewIndex = selectedIndex - maxLines + 1
                    viewIndex = selectedIndex - (maxLines - 1);
                    if (viewIndex < 0) {
                        viewIndex = 0;
                    }
                }
            }
            

            if (onSelect != NULL) {
                selectedValid = onSelect(this);
            } else{
                selectedValid = true;
            }
        }
        
        
        void addItem(const string tag, const string value)
        {
            tags.push_back(tag);
            values.push_back(value);
        }
        
        string getSelectedValue()
        {
            if (selectedIndex == -1) {
                return "NULL";
            } else {
                return values[selectedIndex];
            }
        }
        int getSelectedIndex()
        {
            return selectedIndex;
        }
        ~list()
        {
        }
        void selectItem(int index) {
            int old = selectedIndex;
            if (index >= tags.size() ) {
                selectedIndex = -1;
            } else {
                selectedIndex = index;
            }
            if (old != selectedIndex ) {
                handleSelectEvent( old == -1 );
            }
        }
        void selectItem(string value) {
            int old = selectedIndex;
            for (selectedIndex = 0; selectedIndex < tags.size(); selectedIndex++) {
                if (values[selectedIndex]==value) {
                    handleSelectEvent( old == -1 );
                    return;
                }
            }
            selectedIndex = -1;
            handleSelectEvent();
        }
        
        int maxLines;
        
        void updateMaxLines()
        {
            int cy = y;
            int fh = NormalFont->Height();
            int i = 0;
            maxLines = 0;
            while ( cy + fh < y + h) {
                maxLines++;
                cy += fh;
            }
        }
        
        void draw(SDL_Surface* target)
        {
            Uint32 back=SDL_MapRGB(target->format, BackgroundColor);
            SDL_FillRect(target, x,y,w,h, back);
            
            int cx = x + PADDING_LEFT;
            int cy = y;
            int fh = NormalFont->Height();
            int i = 0;
            
            int bh = barWidth;
            if ( tags.size() <= maxLines ) {
                bh = 0;
            }
            maxLines = 0;
            while ( cy + fh < y + h)
            {
                if (viewIndex + i < tags.size() ) {
                    
                    if ( viewIndex + i == selectedIndex) {
                        Uint32 col;
                        if (selectedValid) {
                            col=SDL_MapRGB(target->format, SelectedColor);
                        } else {
                            col=SDL_MapRGB(target->format, InvalidColor);
                        }
                        SDL_FillRect(target, x,cy ,w - bh, fh,  col);
                        SelectedFont->Write(target,cx,cy, tags[viewIndex + i]);
                    } else{
                        NormalFont->Write(target,cx,cy, tags[viewIndex + i]);
                    }
                    
                    i++;
                }
                maxLines++;
                cy += fh;
            }
            // the bar!
            if ( bh > 0 ) {
                int barheight = (maxLines*h)/tags.size();
                int barpos = (viewIndex*h)/tags.size();
                
                Uint8 r = BarColor.r/2 + BackgroundColor.r/2;
                Uint8 g = BarColor.g/2 + BackgroundColor.g/2;
                Uint8 b = BarColor.b/2 + BackgroundColor.b/2;
                
                Uint32 black = SDL_MapRGB(target->format, 0,0,0);
                
                Uint32 barfcol = SDL_MapRGB(target->format, r,g,b);
                /*SDL_FillRect(target, x +w - barWidth, 0, barWidth, h, black);
                SDL_FillRect(target, x +w - barWidth + 1, 1, barWidth-2, h-2, barfcol);*/
                SDL_FillRect(target, x +w - barWidth, 0, barWidth, h, barfcol);
                
                Uint32 barcol = SDL_MapRGB(target->format, BarColor);
                SDL_FillRect(target, x +w - barWidth, y+barpos, barWidth, barheight, black);
                SDL_FillRect(target, x +w - barWidth+1, y+barpos+1, barWidth-2, barheight-2, barcol);
            }
        }
        
        bool barClicked;
        bool clicked;
        
        bool insideBar(int px, int py)
        {
            if ( tags.size() > maxLines ) {
                return px >= w - barWidth;
            }
            return false;
        }
     
        void verifyBarClick(int mousex, int mousey)
        {
            if ( barClicked && ( tags.size() > maxLines ) ) {
                int p = (mousey*tags.size()) / h;
                if (p <= tags.size() - maxLines) {
                    viewIndex = p;
                } else {
                    //v + maxLines == tags.size()
                    viewIndex = tags.size() - maxLines; 
                }
            }
        }
        
        void onMouseMove(int mousex, int mousey)
        {
            verifyBarClick(mousex, mousey);
        }
        void onMouseOut()
        {
            clicked = false;
        }
        void onMouseDown(int mousex, int mousey)
        {
            if (insideBar(mousex, mousey) ) {
                barClicked = true;
            } else {
                clicked = true;
            }
        }
        void onMouseUp(int mousex, int mousey)
        {
            if (barClicked) {
                barClicked = false;
                return ;
            }
            clicked = false;
            if ( (tags.size() <= maxLines) || (mousex < w - barWidth) ) {
                //item click.
                int old = selectedIndex;
                int p = mousey/ NormalFont->Height();
                if ( (p < maxLines) && (p + viewIndex < tags.size() ) ) {
                    selectedIndex = viewIndex + p;
                    if (old == selectedIndex) {
                        if ( onItemDoubleClick != NULL) {
                            onItemDoubleClick(this);
                        }
                    } else {
                        handleSelectEvent();
                    }
                }
            }
        }
        void onMouseRightUp(int x, int y)
        {
        }
        
        void loop()
        {
        }
        
        void onKeyUp(SDLKey keysim, Uint16 unicode);


    };

    bool IsCharKeyEvent(SDLKey& k,char & a,char &b)
    {
    
        a='\0';
        if ((k >= SDLK_a) && (k<=SDLK_z))
        {
            a= 'a'+(k-SDLK_a);
            b = 'A'+(k-SDLK_a);
        }
        else if ((k >= SDLK_0) && (k<=SDLK_9))
            a='0'+(k-SDLK_0);
        return (a!='\0');
    }

    void list::onKeyUp(SDLKey keysim, Uint16 unicode)
    {
        char a='\0',b=a;
        int oldsel = selectedIndex;
        
        int s = tags.size();
        
        if (IsCharKeyEvent(keysim,a,b)) {
            int l = selectedIndex;
            int i = l + 1;
            while (i != selectedIndex) {
                if (i==s) {
                    i=0;
                }
                const string & p = tags[i];
                if (  (p.length()>0) && ( (p[0]==a) || (p[0]==b)) ) {
                    selectedIndex=i;
                } else {
                    i++;
                }
            }
        } else {
            switch (keysim)
            {
        
                case(SDLK_UP):
                    selectedIndex--;
                    if (selectedIndex<0) selectedIndex=s-1;
                    break;
                case(SDLK_DOWN):
        
                    selectedIndex++;
                    if (selectedIndex>=s) selectedIndex=0;
                    break;
        
                case(SDLK_PAGEUP):
                    selectedIndex -= maxLines/2+1;
                    if (selectedIndex<0) selectedIndex=s-1;
                    break;
                case(SDLK_PAGEDOWN):
        
                    selectedIndex += maxLines/2+1;
                    if (selectedIndex>=s) selectedIndex=0;
                    break;
            }
        }
        if (oldsel != selectedIndex) {
            handleSelectEvent();
        }
    
    }

} 


listbox* listbox::makeNew(int x, int y, int w, int h)
{
    return new listbox_private::list(x,y,w,h);
}


 