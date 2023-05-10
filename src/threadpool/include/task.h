#ifndef __TASK__
#define __TASK__
#include<string>
class Task
{
public:
	virtual bool Init() {
    return true;
  }

  int sock_ = 0;
	int thread_id_ = 0;
  std::string ipaddr_;
  int port_from_;
  // 初始化任务
  struct event_base* base_ = 0;
};
#endif
