#include <signal.h>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <process.h>
#include <thread>
#endif
#include "func.h"
using namespace std;
void SignalHandler(int signo)
{
#ifndef _WIN32
  switch (signo)
  {
  case SIGHUP:
    log(NOTICE, "%d 接收到 SIGHUP 信号,FTP服务已终止！", getpid());
    break;
  case SIGQUIT:
    log(NOTICE, "%d 接收到 SIGQUIT 信号,FTP服务已终止！", getpid());
    break;

  case SIGINT:
    log(NOTICE, "%d 接收到 SIGINT 信号,FTP服务已终止！", getpid());
    break;
  }
#else
  log(NOTICE, "%d 接收到 SIGINT 信号,FTP服务已终止！", getpid());
#endif
  ClearWork();
  exit(0);
}

bool InitSignal()
{
  signal(SIGINT, SignalHandler);
#ifndef _WIN32
  signal(SIGHUP, SignalHandler);
  signal(SIGQUIT,SignalHandler);
#endif
  return true;
}