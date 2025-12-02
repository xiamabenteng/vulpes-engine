#define HISTOGRAM_SIZE 256
#define DOUBLE_ESPLION 1e-8
#define FLOAT_ESPLION 1e-6F
#define FLOAT_MAX 3.402823466e+38F
#define FLOAT_MIN 1.175494351e-38F

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#include <afxwin.h>
#include <afxext.h>
#include <afxmt.h>
#include <afxole.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif
#include<algorithm>
#include <atlimage.h>
#include <list>
#include <vector>
#include <direct.h>
using namespace std;
void OutputDebug(const WCHAR * strOutputString, ...);