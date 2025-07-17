#include "vxsdl.h"
#include "window.h"


namespace LevelBrowser
{


void AssignLevelFile( const char * path, int n);

void StartSection(window* wind);

const char* GetLevelFile();

void SetupNormalFonts(SDL_Surface* SS);
void SetupNormalFonts(const char* ttf, const int & size);
void SetupBoldFonts(SDL_Surface* SS);
void SetupBoldFonts(const char* ttf, const int & size);

void Clean();
}
