/*!
  dll entry wrap this module.

*/
#include "kmdllentry.h"
#include "warkeymod.h"

DllEntryApp::~DllEntryApp() {
  wkmod.reset();
}

DllEntryApp::DllEntryApp(HANDLE h) : handle(h), wkmod(nullptr) {
  entries[DLL_PROCESS_ATTACH] = &DllEntryApp::ProcessAttach;
  entries[DLL_PROCESS_DETACH] = &DllEntryApp::ProcessDetach;
  entries[DLL_THREAD_ATTACH] = &DllEntryApp::ThreadAttach;
  entries[DLL_THREAD_DETACH] = &DllEntryApp::ThreadDetach;
}

DllEntryApp::DllEntryApp() : DllEntryApp(nullptr) {
}

xk::KbdModInterface* DllEntryApp::CreateModifierOnce(DWORD thread_id) {
  if (!wkmod) {
    wkmod.reset(new WarkeyModifier(thread_id));
  }
  return wkmod.get();
}

int DllEntryApp::ProcessAttach(LPVOID) {
  return 1;
}

int DllEntryApp::ProcessDetach(LPVOID) {
  return 1;
}

int DllEntryApp::ThreadAttach(LPVOID) {
  return 1;
}

int DllEntryApp::ThreadDetach(LPVOID) {
  return 1;
}

DllEntryApp g_app;

////////////////////////////////////////////////////////////////////////////////

int WINAPI DllMain(HANDLE dllhandle, DWORD reason, LPVOID reserved) {
  g_app.set_handle_once(dllhandle);
  return g_app.run(reason, reserved);
}
