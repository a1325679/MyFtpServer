#include "ftp_task.h"
#include "func.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
#include<string.h>
#ifdef _WIN32
#include<io.h>
#endif
#include <iostream>
using namespace std;
void FtpTask::SetRootDir() {
  string path = Config::GetInstance()->GetString("RootPath");
  rootDir_ = path;
}
void FtpTask::Send(std::string data)
{
	Send(data.c_str(), data.size());
}
void FtpTask::Send(const char* data, int datasize)
{
	if (!bev_)return;
	bufferevent_write(bev_, data, datasize);
}
void FtpTask::Close()
{
	if (bev_)
	{
		bufferevent_free(bev_);
		bev_ = 0;
	}
	if (fp_)
	{
    printf("fp is free\n");
		fclose(fp_);
    fp_ = 0;
  }
}
//连接数据通道
void FtpTask::ConnectPORT()
{
	if (ip_.empty() || port_ <= 0 || !base_)
	{
		//cout << "ConnectPORT failed ip or port or base is null" << endl;
		return;
	}
	Close();
	bev_ = bufferevent_socket_new(base_, -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_THREADSAFE);
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port_);
	evutil_inet_pton(AF_INET, ip_.c_str(), &sin.sin_addr.s_addr);
	//设置回调和权限
	SetCallback(bev_);
	//添加超时 
	timeval rt = { 600,0 };
	bufferevent_set_timeouts(bev_, &rt, 0);
	bufferevent_socket_connect(bev_, (sockaddr*)&sin, sizeof(sin));
}
//回复cmd消息
void FtpTask::ResCMD(string msg)
{
	if (!cmd_task_ || !cmd_task_->bev_)return;
	//cout << "ResCMD:" << msg << endl;
	if (msg[msg.size() - 1] != '\n')
		msg += "\r\n";
	bufferevent_write(cmd_task_->bev_, msg.c_str(), msg.size());
}
void FtpTask::SetCallback(struct bufferevent* bev)
{
	bufferevent_setcb(bev, ReadCB, WriteCB, EventCB, this);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
}

void FtpTask::ReadCB(bufferevent* bev,void* arg)
{
	FtpTask* t = (FtpTask*)arg;
	t->Read(bev);
}
void FtpTask::WriteCB(bufferevent* bev, void* arg)
{
	FtpTask* t = (FtpTask*)arg;
	t->Write(bev);
}
void FtpTask::EventCB(struct bufferevent* bev, short what, void* arg)
{
	FtpTask* t = (FtpTask*)arg;
	t->Event(bev, what);
}
std::string FtpTask::GetListData(std::string path) {
#ifndef _WIN32

  string data = "";

  string cmd = "ls -l ";
  cmd += path;
  //std::cout << "popen:" << cmd << std::endl;
  FILE *f = popen(cmd.c_str(), "r");
  if (!f)
    return data;
  char buffer[1024] = {0};
  for (;;)
  {
    int len = fread(buffer, 1, sizeof(buffer) - 1, f);
    if (len <= 0)
      break;
    buffer[len] = '\0';
    data += buffer;
  }
  pclose(f);
  //std::cout << "data : " << data << std::endl;
#else
  	//-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n
	string data = "";
	// 存储文件信息
	_finddata_t file;
	// 目录上下文
	path += "/*.*";
	intptr_t dir = _findfirst(path.c_str(), &file);
	if (dir <= 0)
		return data;
	do
	{
		string tmp = "";
		// 是否是目录去掉 . ..
		if (file.attrib & _A_SUBDIR)
		{
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
			{
				continue;
			}
			tmp = "drwxrwxrwx 1 root group ";
		}
		else
		{
			tmp = "-rwxrwxrwx 1 root group ";
		}
		// 文件大小
		char buf[1024];
		//_CRT_SECURE_NO_WARNINGS
		sprintf(buf, "%u ", file.size);
		tmp += buf;

		// 日期时间
		strftime(buf, sizeof(buf) - 1, "%b %d %H:%M ", localtime(&file.time_write));
		tmp += buf;
		tmp += file.name;
		tmp += "\r\n";
		data += tmp;

	} while (_findnext(dir, &file) == 0);
#endif
  return data;
}