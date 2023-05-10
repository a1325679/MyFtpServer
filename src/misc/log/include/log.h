#ifndef _LOG_H_
#define _LOG_H_
#include <list>
#include <string>
#include <stdarg.h>

class MyLog
{
public:
  ~MyLog();

  static MyLog *GetInstance();

  static void PrintLogsThread(void *args);

  bool Init(const char *logfile);


  bool Log(int level,const char *fmt, ...);

  bool AddMessage(std::string str);

  std::string GetMessageQueueFrontElement();

  bool IsEmptyMessageQueue();

  FILE *GetFd();

  void SetLogStatus(bool flag = true);

  void flushMessage();

  class CGarge
  {
  public:
    CGarge()
    {
    }
    ~CGarge()
    {
      delete m_instance_;
    }
  };

private:
  MyLog();
  FILE *fd_;
  int level_;
  bool program_exit_;
  static CGarge cg_;
  static MyLog *m_instance_;
  std::list<std::string> message_queue_;
};
#endif