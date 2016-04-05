/*!
  \brief  
  xkey2 App Framework.

  \file 
  xkey2.cc

  \author 

  \date 
  2016/04/04

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
/*
  // the var on stack will make the app crash!
  wxPNGHandler png_handler;
  wxImage::AddHandler(&png_handler);
*/

  WarkeyDlg dlg;
  auto retcode = dlg.ShowModal();
  if (wxID_OK == retcode) {
  }

  //wxImage::RemoveHandler(png_handler.GetName());
  return false;
}

WarkeyDlg::~WarkeyDlg() { }

WarkeyDlg::WarkeyDlg() 
    // : wxDialog(nullptr, wxID_ANY, "Hello?") { 
    : wxDialog() {
  bkgnd_.LoadFile(wxT("bkgnd.bmp"), wxBITMAP_TYPE_BMP);
  {
    //wxBitmap win_shape("res/s.png", wxBITMAP_TYPE_PNG);
    wxBitmap win_shape("bkgnd_mask.bmp", wxBITMAP_TYPE_BMP);
    long dlgstyle(0);
    Create(nullptr, wxID_ANY, "Hei", wxDefaultPosition, 
        wxSize(win_shape.GetWidth(), win_shape.GetHeight()), dlgstyle);
    wxRegion win_region(win_shape, *wxWHITE);
    SetWindowStyle(GetWindowStyle() | wxFRAME_SHAPED);
    SetShape(win_region);
  }
  //wxImage img("res/w.png");
  //wxICON_DEFAULT_TYPE;
  //wxIcon ico("res/w.ico", wxBITMAP_TYPE_ICO); // wxBITMAP_TYPE_PNG);
/*
  wxBitmap bmp("res/w.png", wxBITMAP_TYPE_PNG);
  wxIcon ico;
  ico.CopyFromBitmap(bmp);
  SetIcon(ico);
*/
  //SetIcon(wxICON(MAKEINTRESOURCE(IDI_ICON1)));  // not works
  //SetIcon(wxICON(main_ico));
  //SetIcon(wxICON(IDI_ICON1)); // not works!!
/*
  wxIcon ico("res/w.png", wxBITMAP_TYPE_PNG);
  SetIcon(ico);
*/

  {
    //wxCursor a(wxT("a.cur"), wxBITMAP_TYPE_CUR);
    wxCursor a(wxT("b.ani"), wxBITMAP_TYPE_ANI);
    SetCursor(a);
  }

/*
  // This button would carray the hole area of the window!
  {
    auto pos = wxDefaultPosition;
    auto sz = wxDefaultSize;
    auto okay_btn = new wxButton(this, ID_EXIT, wxT("E&xit"), pos, sz);
  }
  {
    new wxButton(this, ID_EXIT, wxT("E&xit"), wxPoint(150, 200));
  }
*/

  { // create widgets on the panel
    auto panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
        wxTRANSPARENT_WINDOW | wxTAB_TRAVERSAL | wxNO_BORDER);
    auto vertbox = new wxBoxSizer(wxVERTICAL);
    auto horzbox1 = new wxBoxSizer(wxHORIZONTAL);
    auto horzbox2 = new wxBoxSizer(wxVERTICAL);
    horzbox1->Add(new wxPanel(panel, wxID_ANY));
    //vertbox->Add(horzbox1, 1, wxEXPAND);
    vertbox->Add(horzbox1, wxSizerFlags(1).Expand());
    //vertbox->Add(horzbox2, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 8);
    vertbox->Add(horzbox2, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 8));
    panel->SetSizer(vertbox);
    //wxPoint pos(8, 8);
    auto exit_btn = new wxButton(panel, ID_EXIT, wxT("E&xit"));
    exit_btn->SetFocus();
    horzbox2->Add(exit_btn);
    horzbox1->Add(new wxButton(panel, wxID_OK, wxT("Ok")),
        wxSizerFlags(1).Center());
    horzbox1->Add(new wxButton(panel, wxID_CANCEL, wxT("Cancel")), 
        wxSizerFlags(0).Center());
    horzbox1->Add(new wxButton(panel, wxID_YES, wxT("Yes")), 
        wxSizerFlags(1).Top().Border(wxTOP, 64));
    horzbox1->Add(new wxButton(panel, wxID_NO, wxT("No")), 
        wxSizerFlags(1).Bottom());
  }

/*
  {
    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );    
    // create text ctrl with minimal size 100x60 that is horizontally and    
    // vertically stretchable with a border width of 10    
    topsizer->Add(        new wxTextCtrl( this, -1, "My text.", wxDefaultPosition, wxSize(100,60), wxTE_MULTILINE), 
        wxSizerFlags(1).Align(wxALIGN_TOP).Expand().Border(wxALL, 10));    
    wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );    
    //create two buttons that are horizontally unstretchable,    
    // with an all-around border with a width of 10 and implicit top alignment    
    button_sizer->Add(        new wxButton( this, wxID_OK, "OK" ),        
        wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 10));    
    button_sizer->Add(        new wxButton( this, wxID_CANCEL, "Cancel" ),        
        wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 10));    
    //create a sizer with no border and centered horizontally    
    topsizer->Add(        button_sizer,        wxSizerFlags(0).Center() );    
    SetSizerAndFit(topsizer); // use the sizer for layout and set size and hints
  }
*/

/*
  // doesn't work....
  { // another way to creates widgets...
    auto btn_sizer = new wxBoxSizer(wxVERTICAL);
    btn_sizer->Add(new wxButton(this, ID_EXIT, wxT("E&xit")), 
        wxSizerFlags(0).Center().Border(wxALL, 8));
    SetSizerAndFit(btn_sizer);
  }
*/

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
