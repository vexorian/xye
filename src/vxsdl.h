#define vxsdlh
#include <stdio.h>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

void Init_RecolorTable();

#ifndef DEF_FIXEDRGBA
#define DEF_FIXEDRGBA
struct FixedRGBA
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};
#endif


#ifndef SDL_32BYTE1
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        #define SDL_32BYTE1 0xff000000
        #define SDL_32BYTE2 0x00ff0000
        #define SDL_32BYTE3 0x0000ff00
        #define SDL_32BYTE4 0x000000ff
    #else
        #define SDL_32BYTE1 0x000000ff
        #define SDL_32BYTE2 0x0000ff00
        #define SDL_32BYTE3 0x00ff0000
        #define SDL_32BYTE4 0xff000000

    #endif
    #define SDL_ENDIAN32MASKS SDL_32BYTE1,SDL_32BYTE2,SDL_32BYTE3,SDL_32BYTE4
#endif


//A surface where it is possible to freely use the FixedRGBA struct as a pixel.
SDL_Surface * CreateFixedRGBASurface(Uint32 flags, int width, int height);

Uint32 SDL_MapRGB(SDL_PixelFormat *fmt, SDL_Color&S);

SDL_Surface * RecolorPartOfSurface(SDL_Surface *src, Sint16 sx, Sint16 sy, Uint16 sw, Uint16 sh, Uint8 newR, Uint8 newG, Uint8 newB, Uint8 newA);

SDL_Surface * RecolorPartOfSurface(SDL_Surface *src, SDL_Rect *srcrect, Uint8 newR, Uint8 newG, Uint8 newB, Uint8 newA);

SDL_Surface * RecolorPartOfSurface(SDL_Surface *src, SDL_Rect *srcrect, Uint8 newR, Uint8 newG, Uint8 newB, Uint8 newA);

int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect, Uint8 newR, Uint8 newG, Uint8 newB, Uint8 newA);


int SDL_BlitSurface(SDL_Surface *src, Sint16 sx, Sint16 sy, Uint16 sw, Uint16 sh, SDL_Surface *dst, Sint16 dx, Sint16 dy, Uint8 newR, Uint8 newG, Uint8 newB, Uint8 newA);

int SDL_BlitSurface(SDL_Surface *src, Sint16 sx, Sint16 sy, Uint16 sw, Uint16 sh, SDL_Surface *dst, Sint16 dx, Sint16 dy);
int SDL_FillRect(SDL_Surface *dst, Sint16 x, Sint16 y, Uint16 w, Uint16 h, Uint32 color);







#ifndef DEF_DAVINCI
#define DEF_DAVINCI 1

struct LuminositySprites
{
    SDL_Surface* sprites;
    SDL_Surface* luminosity;
};

class Drawer
{
 private:
  SDL_Surface* src;
  SDL_Surface* src_lights;
  Uint8 R;
  Uint8 G;
  Uint8 B;
  Uint8 A;
  SDL_Rect sr;
  bool recolor;
 public:

  Drawer(SDL_Surface*SourceTexture, SDL_Rect* rect);
  Drawer(SDL_Surface*SourceTexture, Sint16 x, Sint16 y, Uint16 w, Uint16 h);

  Drawer(LuminositySprites & Source, SDL_Rect* rect);
  Drawer(LuminositySprites & Source, Sint16 x, Sint16 y, Uint16 w, Uint16 h);


  void ChangeRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h);
  void ChangeRect(SDL_Rect* nw);
  void SetColors(Uint8 nR, Uint8 nG, Uint8 nB, Uint8 nA=255);
  void SetColors(SDL_Color* C,Uint8 alpha=255);
  void SetColors(SDL_Color& C,Uint8 alpha=255);
  int Draw(SDL_Surface* target, Sint16 x, Sint16 y);



};


#endif




#ifndef DEF_RECOLORCACHE
#define DEF_RECOLORCACHE 1

struct recolorcache_entry
{
    Uint8 r,g,b,a;
    SDL_Surface* cached;
    recolorcache_entry* next;
};

class RecolorCache //we only need to cache for one texture, so it is a static member class ,
{
 private:
  static SDL_Surface* cacheseed;
  static recolorcache_entry* first;
  static recolorcache_entry* entry(Uint8 r,Uint8 g,Uint8 b, Uint8 a,bool createifmissing);

 public:

  static void init();
  static void restart(SDL_Surface* scacheseed);
  static void clean();
  static void savecolor(Uint8 r,Uint8 g,Uint8 b, Uint8 a);
  static void savecolor(SDL_Color *c);
  static SDL_Surface* loadcolor(SDL_Surface* sf,Uint8 r,Uint8 g,Uint8 b, Uint8 a);



};


#endif
