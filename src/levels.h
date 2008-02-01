#include "vxsdl.h"

namespace LevelBrowser
{




const char* GetLevelFile();

void SetupNormalFonts(SDL_Surface* SS);
void SetupNormalFonts(const char* ttf, const int & size);
void SetupBoldFonts(SDL_Surface* SS);
void SetupBoldFonts(const char* ttf, const int & size);

void DeleteFonts();
}

