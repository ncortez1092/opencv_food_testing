#include "Food.h"

Food::Food(void)
{

}

Food::Food(string name)
{
	setType(name);
	if (name == "broccoli")
	{
		this->setHSVmin(Scalar(30,59,50));
		this->setHSVmax(Scalar(79,158,171));
		this->setColor(Scalar(0,255,0));
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