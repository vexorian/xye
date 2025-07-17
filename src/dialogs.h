#ifndef DIALOGSINCLUDED

#ifndef WINDOWINCLUDED
    #include "window.h"
#endif
#ifndef FONTINCLUDED
   #include "font.h"
#endif
#define DIALOGSINCLUDED

#include<string>
#include<stack>
using std::string;
using std::stack;




typedef void (*onDialogEnd)(bool yesclicked);
void onDialogClickDoNothing(bool yesclicked);

struct yesnoData
{
    window* target;
    onDialogEnd func;
    bool enterdown;
    bool escdown;
};

class inputDialogData {};

typedef void (*onTextDialogEnd)(bool okclicked, const string resulttext, inputDialogData* data);


struct inputDialog
{
    inputDialogData* data;
    onTextDialogEnd func;
    string text;
    vector<string> textsplit;
    int lines;
    window* target;

    inputDialog() {data=NULL; }

};

class dialogs
{
 private:
    static stack<yesnoData> yesnoCurrent;
    static stack<inputDialog> inputCurrent;
    static void yesButtonClick(const buttondata* data);
    static void noButtonClick(const buttondata* data);
    static void yesnoOnKeyUp(SDLKey keysym,Uint16 unicode);
    static void yesnoOnKeyDown(SDLKey keysym,Uint16 unicode);


    static void makeButtonDialog(window* target, string prompt, string yescaption, bool showno, string nocaption, onDialogEnd func);


    static void inputOnKeyDown(SDLKey keysym,Uint16 unicode);
    static void inputOnKeyUp(SDLKey keysym,Uint16 unicode);


 public:
    static Font* FontResource;
    static SDL_Color BackgroundColor;
    static SDL_Color TextBoxColor;



    static void makeYesNoDialog(window* target, string prompt, string yescaption, string nocaption, onDialogEnd func);
    static void makeMessageDialog(window* target, string prompt, string buttoncaption, onDialogEnd func);

    static void makeTextInputDialog(window* target,  string prompt, string defaulttext, int lines, string okcaption, string cancelcaption, onTextDialogEnd func, inputDialogData* data);

    static void textDialogClick(bool ok);
    static void textDialogClickOk(const buttondata* data) { textDialogClick(true); }
    static void textDialogClickClear(const buttondata* data);
    static void textDialogClickCancel(const buttondata* data) { textDialogClick(false); }

    static string getCurrentInputText();

};


#endif
