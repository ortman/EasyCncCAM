#ifndef _SETTINGS_HPP_
#define _SETTINGS_HPP_

#include "Tool.hpp"

#define SETTINGS_FILENAME "EasyCncCAM_settings.json"

class Settings {
private:
	static Settings settings;
public:
	void Jsonize(JsonIO& jio) {
		if (jio.IsStoring()) {
			JsonIO viewer;
			double arrowAngleGrad = measurersArrowAngle * 360. / M_PI;
			String colorBG = ColorToHtml(viewerBG);
			String colorD = ColorToHtml(drillColor);
			String colorT = ColorToHtml(threadColor);
			String colorDC = ColorToHtml(drillCenterColor);
			String colorM = ColorToHtml(measurersColor);
			viewer
				("background", colorBG)
				("drillColor", colorD)
				("drillLineWidth", drillLineWidth)
				("drillCenterColor", colorDC)
				("threadColor", colorT)
				("threadLineWidth", threadLineWidth)
				("measurersColor", colorM)
				("measurersFont", measurersFont)
				("measurersLineWidth", measurersLineWidth)
				("measurersArrowSize", measurersArrowSize)
				("measurersArrowAngle", arrowAngleGrad)
				("viewerAutoRescale", viewerAutoRescale);
			jio("tools", tools)("fileExt", fileExt);
			jio.Set("viewer", viewer.GetResult());
		} else {
			Value v, j = jio.Get();
			if (!j.IsNull()) {
				tools.Clear();
				jio("tools", tools);
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
					if (!(v = viewer["threadColor"]).IsNull()) {
						threadColor = ColorFromText((String)v);
					}
					if (!(v = viewer["threadLineWidth"]).IsNull()) {
						threadLineWidth = v;
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
						//measurersFont.Height(measurersFont.GetHeight());
					}
					if (!(v = viewer["measurersLineWidth"]).IsNull()) {
						measurersLineWidth = v;
					}
					if (!(v = viewer["measurersArrowSize"]).IsNull()) {
						measurersArrowSize = v;
					}
					if (!(v = viewer["measurersArrowAngle"]).IsNull()) {
						measurersArrowAngle = (double)v * M_PI / 360.;
					}
					if (!(v = viewer["viewerAutoRescale"]).IsNull()) {
						viewerAutoRescale = (bool)v;
					}
				}
				if (!(v = j["fileExt"]).IsNull()) {
					fileExt = (String)v;
					//TODO: check string
				}
			}
		}
	}

	static void Load() {
		RestoreDefault();
		LoadFromJsonFile(Settings::settings, GetExeDirFile(SETTINGS_FILENAME));
		if (tools.GetCount() == 0) {
			tools.Add(Tool(Tool::Drill, 3.2, 10., 6000, 0, 0, 3.2, 0.));
		}
	}
	static void Save() {
		FileOut out(GetExeDirFile(SETTINGS_FILENAME));
		out.Put(StoreAsJson(Settings::settings, true));
		out.Close();
	}
	
	static void RestoreDefault() {
		viewerBG = Color(240, 240, 255);
		drillColor = Blue;
		threadColor = Red;
		threadLineWidth = 1.;
		drillLineWidth = 2.;
		drillCenterColor = LtRed;
		measurersColor = Black;
		measurersFont = StdFont(15);
		measurersLineWidth = 1.;
		measurersArrowSize = 15.;
		measurersArrowAngle = M_PI / 10.;
		viewerAutoRescale = true;
		fileExt = "TAP";
	}
	
	static Vector<Tool> tools;
	static Color viewerBG;
	static Color drillColor;
	static double drillLineWidth;
	static Color threadColor;
	static double threadLineWidth;
	static Color drillCenterColor;
	static Color measurersColor;
	static Font measurersFont;
	static double measurersLineWidth;
	static double measurersArrowSize;
	static double measurersArrowAngle;
	static String fileExt;
	static bool viewerAutoRescale;
};

Settings Settings::settings;

Vector<Tool> Settings::tools;
Color Settings::viewerBG = Color(240, 240, 255);
Color Settings::drillColor = Blue;
double Settings::drillLineWidth = 2.;
Color Settings::drillCenterColor = LtRed;
Color Settings::threadColor = Red;
double Settings::threadLineWidth = 1.;
Color Settings::measurersColor = Black;
Font Settings::measurersFont = StdFont(15);
double Settings::measurersLineWidth = 1.;
double Settings::measurersArrowSize = 15.;
double Settings::measurersArrowAngle = M_PI / 10.;
String Settings::fileExt = "TAP";
bool Settings::viewerAutoRescale = true;


#endif
