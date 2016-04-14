/*!
  \brief  
  This is a memory patcher or actually called modifier. For 32bit apps only (x86 platform).

  The implementation is picked from one of my project that modifies the 'Plant vs Zombies' runtime memories.

  \file 
  memopatcher.h

  \author 

  \date 
  2016/04/14

  \version  
  1.0

  \remarks  

*/
#ifndef _MEMOPATCHER_H_
#define _MEMOPATCHER_H_

#include "winutilities.h"
#include <cstdint>
#include <memory>
#include <set>

XK_NAMESPACE_BEGIN

/*!
  \brief  
  Modifies the memories of the process. Debug privilege is needed. It's only for 32bit applications now.

  \todo 
  x64.
*/
struct ProcessMemoPatch {
  typedef uintptr_t AddrType;
  typedef uint64_t AddrLongType;
  typedef uint32_t BaseAddrType;
  typedef uint8_t byte;
  typedef std::unique_ptr<byte[]> BytesBuffer;
  typedef std::set<BaseAddrType> BaseAddrSet;

  ~ProcessMemoPatch() {
  }

  ProcessMemoPatch(HANDLE p) 
      : proc(p), base_start(64*1024), base_end(2U*1024U*1024U*1024U), 
        bytes_per_page(4*1024) {
    OSVERSIONINFOEX os_ver = {sizeof os_ver};
    DWORDLONG cond(0);
    VER_SET_CONDITION(cond, VER_PLATFORMID, VER_GREATER_EQUAL);
    if (!VerifyVersionInfo(&os_ver, VER_PLATFORMID, cond)) {
      throw RuntimeError(fmt_errmsg2(GetLastError()));
    }
    base_start = (VER_PLATFORM_WIN32_WINDOWS == 
        os_ver.dwPlatformId) ? 4 * 1024 * 1024 : 64 * 1024;
    base_end -= base_start;

    SYSTEM_INFO si = {0};
    GetSystemInfo(&si);
    bytes_per_page = si.dwPageSize;

    page_buffer.reset(new byte[bytes_per_page]);
    memset(page_buffer.get(), 0, bytes_per_page);
  }

  template <class T> void Read(BaseAddrType addr, T &b) {
    SIZE_T bytes_read(0);
    auto readokay = ReadProcessMemory(proc, (void*)addr, 
        &b, sizeof b, &bytes_read);
    if (!readokay) {
      throw RuntimeError(fmt_errmsg2(GetLastError()));
    }
  }

  template <class T> bool Write(BaseAddrType addr, const T &d) {
    SIZE_T bytes_written(0);
    auto wrokay = WriteProcessMemory(proc, (void*)addr, 
        &d, sizeof d, &bytes_written);
    if (!wrokay) {
      throw RuntimeError(fmt_errmsg2(GetLastError()));
    }
  }

  template <class T> void Find(const T &d, BaseAddrSet &addr) {
    Find(base_start, base_end, d, addr);
  }

  template <class T> void Find(BaseAddrType low, BaseAddrType high, const T &d, 
      BaseAddrSet &addr) {
    AddrLongType base = low;
    AddrLongType endian = high;
    while (base < endian) {
      auto delta = (BaseAddrType)(endian - base);

      MEMORY_BASIC_INFORMATION mbi = {0};
      auto bytes_ret = VirtualQueryEx(proc, (void*)base, &mbi, sizeof mbi);
      if (0 == bytes_ret) {
        throw RuntimeError(fmt_errmsg2(GetLastError()));
      }
      if (MEM_COMMIT == mbi.State && PAGE_READWRITE == mbi.Protect && 
          PAGE_READWRITE == mbi.AllocationProtect) {
        auto offset = (BaseAddrType)(base-(AddrLongType)mbi.BaseAddress);
        auto begpage = offset / bytes_per_page;
        auto end_page = mbi.RegionSize / bytes_per_page;
        if (delta < mbi.RegionSize) {
          end_page = (BaseAddrType)(endian - (BaseAddrType)mbi.BaseAddress - 
              1) / bytes_per_page + 1;
        }
        for (size_t p(begpage); p < end_page; ++p) {
          auto page_base = (BaseAddrType)mbi.BaseAddress + p * bytes_per_page;
          SIZE_T bytes_read(0);
          auto readokay = ReadProcessMemory(proc, (void*)page_base, 
              page_buffer.get(), bytes_per_page, &bytes_read);
          if (!readokay) {
            // NOTE: should log it.
            //throw RuntimeError(fmt_errmsg2(GetLastError()));
            continue;
          }
          size_t i = offset;
          auto address = page_base + i;
          for (size_t endi(bytes_read-sizeof(T)+1); i < endi && address < high; ++i) {
            address = page_base + i;
            auto v = *reinterpret_cast<T*>(page_buffer.get() + i);
            if (d == v) {
              addr.insert(address);
            }
          }
          offset = 0;
        }
      }
      base += mbi.RegionSize;
    }
  }

/*
  template <class T> bool Replace(const T &src, const T &tar) {
    return Replace(base_start, base_end, src, tar);
  }

  template <class T> bool Replace(BaseAddrType low, BaseAddrType high, 
      const T &src, const T &tar) {
    BaseAddrSet addrs;
    Find(low, high, src, addrs);
    for (auto ad : addrs) {
      Write(ad, tar);
    }
    return true;
  }
*/

 private:
  HANDLE proc;
  BaseAddrType base_start;
  BaseAddrType base_end;
  size_t bytes_per_page;
  BytesBuffer page_buffer;
};

XK_NAMESPACE_END

#endif 
