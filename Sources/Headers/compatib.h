// compatib.h
// Compatibility interface for pseudo ANSI calls
// 10 june 1993, by Jelle Wind
// (c) Computer Department ITC


#ifndef ILWCOMPATIB_H
#define ILWCOMPATIB_H
#include "Headers\stdafx.h"
#include <list>

using namespace std;

#ifdef _MSC_VER
//#define strcmpi _stricmp
#endif

#define __export __declspec(dllexport)
#define _export __declspec(dllexport)
#define _import
#define __import

#define _Windows _WIN32

#ifdef _Windows
  #define ZAPPEXPORT __import
  #define DATEXPORT __import
  #define CALCEXPORT __import
  #define ZAPP
  #define HUGEBUFPTR
  #ifdef ILWAPPDLL
    #define APPEXPORT
  #else
    #define APPEXPORT __import
  #endif
//  #include "zapp.hpp"
#endif  // _Windows

#endif // ILWCOMPATIB_H





