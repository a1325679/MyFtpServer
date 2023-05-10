#include "threads.h"
#include <thread>
#include <iostream>
#include <event2/event.h>
#include "task.h"
#include "macor.h"
#ifndef _WIN32
#include <unistd.h>
#endif

void Thread::Start()
{
  Setup();
  // 启动线程
  std::thread th(&Thread::Main, this);
  // 断开与主线程联系
  th.detach();
}

void Thread::Main()
{
  event_base_dispatch(base_);
  event_base_free(base_);
}
static void NotifyCB(evutil_socket_t fd, short which, void *args)
{
  Thread *t = (Thread *)args;
  t->Notify(fd, which);
}
// 安装线程，初始化event_base和管道监听事件用于激活
bool Thread::Setup()
{
#ifdef _WIN32
  evutil_socket_t fds[2];
  if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
  {
    log(ERROR, "evutil_socketpair failed!");
    return false;
  }
  evutil_make_socket_nonblocking(fds[0]);
  evutil_make_socket_nonblocking(fds[1]);
#else
  int fds[2];
  if (pipe(fds))
  {
    std::cerr << "pipe_failed" << std::endl;
    return false;
  }
#endif

  notify_send_fd_ = fds[1];

  event_config *ev_conf = event_config_new();
  event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
  this->base_ = event_base_new_with_config(ev_conf);
  event_config_free(ev_conf);
  if (!base_)
  {
    log(ERRORS, "event_base_new_with_config failed in thread! %s:%d",__FILE__,__LINE__);
    return false;
  }
  event *ev = event_new(base_, fds[0], EV_READ | EV_PERSIST, NotifyCB, this);
  event_add(ev, 0);
  return true;
}

// 收到主线程发出的激活消息（线程池的分发）
void Thread::Notify(evutil_socket_t fd, short which)
{
  // 水平触发 只要没有接受完成，会再次进来
  char buf[2] = {0};
#ifdef _WIN32
  int re = recv(fd, buf, 1, 0);
#else
  // linux中是管道，不能用recv
  int re = read(fd, buf, 1);
#endif
  if (re <= 0)
    return;

  Task *task = nullptr;
  tasks_mutex_.lock();
  if (tasks_.empty())
  {
    tasks_mutex_.unlock();
    return;
  }
  task = tasks_.front();
  tasks_.pop_front();
  tasks_mutex_.unlock();
  task->Init();
}

void Thread::Activate()
{

#ifdef _WIN32
  int re = send(this->notify_send_fd_, "c", 1, 0);
#else
  int re = write(this->notify_send_fd_, "c", 1);
#endif
  if (re <= 0)
  {
    std::cerr << "XThread::Active()failed!" << std::endl;
  }
}

void Thread::AddTask(Task *t)
{
  if (!t)
    return;
  t->base_ = this->base_;
  tasks_mutex_.lock();
  tasks_.push_back(t);
  tasks_mutex_.unlock();
}

Thread::Thread()
{
}

Thread::~Thread()
{
}