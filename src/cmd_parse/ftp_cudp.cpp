#include "ftp_cudp.h"
#include "func.h"
#include <iostream>
#include <string>
using namespace std;
void FtpCudp::Parse(std::string type, std::string msg)
{
  if (msg.find("\r\n") != std::string::npos)
  {
    msg.pop_back();
    msg.pop_back();
  }
  log(NOTICE, "%s:%d -> 解析命令%s,命令内容为%s",ipaddr_.c_str(), port_from_, type.c_str(), msg.c_str());

  string path = cmd_task_->curDir_;
  if (path[path.size() - 1] == '/')
  {
    path = path.substr(0, path.size() - 1);
  }
  int pos = path.rfind("/");
  path = path.substr(0, pos);
  cmd_task_->curDir_ = path;
  ResCMD("250 Directory success changed. \r\n");
}