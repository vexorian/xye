/*
 Xye License (it is a PNG/ZLIB license)

Copyright (c) 2025 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/



#include "vxsdl.h"

Uint8 Look[256][256];
bool LookSet=false;

void Init_RecolorTable()
{
    RecolorCache::init();
    if (! LookSet)
    {
    LookSet=true;
    Uint32 i,j;
    for ( i=0; i<256;i++)
        for ( j=0; j<256;j++)
            {
                Look[i][j]= (Uint8)( ( i / 255.)*j);
            }
    }

}
/*
Uint32 FixedRGBAPixel(Uint8 R, Uint8 G, Uint8 B, Uint8 A)
{
    FixedRGBA F;
    F.r=R;
    F.g=G;
    F.b=B;
    F.a=A;
    return *((Uint32*)(&F));
}

Uint32 SDL_Color2FixedRGBAPixel(SDL_Color& S)
{
    FixedRGBA F;
    F.r=S.r;
    F.g=S.g;
    F.b=S.b;
    F.a=S.unused;
    return *((Uint32*)(&F));

}
*/

Uint32 SDL_MapRGB(SDL_PixelFormat *fmt, SDL_Color&S)
{
    return SDL_MapRGB(fmt,S.r,S.g,S.b);
}

/*
Uint32 RGBAfromSDLColor(SDL_Color&S)
{
    FixedRGBA F;
    F.r=S.r;
    F.g=S.g;
    F.b=S.b;
    F.a=S.unused;
    return *((Uint32*)(&F));
}
*/

SDL_Surface * CreateFixedRGBASurface(Uint32 flags, int width, int height)
{
    return (SDL_CreateRGBSurface(SDL_SRCALPHA|flags,width,height,32,
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        0xff000000,0x00ff0000, 0x0000ff00, 0x000000ff));
    #else
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000));
    #endif
}

SDL_Surface * RecolorPartOfSurface(SDL_Surface *src, Sint16 sx, Sint16 sy, Uint16 sw, Uint16 sh, Uint8 newR, Uint8 newG, Uint8 newB, Uint8 newA)
{


   FixedRGBA col;
   Uint32* colInt32= (Uint32*)(&col);

   Uint32 siw= src->w;

   SDL_Surface* r = CreateFixedRGBASurface(SDL_SRCALPHA, sw, sh);



   SDL_LockSurface(src);
   SDL_LockSurface(r);



   Uint32 tm=0,tm2=0;
   Uint32* src_pix= (Uint32*)(src->pixels);
   Uint32* r_pix= (Uint32*)(r->pixels);

   Uint8 rd=0,g=0,b=0,a=0,aux=0;

   Uint32 i,j;

   Uint32 k=0,l=0;

   for (i=sx;i<(sx+sw);i++)
   {
       l=0;
       for (j=sy;j<(sy+sh);j++)
       {
            tm= *(src_pix + (j * siw) +  i);

            SDL_GetRGBA(tm, src->format, &rd, &g, &b, &a);
            if (a>0)
            {
                rd =  Look[newR][rd];
                g =  Look[newG][g];
                b =  Look[newB][b];
                a =  Look[newA][a];
            }
            col.a=a;
            col.b=b;
            col.g=g;
            col.r=rd;

            //if ( (col.b|col.g|col.r) == 0) col.a=255;


            *(r_pix + ((l) * sw) +  (k)) = *colInt32;
            l++;
        }
        k++;
   }
   SDL_UnlockSurface(src);
   SDL_UnlockSurface(r);
   return(r);

}






SDL_Surface * RecolorPartOfSurface(SDL_Surface *src, SDL_Rect *srcrect, Uint8 newR, Uint8 newG, Uint8 newB, Uint8 newA)
{
    return(RecolorPartOfSurface(src,srcrect->x, srcrect->y, srcrect->w, srcrect->h, newR, newG, newB, newA));
}

int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect, Uint8 newR, Uint8 newG, Uint8 newB, Uint8 newA)
{
   SDL_Surface *tm= RecolorCache::loadcolor(src,newR,newG,newB,newA);
   if (tm)
   {
       return SDL_BlitSurface(tm,srcrect,dst,dstrect);
   }



   tm= RecolorPartOfSurface(src,srcrect,newR,newG,newB,newA)    ;
   int Result=SDL_BlitSurface(tm,0,dst,dstrect);
   SDL_FreeSurface(tm);
   return(Result);
}


int SDL_BlitSurface(SDL_Surface *src, Sint16 sx, Sint16 sy, Uint16 sw, Uint16 sh, SDL_Surface *dst, Sint16 dx, Sint16 dy, Uint8 newR, Uint8 newG, Uint8 newB, Uint8 newA)
{
   SDL_Surface *tm= RecolorCache::loadcolor(src,newR,newG,newB,newA);
   SDL_Rect tar;
   tar.x=dx;
   tar.y=dy;
   tar.w=sw; //I think these are ignored
   tar.h=sh;

   if (tm)
   {
       SDL_Rect srt;
       srt.y=sy;
       srt.x=sx;
       srt.w=sw;
       srt.h=sh;
       return SDL_BlitSurface(tm,&srt,dst,&tar);
   }


   tm=RecolorPartOfSurface(src,sx,sy,sw,sh,newR,newG,newB,newA) ;
   int Result=SDL_BlitSurface(tm,0,dst,&tar);
   SDL_FreeSurface(tm);
   return(Result);
}

int SDL_BlitSurface(SDL_Surface *src, Sint16 sx, Sint16 sy, Uint16 sw, Uint16 sh, SDL_Surface *dst, Sint16 dx, Sint16 dy)
{
    SDL_Rect srr;
    SDL_Rect tar;
    srr.x=sx;
    srr.y=sy;
    srr.w=sw;
    srr.h=sh;

    tar.x=dx;
    tar.y=dy;
    tar.w=sw; //I think these are ignored
    tar.h=sh;

    return(SDL_BlitSurface(src,&srr,dst,&tar) );

}


int SDL_FillRect(SDL_Surface *dst, Sint16 x, Sint16 y, Uint16 w, Uint16 h, Uint32 color)
{
    SDL_Rect R;
    R.x=x;
    R.y=y;
    R.w=w;
    R.h=h;
    return SDL_FillRect(dst, &R, color);
}



/*** Class Drawer - Our Artist **/
Drawer::Drawer(SDL_Surface*SourceTexture, SDL_Rect* rect)
{
    R=G=B=A=255;recolor=false;
    sr.x=rect->x; sr.y=rect->y; sr.w=rect->w; sr.h=rect->h;
    src=SourceTexture;
    src_lights = NULL;
}

Drawer::Drawer(SDL_Surface*SourceTexture, Sint16 x, Sint16 y, Uint16 w, Uint16 h)
{
    R=G=B=A=255;recolor=false;
    sr.x=x; sr.y=y; sr.w=w; sr.h=h;
    src=SourceTexture;
    src_lights = NULL;
}

Drawer::Drawer( LuminositySprites & source, SDL_Rect* rect)
{
    R=G=B=A=255;recolor=false;
    sr.x=rect->x; sr.y=rect->y; sr.w=rect->w; sr.h=rect->h;
    src=source.sprites;
    src_lights = source.luminosity;
}

Drawer::Drawer(LuminositySprites & source, Sint16 x, Sint16 y, Uint16 w, Uint16 h)
{
    R=G=B=A=255;recolor=false;
    sr.x=x; sr.y=y; sr.w=w; sr.h=h;
    src=source.sprites;
    src_lights = source.luminosity;
}


void Drawer::ChangeRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h)
{
    sr.x=x; sr.y=y; sr.w=w; sr.h=h;
}

void Drawer::ChangeRect(SDL_Rect* rect)
{
    sr.x=rect->x; sr.y=rect->y; sr.w=rect->w; sr.h=rect->h;
}

void Drawer::SetColors(Uint8 nR, Uint8 nG, Uint8 nB, Uint8 nA)
{
    R=nR;G=nG;B=nB;A=nA;
    recolor=((R!=255) || (G!=255) || (B!=255) || (A!=255));
}

void Drawer::SetColors(SDL_Color* C,Uint8 alpha)
{
    R=C->r;G=C->g;B=C->b;A=alpha;
    recolor=((R!=255) || (G!=255) || (B!=255) || (A!=255) );
}

void Drawer::SetColors(SDL_Color& C,Uint8 alpha)
{
    R=C.r;G=C.g;B=C.b;A=alpha;
    recolor=((R!=255) || (G!=255) || (B!=255) || (A!=255) );
}


int Drawer::Draw(SDL_Surface* target, Sint16 x, Sint16 y)
{
    SDL_Rect T;
    T.x=x;
    T.y=y;
    T.w=sr.w;
    T.h=sr.h;
    int r;
    if (recolor)
        r=SDL_BlitSurface(src, &sr, target, &T, R, G, B, A);
    else
        r=SDL_BlitSurface(src, &sr, target, &T);

    if( src_lights != NULL)
    {
        if(SDL_BlitSurface(src_lights, &sr, target, &T) == -1)
            r=-1;
    }

    return r;


}

/** endo of class Drawer **/

/** start of recolor cache **/

SDL_Surface* RecolorCache::cacheseed;
recolorcache_entry* RecolorCache::first;

void RecolorCache::init()
{
    cacheseed=NULL;
    first=NULL;
}

void RecolorCache::restart(SDL_Surface* scacheseed)
{
    clean();
    cacheseed=scacheseed;
}

void RecolorCache::clean()
{
    recolorcache_entry* tm;
    while(first)
    {
        tm=first->next;
        SDL_FreeSurface(first->cached);
        delete first;
        first=tm;
    }
    cacheseed=NULL;
}

recolorcache_entry* RecolorCache::entry(Uint8 r,Uint8 g,Uint8 b, Uint8 a,bool createifmissing)
{
    recolorcache_entry* tm=first,*ls=NULL;
    while (tm)
    {
        if ((tm->a==a) && (tm->g==g) && (tm->b==b) && (tm->r==r))
            return tm;
        ls=tm;
        tm=tm->next;
    }
    if (! createifmissing) return NULL;
    tm=new recolorcache_entry;
    tm->a=a;
    tm->b=b;
    tm->g=g;
    tm->r=r;
    tm->next=NULL;
    tm->cached=NULL;
    if (ls) ls->next=tm;
    else first=tm;

 return tm;
}

void RecolorCache::savecolor(Uint8 r,Uint8 g, Uint8 b, Uint8 a)
{
    if (!cacheseed) return;
    recolorcache_entry* tm=entry(r,g,b,a,true);
    if (! (tm->cached))
        tm->cached=RecolorPartOfSurface(cacheseed,0,0,cacheseed->w,cacheseed->h,r,g,b,a);
}


void RecolorCache::savecolor(SDL_Color *c)
{
    SDL_Color &i=*c;
    savecolor(i.r,i.g,i.b,i.unused);
}

SDL_Surface* RecolorCache::loadcolor(SDL_Surface* sf,Uint8 r,Uint8 g,Uint8 b, Uint8 a)
{
    if (sf!=cacheseed) return NULL;
    recolorcache_entry* tm=entry(r,g,b,a,false);
    if (tm) return tm->cached;
    return NULL;
}




/** Recolor Cache ends**/
