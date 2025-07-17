/*
 License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/

#include "browser.h"

#include <errno.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>



/*
class Folder
{
 private:
    dir* D;
 public:
    Folder(const char* dirname);
    bool Open();
    void Reset(); //go back to first
    const char* NextSubFolder();
    const char* NextFile();
    const char* NextFileOfType(const char* ext);
    void Close();

}*/

Folder::Folder(const char* dirname)
{
    strcpy(Folder::dirname,dirname);
    D=NULL;
}

bool Folder::Open()
{
    return (D=opendir(dirname));
}


void Folder::Close()
{
    closedir(D);
    D=NULL;
}

bool Folder::Reset()
{
    closedir(D);
    return (D=opendir(dirname));
}

Folder::~Folder()
{
    if(D) closedir(D);
}



bool AlwaysTrue(const char* c) {return true;}
const char* Folder::NextFile()
{
 return NextFileMatching(AlwaysTrue);
}

bool HasExtension(const char* n, const char* ext )
{
    int l1=strlen(n) ,l2=strlen(ext);
    int i=l1-1,j=l2-1;
    while ((i>=0) && (j>=0) && (n[i]==ext[j]))
    { i--; j--; }
    return ((i>=0) && (j<0) && (n[i]=='.'));


}

void ToLowerCase(char* n)
{
    for (int i=0;n[i]!='0';i++)
        if ((n[i]>=65) && (n[i]<=90)) n[i]+=32;
}

const char* Folder::NextFileOfType(const char* ext)
{
    dirent *ent;
    DIR* tm;
    const char * r;
    int L=strlen(dirname),L2;
    char realpath[FILENAME_MAX];
    while (ent=readdir(D))
    {
        r=ent->d_name;
        errno=0;
        L2=strlen(r);
        strcpy(realpath,dirname);
        strcat(realpath,r);

        tm=opendir(realpath);
        if (tm)
           closedir(tm);
        else if (HasExtension(r,ext))
            return(r);

    }
 return NULL;

}

const char* Folder::NextFileMatching(  bool (*cond)(const char*)    )
{
    dirent *ent;
    DIR* tm;
    const char * r;
    int L=strlen(dirname),L2;
    char realpath[FILENAME_MAX];
    while (ent=readdir(D))
    {
        r=ent->d_name;
        errno=0;
        L2=strlen(r);
        strcpy(realpath,dirname);
        strcat(realpath,r);

        tm=opendir(realpath);
        if (tm)
           closedir(tm);
        else if (cond(r) )
            return(r);

    }
 return NULL;
}

const char* Folder::NextSubFolder()
{
    dirent *ent;
    DIR* tm;
    const char * r;
    int L=strlen(dirname),L2;
    char realpath[FILENAME_MAX];
    while (ent=readdir(D))
    {


        r=ent->d_name;

        if ((! strcmp(r,".")) || (! strcmp(r,"..")))
            continue;

        errno=0;
        L2=strlen(r);
        strcpy(realpath,dirname);
        strcat(realpath,r);

        tm=opendir(realpath);
        if (tm)
        {
           closedir(tm);
           return(r);
        }

    }
 return NULL;
}

bool TryToOpenFolder(const char* path)
{
    Folder F(path);
    if (F.Open()) {
        F.Close();
        return true;
    }
    return false;
}
