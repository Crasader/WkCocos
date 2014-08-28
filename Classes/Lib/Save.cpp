#include "WkCocos/Save.h"

#include "WkCocos/Utils/log/logstream.h"

namespace WkCocos
{
	Save::Save(const std::string& saveName, Mode mode)
		: m_name(saveName)
	{
		m_saveModes[static_cast<int>(mode)] = 1;
	}
	
	Save::~Save()
	{}

	bool Save::requestLoadData(std::function<void()> loaded_cb)
	{
		/*	m_localdata->loadPlayerData([=](std::string data)
		{
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if (doc.HasParseError())
		{
		//if parse error (also empty string), we ignore existing data.
		doc.SetObject();
		}
		set_data_json(doc);
		});*/

		bool loaded = true;

		if (isMode(Mode::ONLINE))
		{
			if (m_onlinedata)
			{
				m_onlinedata->load(m_user, [=](std::string data)
				{
					m_onLoading(data);
					CCLOG("user data loaded : %s", data.c_str());

					loaded_cb();
				});
			}
			else
			{
				LOG_WARNING << "Online load requested but no online manager!" << std::endl;
				loaded = false;
			}
		}

		if (isMode(Mode::OFFLINE))
		{
			if (m_localdata)
			{
				m_localdata->loadPlayerData([=](std::string data){
					m_onLoading(data);

					loaded_cb();
				});
			}
			else
			{
				LOG_WARNING << "Offline load requested but no offline manager!" << std::endl;
				loaded = false;
			}
		}

		return loaded;
	}

	bool Save::requestSaveData(std::function<void()> saved_cb)
	{
		/*	// create document
		rapidjson::Document doc;
		doc.SetObject();
		// must pass an allocator when the object may need to allocate memory
		rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

		get_data_json(doc, allocator);

		// Get the save string
		rapidjson::StringBuffer strbuf;
		rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
		doc.Accept(writer);

		std::string save = std::string(strbuf.GetString());

		m_localdata->savePlayerData(save);*/
		bool loaded = true;

		if (isMode(Mode::ONLINE))
		{
			if (m_onlinedata)
			{
				m_onlinedata->save(m_user, m_onSaving(), [=](std::string data)
				{
					CCLOG("user data saved : %s", data.c_str());
					saved_cb();
				});

				return true;
			}
			else
			{
				LOG_WARNING << "Online save requested but no online manager!" << std::endl;
				loaded = false;
			}
		}

		if (isMode(Mode::OFFLINE))
		{
			if (m_localdata)
			{
				m_localdata->savePlayerData(m_onSaving());
				saved_cb();
				return true;
			}
			else
			{
				LOG_WARNING << "Offline save requested but no offline manager!" << std::endl;
				loaded = false;
			}
		}

		return loaded;
		
	}
}// namespace WkCocos