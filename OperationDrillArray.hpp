#ifndef _OPERATION_DRILL_ARRAY_HPP_
#define _OPERATION_DRILL_ARRAY_HPP_

#include "OperationDrill.hpp"

class OperationDrillArray : public OperationDrill {
public:
	typedef enum {
		LenAll = 1,
		LenBetween
	} LenType;

private:
	Sizef size;
	Size count;
	bool onCenter;
	LenType widthType;
	LenType heightType;
	
	void calculate() {
		drills.clear();
		double sx, sy, startX, startY;
		if (count.cx > 1) {
			sx = size.cx / (count.cx - 1);
			startX = center.x - (onCenter ? size.cx / 2. : 0.);
		} else {
			sx = 0;
			startX = center.x;
		}
		if (count.cy > 1) {
			sy = size.cy / (count.cy - 1);
			startY = center.y - (onCenter ? size.cy / 2. : 0.);
		} else {
			sy = 0.;
			startY = center.y;
		}
		for (int x = 0; x < count.cx; ++x) {
			for (int y = 0; y < count.cy; ++y) {
				drills.Add({startX + x * sx, startY + y * sy});
			}
		}
	}
	
public:
	OperationDrillArray() {
		size = {30., 20.};
		count = {3, 2};
		onCenter = false;
		widthType = LenAll;
		heightType = LenAll;
		calculate();
	}
	
	OperationDrillArray(Operation *operation) : OperationDrill(operation) {
		size = {30., 20.};
		count = {3, 2};
		onCenter = false;
		widthType = LenAll;
		heightType = LenAll;
		calculate();
	}
	
	OperationDrillArray(Pointf center, Sizef size, Size count) {
		onCenter = false;
		widthType = LenAll;
		heightType = LenAll;
		setArray(center, size, count);
	}
	
	Sizef getSize() {return size;}
	void setSize(const Sizef s) {
		size = s;
		calculate();
	}
	
	Rectf getRect() {
		if (onCenter) {
			double width2 = (size.cx + tool.diameter) / 2.;
			double height2 = (size.cy + tool.diameter) / 2.;
			return {
				-width2 + center.x,
				-height2 + center.y,
				width2 + center.x,
				height2 + center.y
			};
		} else {
			double toolRadius = tool.diameter / 2.;
			return {
				center.x - toolRadius,
				center.y - toolRadius,
				center.x + toolRadius + size.cx,
				center.y + toolRadius + size.cy
			};
		}
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
	
	bool isOnCenter() {
		return onCenter;
	}
	
	void SetOnCenter(bool c) {
		onCenter = c;
		calculate();
	}
	
	LenType getWidthType() {
		return widthType;
	}
	
	void setWidthType(LenType t) {
		widthType = t;
	}
	
	LenType getHeightType() {
		return heightType;
	}
	
	void setHeightType(LenType t) {
		heightType = t;
	}
	
	virtual String ToString() {
		return String(t_("Drilling")) + "(" + DblStr(tool.diameter) + " x " + DblStr(depth) + ") " + t_("Array:") + " " + count.ToString();
	}
	
	virtual void Draw(ImageDraw& draw, Size& imgSz, Rectf& viewRect, bool isMeasurers = false, bool isDrawDrillCenter = false) {
		OperationDrill::Draw(draw, imgSz, viewRect, isMeasurers, isDrawDrillCenter);
		if (isMeasurers) {
			Sizef viewSize = viewRect.GetSize();
			double scale = min(imgSz.cx / viewSize.cx, imgSz.cy / viewSize.cy);
			double shiftX = viewRect.left * scale - (imgSz.cx - viewSize.cx * scale) / 2.;
			double shiftY = viewRect.top * scale - (imgSz.cy - viewSize.cy * scale) / 2.;
			
			int xLeft = (int)((center.x - (onCenter ? size.cx / 2. : 0.)) * scale - shiftX);
			int xRight = xLeft + (int)(size.cx * scale);
			int yBottom =  imgSz.cy - (int)((center.y - (onCenter ? size.cy / 2. : 0.)) * scale - shiftY);
			int yTop = yBottom - (int)(size.cy * scale);
			if (count.cx > 1) {
				if (count.cy > 1) {
					draw.DrawPolygon({
						{xLeft, yTop},
						{xRight, yTop},
						{xRight, yBottom},
						{xLeft, yBottom}},
						Null, Settings::measurersLineWidth, Settings::measurersColor);
				} else {
					yBottom =  imgSz.cy - (int)(center.y * scale - shiftY);
				}
				DrawMeasureLine(draw, xLeft, yBottom, xRight, yBottom, DblStr(size.cx), tool.diameter / 2. * scale + 10. * Settings::subsampling);
			} else {
				xRight = (int)(center.x * scale - shiftX);
			}
			if (count.cy > 1) {
				DrawMeasureLine(draw, xRight, yBottom, xRight, yTop, DblStr(size.cy), tool.diameter / 2. * scale + 10. * Settings::subsampling);
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
		dlWidthType.Add({
			{OperationDrillArray::LenAll, t_("Total width")},
			{OperationDrillArray::LenBetween, t_("Width between drills")}
		});
		dlWidthType.WhenAction = [=] {
			int t = dlWidthType.GetData();
			if (operation && t > 0) {
				OperationDrillArray::LenType type = (OperationDrillArray::LenType)t;
				OperationDrillArray::LenType currType = operation->getWidthType();
				if (type != currType) {
					operation->setWidthType(type);
					Sizef size = operation->getSize();
					if (type == OperationDrillArray::LenAll) {
						operation->setSize({size.cx / (operation->getCount().cx - 1), size.cy});
					} else {
						operation->setSize({size.cx * (operation->getCount().cx - 1), size.cy});
					}
				}
			}
			Action();
		};
		eWidth.WhenAction = [=] {
			double w = eWidth;
			if (operation && w > 0.) {
				Sizef size = operation->getSize();
				if (operation->getWidthType() == OperationDrillArray::LenAll) {
					operation->setSize({w, size.cy});
				} else {
					operation->setSize({w * (operation->getCount().cx - 1), size.cy});
				}
				Action();
			}
		};
		dlHeightType.Add({
			{OperationDrillArray::LenAll, t_("Total height")},
			{OperationDrillArray::LenBetween, t_("Height between drills")}
		});
		dlHeightType.WhenAction = [=] {
			int t = dlHeightType.GetData();
			if (operation && t > 0) {
				OperationDrillArray::LenType type = (OperationDrillArray::LenType)t;
				OperationDrillArray::LenType currType = operation->getHeightType();
				if (type != currType) {
					operation->setHeightType(type);
					Sizef size = operation->getSize();
					if (type == OperationDrillArray::LenAll) {
						operation->setSize({size.cx, size.cy / (operation->getCount().cy - 1)});
					} else {
						operation->setSize({size.cx, size.cy * (operation->getCount().cy - 1)});
					}
				}
			}
			Action();
		};
		eHeight.WhenAction = [=] {
			double h = eHeight;
			if (operation && h > 0.) {
				Sizef size = operation->getSize();
				if (operation->getWidthType() == OperationDrillArray::LenAll) {
					operation->setSize({size.cx, h});
				} else {
					operation->setSize({size.cx, h * (operation->getCount().cy - 1)});
				}
				Action();
			}
		};
		eCountX.WhenAction = [=] {
			int c = eCountX;
			if (operation && c > 0) {
				Size count = operation->getCount();
				operation->setCount({c, count.cy});
				if (operation->getWidthType() == OperationDrillArray::LenBetween && count.cx > 1 && c > 1) {
					Sizef s = operation->getSize();
					operation->setSize({s.cx / (count.cx - 1) * (c - 1), s.cy});
				}
				Action();
			}
		};
		eCountY.WhenAction = [=] {
			int c = eCountY;
			if (operation && c > 0) {
				Size count = operation->getCount();
				operation->setCount({count.cx, c});
				if (operation->getHeightType() == OperationDrillArray::LenBetween && count.cy > 1 && c > 1) {
					Sizef s = operation->getSize();
					operation->setSize({s.cx, s.cy / (count.cy - 1) * (c - 1)});
				}
				Action();
			}
		};
		oOnCenter.WhenAction = [=] {
			if (operation) {
				operation->SetOnCenter(~oOnCenter);
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
			oOnCenter <<= false;
			dlWidthType <<= OperationDrillArray::LenAll;
			dlHeightType <<= OperationDrillArray::LenAll;
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
		oOnCenter <<= drArray->isOnCenter();
		dlWidthType <<= drArray->getWidthType();
		dlHeightType <<= drArray->getHeightType();
		
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