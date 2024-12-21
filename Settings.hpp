#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Tool.hpp"

#define SETTINGS_FILENAME "EasyCncCAM_settings.json"

class Settings {
private:
	static Settings settings;
public:
	void Jsonize(JsonIO& json) {
		if (json.IsStoring()) {
			json("tools", Tool::tools);
		} else {
			Value v = json.Get();
			if (!v.IsNull()) {
				Tool::tools.Clear();
				ValueArray valueTools = v["tools"];
				for (Tool valueTool: valueTools) {
					DUMP(valueTool);
					Tool::tools.Add(valueTool);
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
	
	static Color viewerBG;
};

Settings Settings::settings;
Color Settings::viewerBG = Color(240, 240, 255);

#endif