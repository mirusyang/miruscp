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
#include <mutex>
#include "xkit/winutilities.h"
#include "wres/resource.h"

using namespace std;

#ifdef XK_WINDOWS

XK_NAMESPACE_BEGIN

struct KbdModInterface;
class WarkeyModifier;

struct DllEntryApp {
  typedef int (DllEntryApp::*Entry)(LPVOID);

  ~DllEntryApp();
  DllEntryApp(HANDLE h);
  DllEntryApp();

  KbdModInterface* CreateModifierOnce(DWORD);

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

XK_NAMESPACE_END 

static xk::DllEntryApp g_app;

XK_NAMESPACE_BEGIN

KbdModInterface::~KbdModInterface() {
}

class WarkeyModifier : public KbdModInterface {
  typedef pair<WORD, WORD> KeyPair;

 public:
  static LRESULT CALLBACK KbdLowLevelProc(int, WPARAM, LPARAM);

  ~WarkeyModifier();
  bool Initialise();
  void Release();
  void Map(WORD, WORD);
  WarkeyModifier(DWORD);

 private:
  static const DWORD kInvalidTimestamp = 0xDEADBEEF;
  static const auto kNumKeyPairs = 256;
  static HHOOK kbd_hook;
  static KeyPair keymap[kNumKeyPairs];
  static mutex keymap_lock;
};

HHOOK WarkeyModifier::kbd_hook(nullptr);

WarkeyModifier::KeyPair WarkeyModifier::keymap[WarkeyModifier::kNumKeyPairs];

mutex WarkeyModifier::keymap_lock;

LRESULT CALLBACK WarkeyModifier::KbdLowLevelProc(int code, WPARAM wparam, LPARAM lparam) {
  try {
    if (HC_ACTION != code) {
      throw;
    }
    auto kbd((LPKBDLLHOOKSTRUCT)lparam);
    if (!kbd) {
      throw;
    }
    KeyPair km(0, 0);
    {
      std::lock_guard<std::mutex> guard(keymap_lock);
      int i(kbd->vkCode & 0xFF);
      km.first = keymap[i].first;
      km.second = keymap[i].second;
    }
    if (0 != km.first) {
      if (0 == km.second) {
        // It is eaten.
        return 1;
      } else {
        if (0 != km.second && kInvalidTimestamp != kbd->time) {
          const UINT kNumSent(1);
          INPUT inputs = {0};
          INPUT *inp = &inputs;
          inp->type = INPUT_KEYBOARD;
          inp->ki.wVk = km.second;
          inp->ki.wScan = MapVirtualKeyEx(km.second, MAPVK_VK_TO_VSC, 0);
          inp->ki.dwFlags = kbd->flags & LLKHF_UP ? KEYEVENTF_KEYUP : 0;
          inp->ki.time = kInvalidTimestamp; // This is self modification.
          inp->ki.dwExtraInfo = GetMessageExtraInfo(); 
          auto retv = SendInput(kNumSent, inp, sizeof(INPUT));
          if (kNumSent != retv) {
            //LOG(debug) << "SendInput returns " << retv << ",GetLastError:"
            //    << GetLastError();
          }
          return 1;
        }
      }
    }
  } catch (...) {
  }
  // TODO: KbdLowLevelProc
  return CallNextHookEx(kbd_hook, code, wparam, lparam);
}

WarkeyModifier::~WarkeyModifier() {
  if (kbd_hook) {
    UnhookWindowsHookEx(kbd_hook);
    kbd_hook = nullptr;
  }
}

bool WarkeyModifier::Initialise() {
  Map(VK_NUMPAD7, 0x34); Map(VK_NUMPAD8, 0x33);
  Map(VK_NUMPAD4, 0x35); Map(VK_NUMPAD5, 0x38);
  Map(VK_NUMPAD1, 0x36); Map(VK_NUMPAD2, 0x37);
  return true;
  // TODO: WarkeyModifier::Intialise
}

void WarkeyModifier::Release() {
  // TODO: WarkeyModifier::Release
}

void WarkeyModifier::Map(WORD src, WORD target) {
  std::lock_guard<std::mutex> guardian(keymap_lock);
  keymap[src].first = (WORD)src;
  keymap[src].second = (WORD)target;
}

WarkeyModifier::WarkeyModifier(DWORD thread_id) {
  kbd_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KbdLowLevelProc, 
      (HINSTANCE)g_app.get_handle(), thread_id);
  if (!kbd_hook) {
    auto errstr = fmt_errmsg2(GetLastError());
    throw RuntimeError(errstr);
  }
}

XK_NAMESPACE_END

XK_NAMESPACE_BEGIN

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

KbdModInterface* DllEntryApp::CreateModifierOnce(DWORD thread_id) {
  if (!wkmod) {
    wkmod.reset(new WarkeyModifier(thread_id));
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

XK_CLINKAGE_BEGIN

XK_API xk::KbdModInterface* xkGetModifier(DWORD thread_id) {
  return g_app.CreateModifierOnce(thread_id);
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
