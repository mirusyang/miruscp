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
#include <future>
#include <chrono>
#include <atomic>
#include <mutex>
#include "xkit/processenumerator.h"
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

  KbdModInterface* CreateModifierOnce();
  void Release();

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

struct KbdHook {
  static LRESULT CALLBACK _Entry(int, WPARAM, LPARAM);
 ~KbdHook();
 KbdHook(DWORD);

  static HHOOK hk;
};

HHOOK KbdHook::hk(nullptr);

#if 0

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
  // TODO: KbdLowLevelProc
  return CallNextHookEx(kbd_hook, code, wparam, lparam);
}

#endif

LRESULT CALLBACK KbdHook::_Entry(int code, WPARAM wparam, LPARAM lparam) {
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
  return CallNextHookEx(hk, code, wparam, lparam);
}

KbdHook::~KbdHook() {
  if (hk) {
    UnhookWindowsHookEx(hk);
    hk = nullptr;
  }
}

KbdHook::KbdHook(DWORD thread_id) {
  hk = SetWindowsHookEx(WH_KEYBOARD_LL, _Entry, (HINSTANCE)g_app.get_handle(), 
      thread_id);
  if (!hk) {
    auto errstr(fmt_errmsg2(GetLastError()));
    throw RuntimeError(errstr);
  }
}

struct MouseHook {
  static LRESULT CALLBACK _Entry(int, WPARAM, LPARAM);
  ~MouseHook();
  MouseHook();

  static HHOOK hk;
};

HHOOK MouseHook::hk(nullptr);

LRESULT CALLBACK MouseHook::_Entry(int code, WPARAM wparam, LPARAM lparam) {
  return CallNextHookEx(hk, code, wparam, lparam);
}

MouseHook::~MouseHook() {
}

MouseHook::MouseHook() {
}

class WarkeyModifier : public KbdModInterface {
 public:
  ~WarkeyModifier();
  WarkeyModifier();
  bool Initialise(DWORD);
  void Cleanup();

  bool searching() const {
    return searching_;
  }

 private:
  void TryStartWar3Hook();

  unique_ptr<KbdHook> kbd_;
  mutex hookslock_;
  atomic<bool> searching_;
  future<void> war3tracker_;
};

WarkeyModifier::~WarkeyModifier() {
  searching_ = false;
  try {
    war3tracker_.get();
  } catch (...) {
  }
}

WarkeyModifier::WarkeyModifier() : searching_(false) {
  TryStartWar3Hook();
}

bool WarkeyModifier::Initialise(DWORD pid) {
  using xk::WndEnumerator;

  unique_lock<mutex> guard(hookslock_);
  if (!kbd_) {
    //GetWindowThreadProcessId()
    //GetProcessInformation
    auto wnd = WndEnumerator::Main(pid);
    if (wnd) {
      auto thread_id = GetWindowThreadProcessId(wnd, nullptr);
      kbd_.reset(new KbdHook(thread_id));
    }
  }
  return true;
}

void WarkeyModifier::Cleanup() {
  unique_lock<mutex> guard(hookslock_);
}

void WarkeyModifier::TryStartWar3Hook() {
  using std::async;
  using xk::ProcessEnumerator;

  searching_ = true;
  auto &kmod(*this);
  war3tracker_ = async([&kmod]() {
    bool initialised(false);
    while (kmod.searching()) {
      ProcessEnumerator pe;
      auto war3_pid = pe.fuzzy_lookup(_T("war3.exe"));
      if (0 != war3_pid) {
        // The PID found! War3 is running... Initialise the hooks now!
        if (!initialised) {
          { // Can I just beeeeeeep?!!!
            MessageBeep(MB_ICONINFORMATION);
          }
          try {
            kmod.Initialise(war3_pid);
          } catch (exception &e) {
            MessageBoxA(nullptr, e.what(), "Exception", MB_OK);
          }
          initialised = true;
        }
      } else {
        // Not found. Cleanup the hooks.
        if (initialised) {
          {
            MessageBeep(MB_ICONSTOP);
          }
          kmod.Cleanup();
          initialised = false;
        }
      }
      this_thread::sleep_for(chrono::milliseconds(100));
    }
  });
}

XK_NAMESPACE_END

XK_NAMESPACE_BEGIN

DllEntryApp::~DllEntryApp() {
  Release();
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
    wkmod.reset(new WarkeyModifier());
  }
  return wkmod.get();
}

void DllEntryApp::Release() {
  if (wkmod) {
    wkmod.reset();
  }
}

int DllEntryApp::ProcessAttach(LPVOID) {
  CreateModifierOnce();
  return 1;
}

int DllEntryApp::ProcessDetach(LPVOID) {
  Release();
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

XK_API xk::KbdModInterface* xkGetModifier() {
  return g_app.CreateModifierOnce();
}

XK_CLINKAGE_END 

int WINAPI DllMain(HANDLE dllhandle, DWORD reason, LPVOID reserved) {
  g_app.set_handle_once(dllhandle);
  return g_app.run(reason, reserved);
}

#endif
