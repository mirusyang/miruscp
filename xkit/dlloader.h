/*!
  \brief  
  Dynamic Library Loader.

  \file 
  dlloader.h

  \author 

  \date 
  2016/04/14

  \version  
  1.0

  \copyright  
  GPL

  \remarks  

*/
#ifndef _DLLOADER_H_
#define _DLLOADER_H_

#include "winutilities.h"
#include <string>

XK_NAMESPACE_BEGIN

/*!
  \brief  
  Assist class for loading \.dll.
*/
struct DllLoader {
//#if defined(XK_UNICODE)
//  typedef std::wstring string;
//#else
//  typedef std::string string;
//#endif
  //typedef wxString string;

  ~DllLoader() {
    freelib();
  }

  DllLoader() : inst(nullptr) {
  }

  DllLoader(const String &filepath) : DllLoader() {
    operator()(filepath);
  }

  void freelib() {
    if (inst) {
      FreeLibrary(inst);
    }
  }

  bool operator()(const String &filepath) {
    inst = (HMODULE)LoadLibrary(filepath.c_str());
    return *this;
  }

  operator bool() const {
    return nullptr != inst;
  }

  template <class Func> Func get_func(const std::string &name) const {
    if (!*this) {
      return nullptr;
    }
    return (Func)GetProcAddress(inst, name.c_str());
  }

 private:
  HMODULE inst;       //!< handle of the library loaded.
};

XK_NAMESPACE_END

#endif 
