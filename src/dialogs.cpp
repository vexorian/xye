#include "dialogs.h"


Font* dialogs::FontResource;
SDL_Color dialogs::BackgroundColor;
SDL_Color dialogs::TextBoxColor;
stack<yesnoData> dialogs::yesnoCurrent;
stack<inputDialog> dialogs::inputCurrent;
void onDialogClickDoNothing(bool yesclicked) {};

class yesnoButtonData: public buttondata
{
 public:
    onDialogEnd func;
    window* target;
    
    yesnoButtonData(onDialogEnd afunc,window* atarget)
    {
        func=afunc;
        target=atarget;
    }
};

void dialogs::yesButtonClick(const buttondata* data)
{
    const yesnoButtonData* dt=static_cast<const yesnoButtonData*>(data);
    onDialogEnd func=dt->func;
    dt->target->endSub();
    dialogs::yesnoCurrent.pop();
    func(true);
}


void dialogs::noButtonClick(const buttondata* data)
{
    const yesnoButtonData* dt=static_cast<const yesnoButtonData*>(data);
    onDialogEnd func=dt->func;
    dt->target->endSub();
    dialogs::yesnoCurrent.pop();
    func(false);
}

void dialogs::yesnoOnKeyDown(SDLKey keysym, Uint16 unicode)
{
    yesnoData dt= dialogs::yesnoCurrent.top();
    
    switch(keysym)
    {
        case SDLK_RETURN :
            dialogs::yesnoCurrent.pop();
            dt.enterdown=true;
            dialogs::yesnoCurrent.push(dt);
            break;
        case (SDLK_ESCAPE):
            dialogs::yesnoCurrent.pop();
            dt.escdown=true;
            dialogs::yesnoCurrent.push(dt);
            break;
    }
}

void dialogs::yesnoOnKeyUp(SDLKey keysym, Uint16 unicode)
{
    yesnoData dt= dialogs::yesnoCurrent.top();
    
    switch(keysym)
    {
        case SDLK_RETURN :
            if(!dt.enterdown) return;
            dialogs::yesnoCurrent.pop();
            dt.target->endSub();
            dt.func(true);
            break;
        case (SDLK_ESCAPE):
            
            if(!dt.escdown) return;
            dialogs::yesnoCurrent.pop();
            dt.target->endSub();
            dt.func(false);
            
            break;
    }
}
//================================================================================
class inputtextblock: public control
{
 public:
    int lines;
    int tic;
    string oldtext;
    inputDialog* diag;
    inputtextblock(int sx, int sy, int sw, int slines);
    
    vector<string> textsplit;
    
    void draw(SDL_Surface* target);
    void loop() {}
    inline void onMouseMove(int px,int py){}
    inline void onMouseOut() {}
    inline void onMouseDown(int px,int py) {}
    inline void onMouseUp(int px,int py) {}
    inline void onMouseRightUp(int px,int py) {}
};

inputtextblock::inputtextblock(int sx, int sy, int sw, int slines)
{
    lines=slines;
    oldtext="";
    textsplit=vector<string>(1,"");
    x=sx;y=sy;w=sw;h=lines*dialogs::FontResource->Height();
    depth=0;
    tic=0;
}

void inputtextblock::draw(SDL_Surface* target)
{
    tic++;
    string text=dialogs::getCurrentInputText();
    
    if(text!=oldtext)
    {
        textsplit=dialogs::FontResource->splitByLines(text,w);
        if(textsplit.size()==0) textsplit.push_back("");
        oldtext=text;
    }

    
    
   
    int begi=0,endi=lines;
    if(lines<textsplit.size())
    {
        begi=textsplit.size()-lines;
        endi=textsplit.size();
    }
    
    if (lines>textsplit.size())
    {
        endi=textsplit.size();
    }
        
    int fh=dialogs::FontResource->Height();
    int th=0;
    for (int i=begi;i<endi;i++)
    {

        if((i==endi-1) && (tic%8<4) )
            dialogs::FontResource->Write(target, x, y+th, textsplit[i]+"|");
        else
            dialogs::FontResource->Write(target, x, y+th, textsplit[i]);
        th+=fh;
    }
}



//================================================================================

string dialogs::getCurrentInputText()
{
    if(inputCurrent.empty()) return "NULL";
    else return inputCurrent.top().text;
}
void dialogs::makeButtonDialog(window* target, string prompt, string yescaption, bool showno, string nocaption, onDialogEnd func)
{
    
    
    Sint16 pw = target->Width/2;
    Sint16 ph = max(target->Height/3, min<int>(target->Height, FontResource->WrappedTextHeight(prompt, pw-2*button::Size) + button::Size*4) );
    Sint16 px = target->Width/4;
    Sint16 py = (target->Height - ph)/2;
    
    
    
    yesnoData dt;
    dt.target=target;
    dt.func=func;
    dt.enterdown=false;
    dt.escdown=false;
    dialogs::yesnoCurrent.push(dt);
    target->beginSub();

    control* tmcntrl;
    
    tmcntrl = new rectangle(px-1,py-1,pw+2,ph+2, 0,0,0 );
    tmcntrl->depth=1;
    target->addControl(tmcntrl);

    tmcntrl = new rectangle(px+3,py+3,pw,ph, 0,0,0 );
    tmcntrl->depth=1;
    target->addControl(tmcntrl);

    
    tmcntrl=new rectangle(px,py,pw,ph, BackgroundColor );
    tmcntrl->depth=2;
    target->addControl(tmcntrl);
      
    target->onKeyUp=yesnoOnKeyUp;
    target->onKeyDown=yesnoOnKeyDown;

  
    textblock* tx = new textblock(px+button::Size, py+button::Size, pw-2*button::Size, ph - 4*button::Size, dialogs::FontResource);
    tx->text= prompt;
    tx->depth=19;
    target->addControl(tx);
    
    int ybuttonw = FontResource->TextWidth(yescaption)+button::Size;
    if (ybuttonw<button::Size*4) ybuttonw=button::Size*4;
    
    int buttonw = FontResource->TextWidth(nocaption)+button::Size;
    if (buttonw<ybuttonw) buttonw=ybuttonw;   
    int buttonx;
    
    if(showno) buttonx=px+pw/2-buttonw-button::Size/2;
    else buttonx=px+(pw-buttonw)/2;
    

    int buttony=py+ph-2*button::Size+button::Size/2;
    button* tmbut= new button(buttonx,buttony,buttonw,button::Size);
    tmbut->text=yescaption;
    tmbut->depth=20;
    tmbut->onClick=yesButtonClick;
    tmbut->data=new yesnoButtonData(func,target);
    
    target->addControl(tmbut);


    
    if(showno)
    {
        buttonx=px+pw/2+button::Size/2;        
        tmbut= new button(buttonx,buttony,buttonw,button::Size);
        tmbut->text=nocaption;
        tmbut->depth=20;
        tmbut->onClick=noButtonClick;
        tmbut->data=new yesnoButtonData(func,target);
        target->addControl(tmbut);
    }   
}

void dialogs::makeYesNoDialog(window* target, string prompt, string yescaption, string nocaption, onDialogEnd func)
{
    makeButtonDialog(target,prompt, yescaption, true, nocaption, func);
}
void dialogs::makeMessageDialog(window* target, string prompt, string buttoncaption, onDialogEnd func)
{
    makeButtonDialog(target,prompt, buttoncaption, false, "", func);
}


//==================================
void dialogs::textDialogClickClear(const buttondata* data)
{
    inputDialog& d= inputCurrent.top();
    d.text = "";
}

void dialogs::textDialogClick(bool ok)
{
    inputDialog d= inputCurrent.top();
    inputCurrent.pop();
    
    d.target->endSub();
    d.func(ok,d.text,d.data);
}

void dialogs::inputOnKeyDown(SDLKey keysym,Uint16 unicode)
{
    inputDialog d= inputCurrent.top();
    if( unicode < 0x80)
    {
        char c = (char)(unicode);
        if(c>=' ')
        {
            d.text+=c;
            inputCurrent.pop();
            inputCurrent.push(d);
        }
    }
    
    switch(keysym)
    {
        case SDLK_RETURN: textDialogClick(true); break;
        case SDLK_ESCAPE: textDialogClick(false); break;
        
        case SDLK_BACKSPACE:
            d.text=d.text.substr(0,d.text.length()-1);
            inputCurrent.pop();
            inputCurrent.push(d);           
            break;
    }
}

void dialogs::inputOnKeyUp(SDLKey keysym,Uint16 unicode)
{
    
}


void dialogs::makeTextInputDialog(window* target,  string prompt, string defaulttext, int lines, string okcaption, string cancelcaption, onTextDialogEnd func, inputDialogData* data)
{
    
    SDL_EnableUNICODE(1);
    Sint16 pw = target->Width/2;
    Sint16 ph = max(target->Height/3, min<int>(target->Height, FontResource->WrappedTextHeight(prompt, pw-2*button::Size) + button::Size*5) );
    Sint16 px = target->Width/4;
    Sint16 py = (target->Height - ph)/2;
    
    
    Sint16 ith = lines*FontResource->Height(); 
    Sint16 textblockheight=ph - 4*button::Size - 2 - ith;
    Sint16 itw = pw-2*button::Size;  
    Sint16 itx = px+button::Size, ity=py+button::Size+textblockheight+1;
    

    inputDialog cur;
    cur.data=data;
    cur.text=defaulttext;
    cur.lines=lines;
    cur.target=target;
    cur.func=func;
    
    inputCurrent.push(cur);
    
    
    target->beginSub();
    target->onKeyUp=inputOnKeyUp;
    target->onKeyDown=inputOnKeyDown;

    control * tmcntrl;
    tmcntrl = new rectangle(px-1,py-1,pw+2,ph+2, 0,0,0 );
    tmcntrl->depth=1;
    target->addControl(tmcntrl);

    tmcntrl = new rectangle(px+3,py+3,pw,ph, 0,0,0 );
    tmcntrl->depth=1;
    target->addControl(tmcntrl);
    
    
    
    tmcntrl=new rectangle(px,py,pw,ph, BackgroundColor );
    tmcntrl->depth=2;
    target->addControl(tmcntrl);
      
    tmcntrl=new rectangle(itx-1,ity-1,itw+2,ith+2, 0,0,0);
    tmcntrl->depth=3;
    target->addControl(tmcntrl);

    tmcntrl=new rectangle(itx,ity,itw,ith, dialogs::TextBoxColor);
    tmcntrl->depth=4;
    target->addControl(tmcntrl);


    
    textblock* tx = new textblock(px+button::Size, py+button::Size, pw-2*button::Size, textblockheight, dialogs::FontResource);
    tx->text= prompt;
    tx->depth=19;
    target->addControl(tx);
    
    inputtextblock* input = new inputtextblock(itx+2, ity, itw-4,lines);
    input->depth=19;
    target->addControl(input);
    
    
    int ybuttonw = FontResource->TextWidth(okcaption)+button::Size;
    if (ybuttonw<button::Size*4) ybuttonw=button::Size*4;
    
    int buttonw = FontResource->TextWidth(cancelcaption)+button::Size;
    if (buttonw<ybuttonw) buttonw=ybuttonw;   


    int buttonx=px + button::Size  /*+pw/2-buttonw-button::Size/2*/;

    int buttony=py+ph-2*button::Size+button::Size/2;
    button* tmbut= new button(buttonx,buttony,buttonw,button::Size);
    tmbut->text=okcaption;
    tmbut->depth=20;
    tmbut->onClick=textDialogClickOk;
    tmbut->data=NULL;
    target->addControl(tmbut);

    buttonx=/*px+pw/2+button::Size/2*/ px + pw/2 - buttonw/2;        
    tmbut= new button(buttonx,buttony,buttonw,button::Size);
    tmbut->text="Clear";
    tmbut->depth=20;
    tmbut->onClick=textDialogClickClear;
    tmbut->data=NULL;
    target->addControl(tmbut);
    
    
    
    buttonx=/*px+pw/2+button::Size/2*/ px + pw - buttonw - button::Size;        
    tmbut= new button(buttonx,buttony,buttonw,button::Size);
    tmbut->text=cancelcaption;
    tmbut->depth=20;
    tmbut->onClick=textDialogClickCancel;
    tmbut->data=NULL;
    target->addControl(tmbut);
    
    
    

}
