/*
 License (it is a PNG/ZLIB license)

Copyright (c) 2006 Victor Hugo Soliz Kuncar

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*/

//PORTABLE changes behaviour of the win32 mode, for compatibility with portableapps.com menu.
//#define PORTABLE 1


//DATAPATH determines the place where /levels/ and /res/ folders are located
//#define DATAPATH /usr/local/share/xye/

/*todo:

    * Modularize xye.cpp so we don't have 1000 classes in the same file
    * Figure out a good excuse for this main.cpp file
    * GUI
    * Editor
    * More objects


*/

#include "xye.h"
#include "xyedit.h"
#include "xye_script.h"
#include "options.h"

#include "browser.h"


#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
 #define MKDIR(d, p) mkdir(d)
#else
 #define MKDIR mkdir
#endif



bool TryToOpenFolder(const char* path)
{
    Folder F(path);
    if (F.Open())
    {
        F.Close();
        return true;
    }
    return false;
}

int main ( int argc, char** argv )
{
    string editmode("");
    string playmode("");
    options::Dir="";

    if (argc>1)
    {
        string a1=argv[1];
        if(a1=="--edit")
        {
            if(argc==2)
            {
                printf("--edit : Using default level name tmp.xye\n");
                editmode="tmp.xye";

            }
            else
            {
                printf("--edit : will edit %s\n",argv[2]);
                 editmode=argv[2];
                 if(argc>3)
                     options::Dir=argv[3];
            }
        }
        else if (a1=="--playlevel")
        {
            if(argc==2)
            {
                printf("--playlevel : Using default level name tmp.xye\n");
                playmode="tmp.xye";

            }
            else
            {
                printf("--playlevel : will play %s\n",argv[2]);
                 playmode=argv[2];
                 if(argc>3)
                     options::Dir=argv[3];
            }
        }
        else
            options::Dir=a1;
    }
    if(options::Dir=="")
    {
    #ifdef DATA_PATH
        options::Dir=DATA_PATH;
    #else
        #ifdef PORTABLE
            options::Dir=argv[0];
            int i=options::Dir.length()-1;
            while ( (i>=0) && (options::Dir[i]!= '\\') && (options::Dir[i]!= '/') ) i--;
            options::Dir.resize(i+1);
            while (i>=0)
            {
                if (options::Dir[i]=='\\') options::Dir[i]='/';
                i--;
            }

            //if(options::Dir[options::Dir.length()-1]!='/') options::Dir+='/';
        #else
            options::Dir="./";
        #endif

    #endif
    }
    string & pn = options::Dir;
    printf("Will look on %s for data files (non-user levels, skins)\n",  pn.c_str() );


    if (! TryToOpenFolder(pn.c_str()))
    {
        printf("Exception: Can't open/find data folder\n");
        return 1;
    }

    if ( (pn.length() > 0) && ( *pn.rbegin() !='/')) pn+='/'; //for safety

const char* home;
editor::myLevelsPath = "";
string &mylevelsplace = editor::myLevelsPath;

if (home=getenv("HOME"))
{
    printf("initializing home:\n");
    char* tmloc=new char[strlen(home)+strlen("/.xye/")+1];
    strcpy(tmloc,home);strcat(tmloc,"/.xye/");


    if (TryToOpenFolder(tmloc))
        printf("Found: %s\n",tmloc);
    else
    {
        printf("attempt to create %s : ",tmloc);
        if (MKDIR(tmloc,S_IRWXU)>=0)
            printf("ok\n");
        else
            printf("failed, errors might happen\n");
    }
    delete [] tmloc;

    tmloc=new char[strlen(home)+strlen("/.xye/levels/")+1];
    strcpy(tmloc,home);strcat(tmloc,"/.xye/levels/");

    mylevelsplace=tmloc;


    if (TryToOpenFolder(tmloc))
        printf("Found: %s\n",tmloc);
    else
    {
        printf("attempt to create %s : ",tmloc);
        if (MKDIR(tmloc,S_IRWXU)>=0)
            printf("ok\n");
        else
            printf("failed, errors might happen\n");
    }
    delete [] tmloc;

    tmloc=new char[strlen(home)+strlen("/.xye/res/")+1];
    strcpy(tmloc,home);strcat(tmloc,"/.xye/res/");


    if (TryToOpenFolder(tmloc))
        printf("Found: %s\n",tmloc);
    else
    {
        printf("attempt to create %s : ",tmloc);
        if (MKDIR(tmloc,S_IRWXU)>=0)
            printf("ok\n");
        else
            printf("failed, errors might happen\n");
    }
    delete [] tmloc;



}
else
{
    printf("no home folder found, this probably is a win32 build. If there is a home folder in your system then xye is having problems to detect it, make sure to have a correct home environment variable.");
    mylevelsplace=options::Dir+"levels/mylevels/";
    if (TryToOpenFolder(mylevelsplace.c_str()))
    {
        printf("Found: %s\n",mylevelsplace.c_str());
    }
    else
    {
        const char* mylev=mylevelsplace.c_str();
        printf("Attempt to create %s : ",mylev);
        if (MKDIR(mylev,S_IRWXU)>=0)
            printf("ok\n");
        else
            printf("failed, errors might happen\n");
    }
}

    options::ExecutablePath=argv[0];
   if(editmode!="")
   {
       printf("Initializing editor...\n");
       editor::Init(mylevelsplace,editmode);
   }
   else if(playmode!="")
   {
       string finallevel=mylevelsplace;
       finallevel+=playmode;
       printf("Initializing game...\n");
       game::Init(finallevel.c_str());
   }
   else
   {
       printf("Initializing game...\n");
       game::Init();
   }

return 0;
}
