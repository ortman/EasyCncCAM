#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Tool.hpp"

#define SETTINGS_FILENAME "EasyCncCAM_settings.json"

class Settings {
public:
	static Settings settings;
	void Jsonize(JsonIO& json) {
		if (json.IsStoring()) {
			Vector<Tool> vt;
			for (Tool* t : Tool::tools) {
				vt.Add(*t);
			}
			json("tools", vt);
		} else {
			Value v = json.Get();
			if (!v.IsNull()) {
				for (Tool* t : Tool::tools) {
					delete t;
				}
				Tool::tools.Clear();
				ValueArray valueTools = v["tools"];
				for (Tool valueTool: valueTools) {
					DUMP(valueTool);
					Tool::tools.Add(new Tool(valueTool));
				}
			}
		}
	}

	static void Load() {
		LoadFromJsonFile(Settings::settings, GetExeDirFile(SETTINGS_FILENAME));
	}
	static void Save() {
		FileOut out(GetExeDirFile(SETTINGS_FILENAME));
		out.Put(StoreAsJson(Settings::settings, true));
		out.Close();
	}
};

Settings Settings::settings;

#endif