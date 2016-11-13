/*!
  UI frame of the xkey.2.

*/
#include "xkey2.h"
#include "xkey2frame.h"

enum CtrlId {
  ID_EXIT,
  ID_MODENABLED,
};

WarkeyDlg::~WarkeyDlg() { }

WarkeyDlg::WarkeyDlg(XKeyApp *app) : wxDialog(), app_(app) {
  assert(app && "null app pointee!");

  bkgnd_.LoadFile(wxT("bkgnd"), wxBITMAP_TYPE_BMP_RESOURCE);

  wxBitmap win_shape("bkgnd_mask", wxBITMAP_TYPE_BMP_RESOURCE);
  long dlgstyle(0);
  Create(nullptr, wxID_ANY, "WarXKey v1.0 Alpha", wxDefaultPosition, 
      wxSize(win_shape.GetWidth(), win_shape.GetHeight()), dlgstyle);

  wxRegion win_region(win_shape, *wxBLACK);
  SetWindowStyle(GetWindowStyle() | wxFRAME_SHAPED);
  SetShape(win_region);

  wxCursor cursor(wxT("main_cursor"), wxBITMAP_TYPE_CUR_RESOURCE);
  SetCursor(cursor);

  auto root_szr = new wxBoxSizer(wxVERTICAL);
  auto top_szr = new wxBoxSizer(wxHORIZONTAL);
  top_szr->Add(new wxPanel(this), wxSizerFlags(1));
  auto t_szr0 = new wxBoxSizer(wxVERTICAL);
  t_szr0->Add(new wxCheckBox(this, ID_MODENABLED, wxT("Enabled")), 
      wxSizerFlags(1).Border(wxALL, 8));
  t_szr0->Add(new wxCheckBox(this, wxID_ANY, wxT("Enable SP Function0")),
      wxSizerFlags(1).Centre().Border(wxALL, 8));
  top_szr->Add(t_szr0, wxSizerFlags(1).Bottom().Border(wxBOTTOM, 8));
  auto entip = new wxStaticText(this, wxID_ANY, 
      wxT("NOTE: Before you try to make some changes of the key-maps,")
      wxT(" disable it at first!"), 
      wxDefaultPosition, wxDefaultSize);
  entip->Wrap(140);
  //wxString notrans_reason;
  //if (!entip->IsTransparentBackgroundSupported(&notrans_reason)) {
  //  wxMessageBox(notrans_reason);
  //} else {
  //  entip->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
  //}
  entip->SetForegroundColour(*wxGREEN);
  top_szr->Add(entip, wxSizerFlags(1).Bottom().Border(wxLEFT, 8));
  top_szr->Add(new wxPanel(this), wxSizerFlags(1));
  root_szr->Add(top_szr, wxSizerFlags(1));
  auto num_szr = new wxGridBagSizer(8, 8);
  // 3 x 2
  wxString nptitles[2][3] = {
    {wxT("4"), wxT("5"), wxT("6")}, 
    {wxT("3"), wxT("8"), wxT("7")}, 
  };
  for (int i = 0, endi = 2; i < endi; ++i) {
    for (int j = 0, endj = 3; j < endj; ++j) {
      auto ctrl = new wxTextCtrl(this, wxID_ANY, nptitles[i][j]);
      num_szr->Add(ctrl, wxGBPosition(j+1, i+1));
    }
  }
  //num_szr->Add(new wxPanel(this), wxGBPosition(4, 5));
  root_szr->Add(num_szr, wxSizerFlags(0).Center().Border(wxBOTTOM | wxTOP, 8));
  auto sk_szr = new wxGridBagSizer(8, 8);
  // 3 x 4
  wxString sktitle[3][4] = {
    {wxT("M"), wxT("S"), wxT("H"), wxT("A")}, 
    {wxT("P"), wxT("G"), wxT("D"), wxT("F")}, 
    {wxT("Q"), wxT("W"), wxT("E"), wxT("R")}, 
  };
  for (int i = 0, endi = 3; i < endi; ++i) {
    for (int j = 0, endj = 4; j < endj; ++j) {
      auto ctrl = new wxTextCtrl(this, wxID_ANY, sktitle[i][j]);
      sk_szr->Add(ctrl, wxGBPosition(i+1, j));
    }
  }
  //sk_szr->Add(new wxPanel(this), wxGBPosition(3, 0));
  //sk_szr->Add(new wxPanel(this), wxGBPosition(3, 5));
  root_szr->Add(sk_szr, wxSizerFlags(0).Center().Border(wxBOTTOM | wxTOP, 8));
  auto bottom_szr = new wxBoxSizer(wxHORIZONTAL);
  bottom_szr->Add(new wxButton(this, ID_EXIT, wxT("E&xit")), 
      wxSizerFlags(0).Border(wxALL, 8));
  bottom_szr->Add(new wxButton(this, wxID_ABOUT, wxT("&About")), 
      wxSizerFlags(0).Border(wxALL, 8));
  root_szr->Add(bottom_szr, wxSizerFlags(0).Border(wxTOP | wxBOTTOM, 16));
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
  EVT_CHECKBOX(ID_MODENABLED, OnModEnabled)
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

void WarkeyDlg::OnModEnabled(wxCommandEvent& evt) {
  // Activated, Deactivated
  evt.IsChecked() ? app_->enableKeyMod() : app_->disableKeyMod();
}

//////////////////////////////////////////////////

AboutDlg::~AboutDlg() { }

AboutDlg::AboutDlg(wxWindow *parent) 
    : wxDialog(parent, wxID_ABOUT, wxT(""), wxDefaultPosition, wxDefaultSize, 
          wxPOPUP_WINDOW) { 
  wxCursor cursor(wxT("quest_cursor"), wxBITMAP_TYPE_CUR_RESOURCE);
  SetCursor(cursor);
 
  auto vbox = new wxBoxSizer(wxHORIZONTAL);
  auto hbox = new wxBoxSizer(wxVERTICAL);
  hbox->Add(new wxStaticText(this, wxID_ANY, 
      wxT("Copyright 2016, MiRusY. Y's Personal Toolkit")
      wxT("\n\nThis is an Alpha version.") 
      wxT("\nTo get the latest source codes, visit github.com, project: miruscp")
      wxT("\nAny problem, contact me!")
      wxT("\n(Click any-where else to CLOSE)")), 
      wxSizerFlags(1).Centre().Border(wxALL, 48));
  vbox->Add(hbox, wxSizerFlags(1).Centre());
  SetSizer(vbox);
  Fit();
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

