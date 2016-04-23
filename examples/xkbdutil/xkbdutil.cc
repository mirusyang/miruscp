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
#include <atomic>
#include <algorithm>
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
  static LRESULT CALLBACK _KbdLowLevelProc(int, WPARAM, LPARAM);
  static LRESULT CALLBACK _MouseLowLevelProc(int, WPARAM, LPARAM);

  ~WarkeyModifier();
  bool Initialise();
  void Release();
  void Map(WORD, WORD);
  void Enable(bool);
  bool Enabled() const;
  bool IsKeyMapped(WORD) const;

  WarkeyModifier();
  WarkeyModifier(DWORD);

  void track_assistkeys_states(LPKBDLLHOOKSTRUCT kbdinfo) {
    if (!kbdinfo) {
      return;
    }
    auto vkcode = kbdinfo->vkCode;
    if (vkcode < VK_LSHIFT || vkcode > VK_RMENU) {
      return;
    }
    auto vkindex = vkcode - VK_LSHIFT;
    auto pressed = 0 == (LLKHF_UP & kbdinfo->flags);
    if (pressed) {
      // set bit 1
      akstates_ |= (1U << vkindex);
    } else {
      // set bit 0
      akstates_ &= ~(1U << vkindex);
    }
  }

  bool is_any_assistkey_pressed() const {
    return 0U != akstates_;
  }

  bool is_strict_vkcode(WORD vkc) const {
    return vkc > 0 && vkc < 255;
  }

  void release_hooks() {
    if (ms_hook_) {
      UnhookWindowsHookEx(ms_hook_);
      ms_hook_ = nullptr;
    }
    if (kbd_hook_) {
      UnhookWindowsHookEx(kbd_hook_);
      kbd_hook_ = nullptr;
    }
  }

  LRESULT next_llkbd_hook(int code, WPARAM wparam, LPARAM lparam) {
    return CallNextHookEx(kbd_hook_, code, wparam, lparam);
  }

  LRESULT next_llms_hook(int code, WPARAM wparam, LPARAM lparam) {
    return CallNextHookEx(ms_hook_, code, wparam, lparam);
  }

 private:
  bool KbdProc(WPARAM, LPARAM);
  bool MouseProc(WPARAM, LPARAM);
  bool UniqueKeyMapProc(WORD, WORD, LPKBDLLHOOKSTRUCT) const;
  bool SkillKeyMapProc(WORD, LPKBDLLHOOKSTRUCT) const;

  static const DWORD kInvalidTimestamp = 0xDEADBEEF;
  static const auto kNumKeyPairs = 256;
  static WarkeyModifier *inst;
  HHOOK kbd_hook_;
  KeyPair keymap_[kNumKeyPairs];
  mutable mutex keymap_lock_;
  uint32_t akstates_;
  HHOOK ms_hook_;
  atomic<bool> enabled_;
};

WarkeyModifier* WarkeyModifier::inst(nullptr);

LRESULT CALLBACK WarkeyModifier::_KbdLowLevelProc(int code, 
    WPARAM wparam, LPARAM lparam) {
  if (HC_ACTION != code) {
    return inst->next_llkbd_hook(code, wparam, lparam);
  }
  if (inst->KbdProc(wparam, lparam)) {
    return inst->next_llkbd_hook(code, wparam, lparam);
  }
  return 1;
}

LRESULT CALLBACK WarkeyModifier::_MouseLowLevelProc(int code, 
    WPARAM wparam, LPARAM lparam) {
  if (HC_ACTION != code) {
    return inst->next_llms_hook(code, wparam, lparam);
  }
  if (inst->MouseProc(wparam, lparam)) {
    return inst->next_llms_hook(code, wparam, lparam);
  }
  return 1;
}

WarkeyModifier::~WarkeyModifier() {
  release_hooks();
  inst = nullptr;
}

bool WarkeyModifier::Initialise() {
  Map(0x34, VK_NUMPAD7); Map(0x33, VK_NUMPAD8);
  Map(0x35, VK_NUMPAD4); Map(0x38, VK_NUMPAD5);
  Map(0x36, VK_NUMPAD1); Map(0x37, VK_NUMPAD2);
  //Map(0x32, 'A');
  //Map(VK_LMENU, VK_LSHIFT);
  return true;
  // TODO: WarkeyModifier::Intialise
}

void WarkeyModifier::Release() {
  // TODO: WarkeyModifier::Release
}

void WarkeyModifier::Map(WORD src, WORD target) {
  lock_guard<mutex> guardian(keymap_lock_);
  keymap_[src].first = (WORD)src;
  keymap_[src].second = (WORD)target;
}

void WarkeyModifier::Enable(bool enabled) {
  enabled_ = enabled;
}

bool WarkeyModifier::Enabled() const {
  return enabled_;
}

bool WarkeyModifier::IsKeyMapped(WORD srck) const {
  lock_guard<mutex> guard(keymap_lock_);
  return false;
  // TODO: WarkeyModifier::IsKeyMapped
}

WarkeyModifier::WarkeyModifier() 
    : kbd_hook_(nullptr), akstates_(0), ms_hook_(nullptr), enabled_(false) {
}

WarkeyModifier::WarkeyModifier(DWORD thread_id) : WarkeyModifier() {
  kbd_hook_ = SetWindowsHookEx(WH_KEYBOARD_LL, _KbdLowLevelProc, 
      (HINSTANCE)g_app.get_handle(), thread_id);
  if (!kbd_hook_) {
    auto errstr = fmt_errmsg2(GetLastError());
    throw RuntimeError(errstr);
  }
  ms_hook_ = SetWindowsHookEx(WH_MOUSE_LL, _MouseLowLevelProc, 
      (HINSTANCE)g_app.get_handle(), thread_id);
  if (!ms_hook_) {
    release_hooks();
    auto errstr = fmt_errmsg2(GetLastError());
    throw RuntimeError(errstr);
  }
  inst = this;
}

bool WarkeyModifier::KbdProc(WPARAM wparam, LPARAM lparam) {
  auto kbd((LPKBDLLHOOKSTRUCT)lparam);
  if (!kbd || !Enabled()) {
    // call the next hook.
    return true;
  }
  track_assistkeys_states(kbd);

  KeyPair km(0, 0);
  {
    std::lock_guard<std::mutex> guard(keymap_lock_);
    int i(kbd->vkCode & 0xFF);
    km.first = keymap_[i].first;
    km.second = keymap_[i].second;
  }
  // Maybe the target-key code is not the correct vk-code(1~254), then I just
  // ignore this group.
  if (is_strict_vkcode(km.first)) {
    if (0 == km.second) {
      // It is eaten.
      return false;
    }
    if (is_strict_vkcode(km.second)) {
      return UniqueKeyMapProc(km.first, km.second, kbd);
    }
  }
  return true;
  // TODO: WarkeyModifier::KbdProc
}

bool WarkeyModifier::MouseProc(WPARAM wparam, LPARAM lparam) {
  auto msinfo((LPMSLLHOOKSTRUCT)lparam);
  if (!msinfo || !Enabled()) {
    return true;
  }
  return true;
  // TODO: WarkeyModifier::MouseProc
}

bool WarkeyModifier::UniqueKeyMapProc(WORD srck, WORD tark, 
    LPKBDLLHOOKSTRUCT kbd) const {
  if (is_any_assistkey_pressed()) {
    return true;
  }
  if (kInvalidTimestamp != kbd->time) {
    const UINT kNumSent(1);
    INPUT inputs = {0};
    INPUT *inp = &inputs;
    inp->type = INPUT_KEYBOARD;
    inp->ki.wVk = tark;
    inp->ki.wScan = MapVirtualKeyEx(tark, MAPVK_VK_TO_VSC, 0);
    inp->ki.dwFlags = 0 != (kbd->flags & LLKHF_UP) ? KEYEVENTF_KEYUP : 0;
    inp->ki.time = kInvalidTimestamp; // This is self modification.
    inp->ki.dwExtraInfo = GetMessageExtraInfo(); 
    auto retv = SendInput(kNumSent, inp, sizeof(INPUT));
    if (kNumSent != retv) {
      // SendInput failed, just call the next hook.
      return true;
    }
    return false;
  }
  return true;
}

bool WarkeyModifier::SkillKeyMapProc(WORD srck, LPKBDLLHOOKSTRUCT kbd) const {
  return true;
  // TODO: WarkeyModifier::SkillKeyMapProc
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
