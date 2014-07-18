#ifndef COMPAT_H
#define COMPAT_H

#ifdef WIN32
#include <Windows.h>
#include <io.h>
#define SNPRINTF _snprintf
#define ACCESS _access
#define MKDIR _mkdir
#define PATHSEP '\\'
#elif UNIX
#include <unistd.h>
#define SNPRINTF snprintf
#define ACCESS access
#define MKDIR mkdir
#define PATHSEP '/'
#endif

#endif