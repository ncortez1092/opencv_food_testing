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


bool ifClicked, mouseHasMoved, rectangleIsSelected;

//These are used for HSV detection, and will be used on our trackbars
int Hmin = 0;
int Hmax = 256;
int Smin = 0;
int Smax = 256;
int Vmin = 0;
int Vmax = 256;
int counttest = 0;
int ingredientNum;
float HSVmintemp1, HSVmintemp2, HSVmintemp3, HSVmaxtemp1, HSVmaxtemp2, HSVmaxtemp3;
vector<int> H_ROI_ALLVALUES, S_ROI_ALLVALUES, V_ROI_ALLVALUES;
vector<Scalar> HSVMins, HSVMaxs, LocalHSVMins, LocalHSVMaxs;


// Size of the webcam feed
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

// Other obvious declarations
const int MAX_NUM_OBJECTS = 50; // Max number of objects
const int MIN_OBJECT_AREA = 40*40; // Min area of an object we will bother detecting
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

// Matrix to store each frame of the webcam feed
Mat liveFeed, temp, gray;
Mat thresholdImg, thresholdImg1, thresholdImg2, thresholdImg3, thresholdImg4, thresholdImg5, thresholdImg6, thresholdImg7, thresholdImg8, threshShape;
Mat thresholdImg9, thresholdImg10, thresholdImg11;
Mat HSV;
vector<Mat> ThresholdImgs1, ThresholdImgs2, ThresholdImgs3, ThresholdImgs4;
vector<Scalar> HSVMINS, HSVMAXS;
Point initialClickPoint, endClickPoint;
Rect rectangleROI;
// Function Prototypes

char const* relayCoords(Food);
void on_trackbar(int, void*){}
string intToString(int);
void createTrackbars();
void drawObject(vector<Food>, Mat &);
void morphOps(Mat&);
void trackingObjectCalibration(Mat, Mat, Mat &);
void trackingObject(Food& , Mat, Mat, Mat &);
void setUpSerial();
string floatToString(float);

//====================== End Initializations =====================================================



char const* relayCoords(Food theFood, Mat& frame){
/*
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





*/

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
}
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
void createTrackbars(){


	namedWindow(windowControls,0);

	char TrackbarName[50];
	sprintf(TrackbarName, "Hmin", Hmin);
	sprintf(TrackbarName, "Hmax", Hmax);
	sprintf(TrackbarName, "Smin", Smin); 
	sprintf(TrackbarName, "Smax", Smax); 
	sprintf(TrackbarName, "Vmin", Vmin);
	sprintf(TrackbarName, "Vmax", Vmax);    
	createTrackbar("Hmin", windowControls, &Hmin, 255, on_trackbar);
	createTrackbar("Hmax", windowControls, &Hmax, 255, on_trackbar);
	createTrackbar("Smin", windowControls, &Smin, 255, on_trackbar);
	createTrackbar("Smax", windowControls, &Smax, 255, on_trackbar);
	createTrackbar("Vmin", windowControls, &Vmin, 255, on_trackbar);
	createTrackbar("Vmax", windowControls, &Vmax, 255, on_trackbar);
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

					Food Bread;

					Bread.setXPos(moment.m10/area);	
					Bread.setYPos(moment.m01/area);
				
					Ingredients.push_back(Bread);

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

void trackingObject(Food& theFood, Mat thresholdImg,Mat HSV, Mat &liveFeed){
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
				float area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if(area>MIN_OBJECT_AREA){

					theFood.setArea(area);
					theFood.setXPos(moment.m10/area);	
					theFood.setYPos(moment.m01/area);
					theFood.setType(theFood.getType());
					theFood.setColor(theFood.getColor());
				
					Ingredients.push_back(theFood);

					objectFound = true;

				}else objectFound = false;
				if(objectFound ==true){
					putText(liveFeed, "Tracking", Point(0, 50), 1, 1, Scalar(0,0,255), 2);
					drawObject(Ingredients,liveFeed);
					theFood.setXPos(moment.m10/area);	
					theFood.setYPos(moment.m01/area);
					theFood.setType(theFood.getType());
					theFood.setColor(theFood.getColor());}


			}
			//let user know you found an object


		}else putText(liveFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}

}


void setUpSerial()
{
//==================== Serial Initialization =============================================================


	serialPort = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);
	tcgetattr(serialPort, &portOptions);
	tcflush(serialPort, TCIOFLUSH);
	cfsetispeed(&portOptions, B115200);
	cfsetospeed(&portOptions, B115200);
 	// c_cflag contains a few important things- CLOCAL and CREAD, to prevent
 	//   this program from "owning" the port and to enable receipt of data.
 	//   Also, it holds the settings for number of data bits, parity, stop bits,
	//   and hardware flow control. 
	portOptions.c_cflag |= CLOCAL;
	portOptions.c_cflag |= CREAD;
	// Set up the frame information.
	portOptions.c_cflag &= ~CSIZE; // clear frame size info
	portOptions.c_cflag |= CS8;    // 8 bit frames
	portOptions.c_cflag &= ~PARENB;// no parity
	portOptions.c_cflag &= ~CSTOPB;// one stop bit

 	tcsetattr(serialPort, TCSANOW, &portOptions);
  	tcflush(serialPort, TCIOFLUSH);


//==================== End Serial Initialization ============================================================
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
	}

	if (mouseHasMoved == true){
		//if the mouse is held down, we will draw the click and dragged rectangle to the screen
		rectangle(frame, initialClickPoint, cv::Point(endClickPoint.x, endClickPoint.y), cv::Scalar(0, 255, 0), 1, 8, 0);
	}
	HSVMINS.push_back(Scalar(Hmin, Smin, Vmin));
	cout << HSVMINS[HSVMINS.size()-1];
	HSVMAXS.push_back(Scalar(Hmax, Smax, Vmax));

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