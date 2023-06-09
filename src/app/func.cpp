#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include "func.h"
#include "global.h"
#include "log.h"
using namespace std;
void Rtrim(char *str)
{
  size_t len = 0;
  if (str == nullptr)
  {
    return;
  }
  len = strlen(str);
  while (len > 0 && str[len - 1] == ' ')
  {
    str[--len] = 0;
  }
  return;
}
void Ltrim(char *str)
{
  size_t len = 0;
  len = strlen(str);
  char *p_tmp = str;
  if ((*p_tmp) != ' ')
  {
    return;
  }
  while ((*p_tmp) != '\0')
  {
    if ((*p_tmp) == ' ')
      p_tmp++;
    else
      break;
  }
  if ((*p_tmp) == '\0')
  {
    *str = '\0';
    return;
  }
  char *p_tmp2 = str;
  while ((*p_tmp) != '\0')
  {
    (*p_tmp2) = (*p_tmp);
    p_tmp++;
    p_tmp2++;
  }
  (*p_tmp2) = '\0';
  return;
}
void Split(const string& s, vector<string>& sv,const char flag) {
  sv.clear();
  istringstream iss(s);
  string temp;
  while (getline(iss, temp, flag)) {
      sv.push_back(temp);
  }
}
void ClearWork()
{
  MyLog::GetInstance()->flushMessage();
  MyLog::GetInstance()->SetLogStatus(false);
}