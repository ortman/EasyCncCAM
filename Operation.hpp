#ifndef _OPERATION_H_
#define _OPERATION_H_

#include <Core/Core.h>

#define LAYOUTFILE <EasyCncCAM/OperationTab.lay>
#include <CtrlCore/lay.h>
#include "Tool.hpp"
#include "Settings.hpp"
#include "GCode.h"

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
	double safeToolH = -5.;
		
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
		draw->Alpha().DrawPolygon(vertices, color.IsNullInstance() ? Null : GrayColor(alpha), width, outline.IsNullInstance() ? Null : GrayColor(alpha), pattern, doxor);
	}
	void DrawAlphaTextA(int x, int y, int angle, const char *text, Font font = StdFont(),
		          Color ink = DefaultInk(), int alpha = 255, int n = -1, const int *dx = NULL) {
		Size sz = GetTextSize(text, font);
		sz.cx += 2;
		//sz.cy += 2;
		double anglePI = angle * M_PI/-1800.;
		int xx = x + (int)((sz.cx) * cos(anglePI + M_PI_2));
		int yy = y + (int)((sz.cy) * sin(anglePI + M_PI_2));
		DrawAlphaPolygon({
			{x, y},
			{x + (int)round(sz.cx * cos(anglePI)), y + (int)round(sz.cx * sin(anglePI))},
			{xx + (int)round(sz.cx * cos(anglePI)), yy + (int)round(sz.cx * sin(anglePI))},
			{xx, yy}
		}, Settings::viewerBG);
		draw->DrawTextA(x, y, angle, text, font, ink, n, dx);
	}
	void DrawAlphaArc(const Rect& rc, Point start, Point end, int width, Color color, int alpha = 255) {
		draw->DrawArc(rc, start, end, width, color);
		draw->Alpha().DrawArc(rc, start, end, width, GrayColor(alpha));
	}
	void DrawMeasureArrow(int x, int y, double angle) {
		DrawAlphaPolygon({
			{x, y},
			{
				x + (int)(Settings::measurersArrowSize * cos(angle - Settings::measurersArrowAngle)),
				y + (int)(Settings::measurersArrowSize * sin(angle - Settings::measurersArrowAngle))
			}, {
				x + (int)(Settings::measurersArrowSize * cos(angle + Settings::measurersArrowAngle)),
				y + (int)(Settings::measurersArrowSize * sin(angle + Settings::measurersArrowAngle))
			}
		}, Settings::measurersColor);
	}
	void DrawMeasureDiameter(double x, double y, double diameter, double angle = 45., const String& text = "") {
		String str = text.IsEmpty() ? ("Ø" + DblStr(diameter)) : text;
		Size textSize = GetTextSize(str, Settings::measurersFont);
		double radius = diameter / 2.;
		double anglePI = angle * M_PI/180.;
		double xD = (radius + 30. / scale) * cos(anglePI);
		double yD = (radius + 30. / scale) * sin(anglePI);
		int leftAngle = (int)abs(angle) % 360;
		bool leftText = leftAngle > 90 && leftAngle < 270;
		DrawAlphaTextA(
			(int)((x + xD - shiftDraw.x) * scale - (leftText ? textSize.cx : 0)),
			(int)((y - yD - shiftDraw.y) * scale - textSize.cy),
			0, str, Settings::measurersFont, Settings::measurersColor);
		DrawAlphaLine(
			(int)((x - xD - shiftDraw.x) * scale),
			(int)((y + yD - shiftDraw.y) * scale),
			(int)((x + xD - shiftDraw.x) * scale),
			(int)((y - yD - shiftDraw.y) * scale),
			Settings::measurersLineWidth, Settings::measurersColor);
		DrawAlphaLine(
			(int)((x + xD - shiftDraw.x) * scale),
			(int)((y - yD - shiftDraw.y) * scale),
			(int)((x + xD - shiftDraw.x) * scale + (leftText ? -textSize.cx : textSize.cx)),
			(int)((y - yD - shiftDraw.y) * scale),
			Settings::measurersLineWidth, Settings::measurersColor);
		DrawMeasureArrow((int)((x + radius * cos(anglePI) - shiftDraw.x) * scale), (int)((y - radius * sin(anglePI) - shiftDraw.y) * scale), -anglePI);
		DrawMeasureArrow((int)((x - radius * cos(anglePI) - shiftDraw.x) * scale), (int)((y + radius * sin(anglePI) - shiftDraw.y) * scale), -anglePI + M_PI);
	}
	void DrawMeasureRadius(double x, double y, double radius, double angle = 45., const String& text = "") {
		String str = text.IsEmpty() ? ("R" + DblStr(radius)) : text;
		Size textSize = GetTextSize(str, Settings::measurersFont);
		double anglePI = angle * M_PI/180.;
		double xD = (radius + 30. / scale) * cos(anglePI);
		double yD = (radius + 30. / scale) * sin(anglePI);
		int leftAngle = (int)abs(angle) % 360;
		bool leftText = leftAngle > 90 && leftAngle < 270;
		DrawAlphaTextA(
			(int)((x + xD - shiftDraw.x) * scale - (leftText ? textSize.cx : -1)),
			(int)((y - yD - shiftDraw.y) * scale - textSize.cy),
			0, str, Settings::measurersFont, Settings::measurersColor);
		DrawAlphaLine(
			(int)((x - shiftDraw.x) * scale),
			(int)((y - shiftDraw.y) * scale),
			(int)((x + xD - shiftDraw.x) * scale),
			(int)((y - yD - shiftDraw.y) * scale),
			Settings::measurersLineWidth, Settings::measurersColor);
		DrawAlphaLine(
			(int)((x + xD - shiftDraw.x) * scale),
			(int)((y - yD - shiftDraw.y) * scale),
			(int)((x + xD - shiftDraw.x) * scale + (leftText ? -textSize.cx : textSize.cx)),
			(int)((y - yD - shiftDraw.y) * scale),
			Settings::measurersLineWidth, Settings::measurersColor);
		DrawMeasureArrow((int)((x + radius * cos(anglePI) - shiftDraw.x) * scale), (int)((y - radius * sin(anglePI) - shiftDraw.y) * scale), -anglePI);
	}
	void DrawMeasureLine(int x1, int y1, int x2, int y2, const String& text = "") {
		double shiftV = tool.diameter * scale * 0.8;
		double shiftL = shiftV * 0.8;
		double cx = x2 - x1;
		double cy = y2 - y1;
		double angle = atan(cy/cx);
		double len = sqrt(sqr(cx) + sqr(cy));
		String str;
		if (text.IsEmpty()) {
			str = DblStr(round(len / scale));
		} else {
			str = text;
		}
		Size txtSz = GetTextSize(str, Settings::measurersFont);
		int sX = (int)(x1 + shiftL * cos(angle+M_PI_2));
		int sY = (int)(y1 + shiftL * sin(angle+M_PI_2));
		int eX = (int)(sX + len * cos(angle));
		int eY = (int)(sY + len * sin(angle));
		double txtShiftLen = (len - txtSz.cx) / 2.;
		DrawAlphaTextA((int)(sX + txtShiftLen * cos(angle)), (int)(sY + txtShiftLen * sin(angle)), (int)(angle*-1800./M_PI), str, Settings::measurersFont, Settings::measurersColor);
		DrawAlphaLine(sX, sY, eX, eY, Settings::measurersLineWidth, Settings::measurersColor);
		DrawAlphaLine(x1, y1, (int)(x1 + shiftV * cos(angle+M_PI_2)), (int)(y1 + shiftV * sin(angle+M_PI_2)), Settings::measurersLineWidth, Settings::measurersColor);
		DrawAlphaLine(x2, y2, (int)(x2 + shiftV * cos(angle+M_PI_2)), (int)(y2 + shiftV * sin(angle+M_PI_2)), Settings::measurersLineWidth, Settings::measurersColor);
		DrawMeasureArrow(sX, sY, angle);
		DrawMeasureArrow(eX, eY, angle + M_PI);
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
	virtual const String gcode(GCode *g) {
		return "";
	}
};

double Operation::scale = OPERATION_SCALE;

#endif