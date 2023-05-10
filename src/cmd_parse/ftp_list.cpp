#include "ftp_list.h"
#include "func.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
using namespace std;

void FtpList::WriteWork(struct bufferevent *bev)
{
  ResCMD("226 Transform complete\r\n");
  Close();
}
void FtpList::EventWork(struct bufferevent *bev, short what)
{
  if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
  {
    log(NOTICE, "%s:%d -> BEV_EVENT_EOF | BEV_EVENT_ERROR |BEV_EVENT_TIMEOUT", ipaddr_.c_str(), port_from_);
    Close();
  }
  else if (what & BEV_EVENT_CONNECTED)
  {
    log(NOTICE, "%s:%d -> BEV_EVENT_CONNECTED", ipaddr_.c_str(), port_from_);
  }
}
void FtpList::Write(struct bufferevent *bev)
{
  WriteWork(bev);
}
void FtpList::Event(struct bufferevent *bev, short what)
{
  EventWork(bev, what);
}
void FtpList::Parse(std::string type, std::string msg)
{
  if (msg.find("\r\n") != std::string::npos)
  {
    msg.pop_back();
    msg.pop_back();
  }
  log(NOTICE, "%s:%d -> 解析命令%s,命令内容为%s", ipaddr_.c_str(), port_from_, type.c_str(), msg.c_str());

  // 1连接数据通道 2 150 3 发送目录数据通道 4 发送完成226 5 关闭连接
  // 命令通道回复消息 使用数据通道发送目录
  //-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n
  ConnectPORT();
  ResCMD("150 Here comes the directory listing.\r\n");
  // string listdata = "-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n";
  string listdata = GetListData(cmd_task_->rootDir_ + cmd_task_->curDir_);
// 3 数据通道发送
#ifdef _WIN32
  if (listdata.size() == 0)
  {
    listdata = "总用量 0";
    listdata += "\r\n";
  }
#endif
  Send(listdata);
}