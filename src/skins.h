#include "vxsdl.h"
#include "window.h"


namespace SkinBrowser
{

void StartSection(window* wind);

void SetupNormalFonts(SDL_Surface* SS);
void SetupNormalFonts(const char* ttf, const int & size);
void SetupBoldFonts(SDL_Surface* SS);
void SetupBoldFonts(const char* ttf, const int & size);

void Clean();
}

