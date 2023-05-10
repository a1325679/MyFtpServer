#ifndef _FUNC_H
#define _FUNC_H
#include "log.h"
#include "read_conf.h"
#include "macor.h"
#include "ftp_factory.h"
#include "global.h"
#include "func.h"
#include "threadpool.h"

#include<vector>
#include<string>
typedef void *HANDLE;
void Rtrim(char *str);
void Ltrim(char *str);
void Split(const std::string& s, std::vector<std::string>& sv,const char flag = ' ');
bool InitSignal();
void ClearWork();

int FtpDaemon();
int FtpInit(const char* conf_path = "conf.conf");
HANDLE GetHandleFromProcessName(const char *process_name);
const char *GetProcessPidByName(const char *proc_name);
int ParseParamMain(int argc, const char **argv);
#endif