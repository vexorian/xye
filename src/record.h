

struct recentry
{
    recentry* next;
    unsigned int val;
    unsigned int count;
};

class recording
{
 private:
    static recentry* first;
    static recentry* last;
    static recentry* undos;
    static unsigned int undos_r;
    static void add(unsigned int val);

 public:
    static void init();

    static bool load(const string st);
    static char* save(); //use delete[] on returned value.

    static bool saveInFile(const string target,const char* levelfn,unsigned int ln);
    static bool undo();

    static void add(edir a);
    static void add_nomove();
    static bool get(edir &r, bool& nomove);
    static bool get_undo(edir &r, bool& nomove);
    static void clean();
};

