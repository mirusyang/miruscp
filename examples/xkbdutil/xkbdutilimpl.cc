/*!
  \brief  
  X-Keyboard Hooker.

  \file 
  xkbdutilimpl.cc

  \date 
  2016/04/13

  \author 

  \copyright  
  GPL

  \remarks

*/
#include <algorithm>
#include <future>
#include <condition_variable>
#include "xkit/winutilities.h"
#include "wres/resource.h"
#include "kmdllentry.h"
#include "warkeymod.h"

using namespace std;

XK_NAMESPACE_BEGIN

KbdModInterface::~KbdModInterface() {
}

XK_NAMESPACE_END

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

  {
    const auto kScreenWidth = 1366U;
    const auto kScreenHeight = 768U;
    int sk1left = 1058;
    int sk1top = 602;
    int skboxw = 62;
    int skboxh = 46;
    int horzoffset = 74; // 1132, 1206
    int vertoffset = 56; // 658, 713
    uint8_t sks[] = {
      'M', 'S', 'H', 'A', 
      'P', 'G', 'D', 'F', 
      'Q', 'W', 'E', 'R', 
    };
    // 3 x 4
    for (int r = 0, endr = 3; r < endr; ++r) {
      for (int c = 0, endc = 4; c < endc; ++c) {
        auto rl = sk1left + horzoffset * c;
        auto rt = sk1top + vertoffset * r;
        auto rr = rl + skboxw;
        auto rb = rt + skboxh;
        MapSk(sks[r*endc + c], rl, rt, rr, rb);
      }
    }
  }
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
    auto errstr = xk::fmt_errmsg2(GetLastError());
    throw xk::RuntimeError(errstr);
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
    lock_guard<mutex> guard(keymap_lock_);
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
  * Send click event ( the screct area(mouse) )
* If the key up: / or the secondary press event comes:
  * Moves to the original position(mouse)
NOTE: should prevent the key pressed repeatedly.
*/
bool WarkeyModifier::SkillKeyMapProc(const RECT &screct, LPKBDLLHOOKSTRUCT kbd) {
  static enum {
    S_IDLE = 0,
    S_CLICKED,
    S_MOVBACK
  } step = S_IDLE;
  static POINT cursor_original_pos = {0};
  INPUT inputs[4] = {0};
  UINT num_sent(0);
  if (LLKHF_UP != (kbd->flags & LLKHF_UP)) { // key-down
    if (S_IDLE == step) {
      GetCursorPos(&cursor_original_pos);
      auto target_pos_x = screct.left + (screct.right - screct.left) / 2;
      auto target_pos_y = screct.top + (screct.bottom - screct.top) / 2;
      num_sent = 0;
      inputs[num_sent].type = INPUT_MOUSE;
      inputs[num_sent].mi.dx = LONG(double(target_pos_x) / 1366U * 65535);
      inputs[num_sent].mi.dy = LONG(double(target_pos_y) / 768U * 65535);
      inputs[num_sent].mi.mouseData = 0;
      inputs[num_sent].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE
          | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
      inputs[num_sent].mi.time = 0; // kInvalidTimestamp;
      inputs[num_sent].mi.dwExtraInfo = GetMessageExtraInfo();
      ++num_sent;
      //inputs[num_sent].type = INPUT_MOUSE;
      //inputs[num_sent].mi.dx = LONG(double(cursor_original_pos.x) / 1366U * 65535);
      //inputs[num_sent].mi.dy = LONG(double(cursor_original_pos.y) / 768U * 65535);
      //inputs[num_sent].mi.mouseData = 0;
      //inputs[num_sent].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;
      //inputs[num_sent].mi.time = 0; // kInvalidTimestamp;
      //inputs[num_sent].mi.dwExtraInfo = GetMessageExtraInfo();
      //++num_sent;
      INPUT *inp = inputs;
      auto retv = SendInput(num_sent, inp, sizeof(INPUT));
      if (num_sent != retv) {
        // SendInput failed, just call the next hook.
        return true;
      }
      step = S_CLICKED;
    } else if (S_CLICKED == step) {
      // Moves the cursor to the original position.
      num_sent = 0;
      inputs[num_sent].type = INPUT_MOUSE;
      inputs[num_sent].mi.dx = LONG(double(cursor_original_pos.x) / 1366U * 65535);
      inputs[num_sent].mi.dy = LONG(double(cursor_original_pos.y) / 768U * 65535);
      inputs[num_sent].mi.mouseData = 0;
      inputs[num_sent].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;
      inputs[num_sent].mi.time = 0; // kInvalidTimestamp;
      inputs[num_sent].mi.dwExtraInfo = GetMessageExtraInfo();
      ++num_sent;
      INPUT *inp = inputs;
      if (num_sent != SendInput(num_sent, inp, sizeof(INPUT))) {
        return true;
      }
      step = S_MOVBACK;
    }
  } else { // key-up
    if (S_CLICKED == step) {
      // Moves the cursor to the original position.
      num_sent = 0;
      inputs[num_sent].type = INPUT_MOUSE;
      inputs[num_sent].mi.dx = LONG(double(cursor_original_pos.x) / 1366U * 65535);
      inputs[num_sent].mi.dy = LONG(double(cursor_original_pos.y) / 768U * 65535);
      inputs[num_sent].mi.mouseData = 0;
      inputs[num_sent].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;
      inputs[num_sent].mi.time = 0; // kInvalidTimestamp;
      inputs[num_sent].mi.dwExtraInfo = GetMessageExtraInfo();
      ++num_sent;
      INPUT *inp = inputs;
      if (num_sent != SendInput(num_sent, inp, sizeof(INPUT))) {
        return true;
      }
      step = S_MOVBACK;
    }
    step = S_IDLE;
  }
  return false;
}

//////////////// global functions exported here ////////////////////////////////

XK_CLINKAGE_BEGIN

XK_API xk::KbdModInterface* xkGetModifier(DWORD thread_id) {
  return g_app.CreateModifierOnce(thread_id);
}

XK_CLINKAGE_END 
