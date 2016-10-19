#include "Fruit.h"

Fruit::Fruit(void)
{

}

Fruit::Fruit(string name)
{
	setType(name);
	if(name == "apple")
	{
		apple.setHSVmin(Scalar(30,59,50));
		apple.setHSVmax(Scalar(79,158,171));

		setColor(Scalar(0,255,0));
	}
	if(name == "banana")
	{
		apple.setHSVmin(Scalar(30,59,50));
		apple.setHSVmax(Scalar(79,158,171));

		setColor(Scalar(0,255,255));
	}
	if(name == "cherry")
	{
		apple.setHSVmin(Scalar(30,59,50));
		apple.setHSVmax(Scalar(79,158,171));

		setColor(Scalar(0,0,255));
	}
}

Fruit::~Fruit(void)
{

}

int Fruit::getXPos(){

	return Fruit::xPos;

}

void Fruit::setXPos(int x){

	Fruit::xPos = x;

}

int Fruit::getYPos(){

	return Fruit::yPos;

}

void Fruit::setYPos(int y){

	Fruit::yPos = y;

}

Scalar Fruit::getHSVmin(){

	return Fruit::HSVmin;

}

Scalar Fruit::getHSVmax(){

	return Fruit::HSVMax;
}

void Fruit::setHSVmin(Scalar min)
{
	Fruit::HSVmin = min;
}
void Fruit::setHSVmax(Scalar max)
{
	Fruit::HSVmax = max;
}