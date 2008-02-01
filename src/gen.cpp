/*
 Xye License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/


#include "gen.h"
#include "vxsdl.h"
const float F_RAND_MAX = (float)(RAND_MAX);


//Random stuff:
void Randomize() { srand (time (0)); }

int GetRandomInt(int min, int max)
{
	return (int)(min+(rand() / F_RAND_MAX)*(max-min)+0.5);
}


template<class T>
T GetRandom(T min, T max)
{
	return (T)(min+(rand() / F_RAND_MAX)*(max-min)+0.5);
}

template<class T>
bool doublematch(T a,T b,T c)
{
    return((a==b) || (a==c));
}

template bool doublematch<char>(char,char,char);


template char GetRandom<char>(char,char);
template double GetRandom<double>(double,double);


//For example Chance(0.5) is a 50% chance of returning true,
//A Chance(0.75) has a 75% percent chance to return true.
bool Chance(float x)
{
	return (  (rand () / F_RAND_MAX) <= x);
}

unsigned int RandomRGB(unsigned char a,unsigned char b)
{
	return(    (   (255*256+GetRandomInt(a,b))*256 +    GetRandomInt(a,b)) * 256 + GetRandomInt(a,b) );
}

unsigned int RandomRGB()
{
	return(RandomRGB(0,255));
}


//String stuff:

char* string2charp(string* s)
{
    int l=s->length();
    //Oh no! next code is the lamest thing I have ever written, was causing a lot of issues: if (!l) return "";
	char* r=new char[l+1];
	for (int i=0;i<l;i++)
	    r[i]=(*s)[i];
    r[l]='\0';
    return (r);

}


struct ColorStruct
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char A;
};

unsigned char mixChannel(unsigned char A, unsigned char B)
{
	if (A>B)
	    return ( B + (unsigned char)((A-B) / 2 ));

 return ( A + (unsigned char)((B-A) / 2 ));
}

unsigned int MixColors(unsigned int A, unsigned int B)
{
	ColorStruct *sA=(ColorStruct*)(&A);
	ColorStruct *sB=(ColorStruct*)(&B);
	ColorStruct R;
    R.A= ( sA->A < sB->A ? sA->A:sB->A);
    R.B= mixChannel(sA->B,sB->B);
    R.G= mixChannel(sA->G,sB->G);
    R.R= mixChannel(sA->R,sB->R);
    return *( (unsigned int *)(&R)  )	;
}

SDL_Color& MixColors(SDL_Color& A, SDL_Color& B)
{
	SDL_Color* R=new SDL_Color;
	R->r= mixChannel(A.r,B.r);
	R->g= mixChannel(A.g,B.g);
	R->b= mixChannel(A.b,B.b);
	R->unused=255;
	return (*R);
}

bool TryS2I(std::string &str, int& r)
{
    r=0;
    int L=str.length();
    if (L<=0) return false;
    int i=0;
    bool neg=(str[0]=='-');
    if (neg) i++;
    while (i<L)
    {
        //if (str[i]=='\r') { i++; continue; }

        if ((str[i]>'9') || (str[i]<'0')) return false;
        r*=10;
        r+= (str[i] - '0');
        i++;
    }
    if (neg) r*=-1;
    return true;
}











