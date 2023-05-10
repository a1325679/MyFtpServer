#include "ftp_rmd.h"
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
void FtpRmd::Parse(std::string type, std::string msg)
{
  if (msg.find("\r\n") != std::string::npos)
  {
    msg.pop_back();
    msg.pop_back();
  }
  int head = msg.find(" ");
  std::string name = msg.substr(head + 1);
  name = rootDir_ + cmd_task_->curDir_+ name;
  log(NOTICE, "%s:%d -> 解析命令%s,命令内容为%s", ipaddr_.c_str(), port_from_, type.c_str(), name.c_str());

  int ret = rmdir(name.c_str());
  if (ret == -1)
  {
    ResCMD("501 " + msg + " delete failed." + "\r\n");
    return;
  }
  ResCMD("257 " + msg + " success delete." + "\r\n");
  return;
}