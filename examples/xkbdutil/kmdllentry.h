/*!
  key-mod dll entry.
*/
#ifndef _KMDLLENTRY_H_
#define _KMDLLENTRY_H_

#include <windows.h>
#include <memory>
#include <map>
#include "xkbdutil.h"

class WarkeyModifier;

struct DllEntryApp {
  typedef int (DllEntryApp::*Entry)(LPVOID);

  ~DllEntryApp();
  DllEntryApp(HANDLE h);
  DllEntryApp();

  xk::KbdModInterface* CreateModifierOnce(DWORD);

  HANDLE get_handle() const {
    return handle;
  }

 protected:
  int run(DWORD reason, LPVOID reserved) {
    if (0 != entries.count(reason)) {
      return (this->*entries[reason])(reserved);
    }
    return -1;
  }

  void set_handle_once(HANDLE h) {
    if (!handle) {
      handle = h;
    }
  }

 private:
  int ProcessAttach(LPVOID);
  int ProcessDetach(LPVOID);
  int ThreadAttach(LPVOID);
  int ThreadDetach(LPVOID);

  HANDLE handle;
  std::map<DWORD, Entry> entries;
  std::unique_ptr<WarkeyModifier> wkmod;

  friend int WINAPI DllMain(HANDLE, DWORD, LPVOID);
};

extern DllEntryApp g_app;

#endif // _KMDLLENTRY_H_
