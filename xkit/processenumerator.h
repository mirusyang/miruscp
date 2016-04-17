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
#include <memory>

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

  DWORD fuzzy_lookup(const TCHAR *name) {
    using std::unique_ptr;

    if (!procsnap) {
      return 0;
    }
    auto namebuffer_len = _tcslen(name) + 1;
    unique_ptr<TCHAR[]> namebuffer(new TCHAR[namebuffer_len]);
    _tcscpy(namebuffer.get(), name);
    namebuffer[namebuffer_len - 1] = _T('\0');
    _tcsupr(namebuffer.get());
    PROCESSENTRY32 pe = {sizeof pe};
    auto retokay = Process32First(procsnap, &pe);
    while (retokay) {
      _tcsupr(pe.szExeFile);
      bool match = 0 == _tcscmp(namebuffer.get(), pe.szExeFile);
      if (match) {
        return pe.th32ProcessID;
      }
      retokay = Process32Next(procsnap, &pe);
    }
    return 0;
  }

  /*!
    \param          name  
    Process name.

    \return 
    The process ID. Errors occurred or the process not found, 0 returns.
  */
  DWORD operator ()(const TCHAR *name) {
    if (!procsnap) {
      return 0;
    }
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

/*!
  \brief  
  Threads enumerating.

  \deprecated 
  It's seems no useful... It'll be removed in future.
*/
struct ThreadEnumerator {
  ~ThreadEnumerator() {
    if (thrdsnap) {
      CloseHandle(thrdsnap);
      thrdsnap = nullptr;
    }
  }

  /*!
    \param          pid 
    process id. can be 0, it means walk through all the threads current running.
  */
  ThreadEnumerator(DWORD pid) : thrdsnap(nullptr) {
    thrdsnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
    if (INVALID_HANDLE_VALUE == thrdsnap) {
      thrdsnap = nullptr;
      throw RuntimeError(fmt_errmsg2(GetLastError()));
    }
    thrdentry.dwSize = sizeof thrdentry;
    begin();
  }

  void begin() {
    nextvalid = false;
    if (!Thread32First(thrdsnap, &thrdentry)) {
      auto errstr = fmt_errmsg2(GetLastError());
      CloseHandle(thrdsnap);
      thrdsnap = nullptr;
      throw RuntimeError(errstr);
    }
    nextvalid = true;
  }

  /*!
    \return 
    Next valid thread id returned if there is. Otherwise 0 returned, means no valid thread.
  */
  DWORD operator()() {
    if (!thrdsnap) {
      return 0;
    }
    if (!nextvalid) {
      return 0;
    }
    nextvalid = Thread32Next(thrdsnap, &thrdentry);
    return thrdentry.th32ThreadID;
  }

  HANDLE thrdsnap;
  THREADENTRY32 thrdentry;
  bool nextvalid;
};

/*!
  \brief  
  Enumerates windows by process id.
*/
struct WndEnumerator {
  struct WndEnumInfo {
    DWORD pid;
    HWND wnd;
  };

  static HWND Main(DWORD pid) {
    WndEnumInfo wei = {pid, nullptr};
    EnumWindows(_WndEnumProc, (LPARAM)&wei);
    auto mwnd = wei.wnd;
    //while (GetParent(mwnd)) {
    //  mwnd = GetParent(mwnd);
    //}
    return mwnd;
  }

  static BOOL CALLBACK _WndEnumProc(HWND wnd, LPARAM lparam) {
    auto winfo = (WndEnumInfo*)lparam;
    if (!winfo) {
      return TRUE;
    }
    DWORD pid(0);
    GetWindowThreadProcessId(wnd, &pid);
    if (pid == winfo->pid && !GetParent(wnd)) {
      winfo->wnd = wnd;
      return FALSE;
    }
    return TRUE;
  }

  ~WndEnumerator() {
  }

  WndEnumerator() {
  }
};

XK_NAMESPACE_END

#endif 
