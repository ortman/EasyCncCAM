#ifndef _SETTINGS_WINDOW_HPP_
#define _SETTINGS_WINDOW_HPP_

#include "ToolEditor.hpp"

class SettingsWindow : public WithSettingsLayout<TopWindow> {
private:
	void updateFields() {
		eFileExt <<= Settings::fileExt;
		cViewerBG <<= Settings::viewerBG;
		cDrillColor <<= Settings::drillColor;
		eDrillLineWidth <<= Settings::drillLineWidth;
		cDrillCenterColor <<= Settings::drillCenterColor;
		cMeasurersColor <<= Settings::measurersColor;
		eMeasurersFont <<= Settings::measurersFont.GetHeight();
		eMeasurersLineWidth <<= Settings::measurersLineWidth;
		eMeasurersArrowSize <<= Settings::measurersArrowSize;
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
			Settings::drillLineWidth = ~eDrillLineWidth;
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
			Settings::measurersFont.Height(~eMeasurersFont);
			WhenAction();
		};
		eMeasurersLineWidth.WhenAction = [=] {
			Settings::measurersLineWidth = ~eMeasurersLineWidth;
			WhenAction();
		};
		eMeasurersArrowSize.WhenAction = [=] {
			Settings::measurersArrowSize = ~eMeasurersArrowSize;
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