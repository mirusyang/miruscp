/*!
  \brief  
  xkey2 App Framework.

  \file 
  xkey2.cc

  \author 

  \remarks  

*/
//#include <string>
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/gbsizer.h"
#include "wres/resource.h"
#include "xkit/dlloader.h"
#include "xkit/processenumerator.h"
#include "../xkbdutil/xkbdutil.h"

enum CtrlId {
  ID_EXIT,
  ID_ENABLED,
};

class XKeyApp : public wxApp {
 public:
  bool OnInit();

 private:
  xk::DllLoader kmod_;
};

class WarkeyDlg : public wxDialog {
 public:
  ~WarkeyDlg();
  WarkeyDlg();

 private:
  void OnKeyUp(wxKeyEvent&);
  void OnExit(wxCommandEvent&);
  void OnPaint(wxPaintEvent&);
  void OnMotion(wxMouseEvent&);
  void OnLeftDown(wxMouseEvent&);
  void OnAbout(wxCommandEvent&);
  void OnEnabled(wxCommandEvent&);

  wxBitmap bkgnd_;
  wxPoint orig_wnd_pos_;      //!< original window position(screen) before dragging.
  wxPoint orig_mouse_pos_;    //!< original mouse position(screen) before dragging.

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

////////////////////////////////////////////////////////////////////////////////

#pragma region XKeyApp_Impl_BEGIN

bool XKeyApp::OnInit() {
  using std::exception;

  if (!wxApp::OnInit()) {
    return false;
  }

  // load the library here
  xk::KbdModInterface *kbd_mod(nullptr);
  bool modloaded(false);
#ifdef XK_DEBUG
  modloaded = kmod_(wxT("xkbdutil_d.dll"));
#else
  modloaded = kmod_(wxT("xkbdutil.dll"));
#endif
  if (!modloaded) {
    wxString tip;
    //tip.Format(wxT("Fails to load xkbdutil[_d].dll library!\n")
    //    wxT(" GetLastError: %d"), GetLastError());
    tip << wxT("Fails to load xkbdutil[_d].dll library!\n")
        << wxT(" GetLastError: ") << GetLastError();
    wxMessageBox(tip, wxT("Caution!!"), wxICON_WARNING | wxOK);
  } else {
    auto f = kmod_.get_func<xkGetModifierApi>("xkGetModifier");
    if (!f) {
      wxMessageBox(wxT("Fails to get the API address!"));
    } else {
      xk::ProcessEnumerator pe;
      auto war3pid = pe.fuzzy_lookup(_T("war3.exe"));
      if (0 != war3pid) {
        MessageBeep(MB_ICONINFORMATION);
        auto thread_id = GetWindowThreadProcessId(xk::WndEnumerator::Main(war3pid), nullptr);
        try {
        thread_id = 0;
          kbd_mod = f(thread_id);
          auto initialised = kbd_mod->Initialise();
          if (!initialised) {
            wxMessageBox(wxT("Fails to initialise the kbd-hook! App may not works well!"));
          }
        } catch (exception &e) {
          wxMessageBox(e.what());
        } catch (...) {
        }
      }
    }
  }

  // So the wxwidgets lib does the delete job... It's may not be a good idea!
  wxImage::AddHandler(new wxPNGHandler());

  WarkeyDlg dlg;
  auto retcode = dlg.ShowModal();
  if (wxID_OK == retcode) {
  }

  return false;
}

#pragma endregion XKeyApp_Impl_END

//////////////////////////////////////////////////

#pragma region WarkeyDlg_Impl_Begin

WarkeyDlg::~WarkeyDlg() { }

WarkeyDlg::WarkeyDlg() : wxDialog() {
  bkgnd_.LoadFile(wxT("res/bkgnd.bmp"), wxBITMAP_TYPE_BMP);

  wxBitmap win_shape("res/bkgnd_mask.bmp", wxBITMAP_TYPE_BMP);
  long dlgstyle(0);
  Create(nullptr, wxID_ANY, "Hei", wxDefaultPosition, 
      wxSize(win_shape.GetWidth(), win_shape.GetHeight()), dlgstyle);
  wxRegion win_region(win_shape, *wxWHITE);
  SetWindowStyle(GetWindowStyle() | wxFRAME_SHAPED);
  SetShape(win_region);

  wxCursor cursor(wxT("res/b.ani"), wxBITMAP_TYPE_ANI);
  SetCursor(cursor);

  auto root_szr = new wxBoxSizer(wxVERTICAL);
  auto top_szr = new wxBoxSizer(wxHORIZONTAL);
  //top_szr->Add(new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
  //    wxTRANSPARENT), wxSizerFlags(1).Expand());
  top_szr->Add(new wxPanel(this), wxSizerFlags(1));
  //auto top_part0 = new wxBoxSizer(wxVERTICAL);
  top_szr->Add(new wxCheckBox(this, ID_ENABLED, wxT("Enabled")), 
      wxSizerFlags(1).Center().Border(wxBOTTOM | wxLEFT, 16));
  //top_szr->Add(top_part0);
  root_szr->Add(top_szr, wxSizerFlags(1));
  auto num_szr = new wxGridBagSizer(8, 8);
  num_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("1")), wxGBPosition(1, 2));
  num_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("2")), wxGBPosition(1, 3));
  num_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("3")), wxGBPosition(2, 2));
  num_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("4")), wxGBPosition(2, 3));
  num_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("5")), wxGBPosition(3, 2));
  num_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("6")), wxGBPosition(3, 3));
  root_szr->Add(num_szr, wxSizerFlags(0).Center());
  //auto skroot_szr = new wxBoxSizer(wxHORIZONTAL);
  //skroot_szr->Add(new wxPanel(this), wxSizerFlags(1));
  auto sk_szr = new wxGridBagSizer(8, 8);
  sk_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("W")), wxGBPosition(1, 1));
  sk_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("E")), wxGBPosition(1, 2));
  sk_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("R")), wxGBPosition(1, 3));
  sk_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("A")), wxGBPosition(2, 0));
  sk_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("S")), wxGBPosition(2, 1));
  sk_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("D")), wxGBPosition(2, 2));
  sk_szr->Add(new wxTextCtrl(this, wxID_ANY, wxT("F")), wxGBPosition(2, 3));
      //wxDefaultSpan, wxRIGHT, 8);
  //sk_szr->Add(new wxPanel(this), wxGBPosition(2, 5));
  //skroot_szr->Add(sk_szr, wxSizerFlags(1));
  //skroot_szr->Add(new wxPanel(this), wxSizerFlags(1));
  //sk_szr->Add(new wxBoxSizer(wxVERTICAL));
  root_szr->Add(sk_szr, wxSizerFlags(0).Center().Border(wxBOTTOM, 16));
  auto bottom_szr = new wxBoxSizer(wxHORIZONTAL);
  bottom_szr->Add(new wxButton(this, ID_EXIT, wxT("E&xit")), 
      wxSizerFlags(0).Border(wxALL, 8));
  bottom_szr->Add(new wxButton(this, wxID_ABOUT, wxT("&About")), 
      wxSizerFlags(0).Border(wxALL, 8));
  root_szr->Add(bottom_szr, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 32));
  SetSizer(root_szr);
  //root_szr->Fit(this);

  Centre();
}

wxBEGIN_EVENT_TABLE(WarkeyDlg, wxDialog)
  EVT_KEY_UP(OnKeyUp)
  EVT_BUTTON(ID_EXIT, OnExit)
  EVT_PAINT(OnPaint)
  EVT_MOTION(OnMotion)
  EVT_LEFT_DOWN(OnLeftDown)
  EVT_BUTTON(wxID_ABOUT, OnAbout)
  EVT_CHECKBOX(ID_ENABLED, OnEnabled)
wxEND_EVENT_TABLE()

void WarkeyDlg::OnKeyUp(wxKeyEvent &keyevt) {
  auto keycode = keyevt.GetKeyCode();
  if (WXK_ESCAPE == keycode) {
    Close();
  }
}

void WarkeyDlg::OnExit(wxCommandEvent &evt) {
  Close();
}

void WarkeyDlg::OnPaint(wxPaintEvent &evt) {
  wxPaintDC dc(this);
  dc.DrawBitmap(bkgnd_, 0, 0);
}

void WarkeyDlg::OnMotion(wxMouseEvent &evt) {
  if (evt.LeftIsDown() && evt.Dragging()) {
    wxPoint target_pos(evt.GetPosition());
    auto delta = target_pos - orig_mouse_pos_;
    orig_wnd_pos_ += delta;
    Move(orig_wnd_pos_);
  }
}

void WarkeyDlg::OnLeftDown(wxMouseEvent &evt) {
  orig_wnd_pos_ = GetScreenPosition();
  orig_mouse_pos_ = evt.GetPosition();
}

void WarkeyDlg::OnAbout(wxCommandEvent&) {
  AboutDlg dlg(this);
  dlg.ShowModal();
}

void WarkeyDlg::OnEnabled(wxCommandEvent& evt) {
  // TODO: OnEanbled, do the function switch.
  if (evt.IsChecked()) {
    wxMessageBox(wxT("Ouch! You hit me!!"));
  }
}

#pragma endregion WarkeyDlg_Impl_END

//////////////////////////////////////////////////

#pragma region AboutDlg_Begin

AboutDlg::~AboutDlg() { }

AboutDlg::AboutDlg(wxWindow *parent) 
    : wxDialog(parent, wxID_ABOUT, wxT(""), wxDefaultPosition, wxDefaultSize, 
          wxPOPUP_WINDOW) { 
  auto vbox = new wxBoxSizer(wxHORIZONTAL);
  auto hbox = new wxBoxSizer(wxVERTICAL);
  hbox->Add(new wxStaticText(this, wxID_ANY, wxT("Copyright 2016, MiRusY. Y's Personal Toolkit")), 
      wxSizerFlags(1).Centre().Border(wxALL, 8));
  vbox->Add(hbox, wxSizerFlags(1).Centre());
  SetSizer(vbox);
  Centre();
}

wxBEGIN_EVENT_TABLE(AboutDlg, wxDialog)
  EVT_LEFT_UP(OnLeftUp)
wxEND_EVENT_TABLE()

void AboutDlg::OnLeftUp(wxMouseEvent& evt) {
  //evt.Skip(false);
  //evt.StopPropagation();
  Close();
}

#pragma endregion AboutDlg_END

wxIMPLEMENT_APP(XKeyApp);
