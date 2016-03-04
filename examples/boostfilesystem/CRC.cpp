// CRC.cpp

// #define BOOST_ALL_DYN_LINK
// #define BOOST_ALL_NO_LIB
// #define BOOST_LIB_DIAGNOSTIC

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
// #include "boost/filesystem/path.hpp"
// #include "boost/filesystem/operations.hpp"
#include "boost/filesystem.hpp"

// using namespace std;
using namespace boost;
namespace bfs = boost::filesystem;

#ifdef _MSC_VER
# ifdef _DEBUG
// # pragma comment(lib, "libboost_filesystem-mt-gd.lib")
// # pragma comment(linker, "/nodefaultlib:libboost_filesystem-vc90-mt-gd-1_54.lib")
// # pragma comment(linker, "/nodefaultlib:msvcprtd.lib")
# else
// # pragma comment(lib, "libboost_filesystem-mt.lib")
# endif
#endif 

std::ofstream cout("nstdout.txt", std::ios::app);

//----------------------------------------------------------------------
// RecusiveListFiles
//----------------------------------------------------------------------
void RecusiveListFiles(std::string strFPath) {
  // using std::cout;

  bfs::path bfsPath(strFPath, bfs::native);

  bfs::recursive_directory_iterator itrBeg(bfsPath);
  bfs::recursive_directory_iterator itrEnd;
  for (; itrBeg != itrEnd; ++itrBeg) {
    if (bfs::is_directory(*itrBeg)) {
      continue;
    } else {
      // This is available with version 1.40
      // cout << itrBeg->path().file_string() << '\n';
      
      cout << *itrBeg << "\n";

      time_t time = bfs::last_write_time(*itrBeg);
      cout << ctime(&time);

      uintmax_t fileSize = bfs::file_size(*itrBeg);
      cout << fileSize << "\n\n";
    }
  } // end for
}

void RenameFiles(const std::string &folderpath) {
  bfs::path bfspath(folderpath, bfs::native);
  bfs::directory_iterator fit(bfspath);
  bfs::directory_iterator fit_end;
  for (; fit_end != fit; ++fit) {
    if (bfs::is_directory(*fit)) {
      continue;
    } else {
      if (0 == fit->path().extension().string().compare(".img")) {
        cout << *fit << "\n";
        auto s(fit->path().string());
        auto dotpos(s.rfind('.'));
        if (std::string::npos != dotpos) {
          s[dotpos + 1] = 'p';
          s[dotpos + 2] = 'n';
          s[dotpos + 3] = 'g';
          bfs::path newfn(s, bfs::native);
          // boost::filesystem::rename(fit->path(), newfn);
          boost::filesystem::copy(fit->path(), newfn);
        }
      }
    }
  }
  // bfs::initial_path();
}

//----------------------------------------------------------------------
// main(...)
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {
  using std::string;

  // string folderPath = ".";
  // string folderPath = "H:\\temp\\¹Ò»úÐ¡´«Ææv2.09\\Dat";
  string folderPath = "H:\\temp\\Dreamir\\Dat\\";

  // RecusiveListFiles(folderPath);
  RenameFiles(folderPath);
  getchar();

  return 0;
}
