// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// TODO: reference additional headers your program requires here
#include <comdef.h>   
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
 #include <Dbghelp.h>
#pragma comment(lib,"Dbghelp.lib")

#pragma  comment(lib, "HVDLL.lib")