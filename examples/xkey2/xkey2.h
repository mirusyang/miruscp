/*!

  \file
  xkey2.h

  \author

  \date
  2016/11/13

  \version
  0.1

  \copyright

  \remarks

*/
#ifndef _XKEY2_H_
#define _XKEY2_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "../xkbdutil/xkbdutil.h"

class XKeyApp : public wxApp {
 public:
  ~XKeyApp() {
  }

  XKeyApp() {
  }

  void enableKeyMod() {
    if (kbdmod_) {
      kbdmod_->Enable(true);
    }
  }

  void disableKeyMod() {
    if (kbdmod_) {
      kbdmod_->Enable(false);
    }
  }

  bool isKeyModEnabled() const {
    if (kbdmod_) {
      return kbdmod_->Enabled();
    }
    return false;
  }

 private:
  bool OnInit();

  xk::KbdModInterface* kbdmod_ {nullptr};
};

#endif // _XKEY2_H_
