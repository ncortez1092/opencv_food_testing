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
		this->setColor(Scalar(47,173,131));
			setType(name);
	//		setBoarder();
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

//============================== Shape Detection class ====================================================

ShapeDetection::ShapeDetection()
{

}
ShapeDetection::~ShapeDetection()
{

}
string ShapeDetection::detection(vector< vector<Point> > contours)
{
	vector< Point > approx;
	string shape = "Unknown";

	double perimeter = arcLength(contours[0], true);
	approxPolyDP(contours[0], approx, 0.04 * perimeter, true);
	if (approx.size() == 3) shape = "Triangle";
	else if (approx.size() == 4)
	{
		Rect rect = boundingRect(approx);
		float area = rect.width/rect.height;
		if (area >= 0.95 && area <=1.05)
		{
		shape = "Square";
		}
		else shape = "Rectangle";
	}
	else if(approx.size() == 5) shape = "Pentigon";
	else shape = "Circle";
	return shape;
}