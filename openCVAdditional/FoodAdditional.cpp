#include "FoodAdditional.h"

Food::Food(void)
{

}

Food::Food(string name)
{

	if (name == "Broccoli")
	{
		this->setHSVmin(Scalar(29, 151, 51));
		this->setHSVmax(Scalar(36, 255, 130));
		this->setColor(Scalar(0,255,0));
			setType(name);
	}
		
	if (name == "Carrot")
	{
		this->setHSVmin(Scalar(10, 174, 62));
		this->setHSVmax(Scalar(23, 255, 157));
		this->setColor(Scalar(0,255,255));
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

Scalar Food::getHSVmin(){

	return Food::HSVmin;

}

Scalar Food::getHSVmax(){

	return Food::HSVmax;
}

void Food::setHSVmin(Scalar min)
{
	Food::HSVmin = min;
}
void Food::setHSVmax(Scalar max)
{
	Food::HSVmax = max;
}