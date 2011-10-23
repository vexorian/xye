#include "vxsdl.h"
#ifndef FONTINCLUDED
   #include "font.h"
#endif
#include "levels.h"
#include "window.h"
#include <string>


#include "gen.h"


#ifndef XYE_HORZ
    #define XYE_HORZ 30
    #define XYE_VERT 20
#endif

#define XYE_GAMEX 3
#define XYE_GAMEY 3
#define XYE_XTRA_X 3
#define XYE_XTRA_Y 3

#define XYE_FPS 20
//#define XYE_FPS 2
#define XYE_ZOOMFC 1.25

//They Used to be different enums
#define GEM_DIAMOND B_BLUE
#define GEM_RUBY B_RED
#define GEM_TOPAZ B_YELLOW
#define GEM_EMERALD B_GREEN
const int XYE_FASTFORWARD_SPEED=3;


using std::string;


enum edir { D_UP=0,D_DOWN=1,D_LEFT=2,D_RIGHT=3};
enum blockcolor
{
    B_YELLOW=0,
    B_RED=1,
    B_BLUE=2,
    B_GREEN=3
};
enum roundcorner { RC_1,RC_7,RC_9,RC_3 };

typedef blockcolor gemtype;

enum groundskin { GROUND_1, GROUND_2 };

enum killtype { KT_KILLXYE /*Kill Xye*/, KT_KILLORGANICS /*Kill Xye and monsters*/, KT_FIRE /*Kill any thing that is not fire resistant, also shows a fire effect*/};




unsigned int BC2Color(blockcolor bc);

enum otype
{
    OT_XYE,
    OT_ROBOXYE,
    OT_WALL,
    OT_BLOCK,
    OT_TRICKDOOR,
    OT_HINT,
    OT_WARNING,
    OT_EARTH,
    OT_GEM,
    OT_TELEPORT,
    OT_BLACKHOLE,
    OT_MINE,
    OT_FIREBALL,
    OT_SURPRISE,
    OT_PUSHER,
    OT_ARROW,
    OT_TURNER,
    OT_LOWDENSITY,
    OT_AUTO,
    OT_FACTORY,
    OT_SNIPER,
    OT_FILLER,
    OT_NUMBER,
    OT_MAGNETIC,
    OT_BLOCKDOOR,
    OT_MARKEDAREA,
    OT_GEMBLOCK,
    OT_KEY,
    OT_LOCK,
    OT_PORTAL,
    OT_TOGGLE,
    OT_EXIT,
    OT_BEAST,
    OT_RATTLER,
    OT_RATTLERNODE,
    OT_RATTLERFOOD,
    OT_FIREPAD,
    OT_PIT,
    OT_WILDCARD,
    OT_WINDOW,
    OT_SCROLLBLOCK,
    OT_METALBLOCK,
    OT_LARGEBLOCK,
    OT_STAR
};

enum btype
{
    BT_GNASHER=0,
    BT_BLOB=1,
    BT_VIRUS=2,
    BT_SPIKE=3,
    BT_TWISTER=4,
    BT_DARD=5,
    BT_WARD=6,
    BT_SPINNER=7,
    BT_ASPINNER=8,
    BT_PATIENCE=9,
    BT_BLOBBOSS=10,
    BT_STATIC=11,
    BT_RANGER=12,
    BT_TIGER=13
};

#ifndef BEASTN
    #define BEASTN 14
#endif

#ifndef BTYPEN
    #define BTYPEN 14
#endif


struct square;

/**ent**/
//An ent is either an object or ground object, this was required to help the script interface.
//A better definition: an ent is something that has a position in the game and an unique id.
class ent
{
   protected:
     int id;
     char x;
     char y;
     otype type;

   public:
     virtual void move(char px,char py)=0;
     virtual void Draw(unsigned int x, unsigned int y)=0;
     char X();
     char Y();
     otype GetType();
     virtual void Kill()=0;
     virtual void OnDeath()=0;
     unsigned int GetId();
     void UpdateSquare();
};


/** Object Interface**/
class obj: public ent
{
    protected:
     void ObjectConstruct(square* sq);
     bool trypush_common(edir dir,obj* pusher,bool AsRoundObject, bool* died);
     bool KilledByBlackHole;
     bool Magnetism(char ox, char oy, char mx, char my, bool rSticky, bool rHorz, edir godir);

    public:
     bool GoFindXye(edir res[], int &resn, bool &foundpath, bool ignoreloopingedge, bool ignoreSp, bool considerTeleports, bool considerStickies, unsigned int range, bool Randomize=true );
     bool GoFindASquare(bool (*cond)(square* sq), edir res[], int &resn, bool &foundpath, bool ignoreloopingedge, bool ignoreSp, bool considerTeleports, bool considerStickies, unsigned int range, bool Randomize=true );
     unsigned int tic;
     void move(char px,char py);

     virtual bool trypush(edir dir,obj* pusher)=0;
     virtual bool HasRoundCorner(roundcorner rnc)=0;
     void Kill();
     void Kill(bool byBlackHole);
     virtual bool Loop(bool* died)=0;


     bool DoMagnetism(bool horz, bool vert, bool *Moved);
     virtual bool HasBlockColor(blockcolor bc)=0;
     bool AffectedByMagnetism(bool horz);
};

bool ObjectResistsFire(obj * o);

/** Ground Object Interface**/
//Ground objects are just like normal objects but they render differently, and have
//leave/enter events.
// a ground square may only have one normal object and one ground object
class gobj: public ent
{
    protected:
     unsigned int id;
     char x;
     char y;
     void GObjectConstruct(square* sq);


    public:
     char X();
     char Y();

     void move(char px,char py);
     virtual void Draw(unsigned int x, unsigned int y)=0;
     virtual void OnEnter(obj *entering)=0;
     virtual void OnLeave(obj *entering)=0;
     virtual void Loop()=0;


     virtual bool CanEnter(obj *entering, edir dir)=0;
     virtual bool CanLeave(obj *entering, edir dir)=0;
     void Kill();
     unsigned int GetId();
     bool RenderAfterObjects;
};


/** Explosion !! **/
class explosion
{
 private:

    square *pos;
    unsigned char type;
    unsigned int creation;

    int xobjectoffset;
    int yobjectoffset;

 public:
    explosion(square *sq);
    explosion(square *sq, unsigned char kind);
    ~explosion();
    void ex(square *sq, unsigned char kind);
    void Draw(unsigned int x, unsigned int y);
    void getDrawingXYOffset(int &xo, int &yo);
    void setDrawingXYOffset(int xo, int yo);
};



/**Square Struct**/
struct square
{
    obj* object;
    gobj* gobject;
    explosion *ex;
    int x;
    int y;
    char sqx;
    char sqy;
    groundskin gs;
    Uint8 R;
    Uint8 G;
    Uint8 B;
    bool Update;
    bool UpdateLater;
};



/** Xye **/
class xye : public obj
{
 private:
     unsigned char lives;
     square* checkpoint;
     void OnDeath() { throw "Xye Should not die this way"; }
     
 public:
     Uint8 alpha;
     xye(square* sq);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     unsigned char GetLives();
     void SetLives(unsigned char nlives);
     void SetCheckPoint(square* sq);
     void GetCheckPoint(char &cx, char &cy);
     bool Loop(bool* died) { *died=false; return true; }
     unsigned int deadtic;
     bool HasBlockColor(blockcolor bc);
     void Kill();

     bool moved;
     edir lastdir;
     static bool useDirectionSprites;

};


/** Robo-xye **/
class roboxye : public obj
{
 private:
     int anim;
     square* checkpoint;
     void OnDeath() {}
 public:
     roboxye(square* sq);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
};




/** Wall **/
class wall : public obj
{
 private:
     
     unsigned int color;

     bool round7;
     bool round1;
     bool round3;
     bool round9;
     unsigned char kind;
     static unsigned char defkind;
     void OnDeath() {}
     Uint8 R;
     Uint8 G;
     Uint8 B;
     
     bool containsRoundCorner();
     
 public:
     static SDL_Color DefaultColor[6];
     static const int MAX_VARIATIONS = 6;
     wall(square* sq,unsigned char t);
     wall(square* sq);
     bool ResistsFire();
     void Draw(unsigned int x, unsigned int y);
     void ChangeKind(unsigned char t);
     void ChangeColor(Uint8 nR, Uint8 nG, Uint8 nB, bool multiply=true);
     void SetRoundCorners(bool r7, bool r1, bool r3, bool r9);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
     static void SetDefaultColor(SDL_Color cc, bool multiply=true);
     static void SetDefaultColor(SDL_Color cc, bool multiply, int var);
     static void SetDefaultType(signed int def);
     static void ResetDefaults();
     static wall* find(char sx, char sy, unsigned char kind = 6);
     void IntelligentUpdateCorners(wall *by);
     static int GetDefaultVariationVulnerableToFire();
};


/** Window Block **/
class windowblock : public obj
{
 private:
     blockcolor bc;
     static unsigned int count[4];
     static unsigned int activeN[4];
     void OnDeath() { count[bc]--;  if (active) activeN[bc]--; }
     unsigned int anim;
     bool active;
     bool Detect(char x, char y);

 public:

     windowblock(square* sq,blockcolor b);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc) {return(false);}
     bool HasBlockColor(blockcolor bc) ;
     bool Loop(bool* died);

     static void ResetCounts();
     static bool CanMove(windowblock* w);
};



/** Block **/
class block : public obj
{
 private:
     blockcolor c;
     bool round;
     void OnDeath() {}


 public:
     bool colorless;
     block(square* sq,blockcolor tc,bool makeround);
     void Draw(unsigned int x, unsigned int y);
     static void Draw(unsigned int x, unsigned int y,bool colorless, blockcolor c, bool round);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
};

struct largeblockroot
{
    int children;
    largeblockroot() { children=0; }
};

/** "Large" Block **/
class largeblock : public obj
{
 private:

     blockcolor c;
     void OnDeath();
     Uint8 flags;
     largeblockroot *root;
     void setupBlock();
     
     
     void blockDFS( largeblockroot* root);
     static largeblock* getPart( obj* object, largeblockroot* root);
     bool pushingBlocks(edir dir, int ix, int x0, int x1, int iy, int y0, int y1, int dx ,int dy);
     void doPush(edir dir, int dx, int dy);
     bool canPush(edir dir, int dx, int dy);
     void getPushGroup();
     bool pushingBlocks2(edir dir, int dx, int dy);
 public:
     bool colorless;
     largeblock(square* sq,blockcolor tc, bool up, bool right, bool down, bool left );
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
     bool isReallyASmallBlock();
};


/** Block (metal) **/
class metalblock : public obj
{
 private:
     bool round;
     void OnDeath() {}

 public:
     metalblock(square* sq,bool makeround);
     void Draw(unsigned int x, unsigned int y);
     static void Draw(unsigned int x, unsigned int y, bool round);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc) {return false;}
};


/** ScrollBlock **/
class scrollblock : public obj
{
 private:
     blockcolor c;
     bool round;
     edir fac;
     void OnDeath() {}


 public:
     bool colorless;
     scrollblock(square* sq,blockcolor tc,bool makeround,edir d);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
};



/** Wild card **/
class wildcard : public obj
{
 private:
     void OnDeath() {}
     bool round;


 public:
     wildcard(square* sq, bool makeround);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
};




/** gemblock **/
class gemblock : public obj
{
 private:
     blockcolor c;
     void OnDeath() {}


 public:
     gemblock(square* sq,blockcolor tc);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
};






/** magnetic **/
enum mgtype { T_MAGNET=0, T_ANTIMAGNET=1, T_STICKY =2};

class magnetic : public obj
{
 private:
     mgtype mt;
     bool horz;
     bool TryMagneticMove(char ox, char oy, char xx, char xy, edir godir, bool *died, char sx=0, char sy=0);
     void OnDeath() {}
     edir LastPushDir;
     unsigned int MovedTic;

 public:
     magnetic(square* sq,mgtype kind, bool horizontal);

     bool IsSticky(edir reqdir);
     bool IsSticky();
     bool IsHorizontal();
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     static bool IsInstance(obj* object);
     bool HasBlockColor(blockcolor bc);
};


/** Number **/
class number : public obj
{
 private:
     blockcolor c;
     unsigned int GreenActive;
     bool GreenFlash;
     bool round;
     unsigned char time;
     unsigned int tic;
     bool green_activator_present(square * sq);
     bool ignoreDeath;
     void OnDeath();


 public:
     number(square* sq,blockcolor tc,unsigned char t, bool makeround);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died) ;
     bool HasBlockColor(blockcolor bc);
     void explode();
};



/** Impacter **/
class impacter : public obj
{
 private:
     blockcolor c;
     edir fac;
     void OnDeath() {}
     unsigned int time;
 public:
     impacter(square* sq,blockcolor tc, edir facing);
     void Draw(unsigned int x, unsigned int y);
     static void Draw(unsigned int x, unsigned int y,blockcolor c,edir fac);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     void turn();
     bool HasBlockColor(blockcolor bc);
};

/** Arrow **/
class arrow : public obj
{
 private:
     blockcolor c;
     edir fac;
     bool round;
     void OnDeath() {}
     unsigned int time;

 public:
     arrow(square* sq,blockcolor tc, edir facing, bool roundblock);
     void Draw(unsigned int x, unsigned int y);
     static void DrawF(unsigned int x, unsigned int y,blockcolor c, edir fac,bool round);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
};

/** Autoarrow **/
class autoarrow : public obj
{
 private:
     blockcolor c;
     edir fac;
     bool round;
     unsigned char count;
     void OnDeath() {}


 public:
     autoarrow(square* sq,blockcolor tc, edir facing, bool roundblock);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
     static edir GetDefaultEdirByColumn(unsigned int cx);
};

/** Factory **/
class factory : public obj
{
 private:
     blockcolor c;
     edir fac;
     edir swf;
     otype res;
     btype beasttype;
     bool round;
     bool nocolor;
     int pressed;
     unsigned int created;
     bool upd;
     void OnDeath() {}



 public:
     factory(square* sq,otype r,blockcolor tc, edir facing, edir switchfacing, bool roundblock,bool nocolor,btype bs);
     unsigned int limit;
     void Draw(unsigned int x, unsigned int y);
     void DrawSub(unsigned int x,unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc) { return false; }
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc) { return false; }
};

/** Filler **/
class filler : public obj
{
 private:
     blockcolor c;
     edir fac;
     bool round;
     bool anim;
     void OnDeath() {}
     unsigned int count;

 public:
     filler(square* sq,blockcolor tc, edir facing, bool roundblock);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);

};

/** Sniper **/
class sniper : public obj
{
 private:
     blockcolor c;
     edir fac;
     bool round;
     bool anim;
     void OnDeath() {}
     unsigned int count;

 public:
     sniper(square* sq,blockcolor tc,  bool roundblock);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);

};


/** Turner **/
class turner : public obj
{
 private:
     blockcolor c;
     bool round;
     void OnDeath() {}


 public:
     bool cwise;
     bool colorless;
     turner(square* sq,blockcolor tc,bool clockwise,bool roundblock);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
};

/** Low Density **/
class lowdensity : public obj
{
 private:
     blockcolor c;
     edir fac;
     bool active;
     bool round;
     void OnDeath() {}
     unsigned int time;

 public:
     lowdensity(square* sq,blockcolor tc, bool roundblock);
     void Draw(unsigned int x, unsigned int y);
     static void DrawF(unsigned int x, unsigned int y,blockcolor c, bool round);
     void Activate(edir D);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
};

/** Surprise **/
class surprise : public obj
{
 private:
     blockcolor c;
     bool round;
     bool changing;
     edir YellowDir;
     void OnDeath() {}
     static unsigned int Pending;


 public:

     surprise(square* sq,blockcolor tc,bool makeround);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
     void FinalExplode();
     void Transform();
     static void Reset();
     static void TransformAll();
};

/** Toggle **/
class toggle : public obj
{
 private:
     blockcolor c;
     bool round;
     bool kind;
     static bool State[4];
     static Uint32 ChangeTic;
     Uint32 Updated;
     void OnDeath() {}

 public:
     toggle(square* sq,blockcolor tc,bool makeround, bool state);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
     static void Reset();
};




/** Dangerous .-
   A dangerous is a foe but doesn't have a killing aura, Xye needs to step in to die.
   Robo mines are also instances of dangerous, but they can chase you.

   Mines and robo mines can be used to kill any object in the game with the help of sticky/anti sticky
   blocks
**/
class dangerous : public obj
{
 private:
     char absorb;
     char anim;
     Uint8 R;
     Uint8 G;
     Uint8 B;
     edir D;
     bool mine;
     bool fire;
     unsigned char mov;
     void OnDeath() {}
     bool disb;
     

 public:
     
     dangerous(square* sq, otype kind);
     dangerous(square* sq,edir dir,bool d);
     void FireBall(edir dir);
     void LandMine();
     void BlackHole();
     static void DrawMine(unsigned int x, unsigned int y);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     void Eat();
     bool Busy(obj* entering);
     void ChangeColor(Uint8 nR, Uint8 nG, Uint8 nB);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
     bool fromRound;
};

/** All beasts are instances of the same class, it is good some times but a mess some other times **/
class beast: public obj
{
 private:

     static unsigned int count[BTYPEN];
     void OnDeath() { count[kind]-- ; }
     bool (*AI)(beast* dabeast, edir &r);
     bool (*OnFail)(beast* dabeast, edir &r);
     unsigned char anim;
    bool Loop_Sub(bool* died);
     void PickAI();
    void PickFirstAnim()     ;
    btype kind;

 public:

    inline bool BelongsToBeastType(btype x) { return (kind==x); }

    int AIValue;
     void NewAnim();
    edir fac;
    beast(square* sq, btype personality, edir inidir);
    void Draw(unsigned int x, unsigned int y);
    static void Draw(unsigned int x, unsigned int y,btype kind, edir fac, unsigned char anim, int AIValue);
    bool trypush(edir dir,obj* pusher);
    bool HasBlockColor(blockcolor bc);
    bool Loop(bool* died);

    bool Floats();

    bool HasRoundCorner(roundcorner rnc);
    static unsigned int BeastCount(btype k);
    static void ResetCounts();

};

class rnode;

/** rattler **/
class rattler: public obj

{
 friend class rnode;
 private:
     void OnDeath();
     bool anim;
     void NewAnim();
     edir fac;
     rnode* first;
     rnode* last;
     Uint8 tic;
     unsigned int grow;

 public:

    rattler(square* sq, edir inidir, unsigned int G);
    void Node(square* sq);
    void Draw(unsigned int x, unsigned int y);
    bool Floats();
    static void Draw(unsigned int x, unsigned int y,bool anim,edir fac,rnode* first);
    bool trypush(edir dir,obj* pusher) { return false;}
    bool HasBlockColor(blockcolor bc) { return false;}
    bool Loop(bool* died);
    void Grow(unsigned int G);
    bool HasRoundCorner(roundcorner rnc) { return false;}

};


/** rnode **/
class rnode: public obj

{
 friend class rattler;
 private:
     rnode(square* sq, rattler* rhead);
     void OnDeath();
     bool anim;
     rnode* next;
     rnode* prev;
     rattler* head;
     bool prevhead;
     bool forgetkill;
    void NewAnim();
 public:


    void Draw(unsigned int x, unsigned int y);
    bool trypush(edir dir,obj* pusher) { return false;}
    bool HasBlockColor(blockcolor bc) { return false;}
    bool Loop(bool* died) { return false;}

    bool HasRoundCorner(roundcorner rnc) { return false;}

};

/** Rattler Food **/
class rfood : public obj
{
 private:
     void OnDeath() {}
     bool anim;


 public:
     rfood(square* sq);
     void Draw(unsigned int x, unsigned int y);
     static void Draw(unsigned int x,unsigned int y,bool anim);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc) {return true;}
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc) {return false;}
};


/** Teleport **/
class teleport : public obj
{
 private:
     bool anim;
     void OnDeath() {}

 public:
     teleport(square* sq,edir facing);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);

     bool HasRoundCorner(roundcorner rnc);
     bool tryteleport(edir odir, obj* moving, char& nx, char& ny, dangerous* &reason, bool* pushed);
     teleport* FindExit(char stx,char sty);
     edir dir;
     bool Loop(bool* died);
     bool FindCleanExit(obj* ForObject, char &nx, char &ny, bool AI=false);
     bool HasBlockColor(blockcolor bc);
};





/** Earth **/
class earth : public obj
{
 private:
     Uint8 R;
     Uint8 G;
     Uint8 B;
     static Uint8 dR,dG,dB;
     bool round;
     void OnDeath() {}

 public:
     earth(square* sq);
     void SetRound(bool flag);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     void ChangeColor(Uint8 nR, Uint8 nG, Uint8 nB);
     static void ResetDefaults();
     static void SetDefaultColor(Uint8 nR,Uint8 nG,Uint8 nB);
     bool Loop(bool* died) { *died=false; return true; }
     bool HasBlockColor(blockcolor bc);
};

/** key **/
class key : public obj
{
 private:

     void OnDeath() {}
     static unsigned int Got[4];

 public:
     key(square* sq,blockcolor color);
     blockcolor c;
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     void ChangeColor(unsigned int c);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);
     static void UseKey(blockcolor ofcolor);
     static void ResetCounts();
     static bool GotKey(blockcolor ofcolor);
     static bool GetXyesKeys(unsigned int &yl,unsigned int &rd,unsigned int &bl,unsigned int &gr);

};

/** lock **/
class lock : public obj
{
 private:

     void OnDeath() {}

 public:
     lock(square* sq,blockcolor color);
     blockcolor c;
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     void ChangeColor(unsigned int c);
     bool Loop(bool* died) { *died=false; return true; }
     bool HasBlockColor(blockcolor bc);

};



/** Gem **/
class gem : public obj
{
 private:
     bool anim;
     gemtype gemkind;
     void OnDeath() {}
     static unsigned int count[5];
 public:
     gem(square* sq, gemtype t);
     void Draw(unsigned int x, unsigned int y);
     static void Draw(unsigned int x, unsigned int y,gemtype gemkind,bool anim);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);


     static bool GotAllGems();
     static bool GotAllGems(blockcolor c);
     static void ResetCounts();
     static bool GetRemanents(unsigned int &yl,unsigned int &rd,unsigned int &bl,unsigned int &gr);
};

/** Star **/
class star : public obj
{
 private:
     bool anim;
     void OnDeath() {}
     static unsigned int count;
     static unsigned int acquired;
 public:
     star(square* sq);
     void Draw(unsigned int x, unsigned int y);
     bool trypush(edir dir,obj* pusher);
     bool HasRoundCorner(roundcorner rnc);
     bool Loop(bool* died);
     bool HasBlockColor(blockcolor bc);


     static int GetRemaining();
     static int GetAcquired();
     static void ResetCounts();
};

/** Trick Door**/
enum tdtype { td_HORZ,td_VERT,td_SUPER, td_FORCEARROW_UP,td_FORCEARROW_DOWN,td_FORCEARROW_LEFT,td_FORCEARROW_RIGHT };

class tdoor: public gobj
{
    protected:
      tdtype tt;
      bool ALLOW_UP,  ALLOW_RIGHT,ALLOW_DOWN,ALLOW_LEFT;
      bool ALLOW_NONXYE;
      bool anim;

      void OnDeath() {}
      static SDL_Color DefColor;
      static SDL_Color ForceArrowDefColor;
      Uint8 R;
      Uint8 G;
      Uint8 B;


    public:
     tdoor(square* sq,tdtype t,bool up, bool right, bool down, bool left);
     void Loop() {}
     void Draw(unsigned int x, unsigned int y);
     void OnEnter(obj *entering);
     void OnLeave(obj *entering);
     bool CanEnter(obj *entering, edir dir);
     bool CanLeave(obj *entering, edir dir);
     void ChangeColor(Uint8 nR,Uint8 nG,Uint8 nB);

    static void ResetDefaults();
    static void ChangeDefaultColor(Uint8 nR,Uint8 nG,Uint8 nB);
    static void ChangeForceArrowDefaultColor(Uint8 nR,Uint8 nG,Uint8 nB);
};


/** Marked Area**/
class marked: public gobj
{
    protected:
     bool active;
     unsigned char anim;
     void OnDeath();
     static unsigned int count[4];
     static unsigned int activeN[4];


    public:
     blockcolor c;
     marked(square* sq, blockcolor bc);

     void Draw(unsigned int x, unsigned int y);
     void Loop() {}
     void OnEnter(obj *entering);
     void OnLeave(obj *entering);
     bool CanEnter(obj *entering, edir dir);
     bool CanLeave(obj *entering, edir dir);

     static void Reset();
     static bool AllActive(blockcolor c);
     static bool AtLeastOneActive(blockcolor c);

};

/** FirePAd **/
class firepad: public gobj
{
    protected:
     unsigned char anim;
     void OnDeath();
     edir D;

    public:
     firepad(square* sq);

     void Draw(unsigned int x, unsigned int y);
     void Loop() {}
     void OnEnter(obj *entering);
     void OnLeave(obj *entering);
     bool CanEnter(obj *entering, edir dir);
     bool CanLeave(obj *entering, edir dir);

};

/** Pit **/
class pit: public gobj
{
    protected:
     bool end;
     bool dis;
     Uint8 dec;
     Uint8 alpha;
     void OnDeath();
     SDL_Surface * sur;

    public:
     pit(square* sq);

     void Draw(unsigned int x, unsigned int y);
     void Loop();
     void OnEnter(obj *entering);
     void OnLeave(obj *entering);
     bool CanEnter(obj *entering, edir dir);
     bool CanLeave(obj *entering, edir dir);
     static bool IsEnabledInstance(gobj* gobject);
     static bool CanConsume(obj* object);

};


/** Block Door**/
class blockdoor: public gobj
{
    protected:

     unsigned char anim;
     blockcolor c;
     void OnDeath() {}
     bool mode;
     bool trap;
     bool IsOpen();


    public:

     blockdoor(square* sq, bool AsTrap, bool startopen, blockcolor bc);

     void Draw(unsigned int x, unsigned int y);
     void Loop();
     void OnEnter(obj *entering);
     void OnLeave(obj *entering);
     bool IsOfColor(blockcolor bc);
     bool CanEnter(obj *entering, edir dir);
     bool CanLeave(obj *entering, edir dir);
     bool InsideKind(otype o) { return((o==OT_GEM) || (o==OT_KEY) || (o==OT_EARTH)); }




};

/** Hint **/
class hint: public gobj
{
    protected:
       SDL_Color bcolor;
       SDL_Color fcolor;
       bool xcla;
       string text;
       static string globaltext;
       void OnDeath() {}
       static hint* active;

    public:
     hint(square* sq,string ihint,bool warning);
     void Loop() {}
     void Draw(unsigned int x, unsigned int y);
     static void Draw(unsigned int x, unsigned int y,bool xcla);
     void OnEnter(obj *entering);
     void OnLeave(obj *entering);
     bool CanEnter(obj *entering, edir dir);
     bool CanLeave(obj *entering, edir dir);
     /*void ChangeColor(unsigned int nbc,unsigned int nfc);*/
     void SetHint(string hint);

     static bool Active();
     static const char* GetActiveText();
     static void Reset();
     static void GlobalHint(bool enb);
     static void SetGlobalHint(const char* gl);
     static void SetGlobalHint(string &gl);
     static bool GlobalHintExists();




};

/** Portal **/
class portal: public gobj
{
    protected:
        Uint8 R;
        Uint8 G;
        Uint8 B;
        Uint8 xyealpha;
        char canim;
        void OnDeath() {}
        square *target;
        static bool ignore;

    public:
     portal(square* sq,Uint8 cR, Uint8 cG, Uint8 cB, signed int TargetX, signed int TargetY);
     void Draw(unsigned int x, unsigned int y);
     void Loop();
     void OnEnter(obj *entering);
     void OnLeave(obj *entering);
     bool CanEnter(obj *entering, edir dir);
     bool CanLeave(obj *entering, edir dir);
     void ChangeColor(Uint8 cR, Uint8 cG, Uint8 cB);
     static void Reset();
};

class game;

class gameboard : public control
{
    public:
        gameboard( int sx, int sy, int sw, int sh);
        ~gameboard();
        void loop();
        void draw(SDL_Surface* target);
        void onMouseMove(int px,int py);
        void onMouseOut();
        void onMouseDown(int px,int py);
        void onMouseUp(int px,int py);
        inline void onMouseRightUp(int px,int py) {}


};


class gamepanel;

enum recordingmode
{
    RECORDING_MODE_MOVIE,
    RECORDING_MODE_BACKGROUND
};

/**class game*/
class game
{
    friend class gameboard;
    friend class gamepanel;
 private:
    
    game() {}
    
    static bool IsUndoAllowed();
    
    static void AfterLevelLoad();
    static void ExitCommandYesHandler(bool yesClicked);
    static void ExitCommand( const buttondata*bd = NULL);
    static void RestartCommand( const buttondata* bd = NULL);
    static void GoPreviousCommand( const buttondata*bd = NULL);
    static void GoNextCommand( const buttondata*bd = NULL);
    static void HintDownCommand( const buttondata*bd = NULL);
    static void HintUpCommand( const buttondata*bd = NULL);
    static void UndoCommand( const buttondata*bd = NULL);
    static void RecordSolutionCommand( const buttondata*bd = NULL);
    
    static void FFUpCommand( const buttondata*bd = NULL);
    static void FFDownCommand( const buttondata*bd = NULL);
    static void BrowseCommand( const buttondata*bd = NULL);
    static void SolutionCommand( const buttondata*bd = NULL);

    static bool mouse_pressed;
    static bool mouse_valid;
    static unsigned int mouse_x,mouse_y;

    static bool GameOver;
    static bool FinishedLevel;
    static square grid[XYE_HORZ][XYE_VERT];
    static unsigned int id_count;

    static int lastKey;
    static int GameWidth;
    static int GameHeight;
    static unsigned int LastXyeMove;
    static square* deathsq2;
    static unsigned char FastForward;
    static edir LastXyeDir;

    static unsigned char FlashPos;

    static void loop_Sub(char i, char j);
    static void loop_gameplay();
    static bool TryMoveXye(char dx, char dy, edir dir);




    static bool UpdateAll;

    static bool CoordMode;

    static bool ShiftPressed;
    static bool DK_PRESSED;
    static bool DK_UP_PRESSED;
    static bool DK_DOWN_PRESSED;
    static bool DK_LEFT_PRESSED;
    static bool DK_RIGHT_PRESSED;
    static int DK_PRESSED_FIRST;
    static bool DK_GO;
    static edir DK_DIR;

    static void DrawPanel(SDL_Surface* target, Sint16 x, Sint16 y, Sint16 w, Sint16 h);
    static bool EvalDirKeys();

    static void DrawPanelInfo(Drawer& D, Sint16 &cx,Sint16 &cy, Uint8 spx, Uint8 spy , unsigned int num, Uint8 fonadd, SDL_Color col);
    static void DrawPanelInfo(Drawer& D, Sint16 &cx,Sint16 &cy, Uint8 spx, Uint8 spy , unsigned int num, Uint8 fonadd, Uint8 R=255, Uint8 G=255, Uint8 B=255, Uint8 A=255);
    static signed int HintMarquee;
    static unsigned char HintRead;

    static bool EvalDirGrid(Uint16 x1,Uint16 y1,Uint16 x2,Uint16 y2,edir &dir);
    static bool EvalDirGrid(obj* object,Uint16 x2,Uint16 y2,edir &dir);
    static bool cameraon;
    static bool          PlayingRecording;
    static recordingmode RecordingMode;
    static bool undo;
    
    static void onKeyUp(SDLKey keysim, Uint16 unicode);
    static void onKeyDown(SDLKey keysim, Uint16 unicode);
    static void onExitAttempt();
    static void InitGameSection(window* wind);

    static string InitLevelFile;
    static int         InitLevelFileN;
    
    static void onMouseMove(int x,int y);
    static void onMouseOut();
    static void onMouseDown(int x,int y);
    static void onMouseUp(int x,int y);
    static void InitGraphics();
    static void CleanGraphics();
    
    static void loadGame();
    static void saveGame();
    
 public:
    static void RefreshGraphics();
    
    static void PlayLevel( const char* levelfile, int level);
    static void TestLevel( const char* levelfile, int level, bool playsolution=true);
 
    static Font* FontRes;
    static Font* FontRes_White;
    static Font* FontRes_Bold;
    static LuminositySprites sprites;
    static SDL_Surface* screen;


    static unsigned int counter;
    static char counter2;
    static char counter3;
    static char counter4;

    static char counter5;
    static char counter6;
    static char counter7;
    static char counter8;
    static char counter9;
    static unsigned int beastcounter;




    static SDL_Color PlayerColor;
    static square* deathsq1;
    static float NextTic;


    static xye* XYE;
    static bool started;
    static Sint16 GRIDSIZE;
    static string SKIN;





    static int Init(const char*levelfile=NULL);
    static void start(bool undotime=false);
    static void draw(Sint16 x, Sint16 y);
    static void MoveXye();

    static void incCounters();
    static unsigned int Counter();
    static bool Mod2();
    static bool Mod3();
    static bool Mod4();
    static bool Mod5();
    static bool Mod7();
    static bool Mod8();
    static bool Mod9();

    static bool Mod2(unsigned int m);
    static bool Mod3(unsigned int m);
    static bool Mod4(unsigned int m);
    static bool Mod5(unsigned int m);
    static bool Mod7(unsigned int m);
    static bool Mod8(unsigned int m);
    static bool Mod9(unsigned int m);



    static square* Square(unsigned char x,unsigned char y);
    static square* SquareN(signed int ,signed int y);
    static unsigned int NewId();
    static bool FindGoodPoint(char cx, char cy, char &rx, char &ry, obj* togo, bool (*cond)(square* sq,obj* togo)) ;
    static bool GetRevivePoint( char cx, char cy, char &nx, char &ny);
    static void loop();
    static void end();
    static bool AllowRoboXyeMovement();
    static bool TryMoveXye(edir dir);
    static bool LastMoveWasHorizontal();
    static void SmallBoom(square* sq, bool ConditionalFx=false, int  ox=0, int oy=0);
    static void FlashXyePosition();

    static bool Moved(edir d);
    static void TerminateGame(bool good=false);

    static void Davinci(Sint16 sx, Sint16 sy, Uint16 sw, Uint16 sh,Sint16 dx, Sint16 dy);
    static void Error(const char* msg);

    static void PlayRecording(const string rc);
    static void SaveReplay();
    static void Undo();
};


/** Object recycle queue **/
struct recycle_entry
{
    obj* o;
    recycle_entry* next;
};

class recycle
{
private:
 static recycle_entry* first;
public:
 static void init();
 static void run();
 static void add(obj* o);
};

/** /Object recycle queue **/


/** Kill Zone Queue **/
struct killzone
{
    char x;
    char y;
    killtype kt;
    killzone *next;
};

class deathqueue
{
 private:
    static killzone* first;
    static killzone* current;

 public:
    static void reset();
    static void add(char ax, char ay, killtype t);
    static bool KillNow();
    static signed char incx[5];
    static signed char incy[5];

};


/** SquareMemory **/
class sqMem
{
 public:
    static bool mem[XYE_HORZ][XYE_VERT];
    static void reset( bool val);
};





/******************************General Stuff!**********************************/


square* RoundAdvance(obj* ToMove,edir dir,char i, char j);
bool IsXyeAt(square* sq);
bool IsXyeOrBotAt(square* sq);

bool CanPush(otype t);
bool Allowed(obj* object, edir dir, square* sq);
bool Allowed(obj* object, edir dir, square* sq, otype except);

edir Opposite(edir dir);
edir Clock(edir dir);
edir AClock(edir dir);
edir RandomDir();

bool OldBeastPickDirectDir(edir &r, edir dirs[], unsigned char dirn, char x1, char y1, char x2, char y2);
bool PickDirectDir(edir &r,edir dirs[], unsigned char dirn, char x1, char y1, char x2, char y2,bool mode, bool horzpriority);
bool PickMostDirectDir(edir &r, edir dirs[], unsigned char dirn,  char x1, char y1, char x2, char y2);
edir PickDirectDir(char x1, char y1, char x2, char y2,bool mode, bool horzpriority);

void DebugMsg(const char *msgText, void *outParam=NULL);
bool SpotAffectedByAMagnetic(obj* forObject, char x, char y);

bool AI_RoboMine(obj* foro, edir &r);


char FixVert(char vert);
char FixHorz(char horz);
void FixCoordinates(char &horz, char &vert);

