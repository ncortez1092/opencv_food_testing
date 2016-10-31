#include "FoodAdditional.h"

Food::Food(void)
{

}

Food::Food(string name)
{

	if (name == "Broccoli")
	{
		this->setHSVmin(Scalar(28, 189, 41));
		this->setHSVmax(Scalar(49, 255, 105));
		this->setColor(Scalar(0,0,255));
			setType(name);
	}
		
	if (name == "Carrot")
	{
		this->setHSVmin(Scalar(11, 197, 124));
		this->setHSVmax(Scalar(14, 255, 211));
		this->setColor(Scalar(255,255,255));
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