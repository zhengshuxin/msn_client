// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件

#pragma once

#ifdef VC2003
#include "stdafx_vc2003.h"
#else
#include "stdafx_vc2010.h"
#endif

#ifdef WIN32
# ifdef _DEBUG
#  ifndef _CRTDBG_MAP_ALLOC
#   define _CRTDBG_MAP_ALLOC
#   include <crtdbg.h>
#   include <stdlib.h>
#  endif
#   define NEW   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#  else
#  define NEW new
# endif // _DEBUG
#else
# define NEW new
#endif
