/*!
  \brief  
  xkey2 App Framework.

  \file 
  xkey2.cc

  \author 

  \remarks  

*/
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/gbsizer.h"
#include "wres/resource.h"

enum CtrlId {
  ID_EXIT,
};

class XKeyApp : public wxApp {
 public:
  bool OnInit();
};

class WarkeyDlg : public wxDialog {
 public:
  ~WarkeyDlg();
  WarkeyDlg();

 private:
  void OnKeyUp(wxKeyEvent&);
  void OnExit(wxCommandEvent&);
  void OnPaint(wxPaintEvent&);

  wxBitmap bkgnd_;

  wxDECLARE_EVENT_TABLE();
};

////////////////////////////////////////////////////////////////////////////////

bool XKeyApp::OnInit() {
  if (!wxApp::OnInit()) {
    return false;
  }

  // So the wxwidgets lib does the delete job... It's may not be a good idea!
  wxImage::AddHandler(new wxPNGHandler());

  WarkeyDlg dlg;
  auto retcode = dlg.ShowModal();
  if (wxID_OK == retcode) {
  }

  return false;
}

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
  top_szr->Add(new wxCheckBox(this, wxID_ANY, wxT("Enabled")), 
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

wxIMPLEMENT_APP(XKeyApp);
