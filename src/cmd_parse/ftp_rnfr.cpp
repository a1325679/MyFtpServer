#include "ftp_rnfr.h"
#include "func.h"
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
void FtpRnfr::Parse(std::string type, std::string msg)
{
  if (msg.find("\r\n") != std::string::npos)
  {
    msg.pop_back();
    msg.pop_back();
  }
  log(NOTICE, "%s:%d -> 解析命令%s,命令内容为%s", ipaddr_.c_str(), port_from_, type.c_str(), msg.c_str());

  int pos = msg.find(" ");
  std::string file_name = msg.substr(pos + 1);
  file_name = rootDir_ + cmd_task_->curDir_ +"/"+ file_name;
  move_file_ = file_name;
  ResCMD("200 OK.");
}
