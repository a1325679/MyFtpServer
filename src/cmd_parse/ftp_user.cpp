#include "ftp_user.h"
#include "func.h"
#include <iostream>
using namespace std;
// 解析协议
void FtpUser::Parse(std::string type, std::string msg)
{
  if (msg.find("\r\n") != std::string::npos)
  {
    msg.pop_back();
    msg.pop_back();
  }
  log(NOTICE, "%s:%d -> 解析命令%s,命令内容为%s", ipaddr_.c_str(), port_from_, type.c_str(), msg.c_str());
  ResCMD("230 Login successful.\r\n");
}

FtpUser::FtpUser()
{
}

FtpUser::~FtpUser()
{
}
