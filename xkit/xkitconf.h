/*!
  \brief  
  Useful macros defined.

  \file 
  xkitconf.h

  \author 

  \date 
  2016/04/14

  \version  
  1.0

  \copyright  
  GPL

  \remarks  

*/
#ifndef _XKITCONF_H_
#define _XKITCONF_H_

#ifdef __cplusplus
# define XK_CXX 
#endif 

#if defined(_DEBUG) || defined(DEBUG)
# define XK_DEBUG 
#endif
#if defined(_UNICODE) || defined(UNICODE)
# define XK_UNICODE 
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

#ifdef XK_CXX
# define XK_INLINE inline 
# define XK_NAMESPACE_BEGIN namespace xk {
# define XK_NAMESPACE_END }
# define XK_CLINKAGE_BEGIN XK_CLINKAGE {
# define XK_CLINKAGE_END }
#else
# define XK_INLINE 
# define XK_NAMESPACE_BEGIN 
# define XK_NAMESPACE_END 
# define XK_CLINKAGE_BEGIN 
# define XK_CLINKAGE_END 
#endif 

#endif
