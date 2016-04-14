/*!
  \brief  
  Test code for xkit.
*/
#include <iostream>
#include <iomanip>
#include "memopatcher.h"
#include "adjustprivileges.h"
#include "dlloader.h"
#include "processenumerator.h"

using namespace std;

static int test_xkit() {
  try {
    xk::adjust_debug_privilege(GetCurrentProcess());

    xk::ProcessMemoPatch pmp(GetCurrentProcess());

    xk::DllLoader dl;
    xk::DllLoader dl2(_T("xkit.dll"));
    auto hello_func = dl2.get_func<void (*)()>("hello");
    if (hello_func) {
      hello_func();
    } else {
      cout << "hello() is not found!" << endl;
    }

    xk::ProcessEnumerator pe;
    auto war3_id = pe(_T("war3.exe"));
    if (0 != war3_id) {
      cout << "war3 found, it's id is " << war3_id << endl;
    }
  } catch (xk::Exception &e) {
    clog << e.what() << endl;
  }
  return 0;
}
