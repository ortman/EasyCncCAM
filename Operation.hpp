#ifndef _OPERATION_H_
#define _OPERATION_H_

#include <Core/Core.h>

#define LAYOUTFILE <EasyCncCAM/OperationTab.lay>
#include <CtrlCore/lay.h>
#include "Tool.hpp"
#include "Settings.hpp"
#include "GCode.h"

using namespace Upp;

class Operation : public Moveable<Operation> {
protected:
	Tool tool;
	double depth = Null;
	double safeToolH = -5.;
		
	void DrawMeasureArrow(ImageDraw& draw, int x, int y, double angle) {
		draw.DrawPolygon({
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
	
	void DrawMeasureDiameter(ImageDraw& draw, double x, double y, double diameter, const String& text, double angle = 45.) {
		Size textSize = GetTextSize(text, Settings::measurersFont);
		double radius = diameter / 2.;
		double anglePI = angle * M_PI/180.;
		double xD = (radius + 30.) * cos(anglePI);
		double yD = (radius + 30.) * sin(anglePI);
		int leftAngle = (int)abs(angle) % 360;
		bool leftText = leftAngle > 90 && leftAngle < 270;
		draw.DrawText(
			(int)((x + xD) - (leftText ? textSize.cx : 0)),
			(int)((y - yD) - textSize.cy),
			0, text, Settings::measurersFont, Settings::measurersColor);
		draw.DrawLine(
			(int)((x - xD)),
			(int)((y + yD)),
			(int)((x + xD)),
			(int)((y - yD)),
			Settings::measurersLineWidth, Settings::measurersColor);
		draw.DrawLine(
			(int)((x + xD)),
			(int)((y - yD)),
			(int)((x + xD) + (leftText ? -textSize.cx : textSize.cx)),
			(int)((y - yD)),
			Settings::measurersLineWidth, Settings::measurersColor);
		DrawMeasureArrow(draw, (int)((x + radius * cos(anglePI))), (int)((y - radius * sin(anglePI))), -anglePI);
		DrawMeasureArrow(draw, (int)((x - radius * cos(anglePI))), (int)((y + radius * sin(anglePI))), -anglePI + M_PI);
	}
	
	void DrawMeasureRadius(ImageDraw& draw, double x, double y, double radius, const String& text, double angle = 45.) {
		Size textSize = GetTextSize(text, Settings::measurersFont);
		double anglePI = angle * M_PI/180.;
		double xD = (radius + 30.) * cos(anglePI);
		double yD = (radius + 30.) * sin(anglePI);
		int leftAngle = (int)abs(angle) % 360;
		bool leftText = leftAngle > 90 && leftAngle < 270;
		draw.DrawText(
			(int)((x + xD) - (leftText ? textSize.cx : -1)),
			(int)((y - yD) - textSize.cy),
			0, text, Settings::measurersFont, Settings::measurersColor);
		draw.DrawLine(
			(int)x,
			(int)y,
			(int)(x + xD),
			(int)(y - yD),
			Settings::measurersLineWidth, Settings::measurersColor);
		draw.DrawLine(
			(int)(x + xD),
			(int)(y - yD),
			(int)(x + xD + (leftText ? -textSize.cx : textSize.cx)),
			(int)(y - yD),
			Settings::measurersLineWidth, Settings::measurersColor);
		DrawMeasureArrow(draw, (int)(x + radius * cos(anglePI)), (int)(y - radius * sin(anglePI)), -anglePI);
	}
	
	void DrawMeasureLine(ImageDraw& draw, int x1, int y1, int x2, int y2, const String& text, double shift = DOUBLE_NULL) {
		double cx = x2 - x1;
		double cy = y2 - y1;
		double angle = atan(cy/cx);
		double len = sqrt(sqr(cx) + sqr(cy));
		double shiftL = (shift == DOUBLE_NULL) ? len * 0.1 : shift;
		double shiftV = shiftL + 5. * Settings::subsampling;
		String str;
		if (text.IsEmpty()) {
			str = DblStr(round(len));
		} else {
			str = text;
		}
		Size txtSz = GetTextSize(str, Settings::measurersFont);
		int sX = (int)(x1 + shiftL * cos(angle+M_PI_2));
		int sY = (int)(y1 + shiftL * sin(angle+M_PI_2));
		int eX = (int)(sX + len * cos(angle));
		int eY = (int)(sY + len * sin(angle));
		double txtShiftLen = (len - txtSz.cx) / 2.;
		draw.DrawText((int)(sX + txtShiftLen * cos(angle)), (int)(sY + txtShiftLen * sin(angle)), (int)(angle*-1800./M_PI), str, Settings::measurersFont, Settings::measurersColor);
		draw.DrawLine(sX, sY, eX, eY, Settings::measurersLineWidth, Settings::measurersColor);
		draw.DrawLine(x1, y1, (int)(x1 + shiftV * cos(angle+M_PI_2)), (int)(y1 + shiftV * sin(angle+M_PI_2)), Settings::measurersLineWidth, Settings::measurersColor);
		draw.DrawLine(x2, y2, (int)(x2 + shiftV * cos(angle+M_PI_2)), (int)(y2 + shiftV * sin(angle+M_PI_2)), Settings::measurersLineWidth, Settings::measurersColor);
		DrawMeasureArrow(draw, sX, sY, angle);
		DrawMeasureArrow(draw, eX, eY, angle + M_PI);
	}
	
public:
	virtual ~Operation() {};
	virtual Rectf getRect() {return {0., 0., 1., 1.};};
	virtual String ToString() {
		return t_("Operation");
	}
	virtual void Draw(ImageDraw& draw, Size& imgSz, Rectf& viewRect, bool isMeasurers = false) {
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
	double getDepth() {return depth;}
	void setDepth(double d) {depth = d;}
	virtual const String gcode(GCode& g) {
		return "";
	}
};

#endif