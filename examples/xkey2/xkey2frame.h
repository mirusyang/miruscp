/*!

  \file
  xkey2frame.h

  \author

  \date
  2016/11/13

  \version
  0.1

  \copyright

  \remarks

*/
#ifndef _XKEY2FRAME_H_
#define _XKEY2FRAME_H_

#include "xkey2.h"
#include "wx/gbsizer.h"

class WarkeyDlg : public wxDialog {
 public:
  ~WarkeyDlg();
  WarkeyDlg(XKeyApp *app);

 private:
  void OnKeyUp(wxKeyEvent&);
  void OnExit(wxCommandEvent&);
  void OnPaint(wxPaintEvent&);
  void OnMotion(wxMouseEvent&);
  void OnLeftDown(wxMouseEvent&);
  void OnAbout(wxCommandEvent&);
  void OnModEnabled(wxCommandEvent&);

  wxBitmap bkgnd_;
  wxPoint orig_wnd_pos_;      //!< original window position(screen) before dragging.
  wxPoint orig_mouse_pos_;    //!< original mouse position(screen) before dragging.

  XKeyApp *app_;

  wxDECLARE_EVENT_TABLE();
};

class AboutDlg : public wxDialog {
 public:
  ~AboutDlg();
  AboutDlg(wxWindow*);

 private:
  void OnLeftUp(wxMouseEvent&);

  wxDECLARE_EVENT_TABLE();
};

#endif // _XKEY2FRAME_H_
