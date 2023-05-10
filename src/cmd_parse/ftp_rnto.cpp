#include "ftp_rnto.h"
#include "func.h"
#ifdef _WIN32
#include <ctime>
#include <direct.h>
#include <io.h>
#include <Windows.h>
#include <fstream>
#include "cstdlib"
#else
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
using namespace std;

void FtpRnto::Parse(std::string type, std::string msg)
{
  if (msg.find("\r\n") != std::string::npos)
  {
    msg.pop_back();
    msg.pop_back();
  }
  log(NOTICE, "%s:%d -> 解析命令%s,命令内容为%s", ipaddr_.c_str(), port_from_, type.c_str(), msg.c_str());

  int pos = msg.find(" ");
  std::string file_name = msg.substr(pos + 1);
  file_name = rootDir_ + file_name;
#ifdef _WIN32
  std::string cmd = "move ";
  cmd += move_file_;
  cmd += " ";
  cmd += file_name;
  //std::cout << "movefile: " << moveFile << " fiel_name: " << file_name << "\n";
  //MoveFileA(moveFile.c_str(), file_name.c_str());
  //system(cmd.c_str());
  rename(move_file_.c_str(), file_name.c_str());
#else
  rename(move_file_.c_str(), file_name.c_str());
#endif
  ResCMD("250 move success .");
}
