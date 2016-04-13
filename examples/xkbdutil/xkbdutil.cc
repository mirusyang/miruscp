/*!
  \brief  
  X-Keyboard Hooker.

  \file 
  xkbdutil.cc

  \date 
  2016/04/13

  \author 

  \copyright  
  GPL

  \remarks

*/
#include "xkbdutil.h"
#ifdef XK_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#endif
#include <map>
#include <memory>
#include "wres/resource.h"

using namespace std;

#ifdef XK_WINDOWS

XK_NAMESPACE_BEGIN

KbdModInterface::~KbdModInterface() {
}

class WarkeyModifier : public KbdModInterface {
 public:
  static LRESULT CALLBACK KbdLowLevelProc(int, WPARAM, LPARAM);

  ~WarkeyModifier();
  bool Initialise(unsigned long);
  void Release();

  WarkeyModifier(HINSTANCE);

 private:
  static HHOOK kbd_hook;
  HINSTANCE handle_;
};

HHOOK WarkeyModifier::kbd_hook(nullptr);

LRESULT CALLBACK WarkeyModifier::KbdLowLevelProc(int code, WPARAM wparam, LPARAM lparam) {
/*
    // Sample code from the old project 'xkeyutil'.
    KeyMap km = {0};
    {
      std::lock_guard<std::mutex> guardian(keymap_mut);
      int i(kbd->vkCode & 0xFF);
      km.vksrc = keymap[i].vksrc;
      km.vktar = keymap[i].vktar;
    }
    if (0 != km.vksrc) {
      if (0 == km.vktar) {
        // It is eaten.
        return 1;
      } else {
        if (0 != km.vktar && kInvalidTimestamp != kbd->time) {
          const UINT kNumSent(1);
          INPUT inputs = {0};
          INPUT *inp = &inputs;
          inp->type = INPUT_KEYBOARD;
          inp->ki.wVk = km.vktar;
          inp->ki.wScan = MapVirtualKeyEx(km.vktar, MAPVK_VK_TO_VSC, 0);
          inp->ki.dwFlags = kbd->flags & LLKHF_UP ? KEYEVENTF_KEYUP : 0;
          inp->ki.time = kInvalidTimestamp; // This is self modification.
          inp->ki.dwExtraInfo = GetMessageExtraInfo(); 
          auto retv = SendInput(kNumSent, inp, sizeof(INPUT));
          if (kNumSent != retv) {
            LOG(debug) << "SendInput returns " << retv << ",GetLastError:"
                << GetLastError();
          }
          return 1;
        }
      }
    }
*/
  try {
    if (HC_ACTION != code) {
      throw;
    }
    auto kbd((LPKBDLLHOOKSTRUCT)lparam);
    if (!kbd) {
      throw;
    }
    auto vkcode = kbd->vkCode & 0xFF;
    TCHAR tip[MAX_PATH] = {0};
    _stprintf_s(tip, _T("Key pressed: %d"), vkcode);
    MessageBox(nullptr, tip, _T("Tip(s)"), MB_OK);
  } catch (...) {
  }
  // TODO: KbdLowLevelProc
  return CallNextHookEx(kbd_hook, code, wparam, lparam);
}

WarkeyModifier::~WarkeyModifier() {
  Release();
}

bool WarkeyModifier::Initialise(unsigned long thread_id) {
  Release();
  kbd_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KbdLowLevelProc, handle_, thread_id);
  // TODO: WarkeyModifier::Intialise
  return nullptr != kbd_hook;
}

void WarkeyModifier::Release() {
  if (kbd_hook) {
    UnhookWindowsHookEx(kbd_hook);
    kbd_hook = nullptr;
  }
  // TODO: WarkeyModifier::Release
}

WarkeyModifier::WarkeyModifier(HINSTANCE inst) : handle_(inst) {
}

XK_NAMESPACE_END

XK_NAMESPACE_BEGIN

struct DllEntryApp {
  typedef int (DllEntryApp::*Entry)(LPVOID);

  ~DllEntryApp();
  DllEntryApp(HANDLE h);
  DllEntryApp();

  KbdModInterface* CreateModifierOnce();

  int run(DWORD reason, LPVOID reserved) {
    if (0 != entries.count(reason)) {
      return (this->*entries[reason])(reserved);
    }
    return -1;
  }

  void set_handle(HANDLE h) {
    handle = h;
  }

  void set_handle_once(HANDLE h) {
    if (!handle) {
      handle = h;
    }
  }

  HANDLE get_handle() const {
    return handle;
  }

 private:
  int ProcessAttach(LPVOID);
  int ProcessDetach(LPVOID);
  int ThreadAttach(LPVOID);
  int ThreadDetach(LPVOID);

  HANDLE handle;
  map<DWORD, Entry> entries;
  unique_ptr<WarkeyModifier> wkmod;
};

DllEntryApp::~DllEntryApp() {
}

DllEntryApp::DllEntryApp(HANDLE h) : handle(h), wkmod(nullptr) {
  entries[DLL_PROCESS_ATTACH] = &DllEntryApp::ProcessAttach;
  entries[DLL_PROCESS_DETACH] = &DllEntryApp::ProcessDetach;
  entries[DLL_THREAD_ATTACH] = &DllEntryApp::ThreadAttach;
  entries[DLL_THREAD_DETACH] = &DllEntryApp::ThreadDetach;
}

DllEntryApp::DllEntryApp() : DllEntryApp(nullptr) {
}

KbdModInterface* DllEntryApp::CreateModifierOnce() {
  if (!wkmod) {
    wkmod.reset(new WarkeyModifier((HINSTANCE)get_handle()));
  }
  return wkmod.get();
}

int DllEntryApp::ProcessAttach(LPVOID) {
  return 1;
}

int DllEntryApp::ProcessDetach(LPVOID) {
  return 1;
}

int DllEntryApp::ThreadAttach(LPVOID) {
  return 1;
}

int DllEntryApp::ThreadDetach(LPVOID) {
  return 1;
}

XK_NAMESPACE_END 

static xk::DllEntryApp g_app;

XK_CLINKAGE_BEGIN

XK_API xk::KbdModInterface* xkGetModifier() {
  return g_app.CreateModifierOnce();
}

//XK_API void xkRelease(xk::KbdModInterface *kbdmod) {
//
//}

XK_CLINKAGE_END 

int WINAPI DllMain(HANDLE dllhandle, DWORD reason, LPVOID reserved) {
  g_app.set_handle_once(dllhandle);
  return g_app.run(reason, reserved);
}

#endif
