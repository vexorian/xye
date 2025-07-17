#pragma once

#include <cstdio>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

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

#ifndef MKDIR
    #ifdef _WIN32
         #define MKDIR(d, p) mkdir(d)
    #else
         #define MKDIR mkdir
    #endif
#endif

bool TryToOpenFolder(const char* path);
