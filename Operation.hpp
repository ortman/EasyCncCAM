#include <Core/Core.h>

#define LAYOUTFILE <EasyCncCAM/OperationTab.lay>
#include <CtrlCore/lay.h>

using namespace Upp;

#define OPERATION_SCALE 1.5

class Operation : public Moveable<Operation> {
protected:
	ImageDraw *draw = NULL;
	static double scale;
	Pointf shiftDraw = {0., 0.};
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
};

double Operation::scale = OPERATION_SCALE;

//class OperationMilling;
class OperationDrill : public Operation {
protected:
	Vector<Pointf> drills;
	double drill = 10.;
	double depth = 0.1;
	Pointf center = {0., -0.};
	bool isDrawDrillCenter = false;
	OperationDrill(){};
	
	void calculateDraw() {
		Sizef size = getDrawSize();
		this->shiftDraw = {center.x - size.cx/2, center.y - size.cy/2};
		Size imgSize = {(int)(size.cx * scale), (int)(size.cy * scale)};
		if (draw) delete draw;
		draw = new ImageDraw(imgSize);
		if (draw == NULL) return;
		draw->Alpha().DrawRect({0, 0, (int)(imgSize.cx), (int)(imgSize.cy)}, GrayColor(0));

		double diameter = (drill + 1) * scale;
		double radius = diameter / 2;
		int pen = 1;
		int x,y;
		for (Pointf pd : drills) {
			x = (int)((pd.x - shiftDraw.x) * scale - radius);
			y = (int)((pd.y - shiftDraw.y) * scale - radius);
			draw->DrawEllipse(x, y, (int)(diameter), (int)(diameter), Null, pen, Blue());
			draw->Alpha().DrawEllipse(x, y, (int)(diameter), (int)(diameter), Null, pen, GrayColor(255));
			if (isDrawDrillCenter) {
				draw->DrawLine(
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y - 3) * scale - radius),
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y + 3) * scale + radius),
					pen/*(drill < 20) ? pen : PEN_DASHDOT*/, LtRed());
				draw->DrawLine(
					(int)((pd.x - shiftDraw.x - 3) * scale - radius),
					(int)((pd.y - shiftDraw.y) * scale),
					(int)((pd.x - shiftDraw.x + 3) * scale + radius),
					(int)((pd.y - shiftDraw.y) * scale),
					pen/*(drill < 20) ? pen : PEN_DASHDOT*/, LtRed());
				draw->Alpha().DrawLine(
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y - 3) * scale - radius),
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y + 3) * scale + radius),
					pen/*(drill < 20) ? pen : PEN_DASHDOT*/, GrayColor(255));
				draw->Alpha().DrawLine(
					(int)((pd.x - shiftDraw.x - 3) * scale - radius),
					(int)((pd.y - shiftDraw.y) * scale),
					(int)((pd.x - shiftDraw.x + 3) * scale + radius),
					(int)((pd.y - shiftDraw.y) * scale),
					pen/*(drill < 20) ? pen : PEN_DASHDOT*/, GrayColor(255));
			}
		}
	}
	
public:
	
	OperationDrill(Operation* operation) {
		OperationDrill *dr = dynamic_cast<OperationDrill*>(operation);
		if (dr) {
			drill = dr->drill;
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
		calculateDrills();
	}
	
	Vector<Pointf>& getDrills() {return drills;}
	
	double getDrill() {return drill;}
	
	void setDrill(double d) {
		drill = d;
		calculateDraw();
	}
	
	double getDepth() {return depth;}
	
	void setDepth(double d) {depth = d;}
		
	virtual void calculateDrills() {};
	
	bool getDrawDrillCenter() {return isDrawDrillCenter;}
	void setDrawDrillCenter(bool b) {
		isDrawDrillCenter = b;
		calculateDraw();
	}
	
//	virtual Sizef getDrawSize() {
//		return {1., 1.};
//	}
	
	virtual String ToString() {
		return String(t_("Drill(")) + DblStr(drill) + "x" + DblStr(depth) + ")";
	}
	
};

class OperationDrillArray : public OperationDrill {
	Sizef size;
	Size count;
	
private:
	void calculateDrills() {
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
		size = {100., 50.};
		count = {3, 2};
		calculateDrills();
	}
	
	OperationDrillArray(Operation *operation) : OperationDrill(operation) {
		size = {100., 50.};
		count = {3, 2};
		calculateDrills();
	}
	
	OperationDrillArray(Pointf center, Sizef size, Size count) {
		setArray(center, size, count);
	}
	
	Sizef getSize() {return size;}
	void setSize(const Sizef s) {
		size = s;
		calculateDrills();
	}
	
	Size getCount() {return count;}
	void setCount(const Size c) {
		count = c;
		calculateDrills();
	}
	
	void setArray(Pointf center, Sizef size, Size count) {
		if (count.cx <= 0 || count.cy <= 0 || size.cx < 0. || size.cy < 0.) return;
		this->center = center;
		this->size = size;
		this->count = count;
		calculateDrills();
	}
	
	Sizef getDrawSize() {
		return {size.cx + drill + 7., size.cy + drill + 7.};
	}
	
	virtual String ToString() {
		return String(t_("DrillArray(")) + DblStr(drill) + " x " + DblStr(depth) + t_(") Array: ") + count.ToString();
	}
};

class OperationDrillRoundless : public OperationDrill {
private:
	double radius;
	int count;
	double startAngle;
	double sector;
	
	void calculateDrills() {
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
		radius = 50.;
		count = 6;
		startAngle = 0.;
		sector = 360.;
		calculateDrills();
	}
	
	OperationDrillRoundless(Operation *operation) : OperationDrill(operation) {
		radius = 50.;
		count = 6;
		startAngle = 0.;
		sector = 360.;
		calculateDrills();
	}
	
	OperationDrillRoundless(Pointf center, double radius, int count, double startAngle, double sector) {
		setRoundless(center, radius, count, startAngle, sector);
	};
	
	double getRadius() {return radius;}
	void setRadius(const double r) {
		radius = r;
		calculateDrills();
	}
	
	int getCount() {return count;}
	void setCount(const int c) {
		count = c;
		calculateDrills();
	}
	
	double getStartAngle() {return startAngle;}
	void setStartAngle(const double a) {
		startAngle = a;
		calculateDrills();
	}
	
	double getSector() {return sector;}
	void setSector(const double s) {
		sector = s;
		calculateDrills();
	}
	
	void setRoundless(Pointf center, double radius, int count, double startAngle, double sector) {
		if (radius <= 0. || count <= 0) return;
		this->center = center;
		this->radius = radius;
		this->count = count;
		this->startAngle = startAngle;
		this->sector = sector;
		calculateDrills();
	}
	
	Sizef getDrawSize() {
		double width = radius * 2. + drill + 7.;
		return {width, width};
	}
	
	virtual String ToString() {
		return String(t_("DrillRoundless(")) + DblStr(drill) + " x " + DblStr(depth) + t_(") Radius: ") + DblStr(radius) + ", Count: " + IntStr(count);
	}
};

class OperationMilling : public Operation {
private:
	double tool;
	double depth;
	
	void calculateMilling() {
	}
public:
	OperationMilling() {
		OperationMilling(8, 0.1);
	}
	OperationMilling(Operation *operation) {
		OperationDrill *dr = dynamic_cast<OperationDrill*>(operation);
		if (dr) {
			tool = dr->getDrill();
			depth = dr->getDepth();
		}
	}
	OperationMilling(double tool, double depth) {
		this->tool = tool;
		this->depth = depth;
	}
	
	double getTool() {return tool;}
	
	void setTool(double diameter) {
		tool = diameter;
		calculateMilling();
	}
	
	double getDepth() {return depth;}
	
	void setDepth(double d) {
		depth = d;
		calculateMilling();
	}
	Sizef getDrawSize() {
		return {1., 1.};
	}


};

class OperationArrayTab : public WithOperationArray<Ctrl> {
private:
	OperationDrillArray *operation = NULL;
public:
	OperationArrayTab() {
		CtrlLayout(*this);
		eDrill.WhenAction = [=] {
			if (operation) {
				operation->setDrill(eDrill);
			}
			Action();
		};
		eDepth.WhenAction = [=] {
			if (operation) {
				operation->setDepth(eDepth);
			}
			Action();
		};
		eCenterShiftX.WhenAction = [=] {
			if (operation) {
				Pointf center = operation->getCenter();
				operation->setCenter({eCenterShiftX, center.y});
			}
			Action();
		};
		eCenterShiftY.WhenAction = [=] {
			if (operation) {
				Pointf center = operation->getCenter();
				operation->setCenter({center.x, eCenterShiftY});
			}
			Action();
		};
		eWidth.WhenAction = [=] {
			if (operation) {
				Sizef size = operation->getSize();
				operation->setSize({eWidth, size.cy});
			}
			Action();
		};
		eHeight.WhenAction = [=] {
			if (operation) {
				Sizef size = operation->getSize();
				operation->setSize({size.cx, eHeight});
			}
			Action();
		};
		eCountX.WhenAction = [=] {
			if (operation) {
				Size count = operation->getCount();
				operation->setCount({eCountX, count.cy});
			}
			Action();
		};
		eCountY.WhenAction = [=] {
			if (operation) {
				Size count = operation->getCount();
				operation->setCount({count.cx, eCountY});
			}
			Action();
		};
	}
	OperationDrillArray* setOperation(Operation *operation) {
		if (operation == NULL) {
			this->operation = NULL;
			eDrill.Clear();
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
		eDrill <<= drArray->getDrill();
		eDepth <<= drArray->getDepth();
		eCenterShiftX <<= drArray->getCenter().x;
		eCenterShiftY <<= drArray->getCenter().y;
		eWidth <<= drArray->getSize().cx;
		eHeight <<= drArray->getSize().cy;
		eCountX <<= drArray->getCount().cx;
		eCountY <<= drArray->getCount().cy;
		
		return isCreateNew ? drArray : NULL;
	}
};

class OperationRoundlessTab : public WithOperationRoudless<ParentCtrl> {
private:
	OperationDrillRoundless *operation = NULL;
public:
	OperationRoundlessTab() {
		CtrlLayout(*this);
		eDrill.WhenAction = [=] {
			if (operation) {
				operation->setDrill(eDrill);
			}
			Action();
		};
		eDepth.WhenAction = [=] {
			if (operation) {
				operation->setDepth(eDepth);
			}
			Action();
		};
		eCenterShiftX.WhenAction = [=] {
			if (operation) {
				Pointf center = operation->getCenter();
				operation->setCenter({eCenterShiftX, center.y});
			}
			Action();
		};
		eCenterShiftY.WhenAction = [=] {
			if (operation) {
				Pointf center = operation->getCenter();
				operation->setCenter({center.x, eCenterShiftY});
			}
			Action();
		};
		eRadius.WhenAction = [=] {
			if (operation) {
				operation->setRadius(eRadius);
			}
			Action();
		};
		eCount.WhenAction = [=] {
			if (operation) {
				operation->setCount(eCount);
			}
			Action();
		};
		eStartAngle.WhenAction = [=] {
			if (operation) {
				operation->setStartAngle(eStartAngle);
			}
			Action();
		};
		eSector.WhenAction = [=] {
			if (operation) {
				operation->setSector(eSector);
			}
			Action();
		};
	}
	OperationDrillRoundless* setOperation(Operation *operation) {
		if (operation == NULL) {
			this->operation = NULL;
			eDrill.Clear();
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
		eDrill <<= drRoundless->getDrill();
		eDepth <<= drRoundless->getDepth();
		eCenterShiftX <<= drRoundless->getCenter().x;
		eCenterShiftY <<= drRoundless->getCenter().y;
		eRadius <<= drRoundless->getRadius();
		eCount <<= drRoundless->getCount();
		eStartAngle <<= drRoundless->getStartAngle();
		eSector <<= drRoundless->getSector();
		
		return isCreateNew ? drRoundless : NULL;
	}
};

class OperationMillingTab : public WithOperationMilling<ParentCtrl> {
private:
	OperationMilling *operation = NULL;
public:
	OperationMillingTab() {
		CtrlLayout(*this);
	}
	OperationMilling* setOperation(Operation *operation) {
		if (operation == NULL) {
			this->operation = NULL;
			eTool.Clear();
			eDepth.Clear();
			return NULL;
		}
		OperationMilling *milling = dynamic_cast<OperationMilling*>(operation);
		bool isCreateNew = (milling == NULL);
		if (isCreateNew) {
			milling = new OperationMilling(operation);
		}
		this->operation = milling;
		eTool <<= milling->getTool();
		eDepth <<= milling->getDepth();
		return isCreateNew ? milling : NULL;
	}
};