/*!
  War3 key modifier.
*/
#ifndef _WARKEYMOD_H_
#define _WARKEYMOD_H_

#include <windows.h>
#include <atomic>
#include <mutex>
#include "xkbdutil.h"

class WarkeyModifier : public xk::KbdModInterface {
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
  mutable std::mutex keymap_lock_;
  uint32_t akstates_;
  HHOOK ms_hook_;
  std::atomic<bool> enabled_;
};

#endif // _WARKEYMOD_H_
