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
#if 0
  auto panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
      wxTRANSPARENT_WINDOW | wxTAB_TRAVERSAL | wxNO_BORDER);
  auto sbox2 = new wxBoxSizer(wxHORIZONTAL);
  auto sbox = new wxBoxSizer(wxVERTICAL);
  sbox2->Add(new wxPanel(panel), wxSizerFlags(1).Expand());
  sbox2->Add(sbox, wxSizerFlags(1).Bottom());
  panel->SetSizer(sbox2);
  sbox->Add(new wxButton(panel, ID_EXIT, wxT("E&xit")), 
      wxSizerFlags(1).Right().Border(wxALL, 8));
  sbox2->Add(new wxStaticText(panel, wxID_ANY, wxT("Warcraft III kbd&&mouse hook")), 
      wxSizerFlags(1).Bottom());
#endif
  //auto top_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
  //    wxTRANSPARENT_WINDOW | wxTAB_TRAVERSAL | wxNO_BORDER);
  //auto bottom_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
  //    wxTRANSPARENT_WINDOW | wxTAB_TRAVERSAL | wxNO_BORDER);
  auto bs = new wxBoxSizer(wxHORIZONTAL);
  auto bs2 = new wxBoxSizer(wxVERTICAL);
  SetSizer(bs);
  bs->Add(bs2, wxSizerFlags(1).Bottom());
  //bs->Add(top_panel, wxSizerFlags(1));
  //bs->Add(bottom_panel, wxSizerFlags(1));
  bs2->Add(new wxButton(this, ID_EXIT, wxT("E&xit")), 
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
