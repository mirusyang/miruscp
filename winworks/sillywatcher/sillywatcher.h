/*!
  \brief  
  Implementation for watching folder changes.

  Depends on Win32 APIs.

  \file 
  sillywatcher.h

  \author 
  Y***n

  \date 
  2016/07/05

  \version  
  1.0

  \copyright  
  GPL

  \remarks  
  -#  2016/07/05  Created by Y***n
*/
#ifndef _SILLYWATCHER_H_
#define _SILLYWATCHER_H_

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <memory>
#include <functional>
#include <string>
#include <vector>

#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring string;
#else
typedef std::string string;
#endif

struct ChangeNotif {
  typedef std::pair<string, size_t> PathInfo;
  typedef std::vector<PathInfo> InfoSet;

  string folder;
  InfoSet changes;
};

/*!
  \brief  
  Implementation of the file watcher (simplified version).
*/
class SillyWatcher {
 public:
  /*!
    The callback type as user can provided.

    \note 
    You should make sure that it's lifetime is longer than the watcher.
  */
  typedef std::function<void(const ChangeNotif&)> Callback;

  ~SillyWatcher();

  /*!
    \brief  
    Register the folder specified by \c folderpath.

    \param          cb
    The callback user provided. Multiple folders can be notified by one callback.
  */
  bool AddFolder(const TCHAR *folderpath, Callback *cb);

  /*!
    \brief  
    Remove the folder now is watching.
  */
  void RemoveFolder(const TCHAR *folderpath);

  SillyWatcher();

 private:
  struct _Attr;
  std::unique_ptr<_Attr> att_;
};

#endif // _SILLYWATCHER_H_
