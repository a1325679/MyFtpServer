#include "ftp_stor.h"
#include "func.h"
#include <iostream>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <functional>
#include "global.h"
#ifndef _WIN32
#include <unistd.h>
#endif
using namespace std;
FtpStor::FtpStor() {}
FtpStor::~FtpStor()
{
}
// 解析协议
void FtpStor::Parse(std::string type, std::string msg)
{
  // 文件名
  if (msg.find("\r\n") != std::string::npos)
  {
    msg.pop_back();
    msg.pop_back();
  }
  log(NOTICE, "%s:%d -> 解析命令%s,命令内容为%s", ipaddr_.c_str(), port_from_, type.c_str(), msg.c_str());
  int pos = msg.rfind(" ") + 1;
  string filename = msg.substr(pos);
  string path = cmd_task_->rootDir_.c_str();
  path += cmd_task_->curDir_;
  path += "/";
  path += filename;
  fp = fopen(path.c_str(), "wb+");
  
  if (fp)
  {
    // 连接数据通道
    ConnectPORT();
    // 发送开始接收文件的指令
    ResCMD("125 File OK\r\n");
    // 触发读取事件
    //bufferevent_trigger(bev, EV_READ, 0);
    //usleep(500 * 1000);
  }
  else
  {
    ResCMD("450 file open failed!\r\n");
  }
}
void FtpStor::ReadWork(struct bufferevent *bev)
{
  //bufferevent_disable(bev,EV_READ);
  if (!fp)
    return;
  for (;;)
  {
    int len = bufferevent_read(bev, buf, sizeof(buf));
    if (len <= 0) {
      break;
    }
    int size = fwrite(buf, 1, len, fp);
    fflush(fp);
  }
}
void FtpStor::EventWork(struct bufferevent *bev, short what)
{
  //  如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
  if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
  {
    log(NOTICE, "%s:%d -> FtpSTOR BEV_EVENT_EOF | BEV_EVENT_ERROR |BEV_EVENT_TIMEOUT", ipaddr_.c_str(), port_from_);
    
    Close();
    if(fp) {
      fclose(fp);
      fp = 0;
    }
    ResCMD("226 Transfer complete\r\n");
  }
  else if (what & BEV_EVENT_CONNECTED)
  {
    log(NOTICE, "%s:%d -> FtpSTOR BEV_EVENT_CONNECTED", ipaddr_.c_str(), port_from_);
  
  }
}
void FtpStor::Read(struct bufferevent *bev)
{
  ReadWork(bev);
}
void FtpStor::Event(struct bufferevent *bev, short what)
{
  EventWork(bev, what);
}