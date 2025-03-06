#ifndef _SETTINGS_WINDOW_H_
#define _SETTINGS_WINDOW_H_

#include "ToolEditor.hpp"

class SettingsWindow : public WithSettingsLayout<TopWindow> {
private:
	void updateFields() {
		eFileExt <<= Settings::fileExt;
		cViewerBG <<= Settings::viewerBG;
		cDrillColor <<= Settings::drillColor;
		eDrillLineWidth <<= round(Settings::drillLineWidth / Settings::subsampling);
		cDrillCenterColor <<= Settings::drillCenterColor;
		cMeasurersColor <<= Settings::measurersColor;
		eMeasurersFont <<= round(Settings::measurersFont.GetHeight() / Settings::subsampling);
		eMeasurersLineWidth <<= round(Settings::measurersLineWidth / Settings::subsampling);
		eMeasurersArrowSize <<= round(Settings::measurersArrowSize / Settings::subsampling);
		eMeasurersArrowAngle <<= Settings::measurersArrowAngle * 360. / M_PI;
		oViewerAutoRescale <<= Settings::viewerAutoRescale;
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
		eFileExt.WhenAction = [=] {
			Settings::fileExt = ~eFileExt;
			WhenAction();
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
			Settings::drillLineWidth = (int)((int)~eDrillLineWidth * Settings::subsampling);
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
			Settings::measurersFont.Height((int)((int)~eMeasurersFont * Settings::subsampling));
			WhenAction();
		};
		eMeasurersLineWidth.WhenAction = [=] {
			Settings::measurersLineWidth = (int)((int)~eMeasurersLineWidth * Settings::subsampling);
			WhenAction();
		};
		eMeasurersArrowSize.WhenAction = [=] {
			Settings::measurersArrowSize = (int)((int)~eMeasurersArrowSize * Settings::subsampling);
			WhenAction();
		};
		eMeasurersArrowAngle.WhenAction = [=] {
			Settings::measurersArrowAngle = (double)~eMeasurersArrowAngle * M_PI / 360.;
			WhenAction();
		};
		oViewerAutoRescale.WhenAction = [=] {
			Settings::viewerAutoRescale = ~oViewerAutoRescale;
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