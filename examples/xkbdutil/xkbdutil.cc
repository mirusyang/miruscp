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
#include <future>
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
  struct SkillKeyMap {
    WORD tarkey;    //!< target key.
    RECT screct;    //!< screen rect to be clicked by l-mouse clicks.
  };

 public:
  static LRESULT CALLBACK _KbdLowLevelProc(int, WPARAM, LPARAM);
  static LRESULT CALLBACK _MouseLowLevelProc(int, WPARAM, LPARAM);

  static bool is_strict_vkcode(WORD vkc) {
    return vkc > 0 && vkc < 255;
  }

  static bool is_invalid_rect(const RECT &rect) {
    return 0 == rect.left && 0 == rect.top && 0 == rect.right && 0 == rect.bottom;
  }

  static bool is_modified_kbdinfo(LPKBDLLHOOKSTRUCT kbd) {
    return kInvalidTimestamp == kbd->time;
  }

  ~WarkeyModifier();
  bool Initialise();
  void Release();
  void Map(uint8_t, uint8_t);
  void MapSk(uint8_t, long, long, long, long);
  bool IsKeyMapped(uint8_t) const;
  void Clear(uint8_t);
  void Enable(bool);
  bool Enabled() const;

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
  bool SkillKeyMapProc(const RECT&, LPKBDLLHOOKSTRUCT);

  static const DWORD kInvalidTimestamp = 0xDEADBEEF;
  static const auto kNumKeyPairs = (uint32_t)(uint8_t)-1;
  static const WORD kKeyEaten = (WORD)-1;
  static WarkeyModifier *inst;
  HHOOK kbd_hook_;
  SkillKeyMap keymap_[kNumKeyPairs];
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

  //{
  //  const auto kScreenWidth = 1366U;
  //  const auto kScreenHeight = 768U;
  //  int sk1left = 1058;
  //  int sk1top = 602;
  //  int skboxw = 62;
  //  int skboxh = 46;
  //  int horzoffset = 74; // 1132, 1206
  //  int vertoffset = 56; // 658, 713
  //  uint8_t sks[] = {
  //    'M', 'S', 'H', 'A', 
  //    'P', 'G', 'D', 'F', 
  //    'Q', 'W', 'E', 'R', 
  //  };
  //  // 3 x 4
  //  for (int r = 0, endr = 3; r < endr; ++r) {
  //    for (int c = 0, endc = 4; c < endc; ++c) {
  //      auto rl = sk1left + horzoffset * c;
  //      auto rt = sk1top + vertoffset * r;
  //      auto rr = rl + skboxw;
  //      auto rb = rt + skboxh;
  //      MapSk(sks[r*endc + c], rl, rt, rr, rb);
  //    }
  //  }
  //}
  return true;
  // TODO: WarkeyModifier::Intialise
}

void WarkeyModifier::Release() {
  memset(keymap_, 0, sizeof keymap_);
  // TODO: WarkeyModifier::Release
}

void WarkeyModifier::Map(uint8_t src, uint8_t target) {
  lock_guard<mutex> guardian(keymap_lock_);
  keymap_[src].tarkey = (WORD)target;
  keymap_[src].screct = {0};
}

void WarkeyModifier::MapSk(uint8_t src, long l, long t, long r, long b) {
  lock_guard<mutex> guard(keymap_lock_);
  keymap_[src].tarkey = 0U;
  keymap_[src].screct = RECT{l, t, r, b};
}

bool WarkeyModifier::IsKeyMapped(uint8_t srck) const {
  lock_guard<mutex> guard(keymap_lock_);
  auto tarkey = keymap_[srck].tarkey;
  if (is_strict_vkcode(tarkey)) {
    return true;
  }
  if (kKeyEaten == tarkey && is_invalid_rect(keymap_[srck].screct)) {
    return true;
  }
  if (!is_invalid_rect(keymap_[srck].screct)) {
    return true;
  }
  return false;
}

void WarkeyModifier::Clear(uint8_t srck) {
  lock_guard<mutex> guard(keymap_lock_);
  memset(&keymap_[srck], 0, sizeof keymap_[srck]);
}

void WarkeyModifier::Enable(bool enabled) {
  enabled_ = enabled;
}

bool WarkeyModifier::Enabled() const {
  return enabled_;
}

WarkeyModifier::WarkeyModifier() 
    : kbd_hook_(nullptr), akstates_(0), ms_hook_(nullptr), enabled_(false) {
  memset(keymap_, 0, sizeof keymap_);
  //fill(pressed_, pressed_ + kNumKeyPairs, false);
}

WarkeyModifier::WarkeyModifier(DWORD thread_id) : WarkeyModifier() {
  kbd_hook_ = SetWindowsHookEx(WH_KEYBOARD_LL, _KbdLowLevelProc, 
      (HINSTANCE)g_app.get_handle(), thread_id);
  if (!kbd_hook_) {
    auto errstr = fmt_errmsg2(GetLastError());
    throw RuntimeError(errstr);
  }
  //ms_hook_ = SetWindowsHookEx(WH_MOUSE_LL, _MouseLowLevelProc, 
  //    (HINSTANCE)g_app.get_handle(), thread_id);
  //if (!ms_hook_) {
  //  release_hooks();
  //  auto errstr = fmt_errmsg2(GetLastError());
  //  throw RuntimeError(errstr);
  //}
  inst = this;
}

bool WarkeyModifier::KbdProc(WPARAM wparam, LPARAM lparam) {
  auto kbd((LPKBDLLHOOKSTRUCT)lparam);
  if (!kbd || !Enabled()) {
    // call the next hook.
    return true;
  }
  if (is_modified_kbdinfo(kbd)) {
    return true;
  }
  track_assistkeys_states(kbd);
  if (is_any_assistkey_pressed()) {
    return true;
  }

  WORD srckey(0), tarkey(0);
  RECT screct = {0};
  {
    std::lock_guard<std::mutex> guard(keymap_lock_);
    srckey = kbd->vkCode & 0xFF;
    tarkey = keymap_[srckey].tarkey;
    screct = keymap_[srckey].screct;
  }
  // Maybe the target-key code is not the correct vk-code(1~254), then I just
  // ignore this group.
  if (is_strict_vkcode(srckey)) {
    if (kKeyEaten == tarkey && is_invalid_rect(screct)) {
      // It is eaten.
      return false;
    }
    if (is_strict_vkcode(tarkey)) {
      return UniqueKeyMapProc(srckey, tarkey, kbd);
    }
    if (!is_invalid_rect(screct)) {
      return SkillKeyMapProc(screct, kbd);
    }
  }
  // Not mapped.
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

/*
Steps:
* If the key pressed:
  * Moves to the screct area(mouse)
  * Send click event
* If the key up: / or the secondary press event comes:
  * Moves to the original position(mouse)
NOTE: should prevent the key pressed repeatedly.
*/
bool WarkeyModifier::SkillKeyMapProc(const RECT &screct, LPKBDLLHOOKSTRUCT kbd) {
  static auto pressed(false);
  static auto cursor_moved(false);
  static POINT cursor_original_pos = {0};
  if (0 == (kbd->flags & LLKHF_UP)) {
    if (pressed) {
      if (!cursor_moved) {
        SetCursorPos(cursor_original_pos.x, cursor_original_pos.y);
        cursor_moved = true;
      }
      // Do not call the default hook processor.
      return false;
    }
    GetCursorPos(&cursor_original_pos);
    auto target_pos_x = screct.left + (screct.right - screct.left) / 2;
    auto target_pos_y = screct.top + (screct.bottom - screct.top) / 2;
    SetCursorPos(target_pos_x, target_pos_y);
    INPUT inputs[4] = {0};
    UINT num_sent(0);
    inputs[num_sent].type = INPUT_MOUSE;
    inputs[num_sent].mi.dx = 0;
    inputs[num_sent].mi.dx = 0;
    inputs[num_sent].mi.mouseData = 0;
    inputs[num_sent].mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
    inputs[num_sent].mi.time = 0; // kInvalidTimestamp;
    inputs[num_sent].mi.dwExtraInfo = GetMessageExtraInfo();
    ++num_sent;
    INPUT *inp = inputs;
    auto retv = SendInput(num_sent, inp, sizeof(INPUT));
    if (num_sent != retv) {
      // SendInput failed, just call the next hook.
      return true;
    }
    cursor_moved = false;
    pressed = true;
  } else {
    if (!cursor_moved) {
      // Moves the cursor to the original position.
      SetCursorPos(cursor_original_pos.x, cursor_original_pos.y);
      cursor_moved = true;
    }
    pressed = false;
  }
  return false;
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

KbdModInterface* DllEntryApp::CreateModifierOnce(DWORD thread_id) {
  if (!wkmod) {
    wkmod.reset(new WarkeyModifier(thread_id));
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

XK_API xk::KbdModInterface* xkGetModifier(DWORD thread_id) {
  return g_app.CreateModifierOnce(thread_id);
}

XK_CLINKAGE_END 

int WINAPI DllMain(HANDLE dllhandle, DWORD reason, LPVOID reserved) {
  g_app.set_handle_once(dllhandle);
  return g_app.run(reason, reserved);
}

#endif
