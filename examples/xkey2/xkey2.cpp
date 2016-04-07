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
/*
  // create widgets on the panel
  auto root_szr = new wxBoxSizer(wxHORIZONTAL);
  auto left_szr = new wxBoxSizer(wxVERTICAL);
  auto exit_szr = new wxBoxSizer(wxVERTICAL);
  auto numpad_szr1 = new wxBoxSizer(wxVERTICAL);
  auto numpad_szr2 = new wxBoxSizer(wxVERTICAL);
  SetSizer(root_szr);
  left_szr->Add(new wxPanel(this));
  root_szr->Add(left_szr, wxSizerFlags(1));
  wxSizerFlags np_szr_flags(wxSizerFlags(1).Center().Border(wxALL, 8));
  root_szr->Add(numpad_szr1, wxSizerFlags(1).Center());
  //numpad_szr1->Add(new wxTextCtrl(this, wxID_ANY, wxT("3")), np_szr_flags);
  //numpad_szr1->Add(new wxTextCtrl(this, wxID_ANY, wxT("5")), np_szr_flags);
  //numpad_szr1->Add(new wxTextCtrl(this, wxID_ANY, wxT("7")), np_szr_flags);
  for (int i = 1; i < 6; i += 2) {
    auto hszr = new wxBoxSizer(wxHORIZONTAL);
    numpad_szr1->Add(hszr, wxSizerFlags(1));
    wxString label;
    label << "NumPad " << i;
    hszr->Add(new wxStaticText(this, wxID_ANY, label), 
        wxSizerFlags(0).Center().Border(wxLEFT | wxTOP | wxBOTTOM, 8));
    hszr->Add(new wxTextCtrl(this, wxID_ANY, wxT("0")), 
        wxSizerFlags(0).Center().Border(wxALL, 8));
  }
  root_szr->Add(numpad_szr2, wxSizerFlags(1).Center());
  //numpad_szr2->Add(new wxTextCtrl(this, wxID_ANY, wxT("2")), np_szr_flags);
  //numpad_szr2->Add(new wxTextCtrl(this, wxID_ANY, wxT("4")), np_szr_flags);
  //numpad_szr2->Add(new wxTextCtrl(this, wxID_ANY, wxT("6")), np_szr_flags);
  for (int i = 2; i < 7; i += 2) {
    auto hszr = new wxBoxSizer(wxHORIZONTAL);
    numpad_szr2->Add(hszr, wxSizerFlags(1));
    wxString label;
    label << "NumPad " << i;
    hszr->Add(new wxStaticText(this, wxID_ANY, label), 
        wxSizerFlags(0).Center().Border(wxLEFT | wxTOP | wxBOTTOM, 8));
    hszr->Add(new wxTextCtrl(this, wxID_ANY, wxT("0")), 
        wxSizerFlags(0).Center().Border(wxALL, 8));
  }
  root_szr->Add(exit_szr, wxSizerFlags(1).Bottom());
  exit_szr->Add(new wxButton(this, ID_EXIT, wxT("E&xit")), 
      wxSizerFlags(0).Right().Border(wxALL, 8));
*/
  //auto root_szr = new wxGridBagSizer(8, 8);
  auto root_szr = new wxBoxSizer(wxVERTICAL);
  auto top_szr = new wxBoxSizer(wxHORIZONTAL);
  top_szr->Add(new wxPanel(this), wxSizerFlags(1));
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
