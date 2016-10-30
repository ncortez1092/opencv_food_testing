#include "FoodAdditional.h"

Food::Food(void)
{

}

Food::Food(string name)
{
	setType(name);
	if (name == "spatula")
	{
		this->setHSVmin(Scalar(80, 84, 150));
		this->setHSVmax(Scalar(114, 167, 255));
		this->setColor(Scalar(255,0,0));
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