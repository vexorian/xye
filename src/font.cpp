/*
 Xye License (it is a PNG/ZLIB license)

Copyright (c) 2025 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/

#include "font.h"
#include <stdexcept>
#include "gen.h"

static Uint32 GetPixel(SDL_Surface *Surface, Sint32 X, Sint32 Y)
{
   Uint8  *bits;
   Uint32 Bpp;

   X=(X>=0)?X:-X;
   X=(X<Surface->w)?X:Surface->w;

   Bpp = Surface->format->BytesPerPixel;
   bits = ((Uint8 *)Surface->pixels)+Y*Surface->pitch+X*Bpp;

   // Get the pixel
   switch(Bpp) {
      case 1:
         return *((Uint8 *)Surface->pixels + Y * Surface->pitch + X);
         break;
      case 2:
         return *((Uint16 *)Surface->pixels + Y * Surface->pitch/2 + X);
         break;
      case 3: { // Format/endian independent
         Uint8 r, g, b;
         r = *((bits)+Surface->format->Rshift/8);
         g = *((bits)+Surface->format->Gshift/8);
         b = *((bits)+Surface->format->Bshift/8);
         return SDL_MapRGB(Surface->format, r, g, b);
         }
         break;
      case 4:
         return *((Uint32 *)Surface->pixels + Y * Surface->pitch/4 + X);
         break;
   }

   return 0xF0000000;
}
#ifndef NOTRUETYPE
void Font::fontinit(const char* truetype,int size, Uint8 R,  Uint8 G, Uint8 B)
{
    tex=NULL;
    ttf=TTF_OpenFont(truetype,size);
    ttfcol.r=R;
    ttfcol.g=G;
    ttfcol.b=B;

}

Font::Font(const char* truetype,int size, Uint8 R,  Uint8 G, Uint8 B)
{
    fontinit(truetype,size,R,G,B);
}


Font::Font(const char* truetype,int size, SDL_Color& color)
{
    fontinit(truetype,size, color.r, color.g, color.b);
}
#else
Font::Font(const char* truetype,int size, Uint8 R,  Uint8 G, Uint8 B)
{
    fprintf(stderr,"Error: No truetype support but truetype font requested by skin file.\n");
    throw 1;
}


Font::Font(const char* truetype,int size, SDL_Color& color)
{
    fprintf(stderr,"Error: No truetype support but truetype font requested by skin file.\n");
    throw 1;
}
#endif
//nottf

void Font::fontinit(SDL_Surface* surface, Uint8 R,  Uint8 G, Uint8 B)
{
    ttf=NULL;

    int x = 0, i = 0, j;
    Uint32 pink;
    if (surface==NULL)
    {
        fprintf(stderr,"Couldn't initialize font.");
        throw ("Couldn't initialize font.");
    }

    int w=surface->w,h=surface->h;


    tex = CreateFixedRGBASurface(0, w,h);
    SDL_LockSurface(surface);
    SDL_LockSurface(tex);


    SDL_PixelFormat* surform=surface->format;

    pink = SDL_MapRGB(surform, 255, 0, 255);
    while (x < w)
    {
        if (GetPixel(surface, x, 0) == pink)
        {
            CharPos[i++]=x;
            while((x < w) && (GetPixel(surface, x, 0)== pink)) x++;
            CharPos[i++]=x;
        }
        x++;
    }
    MaxPos = x-1;
    Uint8 rd = 0, b = 0;
    FixedRGBA c;
    c.r=R;
    c.g=G;
    c.b=B;
    c.a=175;
    Uint32* c_Uint32 = (Uint32*)(&c);

    Uint8* Calpha= &(c.a);

    Uint32* pixels = (Uint32*)(tex->pixels);

    for (i=0;i<w;i++)
    for (j=0;j<h;j++)
    {
        SDL_GetRGB(GetPixel(surface,i,j), surform, &rd, Calpha, &b);
        *(pixels + (j * w) +  (i)) = *c_Uint32;
    }

    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(tex);
}

Font::Font(SDL_Surface* surface, Uint8 R,  Uint8 G, Uint8 B)
{
    fontinit(surface,R,G,B);
}


Font::Font(SDL_Surface* surface, SDL_Color& color)
{
    fontinit(surface, color.r, color.g, color.b);
}

Font::~Font()
{
    if(tex) SDL_FreeSurface(tex);
    #ifndef NOTRUETYPE
        else if(ttf) TTF_CloseFont(ttf);
    #endif
}


void Font::WriteWrap(SDL_Surface* surface, int x, int y, int w, int h,const std::string& text) const
{
    WriteWrap(surface,x,y,w,h,  text.c_str() );
}


void Font::Write(SDL_Surface *Surface, int x, int y, const char *text, Uint8 r, Uint8 g, Uint8 b, Uint8 alpha) const
{

    bool recolor=( (r!=255) || (g!=255) || (b!=255) || (alpha!=255));


    const char* c;
    int charoffset;
    unsigned int fh=Height();
    SDL_Rect srcrect, dstrect;

    if((text == NULL) || (*text=='\0'))
    return;

    if(ttf)
    {
      #ifndef NOTRUETYPE
        srcrect.y = 0;
        srcrect.x = 0;
        int io1,io2;
        TTF_SizeUTF8(ttf,text, &io1, &io2) ;

        srcrect.w=(Uint16)(io1);
        srcrect.h=(Uint16)(io2);

        dstrect.x = x;
        dstrect.y = y;

        SDL_Surface* sur=TTF_RenderUTF8_Blended(ttf,text,ttfcol);
        if(recolor)
            SDL_BlitSurface(sur, &srcrect, Surface, &dstrect, r,g,b,alpha);
        else
            SDL_BlitSurface(sur, &srcrect, Surface, &dstrect);

        SDL_FreeSurface(sur);
      #endif
        return;

    }




    // these values won't change in the loop
    srcrect.y = 1;
    dstrect.y = y;
    srcrect.h = dstrect.h = tex->h - 1;

    Sint16 sw=Surface->w;
    Sint16 sh=Surface->h;
    int ox=x;
    int tw;


    for(c = text; (*c != '\0') && (x <= sw) && (y<=sh) ; c++)
    {
        charoffset = ((int) (*c - 33)) * 2 + 1;
        // skip spaces and nonprintable characters

        if (*c == '\n')
        {
            x=ox;
            y+=fh;
            dstrect.y=y;
            continue;
        }

        if (*c == ' ' || charoffset < 0 || charoffset > MaxPos) {
            x += CharPos[2]-CharPos[1];
            continue;
        }

         tw=srcrect.w = dstrect.w =
            (CharPos[charoffset+2] + CharPos[charoffset+1])/2 -
            (CharPos[charoffset] + CharPos[charoffset-1])/2;


        srcrect.x = (CharPos[charoffset]+CharPos[charoffset-1])/2;
        dstrect.x = int(x - (CharPos[charoffset] - CharPos[charoffset-1]) /2.0);

        if (x+tw>=0)
        {
            if(recolor)
                SDL_BlitSurface(tex, &srcrect, Surface, &dstrect, r,g,b,alpha);
            else
                SDL_BlitSurface(tex, &srcrect, Surface, &dstrect);
        }

        x += CharPos[charoffset+1] - CharPos[charoffset];
    }
}



bool IsNormalChar(const char &a)
{
    return (((a>='0') && (a<='9')) || ((a>='a') && (a<='z')) || ((a>='A') && (a<='Z')));
}


void Font::WriteWrap(SDL_Surface* Surface, int x, int y, int w, int h,const char* text) const
{
    char word[25];
    int wi,autoprint;
    const char* c;
    int charoffset;
    unsigned int fh=Height();
    SDL_Rect srcrect, dstrect;

    if(text == NULL)
    return;



    // these values won't change in the loop
    srcrect.y = 1;
    dstrect.y = y;
    dstrect.h = fh - 1;

    Sint16 sw=Surface->w;
    Sint16 sh=Surface->h;
    sw= sw<x+w?sw:(x+w);
    sh= sh<y+h?sh:(y+h);


    #ifndef NOTRUETYPE
    if(ttf)
    {
        const char* c=text;
        int cx=x;
        int spcwidth;
        TTF_SizeUTF8(ttf," ",&spcwidth,NULL);
        while(y<=sh)
        {
            while (*c<=' ')
            {
                if(*c=='\0') break;
                if(*c=='\n')
                {
                    y+=fh;
                    cx=x;
                }
                else
                {
                    cx+=spcwidth;
                };
                c++;
            }

            if(*c=='\0') break;
            if(y>sh) break;

            const char*aux=c;
            int count=0;
            int wordwidth=0;

            char tester[2]; tester[1]='\0';
            while(*aux>' ')
            {
                int chw;
                tester[0]=*aux;
                TTF_SizeUTF8(ttf,tester,&chw,NULL);
                if(chw+wordwidth>sw) break;
                wordwidth+=chw;
                count++;
                aux++;
            }
            string tem;

            tem.resize(count);
            count=0;
            while(c<aux) tem[count++]=*(c++);

            int textwidth;
            TTF_SizeUTF8(ttf,tem.c_str(),&textwidth,NULL);
            if ((cx+textwidth>=sw) && (cx!=x))
            {
                cx=x;
                y+=fh;
                if(y>sh) break;
            }

            SDL_Surface * rendered = TTF_RenderUTF8_Blended(ttf,tem.c_str(),ttfcol);
            SDL_BlitSurface(rendered, 0,0, sw-x  , sh-y  , Surface, cx,y);
            SDL_FreeSurface(rendered);
            cx+=textwidth;
        }
        return;
    }
    #endif


    int ox=x;
    int tw;
    srcrect.h = (sh-y<fh)?sh-y:fh;


    word[0]='\0';
    wi=autoprint=0;
    for(c = text; (y<=sh) ; c++)
    {
        //fprintf(stderr,"hoo %c %d\n",*c,autoprint);
        if (autoprint>0)
        {
            if (*c=='\0') break;
            autoprint--;

            charoffset = ((int) (*c - 33)) * 2 + 1;

            if (*c == '\n')
            {
                x=ox;
                y+=fh;
                dstrect.y=y;
                srcrect.h = (sh-y<fh)?sh-y:fh;
                continue;
            }
            // skip spaces and nonprintable characters
            if (*c == ' ' || charoffset < 0 || charoffset > MaxPos) {
                x += CharPos[2]-CharPos[1];
                continue;
            }
            //if (x <= sw) break; //too big
             tw=srcrect.w = dstrect.w =
                (CharPos[charoffset+2] + CharPos[charoffset+1])/2 -
                (CharPos[charoffset] + CharPos[charoffset-1])/2;


            srcrect.x = (CharPos[charoffset]+CharPos[charoffset-1])/2;
            dstrect.x = int(x - (CharPos[charoffset] - CharPos[charoffset-1]) /2.0);

            if (x+tw>=sw)
            {
                dstrect.x=x=ox;
                y+=fh;
                dstrect.y=y;
                srcrect.h = (sh-y<fh)?sh-y:fh;
                if (y>sh) continue;
            }
            x += CharPos[charoffset+1] - CharPos[charoffset];
            if (x+tw>=0) SDL_BlitSurface(tex, &srcrect, Surface, &dstrect);
        }
        else if ( (IsNormalChar(*c)) && (wi<20))
        {
            word[wi]=*c;
            wi++;
            word[wi]='\0';
        }
        else
        {
            if (x+TextWidth(word)+1>=sw)
            {
                dstrect.x=x=ox;
                y+=fh;
                dstrect.y=y;
                srcrect.h = (sh-y<fh)?sh-y:fh;

                //if (y>sh)
                  //  continue;
            }
            autoprint=wi+1;
            c=c-wi-1;
            wi=0;
            word[0]='\0';

        }



    }


}

std::vector<std::string> Font::splitByLines(const char* text, int w) const
{
    std::vector<std::string> r;
    char word[25];
    int wi,autoprint;

    if(text == NULL) return r;


    int tw=0;
    int lw=0;

    word[0]='\0';
    wi=autoprint=0;

    const char * c=text;
    string line="";
    int spc=CharWidth(' ');
    bool lineended=false;
    bool lastwasspace=false;

    while(true)
    {
        int chw = CharWidth(*c);

        if((wi>=20) || (lastwasspace&&(*c>' ')) || (*c<=' ')  )
        {
            lastwasspace=false;
            if (wi>0)
            {
                tw=TextWidth(word);
                if(lw+tw>w)
                {
                    lineended=true;
                    r.push_back(line);
                    strcat(word," ");
                    line=word;
                    lw=tw;
                }
                else
                {
                    line+=word;
                    lw+=tw;
                }
                tw=0;
                word[0]='\0';
                wi=0;
            }
            if (*c=='\n')
            {
                r.push_back(line);
                lw=0;
                line="";
            }
            else if((*c!='\0')&&(*c<=' '))
            {
                if(! lineended)
                {
                    lastwasspace=true;
                    word[0]=' ';
                    word[1]='\0';
                    wi=1;
                    tw=spc;
                }
                else lineended=false;
            }
        }
        if(*c>' ')
        {
            word[wi]=*c;
            tw+=chw;
            wi++;
            word[wi]='\0';
        }
        if(*c=='\0') break;
        c++;
    }
    if(line!="") r.push_back(line);
    return r;


}







unsigned int Font::TextWidth(const char *text) const
{
    #ifndef NOTRUETYPE
    if(ttf)
    {
        int r;
        TTF_SizeUTF8(ttf,text,&r,NULL);
        return (unsigned int)(r);
    }
    #endif

    const char* c;
    int charoffset=0;
    unsigned int width = 0;

    if(text == NULL)
    return 0;

    for(c = text; *c != '\0'; c++) {
        charoffset = ((int) *c - 33) * 2 + 1;
        // skip spaces and nonprintable characters
        if (*c == ' ' || charoffset < 0 || charoffset > MaxPos) {
            width += CharPos[2]-CharPos[1];
            continue;
        }

        width += CharPos[charoffset+1] - CharPos[charoffset];
    }

    return width;

}

unsigned int Font::WrappedTextHeight(const string text, int w) const
{
    return Height() * splitByLines(text, w).size();
}

unsigned int Font::Height() const
{
    #ifndef NOTRUETYPE
    if(ttf)
    {
        int r;
        TTF_SizeUTF8(ttf,"A ",NULL,&r);
        return (unsigned int)(r);
    }
    #endif
    return tex->h - 1;


}

unsigned int Font::CharWidth(const char c) const
{
    #ifndef NOTRUETYPE
    if(ttf)
    {
        int r;
        char s[2];
        s[0]=c;
        s[1]='\0';
        TTF_SizeUTF8(ttf,s,&r,NULL);
        return (unsigned int)(r);
    }
    #endif
    int charoffset = ((int)(c) - 33) * 2 + 1;
    if (c == ' ' || charoffset < 0 || charoffset > MaxPos)  return CharPos[2]-CharPos[1];

    return CharPos[charoffset+1] - CharPos[charoffset];
}

void Font::WriteCenter(SDL_Surface *Surface, int y, const char *text)
{
    Write(Surface, Surface->w/2 - TextWidth(text)/2, y, text,255,255,255,255);
}
