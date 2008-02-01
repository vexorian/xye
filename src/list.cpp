/*
this is a placeholder
*/

/*#include "list.h"
#include <vector>


namespace gui
{
using std::string;
using std::vector;
class List: public ListBox;
{
 private:
     Uint16 h;
     Uint16 w;
    char* items;
     unsigned int N;
     unsigned int active;
 public:
 List(char* items, unsigned int itemN);
 void Draw(SDL_Surface* sr, Sint16 x, Sint16 y);
 void Click(Uint16 rx, Uint16 ry);
 void SetSelectEventFunc(void (Func*)(ListBox* l), int si, char* selected     );
 char* GetSelected();


};


List::List(char* items, unsigned int itemN)
{
	List::items=items;
	N=itemN;
	active=0;
}



//The interface class' methods:
 ListBox* ListBox::New(char* items, unsigned int itemN)
 {
 	return (new List(items,itemN));
 }
 ListBox& ListBox::New()
 {
 	return (* new List(items,itemN));
 }






} //namespace gui*/