// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>

// C RunTime Header Files
#include "Resource.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <algorithm>
#include <functional>
#include <memory>
#include <memory.h>
#include <string>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <thread>
#include <random>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <d3d10_1.h>
#include <d3d11.h>
#include <comdef.h>
#include <wincodec.h>
#include <map>
#include <wrl.h>
#include <wrl/client.h>
#pragma comment(lib,"d2d1")
#pragma comment(lib,"d3d10")
#pragma comment(lib,"d3d10_1")
#pragma comment(lib,"dwrite")
#pragma comment(lib,"windowscodecs")
using Microsoft::WRL::ComPtr;
// TODO: reference additional headers your program requires here
enum CompilerResult{
	ACCEPTED,
	REJECTED
};
static std::wstring random_string(size_t length){
	std::wstring charset(L"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	std::random_device rd;
	std::mt19937 generator(rd());
	std::shuffle(charset.begin(),charset.end(),generator);
	return charset.substr(0,length);
}
using namespace D2D1;
#include "GameSettings.h"
#include "d2dhandle.h"
#include "D2DUI.h"
using namespace D2DUI;
#include "Scene.h"
#include "Parser.h"