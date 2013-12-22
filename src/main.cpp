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
    * Standarize the coding style so that egyptian braces are used in all
      portions of code
    * Get rid of messy C string operations, replace with std::string as much as
     possible.


*/

#include "xye.h"
#include "xyedit.h"
#include "xye_script.h"
#include "options.h"

#include "browser.h"


#include <sys/types.h>
#include <sys/stat.h>


#include <iostream>

using namespace std;


int main ( int argc, char** argv )
{
    string editmode("");
    string playmode("");
    options::Dir="";

    if (argc>1) {
        options::Dir = argv[1];
    }
    if(options::Dir=="") {
        #ifdef DATA_PATH
            options::Dir=DATA_PATH;
        #else
            #ifdef PORTABLE
                options::Dir=argv[0];
                int i=options::Dir.length()-1;
                while ( (i>=0) && (options::Dir[i]!= '\\') && (options::Dir[i]!= '/') ) {
                    i--;
                }
                options::Dir.resize(i+1);
                while (i>=0) {
                    if (options::Dir[i]=='\\') {
                        options::Dir[i] = '/';
                    }
                    i--;
                }
            #else
                options::Dir="./";
            #endif
    
        #endif
    }
    int i=options::Dir.length()-1;
    while ( (i>=0) && (options::Dir[i]!= '\\') && (options::Dir[i]!= '/') ) {
        i--;
    }
    options::Dir.resize(i+1);
    while (i>=0) {
        if (options::Dir[i]=='\\') {
            options::Dir[i] = '/';
        }
        i--;
    }

    cout << "Will look on "<<options::Dir<<" for data files.\n"<<endl;

    if (! TryToOpenFolder(options::Dir.c_str())) {
        printf("Exception: Can't open/find data folder\n");
        return 1;
    }
    printf("Initializing game...\n");
    game::Init();
    return 0;
}
