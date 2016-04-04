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
  //wxBitmap win_shape("res/s.png", wxBITMAP_TYPE_PNG);
  wxBitmap win_shape("p09_shape24.bmp", wxBITMAP_TYPE_BMP);
  {
    long dlgstyle(0);
    Create(nullptr, wxID_ANY, "Hei", wxDefaultPosition, 
        wxSize(win_shape.GetWidth(), win_shape.GetHeight()), dlgstyle);
  }
  wxRegion win_region(win_shape, *wxWHITE);
  SetWindowStyle(GetWindowStyle() | wxFRAME_SHAPED);
  SetShape(win_region);

  //wxImage img("res/w.png");
  wxICON_DEFAULT_TYPE;
  //wxIcon ico("res/w.ico", wxBITMAP_TYPE_ICO); // wxBITMAP_TYPE_PNG);
/*
  wxBitmap bmp("res/w.png", wxBITMAP_TYPE_PNG);
  wxIcon ico;
  ico.CopyFromBitmap(bmp);
  SetIcon(ico);
*/
  //SetIcon(wxICON(MAKEINTRESOURCE(IDI_ICON1)));  // not works
  SetIcon(wxICON(main_ico));
  //SetIcon(wxICON(IDI_ICON1)); // not works!!
/*
  wxIcon ico("res/w.png", wxBITMAP_TYPE_PNG);
  SetIcon(ico);
*/
}

wxBEGIN_EVENT_TABLE(WarkeyDlg, wxDialog)
  EVT_KEY_UP(OnKeyUp)
wxEND_EVENT_TABLE()

void WarkeyDlg::OnKeyUp(wxKeyEvent &keyevt) {
  auto keycode = keyevt.GetKeyCode();
  if (WXK_ESCAPE == keycode) {
    Close();
  }
}

wxIMPLEMENT_APP(XKeyApp);
