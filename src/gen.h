#include <stdio.h>
#include <string>


using std::string;

//Sets random seed
void Randomize();
//returns a random integer in range




template<class T>
T GetRandom(T min, T max);



template<class T>
bool doublematch(T a,T b,T c);


int GetRandomInt(int min, int max);
//returns a bool depending of chance (for example Chance(0.25) has a 25% chance
//to return true
bool Chance(float x);

unsigned int RandomRGB(unsigned char a,unsigned char b);
unsigned int RandomRGB();

unsigned int MixColors(unsigned int A, unsigned int B);

#ifndef vxsdlh
struct SDL_Color;
#endif

SDL_Color& MixColors(SDL_Color& A, SDL_Color& B);

bool TryS2I(std::string &str, int& r);
std::string GetFileNameNoExtension( const char* filename );
