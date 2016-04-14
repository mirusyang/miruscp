/*!
  \brief  
  Adjusts process privileges.

  \file 
  adjustprivileges.h

  \author 

  \date 
  2016/04/14

  \version  
  1.0

  \remarks  

*/
#ifndef _ADJUSTPRIVILEGES_H_
#define _ADJUSTPRIVILEGES_H_

#include "winutilities.h"

XK_NAMESPACE_BEGIN

/*!
  \brief  
  Adjusts the DEBUG privilege of the process specified by proc HANDLE.

  It may be fallen and the exception thown out. Remember to catch that devil.

  \exception        RuntimeError  
  System error.
*/
XK_INLINE void adjust_debug_privilege(HANDLE proc) {
  HANDLE my_token(nullptr);
  try {
    DWORD accs = TOKEN_ADJUST_PRIVILEGES;
    if (!OpenProcessToken(proc, accs, &my_token)) {
      throw RuntimeError(fmt_errmsg2(GetLastError()));
    }
    LUID se_debug_luid = {0};
    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &se_debug_luid)) {
      throw RuntimeError(fmt_errmsg2(GetLastError()));
    }
    TOKEN_PRIVILEGES tps = {0};
    tps.PrivilegeCount = 1;
    tps.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    tps.Privileges[0].Luid = se_debug_luid;
    TOKEN_PRIVILEGES prevp = {0};
    DWORD prevp_sz(0);
    PTOKEN_PRIVILEGES pre_tok = nullptr;
    PDWORD pre_tok_sz = nullptr;
    if (!AdjustTokenPrivileges(my_token, FALSE, &tps, sizeof tps, 
        pre_tok, pre_tok_sz)) {
      throw RuntimeError(fmt_errmsg2(GetLastError()));
    }
    auto lasterr = GetLastError();
    if (ERROR_SUCCESS != lasterr) {
      throw RuntimeError(fmt_errmsg2(lasterr));
    }
  } catch (RuntimeError &rte) {
    if (my_token) {
      CloseHandle(my_token);
    }
    throw rte;
  } catch (...) {
    if (my_token) {
      CloseHandle(my_token);
    }
    throw;
  }
}

XK_NAMESPACE_END

#endif 
