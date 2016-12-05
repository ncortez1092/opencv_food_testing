#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <math.h>
#include <time.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
using namespace std;
using namespace cv;



class Food
{
public:
	Food(void);
	~Food(void);

	Food(string name);

	int getXPos();
	void setXPos(int x);

	int getYPos();
	void setYPos(int y);

	float getArea(){return Area;}
	void setArea(float Area);

	Scalar getHSVmin1();
	Scalar getHSVmax1();
	Scalar getHSVmin2();
	Scalar getHSVmax2();
	Scalar getHSVmin3();
	Scalar getHSVmax3();

	void setHSVmin1(Scalar min);
	void setHSVmax1(Scalar max);
	void setHSVmin2(Scalar min);
	void setHSVmax2(Scalar max);
	void setHSVmin3(Scalar min);
	void setHSVmax3(Scalar max);

	string getType(){return type;}
	void setType(string t){type = t;}

	Scalar getColor(){return Color;}
	void setColor(Scalar c){Color = c;}

	void setShape(string t){shape = t;}
	string getShape(){return shape;}


	void setBoarder();
private:

	int xPos, yPos;
	float Area;
	string type, shape;
	cv::Scalar HSVmin1,HSVmax1,HSVmin2,HSVmax2, HSVmin3, HSVmax3;
	cv::Scalar Color;

};

class ShapeDetection
{
public:
	ShapeDetection(void);
	~ShapeDetection(void);
	string detection(vector< vector<Point> > contours);

};





//==================================== Initializations ===================================================
// This is used for accessing our port connected to gantry
static const char* portName = "/dev/ttyACM0"; 
	int serialPort;
	struct termios portOptions;

char buf[64];
int res;
volatile int STOP = true;
volatile int STOP_NUM = true;
volatile int STOP_STRINGS = true;
// This is for our text file
fstream foodValues;

VideoCapture capture(1);

bool ifClicked, mouseHasMoved, rectangleIsSelected;
string whichStrings;

//These are used for HSV detection, and will be used on our trackbars
int Hmin = 0;
int Hmax = 256;
int Smin = 0;
int Smax = 256;
int Vmin = 0;
int Vmax = 256;
int Bmin = 0;
int Bmax = 255;
int Rmin = 0;
int Rmax = 255;
int Gmin = 0;
int Gmax = 255;
int Bavg = 125;
int Gavg = 125;
int Ravg = 125;
int counttest = 0;
int ingredientNum;
vector<int> H_ROI_ALLVALUES, S_ROI_ALLVALUES, V_ROI_ALLVALUES, B_ROI_ALLVALUES, R_ROI_ALLVALUES, G_ROI_ALLVALUES;
vector<Scalar> BGRVals, LocalBGRVals,
			   HSVMins1, HSVMins2, HSVMins3,
			   HSVMaxs1, HSVMaxs2, HSVMaxs3,
			   LocalHSVMins1, LocalHSVMins2, LocalHSVMins3,
			   LocalHSVMaxs1, LocalHSVMaxs2, LocalHSVMaxs3;


// Size of the webcam feed
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

// Other obvious declarations
const int MAX_NUM_OBJECTS = 50; // Max number of objects
const int MIN_OBJECT_AREA = 10*10; // Min area of an object we will bother detecting
const int MAX_OBJECT_AREA = (FRAME_HEIGHT*FRAME_WIDTH) / 1.5; // Max area of an object we will detect

// Window names for our program
const string windowOriginal1 = "Original1";
const string windowOriginal2 = "Original2";
const string windowHSV = "HSV image";
const string windowThresh = "Threshold Image";
const string windowThresh1 = "Thresholded Image1";
const string windowThresh2 = "Thresholded Image2";
const string windowOps = "Morphological Operations";
const string windowControls = "Controls";
char Name[20];
string gline;
vector<string> Names, LocalNames;
vector<bool> checkIfOnList;

// All classes
ShapeDetection SD;
Food Bread("Bread");
Food Carrot("Carrot");
Food Broccoli("Broccoli");
Food PotSticker("PotSticker");
Food Meat("Meat");
vector<Food> Foodies;

// Matrix to store each frame of the webcam feed
Mat liveFeed, temp, gray;
Mat thresholdImg, thresholdImg1, thresholdImg2, thresholdImg3, thresholdImg4, thresholdImg5, thresholdImg6, thresholdImg7, thresholdImg8, threshShape;
Mat thresholdImg9, thresholdImg10, thresholdImg11;
Mat HSV;
vector<Mat> ThresholdImgs1, ThresholdImgs2, ThresholdImgs3, ThresholdImgs4;
vector<Scalar> HSVMINS, HSVMAXS, BGRVALS;
Point initialClickPoint, endClickPoint;
Rect rectangleROI;
// Function Prototypes

char const* relayCoords(Food);
void on_trackbar(int, void*){}
string intToString(int);
void drawObject(vector<Food>, Mat &);
void morphOps(Mat&);
void trackingObject(Food& , Mat, Mat, Mat &);
void trackingObjects(vector<Food>&, Mat, Mat, Mat&);
void setUpSerial();
string floatToString(float);
bool checkIfFileOpen(fstream&);
//====================== End Initializations =====================================================



void makeCropAndCircle(int cropHeight = 280, int cropWidth = 210, int cropX = 185, int cropY = 60)
{

		capture.read(temp);
		cv::Mat mask = cv::Mat::zeros( temp.rows, temp.cols, CV_8UC1 );
		Point center = Point(330, 196);
		float radius = 85;
		cropX = 330 - radius;
		cropY = 196 - radius;
		cropHeight = 2*radius;
		cropWidth = 2*radius;
		Rect myCropROI(cropX, cropY , cropHeight , cropWidth);
		//VideoWriter video("out.avi",CV_FOURCC('X','V','I','D'),30,Size(cropWidth,cropHeight),true);
		//video.open("out.avi",CV_FOURCC('X','V','I','D'),30,Size(cropWidth,cropHeight),true);
		circle( mask, center, radius, Scalar(255,255,255), -1, 8, 0 ); //-1 means filled
		temp.copyTo( liveFeed, mask );
		liveFeed = liveFeed(myCropROI);
		resize(liveFeed, liveFeed, Size(400,400));
		cvtColor(liveFeed,HSV,COLOR_BGR2HSV);
		float ratio = FRAME_HEIGHT/cropHeight;
}


void makeCalibrationRectangle(int event, int x, int y, int flags, void* userdata)
{
	Mat* liveFeed = (Mat*)userdata;

		if (event == CV_EVENT_LBUTTONDOWN && ifClicked == false)
		{
			//keep track of initial point clicked
			initialClickPoint = cv::Point(x, y);
			//user has begun dragging the mouse
			ifClicked = true;
		}
		/* user is dragging the mouse */
		if (event == CV_EVENT_MOUSEMOVE && ifClicked == true)
		{
			//keep track of current mouse point
			endClickPoint = cv::Point(x, y);
			//user has moved the mouse while clicking and dragging
			mouseHasMoved = true;
		}
		/* user has released left button */
		if (event == CV_EVENT_LBUTTONUP && ifClicked == true)
		{
			//set rectangle ROI to the rectangle that the user has selected
			rectangleROI = Rect(initialClickPoint, endClickPoint);

			//reset boolean variables
			ifClicked = false;
			mouseHasMoved = false;
			rectangleIsSelected = true;
		}

		if (event == CV_EVENT_RBUTTONDOWN)
		{
			Hmin = 0;
			Smin = 0;
			Vmin = 0;
			Hmax = 255;
			Smax = 255;
			Vmax = 255;

		}
		if (event == CV_EVENT_MBUTTONDOWN)
		{
			//Add code to save the rectangle into a pushbacked vector
		}

}

void calibrateVideo()
{
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH); // Set width
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT); // Set height
	namedWindow(windowOriginal1);
	//namedWindow(windowOriginal2);
	setMouseCallback(windowOriginal1, makeCalibrationRectangle, &liveFeed);
	ifClicked = false; 
	mouseHasMoved = false; 
	rectangleIsSelected = false;
}


string relayCoord(Food theFood, Mat& frame){
	string oneMessage;
			int row = frame.rows; // x pixels, for example it will be 640 for a 640 x 480 img
			int col = frame.cols; // y pixels, for example it will be 480 for a 640 x 480 img
			int y = theFood.getXPos();
			int x = theFood.getYPos();
			float Area = theFood.getArea();
			int xCoord, yCoord;
			string myChar;

			string type = theFood.getType();
			if (type == "Broccoli") {myChar = 'q';}
			if (type == "Carrot") {myChar = 'c';}
			if (type == "Bread") {myChar = 'b';}
			if (type == "PotSticker") {myChar = 'p';}
			if (type == "Meat") {myChar = 's';}
			if (type == "TriangleMeat") {myChar = 't';}
			float norm = 0.50;
			int x_0 = col/2;
			int y_0 = row/2;

			if (x < x_0 && y < y_0) // Quadrant 1
				{
					xCoord = (x - x_0)*norm;
					yCoord = (y_0 - y)*norm;
				}
			else if (x > x_0 && y < y_0) // Quadrant 2
				{
					xCoord = (x - x_0)*norm;
					yCoord = (y_0 - y)*norm;
				}
			else if (x < x_0 && y > y_0) // Quadrant 3
				{
					xCoord = (x - x_0)*norm; 
					yCoord = (y_0 - y)*norm;
				}
			else if (x > x_0 && y > y_0) // Quadrant 4
				{
					xCoord = (x - x_0)*norm;
					yCoord = (y_0 - y)*norm;
				}
			else if (x == x_0 && y == y_0)
				{
					xCoord = 0;
					yCoord = 0;
				}
			float scale = frame.rows*frame.cols;
			Area = Area/(scale-scale/6); // This will be a percentage relative to the area of the pot
			Area = (int)100*Area;
			if (Area < 1) Area = 1;
			//Area = (int)(Area*100)/100.0F;

			//if (xCoord >= -75 && yCoord >= -75 && xCoord <= 75 && yCoord <= 75)
			//{
				if(xCoord >= 66) xCoord = 66;
				if(xCoord <= -66) xCoord = -66;
				if(yCoord >= 66) yCoord = 66;
				if(yCoord <= -66) yCoord = 66;
				oneMessage = type  + " " + "\n" + " " + intToString(xCoord) + ", " + intToString(yCoord) + ", " + intToString(Area);
			//string message = intToString(theFood.getXPos())+ ", " + intToString(theFood.getYPos());
			//char const* pchar = message.at(i).c_str();
			
			//}

					return oneMessage;
}


vector<string> relayCoords(vector<Food> theFood, Mat& frame){
	string oneMessage;
	vector<string> message;
	for (int i = 0; i < theFood.size(); i++)
	// This will return a char const* of the coords to whatever food we are using. It will then be passed to the
	// Serial after. Which is why it needs to be char const*.
		{	
			int row = frame.rows; // x pixels, for example it will be 640 for a 640 x 480 img
			int col = frame.cols; // y pixels, for example it will be 480 for a 640 x 480 img
			int y = theFood.at(i).getXPos();
			int x = theFood.at(i).getYPos();
			float Area = theFood.at(i).getArea();
			int xCoord, yCoord;
			string myChar;

			string type = theFood.at(i).getType();
			if (type == "Broccoli") {myChar = 'q';}
			if (type == "Carrot") {myChar = 'c';}
			if (type == "Bread") {myChar = 'b';}
			if (type == "PotSticker") {myChar = 'p';}
			if (type == "Meat") {myChar = 's';}
			if (type == "TriangleMeat") {myChar = 't';}
			float norm = 0.50;
			int x_0 = col/2;
			int y_0 = row/2;

			if (x < x_0 && y < y_0) // Quadrant 1
				{
					xCoord = (x - x_0)*norm;
					yCoord = (y_0 - y)*norm;
				}
			else if (x > x_0 && y < y_0) // Quadrant 2
				{
					xCoord = (x - x_0)*norm;
					yCoord = (y_0 - y)*norm;
				}
			else if (x < x_0 && y > y_0) // Quadrant 3
				{
					xCoord = (x - x_0)*norm; 
					yCoord = (y_0 - y)*norm;
				}
			else if (x > x_0 && y > y_0) // Quadrant 4
				{
					xCoord = (x - x_0)*norm;
					yCoord = (y_0 - y)*norm;
				}
			else if (x == x_0 && y == y_0)
				{
					xCoord = 0;
					yCoord = 0;
				}
			float scale = frame.rows*frame.cols;
			Area = Area/(scale-scale/6); // This will be a percentage relative to the area of the pot
			Area = (int)100*Area;
			if (Area < 1) Area = 1;
			//Area = (int)(Area*100)/100.0F;

			if (xCoord >= -75 && yCoord >= -75 && xCoord <= 75 && yCoord <= 75)
			{
				if(xCoord >= 66) xCoord = 66;
				if(xCoord <= -66) xCoord = -66;
				if(yCoord >= 66) yCoord = 66;
				if(yCoord <= -66) yCoord = 66;
				oneMessage = type  + " " + "\n" + " " + intToString(xCoord) + ", " + intToString(yCoord) + ", " + intToString(Area);
			//string message = intToString(theFood.getXPos())+ ", " + intToString(theFood.getYPos());
			//char const* pchar = message.at(i).c_str();
				message.push_back(oneMessage);
			}
		}

					return message;
}
/*
char const* relayCoords(Food theFood, Mat& frame){

Below is the picture coords 4 quadrants. The idea is we need
 to change them to a similar format as that of GantryProto1.h
 The below is after the initial shift (x - 320, y - 240)
---------------------------------------------|
	Q1				|		Q2				 |
					|						 |
					|						 |
		x < 0		|		x > 0			 |
		y < 0		|		y < 0			 |
					|						 |
					|						 |
----------------------------------------------
	Q3				|		Q4				 |
					|						 |
					|						 |
					|						 |
		x < 0		|		x > 0			 |
		y > 0		|		y > 0			 |
					|						 |
					|						 |
----------------------------------------------

The Gantry is of the following format


---------------------------------------------|
	Q1				|		Q2				 |
					|						 |
					|						 |
		x < 0		|		x < 0			 |
		y > 0		|		y < 0			 |
					|						 |
					|						 |
----------------------------------------------
	Q3				|		Q4				 |
					|						 |
					|						 |
					|						 |
		x > 0		|		x > 0			 |
		y > 0		|		y < 0			 |
					|						 |
					|						 |
----------------------------------------------


So we must do cross quadrant checks. Also keep in mind that this is considering that the picture is taken
wrt the front of the robot, facing the cookpot's face.







// This will return a char const* of the coords to whatever food we are using. It will then be passed to the
// Serial after. Which is why it needs to be char const*.
		int row = frame.rows; // x pixels, for example it will be 640 for a 640 x 480 img
		int col = frame.cols; // y pixels, for example it will be 480 for a 640 x 480 img
		int y = theFood.getXPos();
		int x = theFood.getYPos();
		float Area = theFood.getArea();
		int xCoord, yCoord;
		string myChar;

		string type = theFood.getType();
		if (type == "Broccoli") {myChar = 'q';}
		if (type == "Carrot") {myChar = 'c';}
		if (type == "Bread") {myChar = 'b';}
		if (type == "PotSticker") {myChar = 'p';}
		if (type == "Meat") {myChar = 's';}
		if (type == "TriangleMeat") {myChar = 't';}
		float norm = 0.50;
		int x_0 = col/2;
		int y_0 = row/2;

		if (x < x_0 && y < y_0) // Quadrant 1
			{
				xCoord = (x - x_0)*norm;
				yCoord = (y_0 - y)*norm;
			}
		else if (x > x_0 && y < y_0) // Quadrant 2
			{
				xCoord = (x - x_0)*norm;
				yCoord = (y_0 - y)*norm;
			}
		else if (x < x_0 && y > y_0) // Quadrant 3
			{
				xCoord = (x - x_0)*norm; 
				yCoord = (y_0 - y)*norm;
			}
		else if (x > x_0 && y > y_0) // Quadrant 4
			{
				xCoord = (x - x_0)*norm;
				yCoord = (y_0 - y)*norm;
			}
		else if (x == x_0 && y == y_0)
			{
				xCoord = 0;
				yCoord = 0;
			}
		float scale = frame.rows*frame.cols;
		Area = Area/(scale-scale/6); // This will be a percentage relative to the area of the pot
		Area = (int)100*Area;
		//Area = (int)(Area*100)/100.0F;
		string message = myChar + ", " + intToString(xCoord) + ", " + intToString(yCoord) + ", " + intToString(Area);
		//string message = intToString(theFood.getXPos())+ ", " + intToString(theFood.getYPos());
		char const* pchar = message.c_str();
		return pchar;
}*/
string floatToString(float number)
{
	stringstream s;
	s << number;
	return s.str();
}

string intToString(int number){


	stringstream s;
	s << number;
	return s.str();
}

void drawObject(vector<Food> theFood, Mat &frame){


	for(int i=0; i < theFood.size(); i++)
	{

	cv::circle(frame,cv::Point(theFood.at(i).getXPos(),theFood.at(i).getYPos()),10,cv::Scalar(0,100,100));
	cv::putText(frame,intToString(theFood.at(i).getXPos())+ " , " + intToString(theFood.at(i).getYPos()),cv::Point(theFood.at(i).getXPos(),theFood.at(i).getYPos()+20),1,1,theFood.at(i).getColor());
	cv::putText(frame,theFood.at(i).getType(),cv::Point(theFood.at(i).getXPos(),theFood.at(i).getYPos()-30),3,1,theFood.at(i).getColor(), 2);
	Rect rect(theFood.at(i).getXPos() - sqrt(theFood.at(i).getArea()/2), theFood.at(i).getYPos() - sqrt(theFood.at(i).getArea()/2), sqrt(3*theFood.at(i).getArea()), sqrt(3*theFood.at(i).getArea()));
	rectangle(liveFeed,rect,theFood.at(i).getColor());
	}


}
void morphOps(Mat& thresholdImg){

	// This will dilate then erode the image as to eliminate noise

	Mat Ele_erode = getStructuringElement( MORPH_RECT,Size(3,3));
	Mat Ele_dilate = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(thresholdImg,thresholdImg,Ele_erode);
	erode(thresholdImg,thresholdImg,Ele_erode);


	dilate(thresholdImg,thresholdImg,Ele_dilate);
	dilate(thresholdImg,thresholdImg,Ele_dilate);
}



void trackingObject(Food& theFood,Mat thresholdImg,Mat HSV, Mat &liveFeed){
	vector<Food> Ingredients;
		Mat tempImg;
		thresholdImg.copyTo(tempImg);
		//these two vectors needed for output of findContours
		vector< vector<Point> > contours;
		vector<Vec4i> hierarchy;
		//find contours of filtered image using openCV findContours function
		findContours(tempImg,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
		//use moments method to find our filtered object
		double refArea = 0;
		bool objectFound = false;
		if (hierarchy.size() > 0) 
		{
			int num_of_Objects = hierarchy.size();
			//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
			if(num_of_Objects<MAX_NUM_OBJECTS)
			{
				for (int index = 0; index >= 0; index = hierarchy[index][0]) 
				{

					Moments moment = moments((cv::Mat)contours[index]);
					float area = moment.m00;

					//if the area is less than 20 px by 20px then it is probably just noise
					//if the area is the same as the 3/2 of the image size, probably just a bad filter
					//we only want the object with the largest area so we safe a reference area each
					//iteration and compare it to the area in the next iteration.
					if(area>MIN_OBJECT_AREA)
					{

						theFood.setArea(area);
						theFood.setXPos(moment.m10/area);	
						theFood.setYPos(moment.m01/area);
						theFood.setType(theFood.getType());
						theFood.setColor(theFood.getColor());
					
						Ingredients.push_back(theFood);

						objectFound = true;

					}
					else objectFound = false;
					if(objectFound == true)
					{
						putText(liveFeed, "Object Found", Point(0, 50), 1, 1, Scalar(0,0,255), 2);
						drawObject(Ingredients,liveFeed);
						theFood.setXPos(moment.m10/area);	
						theFood.setYPos(moment.m01/area);
						theFood.setType(theFood.getType());
						theFood.setColor(theFood.getColor());
					}
					else putText(liveFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
		}
	}
}
}
void trackingObjectCalibration(Mat thresholdImg,Mat HSV, Mat &liveFeed){

	vector<Food> Ingredients;

	Mat tempImg;
	thresholdImg.copyTo(tempImg);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(tempImg,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int num_of_Objects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if(num_of_Objects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA){

					Food New_Food;

					New_Food.setXPos(moment.m10/area);	
					New_Food.setYPos(moment.m01/area);
					New_Food.setType("New");
				
					Ingredients.push_back(New_Food);

					objectFound = true;

				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				putText(liveFeed, "Tracking", Point(0, 50), 1, 1, Scalar(0,0,255), 2);
				drawObject(Ingredients,liveFeed);}

		}else putText(liveFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}
void trackingObjects(vector<Food>& theFood, vector<Mat>& thresholdImg,Mat HSV, Mat &liveFeed){
	//vector<Food> Ingredients;
for (int i = 0; i < theFood.size(); i++)
	{
		Mat tempImg;
		thresholdImg.at(i).copyTo(tempImg);
		//imshow("In TrackingObjects", tempImg);
		//these two vectors needed for output of findContours
		vector< vector<Point> > contours;
		vector<Vec4i> hierarchy;
		//find contours of filtered image using openCV findContours function
		findContours(tempImg,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
		//use moments method to find our filtered object
		double refArea = 0;
		bool objectFound = false;
		if (hierarchy.size() > 0) 
		{
			int num_of_Objects = hierarchy.size();
			//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
			if(num_of_Objects<MAX_NUM_OBJECTS)
			{
				for (int index = 0; index >= 0; index = hierarchy[index][0]) 
				{

					Moments moment = moments((cv::Mat)contours[index]);
					float area = moment.m00;

					//if the area is less than 20 px by 20px then it is probably just noise
					//if the area is the same as the 3/2 of the image size, probably just a bad filter
					//we only want the object with the largest area so we safe a reference area each
					//iteration and compare it to the area in the next iteration.
					if(area>MIN_OBJECT_AREA)
					{

						theFood.at(i).setArea(area);
						theFood.at(i).setXPos(moment.m10/area);	
						theFood.at(i).setYPos(moment.m01/area);
						//theFood.at(i).setType(theFood.getType());
						//theFood.at(i).setColor(theFood.getColor());
					
						//Ingredients.push_back(theFood);

						objectFound = true;

					}
					else objectFound = false;
					if(objectFound == true)
					{
						putText(liveFeed, "Object Found", Point(0, 50), 1, 1, Scalar(0,0,255), 2);
						drawObject(theFood,liveFeed);
						//theFood.setXPos(moment.m10/area);	
						//theFood.setYPos(moment.m01/area);
						//theFood.setType(theFood.getType());
						//theFood.setColor(theFood.getColor());}
					}
					else putText(liveFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,0),2);
		}
	}
}
}
}
void setUpSerial()
{
//==================== Serial Initialization =============================================================


	serialPort = open(portName, O_RDWR | O_NOCTTY);
	tcgetattr(serialPort, &portOptions);
	tcflush(serialPort, TCIOFLUSH);
	cfsetispeed(&portOptions, B9600);
	cfsetospeed(&portOptions, B9600);
 	// c_cflag contains a few important things- CLOCAL and CREAD, to prevent
 	//   this program from "owning" the port and to enable receipt of data.
 	//   Also, it holds the settings for number of data bits, parity, stop bits,
	//   and hardware flow control. 
	//portOptions.c_cflag = CLOCAL | CREAD | ~CSIZE | CS8 | ~PARENB | ~CSTOPB;
	//portOptions.c_iflag = IGNPAR | ICRNL | BRKINT;
	portOptions.c_oflag = 0;
	portOptions.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    portOptions.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

 	tcsetattr(serialPort, TCSANOW, &portOptions);
  	tcflush(serialPort, TCIOFLUSH);



//==================== End Serial Initialization ============================================================
}


void recordHSV(Mat frame, Mat HSVCalibration)
{
	if (mouseHasMoved == false && rectangleIsSelected == true){
		
		//clear previous vector values
		if (H_ROI_ALLVALUES.size()>0) H_ROI_ALLVALUES.clear();
		if (S_ROI_ALLVALUES.size()>0) S_ROI_ALLVALUES.clear();
		if (V_ROI_ALLVALUES.size()>0 )V_ROI_ALLVALUES.clear();
		//if the rectangle has no width or height (user has only dragged a line) then we don't try to iterate over the width or height
		if (rectangleROI.width<1 || rectangleROI.height<1) cout << "Please drag a rectangle, not a line" << endl;
		else{
			for (int i = rectangleROI.x; i<rectangleROI.x + rectangleROI.width; i++){
				//iterate through both x and y direction and save HSV values at each and every point
				for (int j = rectangleROI.y; j<rectangleROI.y + rectangleROI.height; j++){
					//save HSV value at this point
					H_ROI_ALLVALUES.push_back((int)HSVCalibration.at<cv::Vec3b>(j, i)[0]);
					S_ROI_ALLVALUES.push_back((int)HSVCalibration.at<cv::Vec3b>(j, i)[1]);
					V_ROI_ALLVALUES.push_back((int)HSVCalibration.at<cv::Vec3b>(j, i)[2]);
					B_ROI_ALLVALUES.push_back((int)frame.at<Vec3b>(j,i)[0]);
					R_ROI_ALLVALUES.push_back((int)frame.at<Vec3b>(j,i)[1]);
					G_ROI_ALLVALUES.push_back((int)frame.at<Vec3b>(j,i)[2]);
				}
			}
		}
		//reset rectangleIsSelected so user can select another region if necessary
		rectangleIsSelected = false;
		//set min and max HSV values from min and max elements of each array
					
		if (H_ROI_ALLVALUES.size()>0){
			//NOTE: min_element and max_element return iterators so we must dereference them with "*"
			Hmin = *std::min_element(H_ROI_ALLVALUES.begin(), H_ROI_ALLVALUES.end());
			Hmax = *std::max_element(H_ROI_ALLVALUES.begin(), H_ROI_ALLVALUES.end());
			cout << "Minimum Hue Value: " << Hmin << endl;
			cout << "Maximum Hue Value:  " << Hmax << endl;
		}
		if (S_ROI_ALLVALUES.size()>0){
			Smin = *std::min_element(S_ROI_ALLVALUES.begin(), S_ROI_ALLVALUES.end());
			Smax = *std::max_element(S_ROI_ALLVALUES.begin(), S_ROI_ALLVALUES.end());
			cout << "Minimum Saturation Value: " << Smin << endl;
			cout << "Maximum Saturation Value: " << Smax << endl;
		}
		if (V_ROI_ALLVALUES.size()>0){
			Vmin = *std::min_element(V_ROI_ALLVALUES.begin(), V_ROI_ALLVALUES.end());
			Vmax = *std::max_element(V_ROI_ALLVALUES.begin(), V_ROI_ALLVALUES.end());
			cout << "Minimum Value Value: " << Vmin << endl;
			cout << "Maximum Value Value:  " << Vmax << endl;
		}
		if (B_ROI_ALLVALUES.size()>0)
		{
			Bmin = *std::min_element(B_ROI_ALLVALUES.begin(), B_ROI_ALLVALUES.end());
			Bmax = *std::min_element(B_ROI_ALLVALUES.begin(), B_ROI_ALLVALUES.end());
			Bavg = (Bmin + Bmax)/2;
			cout << "Our blue value is: " << Bavg << endl;
		}
				if (R_ROI_ALLVALUES.size()>0)
		{
			Rmin = *std::min_element(R_ROI_ALLVALUES.begin(), R_ROI_ALLVALUES.end());
			Rmax = *std::min_element(R_ROI_ALLVALUES.begin(), R_ROI_ALLVALUES.end());
			Ravg = (Rmin + Rmax)/2;
			cout << "Our red value is: " << Ravg << endl;
		}
				if (G_ROI_ALLVALUES.size()>0)
		{
			int Gmin = *std::min_element(G_ROI_ALLVALUES.begin(), G_ROI_ALLVALUES.end());
			int Gmax = *std::min_element(G_ROI_ALLVALUES.begin(), G_ROI_ALLVALUES.end());
			Gavg = (Gmin + Gmax)/2;
			cout << "Our green value is: " << Gavg << endl;
		}
	}

	if (mouseHasMoved == true){
		//if the mouse is held down, we will draw the click and dragged rectangle to the screen
		rectangle(frame, initialClickPoint, cv::Point(endClickPoint.x, endClickPoint.y), cv::Scalar(100, 125, 92), .5 , 4, 0);
	}
	HSVMINS.push_back(Scalar(Hmin, Smin, Vmin));
	HSVMAXS.push_back(Scalar(Hmax, Smax, Vmax));
	BGRVALS.push_back(Scalar(Bavg,Gavg,Ravg));

}

void getCurrentFoods(fstream &file, string line)
{
	file.open("foodValues.txt", ios::in | ios::app | ios::out);
	if(checkIfFileOpen(file)) return;
	while( getline (file,line) )
	{
		float temp, HSVmin1temp1, HSVmin1temp2, HSVmin1temp3,
			  HSVmin2temp1, HSVmin2temp2, HSVmin2temp3, 
			  HSVmin3temp1, HSVmin3temp2, HSVmin3temp3, 
			  HSVmax1temp1, HSVmax1temp2, HSVmax1temp3, 
			  HSVmax2temp1, HSVmax2temp2, HSVmax2temp3, 
			  HSVmax3temp1, HSVmax3temp2, HSVmax3temp3,
			  Btemp, Gtemp, Rtemp;
		Btemp = 120; Gtemp = 120; Rtemp = 120;
		sscanf(line.c_str(), "%s [%f, %f, %f, %f] [%f, %f, %f, %f] [%f, %f, %f, %f] [%f, %f, %f, %f] [%f, %f, %f, %f] [%f, %f, %f, %f] [%f, %f, %f, %f]",
				Name, &HSVmin1temp1, &HSVmin1temp2, &HSVmin1temp3, &temp,
					  &HSVmin2temp1, &HSVmin2temp2, &HSVmin2temp3, &temp,
					  &HSVmin3temp1, &HSVmin3temp2, &HSVmin3temp3, &temp,
					  &HSVmax1temp1, &HSVmax1temp2, &HSVmax1temp3, &temp, 
					  &HSVmax2temp1, &HSVmax2temp2, &HSVmax2temp3, &temp,
					  &HSVmax3temp1, &HSVmax3temp2, &HSVmax3temp3, &temp,
					  &Btemp, &Gtemp, &Rtemp, &temp);
		//cout << Name << HSVmintemp1 << HSVmintemp2 << HSVmintemp3;
		Names.push_back(Name);
		HSVMins1.push_back(Scalar(HSVmin1temp1, HSVmin1temp2, HSVmin1temp3));
		HSVMins2.push_back(Scalar(HSVmin2temp1, HSVmin2temp2, HSVmin2temp3));
		HSVMins3.push_back(Scalar(HSVmin3temp1, HSVmin3temp2, HSVmin3temp3));
		HSVMaxs1.push_back(Scalar(HSVmax1temp1, HSVmax1temp2, HSVmax1temp3));
		HSVMaxs2.push_back(Scalar(HSVmax2temp1, HSVmax2temp2, HSVmax2temp3));
		HSVMaxs3.push_back(Scalar(HSVmax3temp1, HSVmax3temp2, HSVmax3temp3));
		BGRVals.push_back(Scalar(Btemp, Gtemp, Rtemp));
		//for (int i = 0; i < BGRVals.size(); i++)
		//{
		//	cout << "In getCurrentFoods " << i;
		//	cout << BGRVals[i] << endl;
		//}
	}
	file.close();
}

bool checkIfFileOpen(fstream &file)
{	
	if (!file.is_open()) 
	{
		cout << "File not opened " << endl;
		return false;
	}
}

void loadLocalHSV()
{
	cout << "Loading HSV values locally..." << endl;
	for (int k = 0; k < LocalNames.size(); k++)
	{
		for (int j = 0; j < Names.size(); j++)
		{
			if (LocalNames[k] == Names[j])
			{
				LocalBGRVals[k] = BGRVals[j];
				LocalHSVMins1[k] = HSVMins1[j];
				LocalHSVMins2[k] = HSVMins2[j];
				LocalHSVMins3[k] = HSVMins3[j];
				LocalHSVMaxs1[k] = HSVMaxs1[j];
				LocalHSVMaxs2[k] = HSVMaxs2[j];
				LocalHSVMaxs3[k] = HSVMaxs3[j];
			}
		}
	}

}

void storeNewFoods(fstream& file, int i)
{
	cout << "Storing New found foods into file... " << endl;
	if(LocalHSVMins1[i] == Scalar(0,0,0))
				{	
					cout << "Make rectangle in center of food " << LocalNames[i] << " until it tracks, then hit the Q key" << endl;
					unsigned long now = clock();
					while ((clock()- now)/CLOCKS_PER_SEC <= 1000)
					{
						makeCropAndCircle();
						setMouseCallback(windowOriginal1, makeCalibrationRectangle, &liveFeed);
						imshow(windowOriginal1,liveFeed);
						recordHSV(liveFeed, HSV);
						inRange(HSV, Scalar(Hmin, Smin, Vmin), Scalar(Hmax,Smax,Vmax), thresholdImg);
						morphOps(thresholdImg);
						imshow("Calibration", thresholdImg);
						//trackingObjectCalibration(thresholdImg, HSV, liveFeed);
						int c = waitKey(30);
						if (c == 'q') break;
					}
					LocalHSVMins1[i] = HSVMINS[HSVMINS.size()-1];
					LocalHSVMaxs1[i] = HSVMAXS[HSVMAXS.size()-1];
					LocalBGRVals[i] = BGRVALS[BGRVALS.size()-1];

					cout << "Move food " << LocalNames[i] << " and make another rectangle until it tracks, then hit the Q key" << endl;
					now = clock();
					while ((clock()- now)/CLOCKS_PER_SEC <= 1000)
					{
						makeCropAndCircle();
						setMouseCallback(windowOriginal1, makeCalibrationRectangle, &liveFeed);
						imshow(windowOriginal1,liveFeed);
						recordHSV(liveFeed, HSV);
						inRange(HSV, Scalar(Hmin, Smin, Vmin), Scalar(Hmax,Smax,Vmax), thresholdImg);
						morphOps(thresholdImg);
						imshow("Calibration", thresholdImg);
						//trackingObjectCalibration(thresholdImg, HSV, liveFeed);
						int c = waitKey(30);
						if (c == 'q') break;
					}

					cout << "Move food " << LocalNames[i] << " and make another rectangle until it tracks, then hit the Q key" << endl;

					LocalHSVMins2[i] = HSVMINS[HSVMINS.size()-1];
					LocalHSVMaxs2[i] = HSVMAXS[HSVMAXS.size()-1];
					now = clock();
					while ((clock()- now)/CLOCKS_PER_SEC <= 1000)
					{
						makeCropAndCircle();
						setMouseCallback(windowOriginal1, makeCalibrationRectangle, &liveFeed);
						imshow(windowOriginal1,liveFeed);
						recordHSV(liveFeed, HSV);
						inRange(HSV, Scalar(Hmin, Smin, Vmin), Scalar(Hmax,Smax,Vmax), thresholdImg);
						morphOps(thresholdImg);
						imshow("Calibration", thresholdImg);
						//trackingObjectCalibration(thresholdImg, HSV, liveFeed);
						int c = waitKey(30);
						if (c == 'q') break;
					}

					LocalHSVMins3[i] = HSVMINS[HSVMINS.size()-1];
					LocalHSVMaxs3[i] = HSVMAXS[HSVMAXS.size()-1];

					file.open("foodValues.txt", ios::in | ios::app | ios::out);
					if(checkIfFileOpen(file))return;
					cout << "Adding.. " << endl;
					foodValues << LocalNames[i];
					foodValues << " ";
					cout << LocalNames[i] << " ";
					foodValues << LocalHSVMins1[i];
					foodValues << " ";
					cout << LocalHSVMins1[i] << " ";
					foodValues << LocalHSVMins2[i];
					foodValues << " ";
					cout << LocalHSVMins2[i] << " ";
					foodValues << LocalHSVMins3[i];
					foodValues << " ";
					cout << LocalHSVMins3[i] << " ";
					foodValues << LocalHSVMaxs1[i];
					foodValues << " ";
					cout << LocalHSVMaxs1[i] << " ";
					foodValues << LocalHSVMaxs2[i];
					foodValues << " ";
					cout << LocalHSVMaxs2[i] << " ";
					foodValues << LocalHSVMaxs3[i];
					foodValues << " ";
					cout << LocalHSVMaxs3[i] << " ";
					foodValues << LocalBGRVals[i] << endl;
					cout << LocalBGRVals[i] << endl;

					file.close();
				}
}

void setClassValues(vector<Food>& theFood, int index)
{
	cout << "Setting the values to our food class..." << endl;
	theFood.at(index).setHSVmin1(LocalHSVMins1[index]);
	theFood.at(index).setHSVmax1(LocalHSVMaxs1[index]);
	theFood.at(index).setHSVmin2(LocalHSVMins2[index]);
	theFood.at(index).setHSVmax2(LocalHSVMaxs2[index]);
	theFood.at(index).setHSVmin3(LocalHSVMins3[index]);
	theFood.at(index).setHSVmax3(LocalHSVMaxs3[index]);
	theFood.at(index).setColor(LocalBGRVals[index]);
	theFood.at(index).setType(LocalNames[index]);
	//cout << "In setClassValues: ";
	//cout << theFood.at(index).getHSVmin1() << ", " << theFood.at(index).getHSVmax1() << ", " << theFood.at(index).getType()<< endl;
	cout << "Creating a 3-fold mask using 3 seperate calibrations then tracking..." << endl;

}

void createMasks(vector<Food>& theFood, int index)
{
	Mat threshTemp;

	inRange(HSV,theFood.at(index).getHSVmin1(),theFood.at(index).getHSVmax1(), threshTemp);
	morphOps(threshTemp);
	ThresholdImgs1.push_back(threshTemp);
}
void releaseMasks(vector<Mat>& theMask)
{
	for (int i = 0; i < theMask.size(); i++)
	{
		theMask.at(i) = Mat();
	}
}
void recreateMasks(vector<Food>& theFood, int index)
{	
	Mat threshTemp1, threshTemp2, threshTemp3, threshTemp4, threshTemp5;
	inRange(HSV,theFood.at(index).getHSVmin1(),theFood.at(index).getHSVmax1(), threshTemp1);
	inRange(HSV,theFood.at(index).getHSVmin2(),theFood.at(index).getHSVmax2(), threshTemp2);
	inRange(HSV,theFood.at(index).getHSVmin3(),theFood.at(index).getHSVmax3(), threshTemp3);
	bitwise_or(threshTemp1, threshTemp2, threshTemp4);
	bitwise_or(threshTemp3, threshTemp4, threshTemp5);
	morphOps(threshTemp5);
	ThresholdImgs1.push_back(threshTemp5);
}

void getInfoFromUser()
{
	cout << "How many ingredients?" << endl;
	cin >> ingredientNum;
	for (int i=0; i < ingredientNum; i++)
	{
		cout << "Name of ingredient number "<< (i+1) << ": " << endl;
		cin >> Name;
		LocalNames.push_back(Name);
		Food Name;
		Foodies.push_back(Name);
		LocalHSVMins1.push_back(Scalar(0,0,0));
		LocalHSVMaxs1.push_back(Scalar(0,0,0));
		LocalHSVMins2.push_back(Scalar(0,0,0));
		LocalHSVMaxs2.push_back(Scalar(0,0,0));
		LocalHSVMins3.push_back(Scalar(0,0,0));
		LocalHSVMaxs3.push_back(Scalar(0,0,0));
		LocalBGRVals.push_back(Scalar(0,0,0));
	}
}

string convertBufToString(char buf[64])
{
	res = read(serialPort,buf,sizeof(buf));
	buf[res] = 0;
	string tempString = string(buf);
	return tempString;
}

string listOfStrings(int ingredientNum)
{
	string tempString;
	for (int i = 0; i < ingredientNum; i++)
	{
		tempString += LocalNames[i] + " ";
	}
	tempString += "\n";
	return tempString;
}

void shapeCount(vector<string> shapes, int& circleCount, int& triCount, int& rectCount, int& pentaCount)
{
		 for (int k = 0; k < shapes.size(); k++)
		{
			if(shapes[k] == "Rectangle") rectCount += 1;
			if(shapes[k] == "Circle") circleCount += 1;
			if(shapes[k] == "Triangle") triCount += 1;
			if(shapes[k] == "Pentagon") pentaCount += 1;
	 	}
}

//==================== Shape functions ===========================================================================

vector<string> shapeDetection()
{
		vector< vector<Point> > contours;
		vector<Vec4i>hierarchy;
		vector<string> Types;
		string shapeObj;
		cvtColor(liveFeed, gray, COLOR_BGR2GRAY);
	 	morphOps(gray);
	 	threshold(gray, threshShape, 55, 255, THRESH_BINARY);
	 	imshow("threshShape", threshShape);
	 	//morphOps(threshShape);
	 	findContours(threshShape, contours, hierarchy, CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);
	 	if (hierarchy.size() > 0 && hierarchy.size() < MAX_NUM_OBJECTS)
	 	{
		 	for(int index = 0; index >= 0; index = hierarchy[index][0]){
		 		//for(size_t j = 0; j < contours.size(); j++)
		 		//{
					Moments moment = moments(contours[index]);
		 			double area = moment.m00;
		 			if (area > MIN_OBJECT_AREA)
		 			{
		 				int cX = (int)(moment.m10/area);
		 				int cY = (int)(moment.m01/area);
		 				shapeObj = SD.detection(contours);
		 				Types.push_back(shapeObj);
		 				//drawContours(liveFeed, contours[index], (cX, cY), Scalar(255,255,255), 1);
		 				putText(liveFeed, shapeObj, Point(cX,cY), 1, 1,Scalar(255,255,255), 1);
		 			}
		}
}
return Types;
}
void Food::setBoarder()
{
	vector< vector<Point> > contours;

	vector<Vec4i>hierarchy;
	vector< vector<Point> > approx;
	findContours(thresholdImg, contours, hierarchy, CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);
	vector<Moments> moment(contours.size());
	 	for(size_t i = 0; i < contours.size(); i++){
	 			double area = moment[i].m00;
	 			int cX = (int)(moment[i].m10/area);
	 			int cY = (int)(moment[i].m01/area);
	 			drawContours(liveFeed, contours, (cX, cY), this->getColor(), 1);
	 			
	 		}
}