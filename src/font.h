#define FONTINCLUDED 1
#include "vxsdl.h"

#ifndef NOTRUETYPE
    #include <SDL/SDL_ttf.h>
#else
    struct TTF_Font{int a;};
#endif


#include <cstdio>
#include <string>
#include <vector>



class Font
{
public:
 unsigned int Height() const;
 unsigned int TextWidth(const char* text) const;
 unsigned int TextWidth(const std::string text) const { return TextWidth(text.c_str()); };
 unsigned int WrappedTextHeight(const std::string text, int w) const;
 unsigned int CharWidth(const char c) const;



 void Write(SDL_Surface* surface, int x, int y,const char* text, Uint8 r, Uint8 g, Uint8 b, Uint8 alpha) const;

 void WriteWrap(SDL_Surface* surface, int x, int y, int w, int h,const char* text) const;
 void WriteWrap(SDL_Surface* surface, int x, int y, int w, int h,const std::string& text) const;

 std::vector<std::string> splitByLines(const char* s, int w) const;
 inline std::vector<std::string> splitByLines(const std::string s, int w) const { return splitByLines(s.c_str(),w); }

 void WriteCenter(SDL_Surface *Surface, int y, const char *text);


 inline void Write(SDL_Surface* surface, int x, int y,const char* text) const
 {
    Font::Write(surface, x, y,text,255,255,255,255);
 }

 inline void Write(SDL_Surface *surface, int x, int y, const std::string& text) const
 {
    Write(surface,x,y,text.c_str());
 }

 Font(SDL_Surface* surface, SDL_Color& color);
 Font(SDL_Surface* surface, Uint8 R,  Uint8 G, Uint8 B);

 Font(const char* truetype, int size, SDL_Color& color);
 Font(const char* truetype, int size, Uint8 R,  Uint8 G, Uint8 B);

 ~Font();

 private:
    SDL_Surface *tex;
    TTF_Font    *ttf;
    SDL_Color ttfcol;
    int CharPos[512];
    int MaxPos;
    void fontinit(SDL_Surface* surface,  Uint8 R,  Uint8 G, Uint8 B);
    #ifndef NOTRUETYPE
    void fontinit(const char* truetype, int size, Uint8 R,  Uint8 G, Uint8 B);
    #endif
};
