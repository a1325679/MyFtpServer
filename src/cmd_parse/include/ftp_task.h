#ifndef _FTP_TASK_
#define _FTP_TASK_

#include "task.h"
#include <string>
#include "task.h"
struct bufferevent;
class FtpTask : public Task
{
public:
  FtpTask() {
    SetRootDir();
  }
  virtual ~FtpTask() {

  }
  std::string curDir_ = "/";
	//std::string rootDir = ".";
  std::string rootDir_;
  std::string move_file_;
  // PORT 数据通道的IP和端口
  std::string ip_ = "";
	int port_ = 0;

	// 命令通道
	FtpTask *cmd_task_ = 0;

  void SetRootDir();
  // 解析协议
  virtual void Parse(std::string type, std::string msg) {}
	// 回复cmd消息
	void ResCMD(std::string msg);

	// 用来发送建立了连接的数据通道
	void Send(std::string data);
	void Send(const char *data, int datasize);

	// 连接数据通道
	void ConnectPORT();
	void Close();
	virtual void Read(struct bufferevent *bev) {}
	virtual void Write(struct bufferevent *bev) {}
	virtual void Event(struct bufferevent *bev, short what) {}
	void SetCallback(struct bufferevent *bev);
	bool Init() { return true; }

protected:
	static void ReadCB(bufferevent *bev, void *arg);
	static void WriteCB(bufferevent *bev, void *arg);
	static void EventCB(struct bufferevent *bev, short what, void *arg);
	std::string GetListData(std::string path);
	// 命令bev
	struct bufferevent *bev_ = 0;
	FILE *fp_ = 0;
};

#endif