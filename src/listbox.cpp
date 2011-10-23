/*
this is a placeholder
*/

#include "listbox.h"
#include <vector>
#include <iostream>

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
            NormalFont = NULL;
            SelectedFont = NULL;
            viewIndex = 0;
            selectedIndex = -1;
            barWidth = 10;
            clicked = false;
            selectedValid = true;
        }
        
        void handleSelectEvent()
        {
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
            if (index >= tags.size() ) {
                selectedIndex = -1;
            } else {
                selectedIndex = index;
            }
        }
        void selectItem(string value) {
            for (selectedIndex = 0; selectedIndex < tags.size(); selectedIndex++)
            {
                if (values[selectedIndex]==value) {
                    return;
                }
            }
            selectedIndex = -1;
            
        }
        
        int maxLines;
        
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
                Uint32 barcol = SDL_MapRGB(target->format, BarColor);
                SDL_FillRect(target, x +w - barWidth, y+barpos, barWidth, barheight, barcol);
            }
        }
        
        bool clicked;
     
        void verifyBarClick(int mousex, int mousey)
        {
            if ( clicked && ( tags.size() > maxLines ) ) {
                //the bar
                if (mousex >= w - barWidth) {
                    int p = (mousey*tags.size()) / h;
                    if (p <= tags.size() - maxLines) {
                        viewIndex = p;
                    } else {
                        //v + maxLines == tags.size()
                        viewIndex = tags.size() - maxLines; 
                    }
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
            clicked = true;
            verifyBarClick(mousex, mousey);
        }
        void onMouseUp(int mousex, int mousey)
        {
            clicked = false;
            if ( (tags.size() <= maxLines) || (mousex < w - barWidth) ) {
                //item click.
                int p = mousey/ NormalFont->Height();
                if ( (p < maxLines) && (p + viewIndex < tags.size() ) ) {
                    selectedIndex = viewIndex + p;
                    handleSelectEvent();
                }
            }
        }
        void onMouseRightUp(int x, int y)
        {
        }
        
        void loop()
        {
        }

    };

} 


listbox* listbox::makeNew(int x, int y, int w, int h)
{
    return new listbox_private::list(x,y,w,h);
}


 