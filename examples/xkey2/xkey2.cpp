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
