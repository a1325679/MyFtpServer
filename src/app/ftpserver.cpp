#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/listener.h> //libevent头文件
#include <string.h>
#include <iostream>
#include <event2/thread.h>
#include <thread>
#ifndef  _WIN32
#include <arpa/inet.h>
#include <unistd.h>
#else 
#include <windows.h>
#include <conio.h>
#include<tchar.h>
#include <Psapi.h>
#include <string.h>
#include<TlHelp32.h>
#endif
#include "func.h"
using namespace std;

void listen_cb(struct evconnlistener *e, evutil_socket_t s, struct sockaddr *a, int socklen, void *arg)
{
  event_base *base = (event_base *)arg;
  Task* task = FtpFactory::Get()->CreateTask();
  task->sock_ = s;
  task->base_ = base;

  struct sockaddr_in *sock = (struct sockaddr_in *)a;
  int port = ntohs(sock->sin_port);
#ifdef _WIN32 // windows上打印方式
  const char* str = inet_ntoa(sock->sin_addr);
#else // linux上打印方式
  struct in_addr in = sock->sin_addr;
  char str[INET_ADDRSTRLEN]; // INET_ADDRSTRLEN这个宏系统默认定义 16。
  inet_ntop(AF_INET, &in, str, sizeof(str));
#endif
  task->ipaddr_ = str;
  task->port_from_ = port;
  ThreadPool::Get()->Dispatch(task);
  log(NOTICE, "%s:%d 已连接",task->ipaddr_.c_str(), task->port_from_);

}
int main(int argc,const char** argv)
{
  if(argc > 1) {
    int ret = ParseParamMain(argc,argv);
    if(ret == -1) {
      cout<<"param input error ,it's max 2"<<endl;
      return 0;
    }else if(ret == 1) {
      cout<<"program already start"<<endl;
      return 0;
    }else if(ret == -2) {
      cout<<"program none start\n";
      return 0;
    }else if(ret == 3) {
      cout<<"program alread stop\n";
      return 0;
    }
  }
  if(FtpInit() != 0) {
    return 0;
  }
#ifdef _WIN32
  HWND hWnd = GetConsoleWindow();
  ShowWindow(hWnd, SW_HIDE);
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
  evthread_use_windows_threads();
#else
  evthread_use_pthreads();
#endif
  ThreadPool::Get()->Init(thread_count_);
  log(NOTICE, "%d个线程已初始化完成", thread_count_);

  event_base *base = event_base_new();
  if (!base)
  {
    log(ERRORS,"even_base create failed! %s:%d",__FILE__,__LINE__);
  }
  // 监听端口 
  // socket ，bind，listen 绑定事件
  sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port_);
  evconnlistener *ev = evconnlistener_new_bind(base,                                      // libevent的上下文
                                               listen_cb,                                 // 接收到连接的回调函数
                                               base,                                      // 回调函数获取的参数 arg
                                               LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, // 地址重用，evconnlistener关闭同时关闭socket
                                               10,                                        // 连接队列大小，对应listen函数
                                               (sockaddr *)&sin,                          // 绑定的地址和端口
                                               sizeof(sin));
  log(NOTICE,"初始化完成,FTP服务器启动,%d端口已开启监听连接",port_);
  // 事件分发处理
  if (base)
  {
    int ret = event_base_dispatch(base);
  }
  if (ev)
    evconnlistener_free(ev);
  if (base)
    event_base_free(base);
  return 0;
}