#pragma once

#include "targetver.h"

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

// Windows Header Files:
#include <windows.h>

#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = nullptr; }
#define SAFE_DELETE(x) if( x ) { delete(x); (x) = nullptr; }
#define SAFE_DELETE_ARRAY(x) if( x ) { delete[](x); (x) = nullptr; }
#define IF_HR(hr) if(FAILED(hr)) { return hr;}