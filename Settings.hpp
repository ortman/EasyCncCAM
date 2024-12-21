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
			json
				("tools", tools)
				//("viewer", JsonIO("1","2"))
				;
		} else {
			Value v, j = json.Get();
			if (!j.IsNull()) {
				tools.Clear();
				ValueArray valueTools = j["tools"];
				for (Tool valueTool: valueTools) {
					tools.Add(valueTool);
				}
				Value viewer = j["viewer"];
				if (!viewer.IsNull()) {
					if (!(v = viewer["background"]).IsNull()) {
						viewerBG = v;
					}
					if (!(v = viewer["drillColor"]).IsNull()) {
						drillColor = v;
					}
					if (!(v = viewer["drillLineWidth"]).IsNull()) {
						drillLineWidth = v;
					}
					if (!(v = viewer["drillCenterColor"]).IsNull()) {
						drillCenterColor = v;
					}
					if (!(v = viewer["measurersColor"]).IsNull()) {
						measurersColor = v;
					}
					if (!(v = viewer["measurersFont"]).IsNull()) {
						measurersFont = v;
					}
					if (!(v = viewer["measurersLineWidth"]).IsNull()) {
						measurersLineWidth = v;
					}
					if (!(v = viewer["measurersArrowSize"]).IsNull()) {
						measurersArrowSize = v;
					}
					if (!(v = viewer["measurersArrowAngle"]).IsNull()) {
						measurersArrowAngle = v;
					}
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
	
	static Vector<Tool> tools;
	static Color viewerBG;
	static Color drillColor;
	static int drillLineWidth;
	static Color drillCenterColor;
	static Color measurersColor;
	static Font measurersFont;
	static int measurersLineWidth;
	static double measurersArrowSize;
	static double measurersArrowAngle;
};

Settings Settings::settings;

Vector<Tool> Settings::tools;
Color Settings::viewerBG = Color(240, 240, 255);
Color Settings::drillColor = Blue;
int Settings::drillLineWidth = 2;
Color Settings::drillCenterColor = LtRed;
Color Settings::measurersColor = Black;
Font Settings::measurersFont = StdFont(20);
int Settings::measurersLineWidth = 1;
double Settings::measurersArrowSize = 20.;
double Settings::measurersArrowAngle = M_PI/10.;

#endif