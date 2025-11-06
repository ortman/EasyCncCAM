#include "EasyCncCAM.h"
#include "GCodeGenerator.hpp"

EasyCncCAM::EasyCncCAM() {
	CtrlLayout(*this, "Easy CNC CAM");
	Icon(ResourceImage::Mill2());
	LargeIcon(ResourceImage::Mill2());
	Sizeable().Zoomable();
	sel.Type("GCode (*." + Settings::fileExt + ")", "*." + Settings::fileExt);
	sel.Type("All files (*.*)", "*.*");
	
	bShowCoordinates.SetImage(ResourceImage::Coordinates());
	bShowDrillCenters.SetImage(ResourceImage::DrillCenters());
	bShowAll.SetImage(ResourceImage::ShowAll());
	bShowMeasure.SetImage(ResourceImage::Measure());
	
	bGenerate.SetStyle(Button::StyleOk());
	bShowCoordinates.WhenPush = [=] {
			bool enabled = !viewer.GetDrawCoordinates();
			viewer.SetDrawCoordinates(enabled);
			bShowCoordinates.SetStyle(enabled ? Button::StyleOk() : Button::StyleNormal());
	};
	bShowDrillCenters.WhenPush = [=] {
			bool enabled = !viewer.GetDrawDrillCenter();
			viewer.SetDrawDrillCenter(enabled);
			bShowDrillCenters.SetStyle(enabled ? Button::StyleOk() : Button::StyleNormal());
	};
	bShowMeasure.WhenPush = [=] {
			bool enabled = !viewer.GetDrawMeasure();
			viewer.SetDrawMeasure(enabled);
			bShowMeasure.SetStyle(enabled ? Button::StyleOk() : Button::StyleNormal());
	};
	
	AddFrame(menu);
	menu.Set([=](Bar &bar) {
		bar.Sub(t_("File"), [=](Bar &bar) {
			bar.Add(t_("Generate CAM"), [=] {
				bGenerate.WhenPush();
			});
			bar.Add(t_("Tool editor"), [=] {
				if (settingsWindow.toolEditor.IsOpen()) {
					settingsWindow.toolEditor.SetFocus();
				} else {
					settingsWindow.toolEditor.Open();
				}
			});
		});
		bar.Sub(t_("View"), [=](Bar &bar) {
			bar.Add(t_("Show coordinate axis"), [=] {
				bool enabled = !viewer.GetDrawCoordinates();
				viewer.SetDrawCoordinates(enabled);
				bShowCoordinates.SetStyle(enabled ? Button::StyleOk() : Button::StyleNormal());
			}).Check(viewer.GetDrawCoordinates());
			bar.Add(t_("Show drill centers"), [=] {
				bool enabled = !viewer.GetDrawDrillCenter();
				viewer.SetDrawDrillCenter(enabled);
				bShowDrillCenters.SetStyle(enabled ? Button::StyleOk() : Button::StyleNormal());
			}).Check(viewer.GetDrawDrillCenter());
			bar.Add(t_("Show measurers"), [=] {
				bool enabled = !viewer.GetDrawMeasure();
				viewer.SetDrawMeasure(enabled);
				bShowMeasure.SetStyle(enabled ? Button::StyleOk() : Button::StyleNormal());
			}).Check(viewer.GetDrawMeasure());
		});
		bar.Add(t_("Settings"), [=] {
			if (settingsWindow.IsOpen()) {
				settingsWindow.SetFocus();
			} else {
				settingsWindow.Open();
			}
		});
	});
	
	bShowAll.WhenAction = [=] {
		viewer.showAllView();
	};
	
	bRemoveOperation.Disable();
	bAddOperation.WhenPush = [=] {
		OperationDrill *o = new OperationDrillRoundless();
		if (currentOperation != NULL) {
			o->setTool(currentOperation->getTool());
			OperationDrill* od = dynamic_cast<OperationDrill*>(currentOperation);
			if (od != NULL) o->setDepth(od->getDepth());
		} else if (Settings::tools.GetCount() > 0) {
			o->setTool(Settings::tools[0]);
		}
		operations.Add(o);
		clOperations.Add((int64)o);
		clOperations.SetCursor(clOperations.GetCount()-1);
		if (Settings::viewerAutoRescale) {
			viewer.showAllView();
		} else {
			viewer.Refresh();
		}
	};
	
	clOperations.WhenSel = [=] {
		int i = clOperations.GetCursor();
		bRemoveOperation.Enable(i>=0);
		if (i >= 0) {
			currentOperation = operations[i];
			OperationDrillArray *drArray = dynamic_cast<OperationDrillArray*>(currentOperation);
			int tab = 0;
			if (drArray == NULL) {
				OperationDrillRoundless *drRoundless = dynamic_cast<OperationDrillRoundless*>(currentOperation);
				tab = (drRoundless != NULL) ? 1 : 2;
			}
			if (tOperationSettings.Get() != tab) {
				tOperationSettings.Set(tab);
			}
			updateOperationTab();
		}
	};
	
	clOperations.WhenRemove = [=] {
		int idx = FindOperation(currentOperation);
		if (idx >= 0) {
			operations.Remove(idx);
			delete currentOperation;
			currentOperation = NULL;
		}
		if (operations.GetCount() == 0) {
			updateOperationTab();
		}
	};
	
	bRemoveOperation.WhenPush = [=] {
		int i = clOperations.GetCursor();
		if (i >= 0) {
			operations.Remove(i);
			delete currentOperation;
			currentOperation = NULL;
			clOperations.Remove(i);
			updateOperationTab();
		}
	};
	
	tOperationSettings.Add(operationArrayTab, t_("Array"));
	tOperationSettings.Add(operationRoundlessTab, t_("Roundless"));
	tOperationSettings.Add(operationMillingTab, t_("Milling")).Disable();
	
	operationArrayTab.WhenAction =
	operationRoundlessTab.WhenAction =
	operationMillingTab.WhenAction = [=] {
		if (Settings::viewerAutoRescale) {
			viewer.showAllView();
		} else {
			viewer.Refresh();
		}
		clOperations.Refresh();
	};
	
	operationArrayTab.WhenPushToolEditor =
	operationRoundlessTab.WhenPushToolEditor =
	operationMillingTab.WhenPushToolEditor  = [=] {
		if (settingsWindow.toolEditor.IsOpen()) {
			settingsWindow.toolEditor.SetFocus();
		} else {
			settingsWindow.toolEditor.Open();
		}
	};
	settingsWindow.toolEditor.WhenClose = [=] {
		operationArrayTab.updateToolList();
		operationRoundlessTab.updateToolList();
		operationMillingTab.updateToolList();
		settingsWindow.toolEditor.Close();
	};
	settingsWindow.WhenClose = [=] {
		settingsWindow.Close();
		viewer.Refresh();
	};
	settingsWindow.WhenAction = [=] {
		viewer.Refresh();
	};
	
	if (Settings::tools.GetCount() > 0) {
		OperationDrillArray *op = new OperationDrillArray();
		op->setTool(Settings::tools[0]);
		currentOperation = op;
		operations.Add(currentOperation);
		clOperations.Add((int64)currentOperation);
	}
	
	tOperationSettings.WhenAction = [=]{
		updateOperationTab();
	};
	
	viewer.setOperations(&operations);
	
	clOperations.SetCursor(clOperations.GetCount()-1);
	
	bGenerate.WhenPush = [=] {
		String dir = GCodeGenerator::GetSaveDirectory();
		if (!dir.IsEmpty()) sel.ActiveDir(dir);
		
		VectorMap<Tool, Vector<Operation*>> grOperations;
		for (Operation* o : operations) {
			grOperations.GetAdd(o->getTool()).Add(o);
		}
		for (const Vector<Operation*>& toolOperations : grOperations.GetValues()) {
			sel.Set(GCodeGenerator::GenerateFileName(toolOperations));
			if (sel.ExecuteSaveAs() && !sel.Get().IsEmpty()) {
				FileOut f;
				if (f.Open(~sel)) {
					String out = GCodeGenerator::Generate<GCodeMach3>(toolOperations);
					f.Put(out);
					f.Close();
				} else {
					ErrorOK("Can not save file!");
				}
			}
		}
	};
	bShowCoordinates.SetStyle(viewer.GetDrawCoordinates() ? Button::StyleOk() : Button::StyleNormal());
	bShowDrillCenters.SetStyle(viewer.GetDrawDrillCenter() ? Button::StyleOk() : Button::StyleNormal());
	bShowMeasure.SetStyle(viewer.GetDrawMeasure() ? Button::StyleOk() : Button::StyleNormal());
	
	viewer.showAllView();
}

EasyCncCAM::~EasyCncCAM() {
	viewer.setOperations(NULL);
	operationArrayTab.setOperation(NULL);
	operationRoundlessTab.setOperation(NULL);
	operationMillingTab.setOperation(NULL);
	currentOperation = NULL;
	int cnt = operations.GetCount();
	for (int i=cnt-1; i>=0; --i) {
		if (operations[i] != NULL) delete operations[i];
	}
}

int EasyCncCAM::FindOperation(Operation *op) {
	int cnt = operations.GetCount();
	for (int i=0; i<cnt; ++i) {
		if (operations[i] == op) return i;
	}
	return -1;
}

void EasyCncCAM::updateOperationTab() {
	Operation* newOp = NULL;
	switch (tOperationSettings.Get()) {
		case 0: {
			newOp = operationArrayTab.setOperation(currentOperation);
			break;
		}
		case 1: {
			newOp = operationRoundlessTab.setOperation(currentOperation);
			break;
		}
		case 2: {
			newOp = operationMillingTab.setOperation(currentOperation);
			break;
		}
	}
	if (newOp) { // transform operation
		int idx = FindOperation(currentOperation);
		if (idx >= 0) {
			operations[idx] = newOp;
			clOperations.Set(idx, (int64)newOp);
			delete currentOperation;
			currentOperation = newOp;
		}
	}
	tOperationSettings.Enable(currentOperation != NULL);
	if (Settings::viewerAutoRescale) {
		viewer.showAllView();
	} else {
		viewer.Refresh();
	}
}

GUI_APP_MAIN {
	StdLogSetup(LOG_COUT|LOG_FILE);
	Settings::Load();
	int lang = GetSystemLNG();
	lang = SetLNGCharset(lang, CHARSET_UTF8);
	SetLanguage(lang);
	EasyCncCAM().Run();
}
