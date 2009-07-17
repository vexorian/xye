#include <cstdio>
#include <dirent.h>
class Folder
{
 private:
    DIR* D;
    char dirname[FILENAME_MAX];
 public:
    Folder(const char* dirname);
    ~Folder();

    bool Open();
    void Close();
    bool Reset();
    const char* NextSubFolder();
    const char* NextFile();
    const char* NextFileOfType(const char* ext);
    const char* NextFileMatching(  bool (*cond)(const char*)    );

};

bool HasExtension(const char* n, const char* ext );
