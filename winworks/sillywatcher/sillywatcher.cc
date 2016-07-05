/*!

  \brief  
  silly watcher sample.

  \file 
  sillywatcher.cc

  \author 
  Y***n

  \date 
  2016/07/05

  \remarks  
  -#  2016/07/05  created by Y***n
*/
#include <cassert>
#include "sillywatcher.h"
#include "threadtemplate.h"

#ifndef MAYBE_USED
# define MAYBE_USED(var) (void)var
#endif

class Watching : public CrtThread<Watching> {
  enum : size_t {
    BUFFER_SIZE = sizeof(FILE_NOTIFY_INFORMATION) * 1000,
  };

 public:
  ~Watching() {
  }

  void on_entry() {
    auto fh = CreateFile(folder_.c_str(), GENERIC_READ|FILE_LIST_DIRECTORY, 
        FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (INVALID_HANDLE_VALUE == fh || !fh) {
      return;
    }
    while (running()) {
      DWORD bytes_ret(0);
      auto ran_okay = ReadDirectoryChangesW(fh, buffer_.get(), BUFFER_SIZE, 
          TRUE, FILE_NOTIFY_CHANGE_FILE_NAME, &bytes_ret, nullptr, nullptr);
      if (ran_okay) {
        // Notify the user to processing.
        auto base = buffer_.get();
        auto base_end = base + (BUFFER_SIZE > bytes_ret ? bytes_ret : BUFFER_SIZE);
        auto info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(base);
        while (base < base_end && info->NextEntryOffset != 0) {
          info->FileName;
          info->FileNameLength;
          info->Action;
          base += info->NextEntryOffset;
          info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(base);
        }
      }
    }
    CloseHandle(fh);
  }

  Watching(SillyWatcher &watcher): watcher_(watcher), buffer_(new uint8_t[BUFFER_SIZE]) {
    assert(buffer_);
  }

 private:
  typedef std::unique_ptr<uint8_t[]> ByteBuffer;

  SillyWatcher &watcher_;
  string folder_;
  ByteBuffer buffer_;
};

struct SillyWatcher::_Attr {

  ~_Attr() {
  }

  _Attr() {
  }
};

SillyWatcher::~SillyWatcher() {
}

bool SillyWatcher::AddFolder(const TCHAR *path, Callback *cb) {
  return false;
}

void SillyWatcher::RemoveFolder(const TCHAR *path) {
}

SillyWatcher::SillyWatcher() : att_(new _Attr) {
  assert(att_);
}

////////////////////////////////////////////////////////////////////////////////

#include <iostream>

int main(int argc, char **argv) {
  MAYBE_USED(argc);
  MAYBE_USED(argv);

  //FindFirstChangeNotification()
  //FindNextChangeNotification()
  //FindCloseChangeNotification()

  //ReadDirectoryChangesW()

  return 0;
}

