/*!
  \brief  
  Some utilities on the Win32 platform.

  \file 
  winutilities.h

  \author 

  \date 
  2016/04/14

  \version  
  1.0

  \copyright  
  GPL

  \remarks  

*/
#ifndef _WINUTILITIES_H_
#define _WINUTILITIES_H_

#include "xkitconf.h"
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif 
#include <windows.h>
#include <tchar.h>
#include <string>
#include <stdexcept>

XK_NAMESPACE_BEGIN

#ifdef XK_UNICODE
typedef std::wstring String;
#else
typedef std::string String;
#endif
typedef std::exception Exception;
typedef std::runtime_error RuntimeError;

/*!
  \brief  
  Formats the error message got by Win32 API GetLastError or the other system error.

  \note 
  This piece of code picked from MSDN.
*/
XK_INLINE String fmt_errmsg(DWORD last_err) {
  TCHAR msg_buff[512] = {0};
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
      nullptr, last_err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
      msg_buff, sizeof msg_buff, nullptr);
  TCHAR* p = msg_buff;
  while (*p > 31 || 9 == *p) {
    ++p;
  }
  do {
    *p-- = 0;
  } while (p >= msg_buff && (*p == '.' || *p < 33));
  return String(msg_buff);
}

/*!
  \note 
  This is a special version used by the std exception. *It may be deprecated in future.*

  \see  
  fmt_errmsg
*/
XK_INLINE std::string fmt_errmsg2(DWORD last_err) {
  CHAR msg_buff[512] = {0};
  FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
      nullptr, last_err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
      msg_buff, sizeof msg_buff, nullptr);
  CHAR* p = msg_buff;
  while (*p > 31 || 9 == *p) {
    ++p;
  }
  do {
    *p-- = 0;
  } while (p >= msg_buff && (*p == '.' || *p < 33));
  return std::string(msg_buff);
}

XK_NAMESPACE_END

#endif 
