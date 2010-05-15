// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
//#define _SECURE_SCL 0



// c
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <assert.h>
#include <time.h>
#include <stdint.h>

// boost
#include <boost/scoped_array.hpp>

// dx
#include <d3d11.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>
#include <D3DX10math.h>
#include <d2d1.h>
#include <DWrite.h>
#include <d2d1helper.h>
#include <wincodec.h>

// c++, stl
#include <string>
#include <hash_map>
#include <vector>
#include <map>

// win
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <atlbase.h>


#include "targetver.h"
