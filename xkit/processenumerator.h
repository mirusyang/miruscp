/*!
  \brief  
  Process enumerator.

  \file 
  processenumerator.h

  \author 

  \date 
  2016/04/14

  \version  
  1.0

  \copyright  
  GPL

  \remarks

*/
#ifndef _PROCESSENUMERATOR_H_
#define _PROCESSENUMERATOR_H_

#include "winutilities.h"
#include <tlhelp32.h>

XK_NAMESPACE_BEGIN

/*!
  \brief  
  Win-process enumerating object.
*/
struct ProcessEnumerator {
  ~ProcessEnumerator() {
    closehandle();
  }

  //ProcessEnumerator() try : procsnap(nullptr) {
  //  procsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  //  if (INVALID_HANDLE_VALUE == procsnap) {
  //    PrintError(_T("CreateToolhelp32Snapshot (of processes)"));
  //    throw ;
  //  }
  //} catch (...) {
  //  throw ;
  //}

  /*!
    \exception      Exception 
    System error message formatted thrown out.
  */
  ProcessEnumerator() : procsnap(nullptr) {
    if (!snapshot()) {
      throw RuntimeError(fmt_errmsg2(GetLastError()));
    }
  }

  /*!
    \brief  
    Creates a snapshot for current process list.
  */
  bool snapshot() {
    closehandle();
    procsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == procsnap) {
      procsnap = nullptr;
      return false;
    }
    return true;
  }

  void closehandle() {
    if (procsnap) {
      CloseHandle(procsnap);
      procsnap = nullptr;
    }
  }

  /*!
    \param          name  
    Process name.

    \return 
    The process ID. Errors occurred or the process not found, 0 returns.
  */
  DWORD operator ()(const TCHAR *name) {
    PROCESSENTRY32 pe = {sizeof pe};
    auto retokay = Process32First(procsnap, &pe);
    while (retokay) {
      bool match = 0 == _tcscmp(name, pe.szExeFile);
      if (match) {
        return pe.th32ProcessID;
      }
      retokay = Process32Next(procsnap, &pe);
    }
    return 0;
  }

 private:
  HANDLE procsnap;    //!< process list snapshot.
};

XK_NAMESPACE_END

#endif 
