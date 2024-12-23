#ifndef _SETTINGS_WINDOW_H_
#define _SETTINGS_WINDOW_H_

#include "ToolEditor.hpp"

class SettingsWindow : public WithSettingsLayout<TopWindow> {
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
		bCancel.WhenPush = [=] {
			Close();
		};
		bSave.WhenPush = [=] {
			Settings::Save();
			WhenClose();
		};
	}
};

#endif