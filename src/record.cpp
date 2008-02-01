#include "xye.h"
#include "record.h"
#include <iostream>
#include <fstream>


recentry* recording::first;
recentry* recording::last;
recentry* recording::undos;
unsigned int recording::undos_r=0;

void recording::init()
{
    first=last=undos=NULL;
    undos_r=0;
}



bool recording::undo()
{
    recentry* r=NULL,*z=NULL,*q=NULL,*p=first;

    int v;
    while (p!=NULL)
    {
        if (p->val)
        {
            z=r;
            q=p;
        }

        r=p;
        p=p->next;
    }

    if (!q) return false;

    //now z holds the node before last xye step, and q last xye step

    last=z;
    if (z==NULL)
        first=NULL;
    else
        z->next=NULL;

    while (q!=NULL)
    {
        z=q;
        q=q->next;
        delete z;
    }
    undos=first;


return true;
}



void recording::add(unsigned int val)
{

    if (last && (val==last->val) )
    {
        last->count++;
        return;
    }



    recentry* tm=new recentry;
    tm->next=NULL;
    tm->val=val;
    tm->count=1;

    if (last)
    {
        last->next=tm;
        last=tm;
    }
    else
        first=last=tm;

}

void recording::add(edir a)
{
    switch(a)
    {
        case(D_UP): recording::add(8); break;
        case(D_DOWN): recording::add(2); break;
        case(D_LEFT): recording::add(4); break;
        default: recording::add(6);
    }
}

void recording::add_nomove()
{
    recording::add(0);
}

void recording::clean()
{
    recentry* tm;
    while (tm=first)
    {
        first=tm->next;
        delete tm;
    }
    first=last=undos=NULL;
}


bool recording::get_undo(edir &r, bool& nomove)
{
    if (! undos) return false;
    recentry* tm=undos;
    nomove=false;
    switch (tm->val)
    {
        case(4): r=D_LEFT; break;
        case(6): r=D_RIGHT; break;
        case(8): r=D_UP; break;
        case(2): r=D_DOWN; break;
        default: nomove=true;
    }
    if (undos->count -1 == undos_r)
    {
        undos_r=0;;
        undos=undos->next;
    }
    else
        undos_r++;

 return true;
}

bool recording::get(edir &r, bool& nomove)
{
    if (! first) return false;
    recentry* tm=first;
    nomove=false;
    switch (tm->val)
    {
        case(4): r=D_LEFT; break;
        case(6): r=D_RIGHT; break;
        case(8): r=D_UP; break;
        case(2): r=D_DOWN; break;
        default: nomove=true;
    }
    first->count--;
    if (! first->count)
    {
        first=first->next;
        delete tm;
    }


 return true;
}

bool recording::load(const char* st)
{

    unsigned int L,i=0;
    if (  (!st) || (!(L=strlen(st)) ) ) return false;
    recording::clean();
    while (i<L)
    {
        switch(st[i])
        {
            case('4'): recording::add(4); break;
            case('8'): recording::add(8); break;
            case('2'): recording::add(2); break;
            case('6'): recording::add(6); break;
            case('0'): recording::add(0);
            default: //Allows other characters for example line breaks
            ;
        }
        i++;
    }



}
char* recording::save()
{
    unsigned int L=0;
    recentry* tm=NULL;
    while ( tm= (!tm)?first:tm->next)
        L+=tm->count;


    char * r=new char[L+1];
    tm=first;
    unsigned int i=0,j;
    while(i<L)
    {
        for (j=1;j<=tm->count;j++)
        {
            r[i]=(char)('0'+tm->val);
            i++;
        }
        tm=tm->next;
    }
    r[i]='\0';
    return(r);
}

bool recording::saveInFile(const char* target,const char* levelfn,unsigned int ln)
{
    std::ofstream file;
    file.open (target,std::ios::trunc | std::ios::out );
    if (!file.is_open()) return false; //ouch just halt.

    file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?><!--Xye replay file-->\n<xyereplay\n     levelfile='";
    file << levelfn;
    file << "'\n     leveln='";
    file << ln;
    file << "'\n>\n<moves>\n";
    char * rep=save();
    file << rep;
    file << "\n</moves>\n</xyereplay>";

    delete [] rep;



    file.close();

 return true;
}

