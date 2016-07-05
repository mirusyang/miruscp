/*!
  \brief  
  thread template method.

  used on win32 platform. C/C++ thread API(extended, platform dependent) based.

  btw, it looks like the template design pattern :).

  \file 
  threadtemplate.h

  \author 
  Y***n

  \date 
  2016/06/30

  \version  
  1.0

  \copyright  
  GPL

  \remarks
  -#  2016/06/30  created by Y***n.
*/
#ifndef _THREADTEMPLATE_H_
#define _THREADTEMPLATE_H_

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>
#include <atomic>

/*!
  \brief  
  C-runtime library based implementation.

  \tparam       Derived 
  The sub-class derived from this class (A little weird, ah?!).

  \note 
  - At least the C++0x standard supplys atomic library. If you're using an old-fashion compiler,
  do some adaptive jobs to make it works.
  - The subclass should implement one member method, named on_entry.
  - the public members shouldn't be overrided, or you know what you're doing, just do it.

  \todo 
  Does the C++0x support keyword "final"?
*/
template <class Derived> 
class CrtThread {
  typedef std::atomic<bool> atomic_bool;

 public:
  virtual ~CrtThread();
  CrtThread();
  bool create();
  void release();
  bool running() const;

 protected:

 private:
  static unsigned __stdcall _entry(void*);
  bool _join(unsigned timeout=INFINITE);

  uintptr_t thrd_;
  unsigned thrd_addr_;
  atomic_bool running_;
};

template <class D> 
CrtThread<D>::~CrtThread() { }

template <class D>
CrtThread<D>::CrtThread() : thrd_(0), thrd_addr_(0), running_(false) { }

template <class D>
bool CrtThread<D>::create() {
  /*
  auto self = reinterpret_cast<D*>(this);
  if (!self->on_ready()) {
    return false;
  }
  */
  running_ = true;
  thrd_ = _beginthreadex(nullptr, 0, _entry, this, 0, &thrd_addr_);
  if ((uintptr_t)-1 == thrd_ || 0 == thrd_) {
    return false;
  }
  return true;
}

template <class D>
void CrtThread<D>::release() {
  running_ = false;
  _join();
  auto handle = reinterpret_cast<HANDLE>(thrd_);
  if (handle) {
    CloseHandle(handle);
    thrd_ = 0;
    thrd_addr_ = 0;
  }
}

template <class D>
bool CrtThread<D>::running() const {
  return running_;
}

template <class D>
unsigned __stdcall CrtThread<D>::_entry(void *param) {
  auto inst = reinterpret_cast<D*>(param);
  if (inst) {
    inst->on_entry();
  }
  _endthreadex(0);
  return 0;
}

template <class D>
bool CrtThread<D>::_join(unsigned timeout) {
  int retcode(0);
  auto handle = reinterpret_cast<HANDLE>(thrd_);
  if (handle) {
    retcode = WaitForSingleObject(handle, timeout);
  }
  return WAIT_OBJECT_0 == retcode;
}

#endif

