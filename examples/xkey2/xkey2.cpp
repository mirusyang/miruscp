/*!
  \brief  
  xkey2 App Framework.

  \file 
  xkey2.cc

  \author 

  \remarks  

*/
//#include <string>
#include <future>
#include <mutex>
#include <chrono>
#include "xkit/dlloader.h"
#include "xkit/processenumerator.h"
#include "xkey2.h"
#include "xkey2frame.h"
#include "wres/resource.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

bool XKeyApp::OnInit() {
  if (!wxApp::OnInit()) {
    return false;
  }

  // load the library here
  // x-kbd-mod library.
  xk::DllLoader kmodlib;
  {
    xkGetModifierApi xkGetModifier(nullptr);
    bool modloaded(false);
#ifdef XK_DEBUG
    modloaded = kmodlib(wxT("xkbdutil_d.dll"));
#else
    modloaded = kmodlib(wxT("xkbdutil.dll"));
#endif
    if (!modloaded) {
      wxString tip;
      tip << wxT("Fails to load xkbdutil[_d].dll library!\n")
          << wxT(" GetLastError: ") << GetLastError();
      wxMessageBox(tip, wxT("Caution!!"), wxICON_WARNING | wxOK);
      return false;
    }
    xkGetModifier = kmodlib.get_func<xkGetModifierApi>("xkGetModifier");
    if (!xkGetModifier) {
      wxMessageBox(wxT("Fails to get the API address!"));
      return false;
    }
    // Creates the hook(s) now.
    kbdmod_ = xkGetModifier(0);
    try {
      auto initialised = kbdmod_->Initialise();
      if (!initialised) {
        wxMessageBox(wxT("Fails to initialise the kbd-hook! App may not works well!"));
      }
    } catch (exception &e) {
      wxMessageBox(e.what());
    } catch (...) {
      wxMessageBox(wxT("Something UN-KNOWN!!!"));
    }
  }
  auto &kbd_mod(kbdmod_);
  // App listener.
  atomic<bool> listening(true);
  auto app_listener = async([&kbd_mod, &listening]() {
    bool war3running(false);
    while (listening) {
      xk::ProcessEnumerator pe;
      auto war3pid = pe.fuzzy_lookup(_T("war3.exe"));
      if (0 != war3pid) {
        if (!war3running) {
          // The app is running now. Then we can enable the modification(s)!
          // Give a sign about that.
          MessageBeep(MB_ICONINFORMATION);
          war3running = true;
        }
      } else {
        if (war3running) {
          MessageBeep(MB_ICONSTOP);
          war3running = false;
        }
      }
      this_thread::yield();
    }
  });

  // So the wxwidgets lib does the delete job... It's may not be a good idea!
  wxImage::AddHandler(new wxPNGHandler());

  WarkeyDlg dlg(this);
  dlg.ShowModal();

  listening = false;
  app_listener.get();

  return false;
}

//////////////////////////////////////////////////

wxIMPLEMENT_APP(XKeyApp);
