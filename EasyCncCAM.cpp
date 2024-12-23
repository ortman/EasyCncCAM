#include "EasyCncCAM.h"
#include "GCodeGenerator.hpp"

EasyCncCAM::EasyCncCAM() {
	CtrlLayout(*this, "Easy CNC CAM");
	Sizeable().Zoomable();
	
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
			});
			bar.Add(t_("Tool editor"), [=] {
				if (toolEditor.IsOpen()) {
					toolEditor.SetFocus();
				} else {
					toolEditor.Open();
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
		bar.Add(false, t_("Settings"), [=] {
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
		
		o->setDrawDrillCenter(viewer.GetDrawDrillCenter());
		o->setDrawMeasure(viewer.GetDrawMeasure());
		operations.Add(o);
		clOperations.Add((int64)o);
		clOperations.SetCursor(clOperations.GetCount()-1);
		viewer.Refresh();
	};
	
	clOperations.SetDisplay(operationListDisplay);
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
		viewer.Refresh();
		clOperations.Refresh();
	};
	
	operationArrayTab.WhenPushToolEditor =
	operationRoundlessTab.WhenPushToolEditor =
	operationMillingTab.WhenPushToolEditor  = [=] {
		if (toolEditor.IsOpen()) {
			toolEditor.SetFocus();
		} else {
			toolEditor.Open();
		}
	};
	toolEditor.WhenClose = [=] {
		operationArrayTab.updateToolList();
		operationRoundlessTab.updateToolList();
		operationMillingTab.updateToolList();
		toolEditor.Close();
	};
	
	if (Settings::tools.GetCount() > 0) {
		OperationDrillArray *op = new OperationDrillArray();
		op->setTool(Settings::tools[0]);
		op->setDrawMeasure(viewer.GetDrawMeasure());
		op->setDrawDrillCenter(viewer.GetDrawDrillCenter());
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
		//if (sel.ExecuteSaveAs()) {
			GCode* g = new GCodeMach3();
			String out = GCodeGenerator::Generate(operations, g);
			LOG(out);
			//ErrorOK("Oops(. This functionality is not exist!");
		//}
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
		//operations.Remove(i);
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
	switch (tOperationSettings.Get()) {
		case 0: {
			OperationDrill *op = operationArrayTab.setOperation(currentOperation);
			if (op) { // transform operation
				op->setDrawDrillCenter(viewer.GetDrawDrillCenter());
				op->setDrawMeasure(viewer.GetDrawMeasure());
				int idx = FindOperation(currentOperation);
				if (idx >= 0) {
					operations[idx] = op;
					clOperations.Set(idx, (int64)op);
					delete currentOperation;
					currentOperation = op;
				//} else {
				//	operations.Add(op);
				}
			}
			break;
		}
		case 1: {
			OperationDrill *op = operationRoundlessTab.setOperation(currentOperation);
			if (op) { // transform operation
				op->setDrawDrillCenter(viewer.GetDrawDrillCenter());
				op->setDrawMeasure(viewer.GetDrawMeasure());
				int idx = FindOperation(currentOperation);
				if (idx >= 0) {
					operations[idx] = op;
					clOperations.Set(idx, (int64)op);
					delete currentOperation;
					currentOperation = op;
				//} else {
				//	operations.Add(op);
				}
			}
			break;
		}
		case 2: {
			OperationMilling *op = operationMillingTab.setOperation(currentOperation);
			if (op) { // transform operation
				op->setDrawMeasure(viewer.GetDrawMeasure());
				int idx = FindOperation(currentOperation);
				if (idx >= 0) {
					operations[idx] = op;
					clOperations.Set(idx, (int64)op);
					delete currentOperation;
					currentOperation = op;
				//} else {
				//	operations.Add(op);
				}
			}
			break;
		}
	}
	viewer.Refresh();
}

void EasyCncCAM::OperationListDisplay::Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const {
	PaintBackground(w, r, q, ink, paper, style);
	Operation* op = (Operation*)(int64)q;
	if (op != NULL) {
		String s = op->ToString();
		int tcy = GetTextSize(s, StdFont()).cy;
	  //int c = r.Deflated(1).GetSize().cy;
	  int c = 0;
		w.DrawText(r.left + c + 3, r.top+(r.GetHeight() - tcy) / 2, s, StdFont(), ink);
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
