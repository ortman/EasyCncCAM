#ifndef _OPERATION_DRILL_ARRAY_H_
#define _OPERATION_DRILL_ARRAY_H_

#include "OperationDrill.hpp"

class OperationDrillArray : public OperationDrill {
	Sizef size;
	Size count;
	
private:
	void calculate() {
		drills.clear();
		double sx, sy, startX, startY;
		if (count.cx > 1) {
			sx = size.cx / (count.cx-1);
			startX = center.x - size.cx/2;
		} else {
			sx = 0;
			startX = center.x;
		}
		if (count.cy > 1) {
			sy = size.cy / (count.cy-1);
			startY = center.y - size.cy/2;
		} else {
			sy = 0;
			startY = center.y;
		}
		for (int x = 0; x < count.cx; ++x) {
			for (int y = 0; y < count.cy; ++y) {
				drills.Add({startX + x * sx, startY + y * sy});
			}
		}
		calculateDraw();
	}
	
public:
	OperationDrillArray() {
		size = {30., 20.};
		count = {3, 2};
		calculate();
	}
	
	OperationDrillArray(Operation *operation) : OperationDrill(operation) {
		size = {30., 20.};
		count = {3, 2};
		calculate();
	}
	
	OperationDrillArray(Pointf center, Sizef size, Size count) {
		setArray(center, size, count);
	}
	
	Sizef getSize() {return size;}
	void setSize(const Sizef s) {
		size = s;
		calculate();
	}
	
	Size getCount() {return count;}
	void setCount(const Size c) {
		count = c;
		calculate();
	}
	
	void setArray(Pointf center, Sizef size, Size count) {
		if (count.cx <= 0 || count.cy <= 0 || size.cx < 0. || size.cy < 0.) return;
		this->center = center;
		this->size = size;
		this->count = count;
		calculate();
	}
	
	Sizef getDrawSize() {
		return {size.cx + tool.diameter + OPERATION_DRAW_OFFSET, size.cy + tool.diameter + OPERATION_DRAW_OFFSET};
	}
	
	virtual String ToString() {
		return String(t_("Drilling")) + "(" + DblStr(tool.diameter) + " x " + DblStr(depth) + ") " + t_("Array:") + " " + count.ToString();
	}
	
	virtual void calculateDraw() {
		OperationDrill::calculateDraw();
		if (isDrawMeasure) {
			double delta = (tool.diameter + OPERATION_DRAW_OFFSET) / 2.;
			int xLeft = (int)(delta * scale);
			int xRight = (int)((size.cx+delta) * scale);
			int yTop = (int)(delta * scale);
			int yBottom = (int)((((count.cy > 1) ? size.cy : size.cy / 2. ) + delta) * scale);
			if (count.cx > 1) {
				DrawMeasureLine(xLeft, yBottom, xRight, yBottom, DblStr(size.cx));
				if (count.cy > 1) {
					DrawAlphaPolygon({
						{xLeft, yTop},
						{xRight, yTop},
						{xRight, yBottom},
						{xLeft, yBottom}},
						Null, Settings::measurersLineWidth, Settings::measurersColor);
					//DrawMeasureLine(xLeft, yBottom, xRight, yTop);
				}
			}
			if (count.cy > 1) {
				DrawMeasureLine(xRight, yBottom, xRight, yTop, DblStr(size.cy));
			}
		}
	}
};

class OperationArrayTab : public WithOperationArray<Ctrl> {
private:
	OperationDrillArray *operation = NULL;
public:
	Event<> WhenPushToolEditor;
	OperationArrayTab() {
		CtrlLayout(*this);
		updateToolList();
		bToolEditor.WhenPush = [=] {
			WhenPushToolEditor();
		};
		dlTool.WhenAction = [=] {
			int i = dlTool.GetIndex();
			if (operation && i >= 0) {
				operation->setTool(dlTool.GetKey(i));
				Action();
			}
		};
		eDepth.WhenAction = [=] {
			double d = eDepth;
			if (operation && d > 0.) {
				operation->setDepth(d);
				Action();
			}
		};
		eCenterShiftX.WhenAction = [=] {
			if (operation) {
				Pointf center = operation->getCenter();
				operation->setCenter({eCenterShiftX, center.y});
				Action();
			}
		};
		eCenterShiftY.WhenAction = [=] {
			if (operation) {
				Pointf center = operation->getCenter();
				operation->setCenter({center.x, eCenterShiftY});
				Action();
			}
		};
		eWidth.WhenAction = [=] {
			double w = eWidth;
			if (operation && w > 0.) {
				Sizef size = operation->getSize();
				operation->setSize({w, size.cy});
				Action();
			}
		};
		eHeight.WhenAction = [=] {
			double h = eHeight;
			if (operation && h > 0.) {
				Sizef size = operation->getSize();
				operation->setSize({size.cx, h});
				Action();
			}
		};
		eCountX.WhenAction = [=] {
			int c = eCountX;
			if (operation && c > 0) {
				Size count = operation->getCount();
				operation->setCount({c, count.cy});
				Action();
			}
		};
		eCountY.WhenAction = [=] {
			int c = eCountY;
			if (operation && c > 0) {
				Size count = operation->getCount();
				operation->setCount({count.cx, c});
				Action();
			}
		};
	}
	OperationDrillArray* setOperation(Operation *operation) {
		if (operation == NULL) {
			this->operation = NULL;
			dlTool = -1;
			eDepth.Clear();
			eCenterShiftX.Clear();
			eCenterShiftY.Clear();
			eWidth.Clear();
			eHeight.Clear();
			eCountX.Clear();
			eCountY.Clear();
			return NULL;
		}
		OperationDrillArray *drArray = dynamic_cast<OperationDrillArray*>(operation);
		bool isCreateNew = (drArray == NULL);
		if (isCreateNew) {
			drArray = new OperationDrillArray(operation);
		}
		this->operation = drArray;
		dlTool <<= drArray->getTool();
		eDepth <<= drArray->getDepth();
		eCenterShiftX <<= drArray->getCenter().x;
		eCenterShiftY <<= drArray->getCenter().y;
		eWidth <<= drArray->getSize().cx;
		eHeight <<= drArray->getSize().cy;
		eCountX <<= drArray->getCount().cx;
		eCountY <<= drArray->getCount().cy;
		
		return isCreateNew ? drArray : NULL;
	}
	void updateToolList() {
		dlTool.Clear();
		for (Tool &t : Settings::tools) {
			dlTool.Add(t, t.ToString());
		}
		if (operation != NULL) dlTool <<= operation->getTool();
	}
};

#endif