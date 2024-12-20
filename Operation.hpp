#include <Core/Core.h>

#define LAYOUTFILE <EasyCncCAM/OperationTab.lay>
#include <CtrlCore/lay.h>
#include "Tool.hpp"

using namespace Upp;

#define OPERATION_SCALE 1.5
#define OPERATION_DRAW_OFFSET 30.

class Operation : public Moveable<Operation> {
protected:
	ImageDraw *draw = NULL;
	static double scale;
	Pointf shiftDraw = {0., 0.};
	Tool tool;
	bool isDrawMeasure = false;
	static Font measureFont;
		
	void DrawAlphaLine(int x1, int y1, int x2, int y2, int width, Color color, int alpha = 255) {
			draw->DrawLine(x1, y1, x2, y2, width, color);
			draw->Alpha().DrawLine(x1, y1, x2, y2, width, GrayColor(alpha));
	}
	void DrawAlphaEllipse(int x, int y, int cx, int cy, Color color, int pen, Color pencolor, int alpha = 255) {
		draw->DrawEllipse(x, y, cx, cy, color, pen, pencolor);
		draw->Alpha().DrawEllipse(x, y, cx, cy, color, pen, GrayColor(alpha));
	}
	void DrawAlphaPolygon(const Vector<Point>& vertices,
			Color color = DefaultInk(), int width = 0, Color outline = Null, int alpha = 255,
			uint64 pattern = 0, Color doxor = Null) {
		draw->DrawPolygon(vertices, color, width, outline, pattern, doxor);
		draw->Alpha().DrawPolygon(vertices, color, width, GrayColor(alpha), pattern, doxor);
	}
	void DrawAlphaTextA(int x, int y, int angle, const char *text, Font font = StdFont(),
		          Color ink = DefaultInk(), int alpha = 255, int n = -1, const int *dx = NULL) {
		draw->DrawTextA(x, y, angle, text, font, ink, n, dx);
		draw->Alpha().DrawTextA(x, y, angle, text, font, GrayColor(alpha), n, dx);
	}
	void DrawAlphaArc(const Rect& rc, Point start, Point end, int width, Color color, int alpha = 255) {
		draw->DrawArc(rc, start, end, width, color);
		draw->Alpha().DrawArc(rc, start, end, width, GrayColor(alpha));
	}
	void DrawMeasureDiameter(double x, double y, double diameter, double angle) {
		double radius = diameter / 2 + 2;
		double xD = radius * cos(angle * M_PI/180);
		double yD = radius * sin(angle * M_PI/180);
		String text = DblStr(diameter);
		Size textSize = GetTextSize(text, measureFont);
		DrawAlphaLine(
			(int)((x - xD - shiftDraw.x) * scale),
			(int)((y + yD - shiftDraw.y) * scale),
			(int)((x + xD - shiftDraw.x) * scale),
			(int)((y - yD - shiftDraw.y) * scale),
			1, Black());
		DrawAlphaLine(
			(int)((x + xD - shiftDraw.x) * scale),
			(int)((y - yD - shiftDraw.y) * scale),
			(int)((x + xD - shiftDraw.x) * scale + textSize.cx),
			(int)((y - yD - shiftDraw.y) * scale),
			1, Black());
		DrawAlphaTextA(
			(int)((x + xD - shiftDraw.x) * scale),
			(int)((y - yD - shiftDraw.y) * scale - textSize.cy - 1),
			0, text, measureFont, Black());
	}
public:
	virtual ~Operation() {
		if (draw) delete draw;
	};
	ImageDraw* getDraw() {return draw;}
	Pointf& getDrawShift() {return shiftDraw;}
	virtual Sizef getDrawSize() {return {1., 1.};};
	static double getScale() {return scale;}
	static void setScale(double s) {
		scale = s;
	}
	virtual String ToString() {
		return t_("Operation");
	}
	virtual void calculateDraw() {
	}
	virtual void calculate() {
	}
	Tool& getTool() {
		return tool;
	}
	void setTool(const Tool& tool) {
		this->tool = tool;
		calculate();
	}
	bool getDrawMeasure() { return isDrawMeasure; }
	void setDrawMeasure(bool isDraw = true) {
		isDrawMeasure = isDraw;
		calculateDraw();
	}
};

Font Operation::measureFont = StdFont(20);

double Operation::scale = OPERATION_SCALE;

class OperationDrill : public Operation {
protected:
	Vector<Pointf> drills;
	double depth = 0.1;
	Pointf center = {0., 0.};
	bool isDrawDrillCenter = false;
	OperationDrill(){};
	
	virtual void calculateDraw() {
		Sizef size = getDrawSize();
		this->shiftDraw = {center.x - size.cx/2, center.y - size.cy/2};
		Size imgSize = {(int)(size.cx * scale), (int)(size.cy * scale)};
		if (draw) delete draw;
		draw = new ImageDraw(imgSize);
		if (draw == NULL) return;
		//draw->Alpha().DrawRect({0, 0, (int)(imgSize.cx), (int)(imgSize.cy)}, GrayColor(100));
		//draw->DrawRect({0, 0, (int)(imgSize.cx), (int)(imgSize.cy)}, LtGreen());

		double diameter = (tool.diameter + 1) * scale;
		double radius = diameter / 2;
		int x,y;
		for (Pointf pd : drills) {
			x = (int)((pd.x - shiftDraw.x) * scale - radius);
			y = (int)((pd.y - shiftDraw.y) * scale - radius);
			DrawAlphaEllipse(x, y, (int)(diameter), (int)(diameter), Null, 2, Blue());
			if (isDrawDrillCenter) {
				DrawAlphaLine(
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y - 3) * scale - radius),
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y + 3) * scale + radius),
					1/*(drill < 20) ? pen : PEN_DASHDOT*/, LtRed());
				DrawAlphaLine(
					(int)((pd.x - shiftDraw.x - 3) * scale - radius),
					(int)((pd.y - shiftDraw.y) * scale),
					(int)((pd.x - shiftDraw.x + 3) * scale + radius),
					(int)((pd.y - shiftDraw.y) * scale),
					1/*(drill < 20) ? pen : PEN_DASHDOT*/, LtRed());
			}
		}
		if (isDrawMeasure) {
			if (drills.GetCount() > 0) {
				Pointf &d = drills[0];
				DrawMeasureDiameter(d.x, d.y, tool.diameter, 45);
			}
		}
	}
	
public:
	
	OperationDrill(Operation* operation) {
		tool = operation->getTool();
		OperationDrill *dr = dynamic_cast<OperationDrill*>(operation);
		if (dr) {
			depth = dr->depth;
			center = dr->center;
			isDrawDrillCenter = dr->isDrawDrillCenter;
		} else {
//			OperationMilling *ml = dynamic_cast<OperationMilling*>(operation);
//			if (ml) {
//				drill = ml->getTool();
//				depth = ml->getDepth();
//			}
		}
	}

	
	~OperationDrill(){};
	
	Pointf getCenter() {return center;}
	void setCenter(const Pointf p) {
		center = p;
		calculate();
	}
	
	Vector<Pointf>& getDrills() {return drills;}
		
	double getDepth() {return depth;}
	
	void setDepth(double d) {depth = d;}
		
	//virtual void calculateDrills() {};
	
	bool getDrawDrillCenter() {return isDrawDrillCenter;}
	void setDrawDrillCenter(bool b = true) {
		isDrawDrillCenter = b;
		calculate();
	}
		
	virtual String ToString() {
		return String(t_("Drill(")) + DblStr(tool.diameter) + "x" + DblStr(depth) + ")";
	}
	
};

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
			double delta = (tool.diameter + OPERATION_DRAW_OFFSET)/2;
			int pen = 1;
			if (count.cx > 1 && count.cy > 1) {
				DrawAlphaPolygon({
					{(int)(delta * scale), (int)(delta * scale)},
					{(int)((size.cx+delta) * scale), (int)(delta * scale)},
					{(int)((size.cx+delta) * scale), (int)((size.cy+delta) * scale)},
					{(int)(delta * scale), (int)((size.cy+delta) * scale)}},
					Null, pen, Black());
			}
		}
	}
};

class OperationDrillRoundless : public OperationDrill {
private:
	double radius;
	int count;
	double startAngle;
	double sector;
	
	void calculate() {
		drills.clear();
		double shiftAngle;
		if (sector < 360. && count > 1) {
			shiftAngle = M_2PI * sector / 360. / (count- 1);
		} else {
			shiftAngle = M_2PI / count;
		}
		for (int i=0; i<count; ++i) {
			drills.Add({
				center.x + radius * cos(startAngle * M_PI/180 + shiftAngle*i),
				center.y + radius * sin(startAngle * M_PI/180 + shiftAngle*i)
			});
		}
		calculateDraw();
	}
	
public:
	OperationDrillRoundless() {
		radius = 30.;
		count = 6;
		startAngle = 0.;
		sector = 360.;
		calculate();
	}
	
	OperationDrillRoundless(Operation *operation) : OperationDrill(operation) {
		radius = 30.;
		count = 6;
		startAngle = 0.;
		sector = 360.;
		calculate();
	}
	
	OperationDrillRoundless(Pointf center, double radius, int count, double startAngle, double sector) {
		setRoundless(center, radius, count, startAngle, sector);
	};
	
	double getRadius() {return radius;}
	void setRadius(const double r) {
		radius = r;
		calculate();
	}
	
	int getCount() {return count;}
	void setCount(const int c) {
		count = c;
		calculate();
	}
	
	double getStartAngle() {return startAngle;}
	void setStartAngle(const double a) {
		startAngle = a;
		calculate();
	}
	
	double getSector() {return sector;}
	void setSector(const double s) {
		sector = s;
		calculate();
	}
	
	void setRoundless(Pointf center, double radius, int count, double startAngle, double sector) {
		if (radius <= 0. || count <= 0) return;
		this->center = center;
		this->radius = radius;
		this->count = count;
		this->startAngle = startAngle;
		this->sector = sector;
		calculate();
	}
	
	Sizef getDrawSize() {
		double width = radius * 2. + tool.diameter + OPERATION_DRAW_OFFSET;
		return {width, width};
	}
	
	virtual String ToString() {
		return String(t_("Drilling")) + "(" + DblStr(tool.diameter) + " x " + DblStr(depth) + ") " + t_("Radius:") + " " + DblStr(radius) + ", " + t_("Count:") + " " + IntStr(count);
	}
	
	virtual void calculateDraw() {
		OperationDrill::calculateDraw();
		if (isDrawMeasure) {
			int pen = 1;
			if (sector > 0. && sector < 360.) {
				double radiusArc = radius/2;
				Size ds = getDrawSize();
				Pointf c = Pointf(
					(ds.cx+0.5)/2,
					(ds.cy+0.5)/2
				);
				DrawAlphaArc(RectC(
						(int)((c.x - radiusArc) * scale),
						(int)((c.y - radiusArc) * scale),
						(int)(radiusArc * 2 * scale),
						(int)(radiusArc * 2 * scale)
					),
					Point(
						(int)((c.x + radiusArc * cos((sector + startAngle) * M_PI/180)) * scale),
						(int)((c.y + radiusArc * sin((sector + startAngle) * M_PI/180)) * scale)
					),
					Point(
						(int)((c.x + radiusArc * cos(startAngle * M_PI/180)) * scale),
						(int)((c.y + radiusArc * sin(startAngle * M_PI/180)) * scale)
					),
					pen, Black
				);
				DrawAlphaLine(
					(int)(c.x * scale),
					(int)(c.y * scale),
					(int)((c.x + radius * cos((sector + startAngle) * M_PI/180)) * scale),
					(int)((c.y + radius * sin((sector + startAngle) * M_PI/180)) * scale),
					pen, Black
				);
				DrawAlphaLine(
					(int)(c.x * scale),
					(int)(c.y * scale),
					(int)((c.x + radius * cos(startAngle * M_PI/180)) * scale),
					(int)((c.y + radius * sin(startAngle * M_PI/180)) * scale),
					pen, Black
				);
				DrawAlphaArc(RectC(
						(int)((c.x - radius) * scale),
						(int)((c.y - radius) * scale),
						(int)(radius * 2 * scale),
						(int)(radius * 2 * scale)
					),
					Point(
						(int)((c.x + radius * cos((sector + startAngle) * M_PI/180)) * scale),
						(int)((c.y + radius * sin((sector + startAngle) * M_PI/180)) * scale)
					),
					Point(
						(int)((c.x + radius * cos(startAngle * M_PI/180)) * scale),
						(int)((c.y + radius * sin(startAngle * M_PI/180)) * scale)
					),
					pen, Black
				);
			} else {
				double delta = (tool.diameter + OPERATION_DRAW_OFFSET)/2;
				DrawAlphaEllipse(
					(int)(delta * scale), (int)(delta * scale),
					(int)((radius*2) * scale), (int)((radius*2) * scale),
					Null, pen, Black());
				DrawMeasureDiameter(center.x, center.y, radius*2, 45);
			}
		}
	}
};

class OperationMilling : public Operation {
private:
	double depth;
	
	void calculate() {
	}
public:
	OperationMilling() {
		Tool tool;
		OperationMilling(tool, 0.1);
	}
	OperationMilling(Operation *operation) {
		OperationDrill *dr = dynamic_cast<OperationDrill*>(operation);
		if (dr) {
			tool = dr->getTool();
			depth = dr->getDepth();
		}
	}
	OperationMilling(Tool &tool, double depth) {
		this->tool = tool;
		this->depth = depth;
	}
	
	double getDepth() {return depth;}
	
	void setDepth(double d) {
		depth = d;
		calculate();
	}
	Sizef getDrawSize() {
		return {1., 1.};
	}

	virtual String ToString() {
		return String(t_("Milling")) + "(" + DblStr(tool.diameter) + " x " + DblStr(depth) + ")";
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
			if (operation) {
				operation->setDepth(eDepth);
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
			if (operation) {
				Sizef size = operation->getSize();
				operation->setSize({eWidth, size.cy});
				Action();
			}
		};
		eHeight.WhenAction = [=] {
			if (operation) {
				Sizef size = operation->getSize();
				operation->setSize({size.cx, eHeight});
				Action();
			}
		};
		eCountX.WhenAction = [=] {
			if (operation) {
				Size count = operation->getCount();
				operation->setCount({eCountX, count.cy});
				Action();
			}
		};
		eCountY.WhenAction = [=] {
			if (operation) {
				Size count = operation->getCount();
				operation->setCount({count.cx, eCountY});
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
		for (Tool &t : Tool::tools) {
			dlTool.Add(t, t.ToString());
		}
		if (operation != NULL) dlTool <<= operation->getTool();
	}
};

class OperationRoundlessTab : public WithOperationRoudless<ParentCtrl> {
private:
	OperationDrillRoundless *operation = NULL;
public:
	Event<> WhenPushToolEditor;
	OperationRoundlessTab() {
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
			if (operation) {
				operation->setDepth(eDepth);
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
		eRadius.WhenAction = [=] {
			if (operation) {
				operation->setRadius(eRadius);
				Action();
			}
		};
		eCount.WhenAction = [=] {
			if (operation) {
				operation->setCount(eCount);
				Action();
			}
		};
		eStartAngle.WhenAction = [=] {
			if (operation) {
				operation->setStartAngle(eStartAngle);
				Action();
			}
		};
		eSector.WhenAction = [=] {
			if (operation) {
				operation->setSector(eSector);
				Action();
			}
		};
	}
	OperationDrillRoundless* setOperation(Operation *operation) {
		if (operation == NULL) {
			this->operation = NULL;
			dlTool = -1;
			eDepth.Clear();
			eCenterShiftX.Clear();
			eCenterShiftY.Clear();
			eRadius.Clear();
			eCount.Clear();
			eStartAngle.Clear();
			eSector.Clear();
			return NULL;
		}
		OperationDrillRoundless *drRoundless = dynamic_cast<OperationDrillRoundless*>(operation);
		bool isCreateNew = (drRoundless == NULL);
		if (isCreateNew) {
			drRoundless = new OperationDrillRoundless(operation);
		}
		this->operation = drRoundless;
		dlTool <<= drRoundless->getTool();
		eDepth <<= drRoundless->getDepth();
		eCenterShiftX <<= drRoundless->getCenter().x;
		eCenterShiftY <<= drRoundless->getCenter().y;
		eRadius <<= drRoundless->getRadius();
		eCount <<= drRoundless->getCount();
		eStartAngle <<= drRoundless->getStartAngle();
		eSector <<= drRoundless->getSector();
		
		return isCreateNew ? drRoundless : NULL;
	}
	void updateToolList() {
		dlTool.Clear();
		for (Tool &t : Tool::tools) {
			dlTool.Add(t, t.ToString());
		}
		if (operation != NULL) dlTool <<= operation->getTool();
	}
};

class OperationMillingTab : public WithOperationMilling<ParentCtrl> {
private:
	OperationMilling *operation = NULL;
public:
	Event<> WhenPushToolEditor;
	OperationMillingTab() {
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
			if (operation) {
				operation->setDepth(eDepth);
				Action();
			}
		};
	}
	OperationMilling* setOperation(Operation *operation) {
		if (operation == NULL) {
			this->operation = NULL;
			dlTool = -1;
			eDepth.Clear();
			return NULL;
		}
		OperationMilling *milling = dynamic_cast<OperationMilling*>(operation);
		bool isCreateNew = (milling == NULL);
		if (isCreateNew) {
			milling = new OperationMilling(operation);
		}
		this->operation = milling;
		dlTool <<= milling->getTool();
		eDepth <<= milling->getDepth();
		return isCreateNew ? milling : NULL;
	}
	void updateToolList() {
		dlTool.Clear();
		for (Tool &t : Tool::tools) {
			dlTool.Add(t, t.ToString());
		}
		if (operation != NULL) dlTool <<= operation->getTool();
	}
};