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
#include<cstdio>
#include<iostream>
#include<fstream>
#include <unistd.h>

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
    return *( (unsigned int *)(&R)  )   ;
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

bool TryS2I(const std::string &str, int& r)
{
    return (sscanf(str.c_str(),"%d", &r)==1);
}

std::string GetFileNameNoExtension( const char* filename )
{
    int len= strlen(filename), k=len-1, r=len-1;
    while ( (k>=0) && (filename[k]!='/') && (filename[k]!='\\') )
        k--;
    while ( (r>=0) && (filename[r]!='.') )
        r--;
    string name;
    if ( r>k)
        name = string(filename).substr(k+1, r-k-1);
    else
        name = &(filename[k+1]);
    return name;
}

std::string StripPath( const std::string in)
{
    int i=in.length()-1;
    while( i>=0 && (in[i]!='/') && (in[i]!='\\') ) {
        i--;
    }
    return in.substr(i+1);
}

bool DoesFileExist( const char* filename)
{
    #ifdef _WIN32
        std::ifstream st;
        st.open(filename,std::ios::in);
        if (st.is_open()) {
            st.close();
            return true;
        } else {
            return false;
        }
    #else
        return (access(filename,0)==0);
    #endif
}
bool DoesFileExist( const string filename)
{
    return DoesFileExist(filename.c_str());
}


string  StripXML(const string s)
{
    string r="";
    
    int k=0;
    for (int i=0;i<s.length();i++)
    {
        switch(s[i])
        {
            case '<':
                r+=s.substr(k,i-k);r+="&lt;";k=i+1;break;
            case '>':
                r+=s.substr(k,i-k);r+="&gt;";k=i+1;break;
            case '&':
                r+=s.substr(k,i-k);r+="&amp;";k=i+1;break;
            case '"':
                r+=s.substr(k,i-k);r+="&quot;";k=i+1;break;    
                
            case '\'':
                r+=s.substr(k,i-k);r+="&apos;";k=i+1;break;    
            
        }
    }
    if(k<s.length()) r+=s.substr(k,s.length()-k);
    
    return r;
}





