#include "func.h"
#ifndef _WIN32
#include <unistd.h>
#else
#include<windows.h>
#include<tchar.h>
#include <Psapi.h>
#include<TlHelp32.h>
#endif
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <thread>

using namespace std;
int thread_count_;
unsigned short port_;
const char *root_path_;

#ifndef _WIN32
int FtpDaemon()
{
  switch (fork())
  {
  case -1:
    log(EMERG, "FtpDaemon()中fork()失败!");
    return -1;
  case 0:
    break;
  default:
    return 1;
  }
  pid_t ngx_pid = getpid();
  if (setsid() == -1)
  {
    return -1;
  }

  //(3)设置为0，不要让它来限制文件权限，以免引起混乱
  umask(0);

  //(4)打开黑洞设备，以读写方式打开
  int fd = open("/dev/null", O_RDWR);
  if (fd == -1)
  {
    log(EMERG, "FtpDaemon()中open(/dev/null)失败");
    return -1;
  }
  if (dup2(fd, STDIN_FILENO) == -1) // 先关闭STDIN_FILENO[这是规矩，已经打开的描述符，动他之前，先close]，类似于指针指向null，让/dev/null成为标准输入；
  {
    log(EMERG, "FtpDaemon()中dup2(fd, STDIN_FILENO)失败");
    return -1;
  }
  if (dup2(fd, STDOUT_FILENO) == -1) // 再关闭STDIN_FILENO，类似于指针指向null，让/dev/null成为标准输出；
  {
    log(EMERG, "ftp_daemon()中dup2(fd, STDOUT_FILENO)失败");
    return -1;
  }
  if (fd > STDERR_FILENO) // fd应该是3，这个应该成立
  {
    if (close(fd) == -1) // 释放资源这样这个文件描述符就可以被复用；不然这个数字【文件描述符】会被一直占着；
    {
      return -1;
    }
  }
  return 0; // 子进程返回0
}
#endif
int FtpInit(const char *conf_path)
{
  Config *p_config = Config::GetInstance(); // 单例类
  if (p_config->Load(conf_path) == false)   // 把配置文件内容载入到内存
  {
    cout << "config load is failed, please check conf file is exist!\n";
    return -1;
  }

  std::string log_path = p_config->GetString("Log");
  if (!MyLog::GetInstance()->Init(log_path.c_str()))
  {
    cout << "Log file is init error! \n";
    return -1;
  }
  thread_count_ = p_config->GetIntDefault("WorkThreadCount", 5);

  port_ = p_config->GetIntDefault("ListenPort0", 9090);
  root_path_ = p_config->GetString("RootPath");
  InitSignal();

#ifndef _WIN32
  if (p_config->GetIntDefault("Daemon", 0) == 1)
  {
    // 1：按守护进程方式运行
    int cdaemonresult = FtpDaemon();
    if (cdaemonresult == -1) // fork()失败
    {
      cout << "ftpserver daemon start failed ! \n";
      return -1;
    }
    if (cdaemonresult == 1)
    {
      return 1;
    }
  }
#endif
  thread t(MyLog::PrintLogsThread, MyLog::GetInstance());
  t.detach();
  return 0;
}

#ifndef _WIN32
const char* GetProcessPidByName(const char *proc_name)
{
  FILE *fp;
  char buf[100];
  char cmd[200] = {'\0'};
  pid_t pid = -1;
  sprintf(cmd, "pidof %s", proc_name);
  if((fp = popen(cmd, "r")) != NULL)
  {
    if(fgets(buf, 255, fp) != NULL)
    {
      pclose(fp);
      pid_t p = getpid();
      std::string own = to_string(p);
      std::string procid = buf;
      vector<string> sv;
      Split(procid,sv,' ');
      if(sv.size()==1){
        return nullptr;
      }
      for(auto &a:sv){
        if(a!=own) {
          return a.c_str();
        }
      }
      return nullptr;
    }
  }
  pclose(fp);
  return nullptr;
}
int ParseParamMain(int argc,const char** argv) {
  if(argc > 2) {
    return -1;
  }
  if(strcmp(argv[1],"start")==0) {
    const char* proc = GetProcessPidByName("./FtpServer");
    if(proc!=nullptr) {
      return 1;
    }
    return 0;
  }else if(strcmp(argv[1],"stop")==0) {
    const char* proc = GetProcessPidByName("./FtpServer");
    if(proc==nullptr) {
      return -2;
    }
    std::string emit_SIGUSR1_to_proc = "kill -USR1 ";
    emit_SIGUSR1_to_proc+=proc;
    system(emit_SIGUSR1_to_proc.c_str());
    return 3;
  }
  return -1;
}
#else
HANDLE GetHandleFromProcessName(const char* process_name) {
	HANDLE handle;
	HANDLE handle1 = NULL;
	handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32* info;

	info = new PROCESSENTRY32;

	info->dwSize = sizeof(PROCESSENTRY32W);


	Process32First(handle, info);

	while (Process32Next(handle, info) != FALSE)
	{
    if (strcmp(process_name, (const char *)info->szExeFile) == 0)
    {
			CloseHandle(handle);
			handle1 = OpenProcess(PROCESS_TERMINATE, FALSE, info->th32ProcessID);
			//TerminateProcess(handle1, 0);
			return handle1;
		}
	}
	printf("end\n");
	CloseHandle(handle);
	//CloseHandle(handle1);
	return NULL;
}
int ParseParamMain(int argc,const char** argv){
  if(argc > 2) {
    return -1;
  }
  HANDLE proc = GetHandleFromProcessName("FtpServer.exe");

  if(strcmp(argv[1],"start")==0) {
    return 0;
  }else if(strcmp(argv[1],"stop")==0) {
    if(proc==nullptr) {
      return -2;
    }
    TerminateProcess(proc, 0);
    return 3;
  }
  return -1;
}
#endif
