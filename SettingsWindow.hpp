#ifndef _SETTINGS_WINDOW_H_
#define _SETTINGS_WINDOW_H_

#include "ToolEditor.hpp"

class SettingsWindow : public WithSettingsLayout<TopWindow> {
private:
	void updateFields() {
		cViewerBG <<= Settings::viewerBG;
		cDrillColor <<= Settings::drillColor;
		eDrillLineWidth <<= Settings::drillLineWidth;
		cDrillCenterColor <<= Settings::drillCenterColor;
		cMeasurersColor <<= Settings::measurersColor;
		eMeasurersFont <<= Settings::measurersFont.GetHeight();
		eMeasurersLineWidth <<= Settings::measurersLineWidth;
		eMeasurersArrowSize <<= Settings::measurersArrowSize;
		eMeasurersArrowAngle <<= Settings::measurersArrowAngle * 180. / M_PI;
	}
public:
	ToolEditor toolEditor;
	
	SettingsWindow() {
		CtrlLayout(*this, t_("Settings"));
		Sizeable().Zoomable();
		
		bToolEditor.WhenPush = [=] {
			if (toolEditor.IsOpen()) {
				toolEditor.SetFocus();
			} else {
				toolEditor.Open();
			}
		};
		bDefault.WhenPush = [=] {
			Settings::RestoreDefault();
			updateFields();
			WhenAction();
		};
		bSave.WhenPush = [=] {
			Settings::Save();
			WhenClose();
		};
		bCancel.WhenPush = [=] {
			WhenClose();
		};
		cViewerBG.WhenAction = [=] {
			Settings::viewerBG = ~cViewerBG;
			WhenAction();
		};
		cDrillColor.WhenAction = [=] {
			Settings::drillColor = ~cDrillColor;
			WhenAction();
		};
		eDrillLineWidth.WhenAction = [=] {
			Settings::drillLineWidth = eDrillLineWidth;
			WhenAction();
		};
		cDrillCenterColor.WhenAction = [=] {
			Settings::drillCenterColor = ~cDrillCenterColor;
			WhenAction();
		};
		cMeasurersColor.WhenAction = [=] {
			Settings::measurersColor = ~cMeasurersColor;
			WhenAction();
		};
		eMeasurersFont.WhenAction = [=] {
			Settings::measurersFont.Height(eMeasurersFont);
			WhenAction();
		};
		eMeasurersLineWidth.WhenAction = [=] {
			Settings::measurersLineWidth = eMeasurersLineWidth;
			WhenAction();
		};
		eMeasurersArrowSize.WhenAction = [=] {
			Settings::measurersArrowSize = eMeasurersArrowSize;
			WhenAction();
		};
		eMeasurersArrowAngle.WhenAction = [=] {
			Settings::measurersArrowAngle = eMeasurersArrowAngle * M_PI / 180.;
			WhenAction();
		};
	}

	void Close() {
		Settings::Load();
		TopWindow::Close();
	}
	void Open() {
		updateFields();
		TopWindow::Open();
	}
};

#endif