#include "ftp_pwd.h"
#include "func.h"
#include <iostream>
#include <string>
void FtpPWD::Parse(std::string type, std::string msg)
{
  if(msg.find("\r\n")!=std::string::npos) {
    msg.pop_back();
    msg.pop_back();
  }
  log(NOTICE, "%s:%d -> 解析命令%s,命令内容为%s", ipaddr_.c_str(), port_from_, type.c_str(), msg.c_str());
  std::string resmsg = "";
  resmsg += "257 \"";
  resmsg += cmd_task_->curDir_;
  resmsg += "\" is current dir. \r\n";
  ResCMD(resmsg);
}