/*!
  \brief  
  The first example of using wxWidgets library.

  \file 
  wx01hi.cc

  \date 
  2016/03/25
*/

#if 1

//#define __WXMSW__
#define WXUSINGDLL
#if defined(_MSC_VER) && (_MSC_VER < 1400)
#define wxUSE_RC_MANIFEST=1 and WX_CPU_X86
#endif

//#include <windows.h>
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

enum {
  ID_HELLO = 1,
};

class MyFrame : public wxFrame {
 public:
  MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size) 
      : wxFrame(nullptr, wxID_ANY, title, pos, size) { 
    auto menu = new wxMenu;
    assert(menu);
    menu->Append(ID_HELLO, "&Hello...\tCtrl-H", "Help string shown in status"
        " bar for this menu item");
    menu->AppendSeparator();
    menu->Append(wxID_EXIT);

    auto help_menu = new wxMenu;
    assert(help_menu);
    help_menu->Append(wxID_ABOUT);

    auto menu_bar = new wxMenuBar;
    assert(menu_bar);
    menu_bar->Append(menu, "&File");
    menu_bar->Append(help_menu, "&Help");
    SetMenuBar(menu_bar);

    CreateStatusBar();
    SetStatusText("Welcome, welcome, welcome to the fifth November of December");
  }

 private:
  void OnHello(wxCommandEvent &evt) { 
    wxLogMessage("Hello from wxWidgets!");
  }

  void OnExit(wxCommandEvent &evt) { 
    Close(true);
  }

  void OnAbout(wxCommandEvent &evt) { 
    wxMessageBox("Hello world bala bala bala...", "About Hello", 
        wxOK | wxICON_INFORMATION);
  }

  wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(ID_HELLO, MyFrame::OnHello)
  EVT_MENU(wxID_EXIT, MyFrame::OnExit)
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()



class MyApp : public wxApp {
 public:
  virtual bool OnInit() {
    wxApp::OnInit();
    auto frame = new MyFrame("Hello", wxPoint(50, 50), wxSize(450, 340));
    assert(frame);
    frame->Show(true);
    return true;
  }
};

wxIMPLEMENT_APP(MyApp);

#else

// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP    
#include <wx/wx.h>
#endif
class MyApp: public wxApp{
public:    virtual bool OnInit();
};
class MyFrame: public wxFrame{
public:  
  MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:    
  void OnHello(wxCommandEvent& event);  
  void OnExit(wxCommandEvent& event);  
  void OnAbout(wxCommandEvent& event);  
  wxDECLARE_EVENT_TABLE();
};
enum{    ID_Hello = 1};
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)  
  EVT_MENU(ID_Hello,   MyFrame::OnHello)    
  EVT_MENU(wxID_EXIT,  MyFrame::OnExit)    
  EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit(){  
  MyFrame *frame = new MyFrame( "Hello World", wxPoint(50, 50), wxSize(450, 340) );    
  frame->Show( true );    
  return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)    
    : wxFrame(NULL, wxID_ANY, title, pos, size){    
  wxMenu *menuFile = new wxMenu;    
  menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",                   
      "Help string shown in status bar for this menu item");    
  menuFile->AppendSeparator();    
  menuFile->Append(wxID_EXIT);    
  wxMenu *menuHelp = new wxMenu;  
  menuHelp->Append(wxID_ABOUT);    
  wxMenuBar *menuBar = new wxMenuBar;    
  menuBar->Append( menuFile, "&File" );    
  menuBar->Append( menuHelp, "&Help" );    
  SetMenuBar( menuBar );    
  CreateStatusBar();    
  SetStatusText( "Welcome to wxWidgets!" );
}

void MyFrame::OnExit(wxCommandEvent& event){    Close( true );}

void MyFrame::OnAbout(wxCommandEvent& event){    
  wxMessageBox( "This is a wxWidgets' Hello world sample",              
      "About Hello World", wxOK | wxICON_INFORMATION );
}

void MyFrame::OnHello(wxCommandEvent& event){    wxLogMessage("Hello world from wxWidgets!");}

#endif
