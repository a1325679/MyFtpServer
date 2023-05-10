#ifndef _X_CONF_H_
#define _X_CONF_H_
#include <vector>
#include "global.h"
class Config
{
public:
	~Config();
	static Config *GetInstance()
	{
		if (m_instance_ == nullptr)
		{

			if (m_instance_ == nullptr)
			{
				m_instance_ = new Config();
			}
		}
		return m_instance_;
	}
	class CGarhuishou
	{
	public:
		~CGarhuishou()
		{
			if (Config::m_instance_)
			{
				delete Config::m_instance_;
				Config::m_instance_ = nullptr;
			}
		}
	};
	bool Load(const char *pconfName);
	const char *GetString(const char *p_itemname);
	int GetIntDefault(const char *p_itemname, const int def);
private:
	Config();
	static Config *m_instance_;
  static CGarhuishou cg_;
  std::vector<LPConfItem>m_config_item_list_;
};

#endif
