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
			double arrowAngleGrad = measurersArrowAngle * 360. / M_PI;
			String colorBG = ColorToHtml(viewerBG);
			String colorD = ColorToHtml(drillColor);
			String colorT = ColorToHtml(threadColor);
			String colorDC = ColorToHtml(drillCenterColor);
			String colorM = ColorToHtml(measurersColor);
			int saveDLW = (int)(round(drillLineWidth / subsampling));
			int saveTLW = (int)(round(threadLineWidth / subsampling));
			int saveMLW = (int)(round(measurersLineWidth / subsampling));
			int saveMAS = (int)(round(measurersArrowSize / subsampling));
			Font saveMF = measurersFont;
			saveMF.Height((int)(round(saveMF.GetHeight() / subsampling)));
			viewer
				("background", colorBG)
				("drillColor", colorD)
				("drillLineWidth", saveDLW)
				("threadColor", colorT)
				("threadLineWidth", saveTLW)
				("drillCenterColor", colorDC)
				("measurersColor", colorM)
				("measurersFont", saveMF)
				("measurersLineWidth", saveMLW)
				("measurersArrowSize", saveMAS)
				("measurersArrowAngle", arrowAngleGrad)
				("viewerAutoRescale", viewerAutoRescale);
			jio("tools", tools)("fileExt", fileExt);
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
						double dlw = v;
						drillLineWidth = (int)(dlw * Settings::subsampling);
					}
					if (!(v = viewer["threadColor"]).IsNull()) {
						threadColor = ColorFromText((String)v);
					}
					if (!(v = viewer["threadLineWidth"]).IsNull()) {
						double dlw = v;
						threadLineWidth = (int)(dlw * Settings::subsampling);
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
						measurersFont.Height((int)(measurersFont.GetHeight() * Settings::subsampling));
					}
					if (!(v = viewer["measurersLineWidth"]).IsNull()) {
						double mlw = v;
						measurersLineWidth = (int)(mlw * Settings::subsampling);
					}
					if (!(v = viewer["measurersArrowSize"]).IsNull()) {
						double mas = v;
						measurersArrowSize = (int)(mas * Settings::subsampling);
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
		drillLineWidth = (int)(2. * Settings::subsampling);
		threadColor = Red;
		threadLineWidth = (int)(1. * Settings::subsampling);
		drillCenterColor = LtRed;
		measurersColor = Black;
		measurersFont = StdFont((int)(15. * Settings::subsampling));
		measurersLineWidth = (int)(1. * Settings::subsampling);
		measurersArrowSize = 15. * Settings::subsampling;
		measurersArrowAngle = M_PI / 10.;
		viewerAutoRescale = true;
		fileExt = "TAP";
	}
	
	static Vector<Tool> tools;
	static Color viewerBG;
	static Color drillColor;
	static int drillLineWidth;
	static Color threadColor;
	static int threadLineWidth;
	static Color drillCenterColor;
	static Color measurersColor;
	static Font measurersFont;
	static int measurersLineWidth;
	static double measurersArrowSize;
	static double measurersArrowAngle;
	static String fileExt;
	static double subsampling;
	static bool viewerAutoRescale;
};

Settings Settings::settings;

Vector<Tool> Settings::tools;
Color Settings::viewerBG = Color(240, 240, 255);
Color Settings::drillColor = Blue;
int Settings::drillLineWidth = 2;
Color Settings::drillCenterColor = LtRed;
Color Settings::threadColor = Red;
int Settings::threadLineWidth = 1;
Color Settings::measurersColor = Black;
Font Settings::measurersFont = StdFont(15);
int Settings::measurersLineWidth = 1;
double Settings::measurersArrowSize = 15.;
double Settings::measurersArrowAngle = M_PI / 10.;
String Settings::fileExt = "TAP";
double Settings::subsampling = 5./3.;
bool Settings::viewerAutoRescale = true;


#endif
