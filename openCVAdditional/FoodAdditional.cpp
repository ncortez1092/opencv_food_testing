#include "FoodAdditional.h"

Food::Food(void)
{

}

Food::Food(string name)
{

	if (name == "Bread")
	{
		this->setHSVmin1(Scalar(16, 176, 89));
		this->setHSVmax1(Scalar(25, 255, 157));
		this->setHSVmin2(Scalar(15,.60*255,.55*255));
		this->setHSVmin2(Scalar(25,.70*255,.75*255));
		this->setHSVmin3(Scalar(16, 216, 66));
		this->setHSVmax3(Scalar(21, 255, 105));
		this->setColor(Scalar(47,173,131));
			setType(name);
	}
	if (name == "PotSticker")
	{
		this->setHSVmin1(Scalar(18,195,94));
		this->setHSVmax1(Scalar(24,255,159));
		this->setHSVmin2(Scalar(20, 138, 66));
		this->setHSVmin2(Scalar(26,255,140));
		this->setHSVmin3(Scalar(18,187,65));
		this->setHSVmin3(Scalar(24,255,176));
		this->setColor(Scalar(47,170,130));
		setType(name);
	}
		if (name == "Meat")
	{
		this->setHSVmin1(Scalar(6, 211, 44));
		this->setHSVmax1(Scalar(14,255, 84));
		this->setHSVmin2(Scalar(10, 223, 107));
		this->setHSVmin2(Scalar(14,255,132));
		this->setHSVmin3(Scalar(7,192,55));
		this->setHSVmin3(Scalar(12,255,89));
		this->setColor(Scalar(0,10,200));
		setType(name);
	}
		if (name == "TriangleMeat")
	{
		this->setHSVmin1(Scalar());
		this->setHSVmax1(Scalar());
		this->setHSVmin2(Scalar());
		this->setHSVmin2(Scalar());
		this->setHSVmin3(Scalar());
		this->setHSVmin3(Scalar());
		this->setColor(Scalar(0,170,130));
		setType(name);
	}
}

Food::~Food(void)
{

}

int Food::getXPos(){

	return Food::xPos;

}

void Food::setXPos(int x){

	Food::xPos = x;

}

int Food::getYPos(){

	return Food::yPos;

}

void Food::setYPos(int y){

	Food::yPos = y;

}

void Food::setArea(float Area)
{
	Food::Area = Area;
}

Scalar Food::getHSVmin1(){

	return Food::HSVmin1;

}

Scalar Food::getHSVmax1(){

	return Food::HSVmax1;
}

Scalar Food::getHSVmin2(){

	return Food::HSVmin2;

}

Scalar Food::getHSVmax2(){

	return Food::HSVmax2;
}
Scalar Food::getHSVmin3(){

	return Food::HSVmin2;

}

Scalar Food::getHSVmax3(){

	return Food::HSVmax2;
}

void Food::setHSVmin1(Scalar min)
{
	Food::HSVmin1 = min;
}
void Food::setHSVmax1(Scalar max)
{
	Food::HSVmax1 = max;
}
void Food::setHSVmin2(Scalar min)
{
	Food::HSVmin2 = min;
}
void Food::setHSVmax2(Scalar max)
{
	Food::HSVmax2 = max;
}
void Food::setHSVmin3(Scalar min)
{
	Food::HSVmin2 = min;
}
void Food::setHSVmax3(Scalar max)
{
	Food::HSVmax2 = max;
}


//============================== Shape Detection class ====================================================

ShapeDetection::ShapeDetection()
{

}
ShapeDetection::~ShapeDetection()
{

}
string ShapeDetection::detection(vector<vector<Point> > contours)
{
	vector< Point > approx;
	vector< Point > contours2;
	contours2 = contours[0];
	string shape = "Unknown";

	double perimeter = arcLength(contours2, true);
	approxPolyDP(contours2, approx, 0.04 * perimeter, true);
	if (approx.size() == 3) shape = "Triangle";
	else if (approx.size() == 4) shape = "Rectangle";
	/*{
		Rect rect = boundingRect(approx);
		float area = rect.width/rect.height;
		if (area >= 0.95 && area <=1.05)
		{
		shape = "Square";
		}
		else shape = "Rectangle";
	}*/// Didnt want to differenetiate between the two yet
	else if(approx.size() == 5) shape = "Pentigon";
	else shape = "Circle";
	return shape;
}