/*!
  \brief  
  X-Keyboard & Mouse Hook App based on the Win32 APIs.

  \file 
  xkbdutil.h

  \author 

  \date 
  2016/04/13

  \version  
  1.0

  \copyright  
  GPL

  \remarks  

*/
#ifndef _XKBDUTIL_H_
#define _XKBDUTIL_H_

#if defined(_DEBUG) || defined(DEBUG)
# define XK_DEBUG 
#endif
#ifdef _MSC_VER
# define XK_MSC
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
# define XK_WIN32 
#endif
#if defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
# define XK_WIN64
#endif 
#if defined(XK_WIN32) || defined(XK_WIN64) || defined(_WINDLL)
# define XK_WINDOWS 
#endif 

#if defined(_XK_EXPORT_DLL) && defined(_XK_XPORT_LIB)
#error "Export dll and import or export static lib both not supported!"
#endif 

#ifdef _XK_EXPORT_DLL
# define XK_API __declspec(dllexport) 
#elif defined(_XK_XPORT_LIB)
# define XK_API 
#else
# define XK_API __declspec(dllimport) 
#endif
#ifdef _XK_XPORT_LIB
# define XK_CLINKAGE 
#else
# define XK_CLINKAGE extern "C" 
#endif

#ifdef __cplusplus
# define XK_CXX 
#endif 

#ifdef XK_CXX
# define XK_NAMESPACE_BEGIN namespace xk {
# define XK_NAMESPACE_END }
# define XK_CLINKAGE_BEGIN XK_CLINKAGE {
# define XK_CLINKAGE_END }
#else
# define XK_NAMESPACE_BEGIN 
# define XK_NAMESPACE_END 
# define XK_CLINKAGE_BEGIN 
# define XK_CLINKAGE_END 
#endif 



XK_CLINKAGE_BEGIN

// NOTE: functions exported here.

XK_CLINKAGE_END

XK_NAMESPACE_BEGIN

struct KbdModInterface {
  virtual ~KbdModInterface();
  virtual bool Initialise(unsigned long) = 0;
  virtual void Release() = 0;
};

XK_NAMESPACE_END

typedef xk::KbdModInterface* (*xkGetModifierApi)();


#ifdef XK_MSC
# ifdef _XK_AUTOLINK_DLL
#   ifdef XK_DEBUG
#pragma comment(lib, "xkbdutil_d.lib")
#   else
#pragma comment(lib, "xkbdutil.lib")
#   endif
# elif defined(_XK_AUTOLINK_LIB)
#   ifdef XK_DEBUG
#pragma comment(lib, "libxkbdutil_d.lib")
#   else
#pragma comment(lib, "libxkbdutil.lib")
#   endif
# endif
#endif

#endif 
