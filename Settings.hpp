#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "Tool.hpp"

#define SETTINGS_FILENAME "EasyCncCAM_settings.json"

class Settings {
private:
	static Settings settings;
public:
	void Jsonize(JsonIO& jio) {
		if (jio.IsStoring()) {
			JsonIO viewer;
			double arrowAngleGrad = measurersArrowAngle * 180. / M_PI;
			String colorBG = ColorToHtml(viewerBG);
			String colorC = ColorToHtml(drillColor);
			String colorDC = ColorToHtml(drillCenterColor);
			String colorM = ColorToHtml(measurersColor);
			viewer
				("background", colorBG)
				("drillColor", colorC)
				("drillLineWidth", drillLineWidth)
				("drillCenterColor", colorDC)
				("measurersColor", colorM)
				("measurersFont", measurersFont)
				("measurersLineWidth", measurersLineWidth)
				("measurersArrowSize", measurersArrowSize)
				("measurersArrowAngle", arrowAngleGrad);
			jio("tools", tools);
			jio.Set("viewer", viewer.GetResult());
		} else {
			Value v, j = jio.Get();
			if (!j.IsNull()) {
				tools.Clear();
				ValueArray valueTools = j["tools"];
				for (Tool valueTool: valueTools) {
					tools.Add(valueTool);
				}
				Value viewer = j["viewer"];
				if (!viewer.IsNull()) {
					if (!(v = viewer["background"]).IsNull()) {
						viewerBG = ColorFromText((String)v);
					}
					if (!(v = viewer["drillColor"]).IsNull()) {
						drillColor = ColorFromText((String)v);
					}
					if (!(v = viewer["drillLineWidth"]).IsNull()) {
						drillLineWidth = v;
					}
					if (!(v = viewer["drillCenterColor"]).IsNull()) {
						drillCenterColor = ColorFromText((String)v);
					}
					if (!(v = viewer["measurersColor"]).IsNull()) {
						measurersColor = ColorFromText((String)v);
					}
					if (!(v = viewer["measurersFont"]).IsNull()) {
						JsonIO jioF(v);
						measurersFont.Jsonize(jioF);
						if (measurersFont == Null) {
							measurersFont = StdFont(20);
						}
					}
					if (!(v = viewer["measurersLineWidth"]).IsNull()) {
						measurersLineWidth = v;
					}
					if (!(v = viewer["measurersArrowSize"]).IsNull()) {
						measurersArrowSize = v;
					}
					if (!(v = viewer["measurersArrowAngle"]).IsNull()) {
						measurersArrowAngle = (double)v * M_PI / 180.;
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
	
	static void RestoreDefault() {
		Settings::viewerBG = Color(240, 240, 255);
		Settings::drillColor = Blue;
		Settings::drillLineWidth = 2;
		Settings::drillCenterColor = LtRed;
		Settings::measurersColor = Black;
		Settings::measurersFont = StdFont(20);
		Settings::measurersLineWidth = 1;
		Settings::measurersArrowSize = 20.;
		Settings::measurersArrowAngle = M_PI / 10.;
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
double Settings::measurersArrowAngle = M_PI / 10.;


#endif
