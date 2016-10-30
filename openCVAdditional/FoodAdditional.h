#pragma once
#include <string>
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

	Scalar getHSVmin();
	Scalar getHSVmax();

	void setHSVmin(Scalar min);
	void setHSVmax(Scalar max);

	string getType(){return type;}
	void setType(string t){type = t;}

	Scalar getColor(){return Color;}
	void setColor(Scalar c){Color = c;}
private:

	int xPos, yPos;
	string type;
	cv::Scalar HSVmin,HSVmax;
	cv::Scalar Color;

};





//==================================== Initializations ===================================================
// This is used for accessing our port connected to gantry
static const char* portName = "/dev/ttyACM0"; 
	int serialPort;
	struct termios portOptions;

//These are used for HSV detection, and will be used on our trackbars
int Hmin = 0;
int Hmax = 256;
int Smin = 0;
int Smax = 256;
int Vmin = 0;
int Vmax = 256;



// Size of the webcam feed
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

// Other obvious declarations
const int MAX_NUM_OBJECTS = 50; // Max number of objects
const int MIN_OBJECT_AREA = 20*20; // Min area of an object we will bother detecting
const int MAX_OBJECT_AREA = (FRAME_HEIGHT*FRAME_WIDTH) / 1.5; // Max area of an object we will detect

// Window names for our program
const string windowOriginal = "Original";
const string windowHSV = "HSV image";
const string windowThresh = "Thresholded Image";
const string windowOps = "Morphological Operations";
const string windowControls = "Controls";

// All classes ( foods )
Food spatula("spatula");

// Matrix to store each frame of the webcam feed
Mat liveFeed;
Mat thresholdImg;
Mat HSV;
Mat gray;
Mat temp;

// Function Prototypes

char const* relayCoords(Food);
void on_trackbar(int, void*){}
string intToString(int);
void createTrackbars();
void drawObject(vector<Food>, Mat &);
void morphOps(Mat&);
void trackingObjectCalibration(Mat, Mat, Mat &);
void trackingObject(Food , Mat, Mat, Mat &);
void setUpSerial();

//====================== End Initializations =====================================================



char const* relayCoords(Food theFood){
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
		int x = theFood.getXPos();
		int y = theFood.getYPos();
		int xCoord, yCoord;

		// Initial shift
		x = x - 125;
		y = y - 125;

		// Initializing the normalization coefficients
		double xNorm = (double)(114)/(250);
		double yNorm = (double)(138)/(250);

		if (x < 0 && y < 0) // Quadrant 1
			{
				xCoord = xNorm*x;
				yCoord = (-1)*yNorm*y;
			}
		else if (x > 0 && y < 0) // Quadrant 2
			{
				xCoord = (-1)*xNorm*x;
				yCoord = yNorm*y;
			}
		else if (x < 0 && y > 0) // Quadrant 3
			{
				xCoord = (-1)*xNorm*x; 
				yCoord = yNorm*y;
			}
		else if (x > 0 && y > 0) // Quadrant 4
			{
				xCoord = xNorm*x;
				yCoord = (-1)*yNorm*y;
			}
		else if (x == 0 && y == 0)
			{
				xCoord = 0;
				yCoord = 0;
			}



		string message = intToString(xCoord) + ", " + intToString(yCoord);
		//string message = intToString(theFood.getXPos())+ ", " + intToString(theFood.getYPos());
		char const* pchar = message.c_str();
		return pchar;
}

string intToString(int number){

	// This will convert any integers into strings for our coordinate relay

	stringstream s;
	s << number;
	return s.str();
}
void createTrackbars(){

	// Makes trackbars

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
void drawObject(vector<Food> Foods, Mat &frame){

	// This creates the circle and tracks it

	for(int i=0; i < Foods.size(); i++)
	{

	cv::circle(frame,cv::Point(Foods.at(i).getXPos(),Foods.at(i).getYPos()),10,cv::Scalar(100,100,100));
	cv::putText(frame,intToString(Foods.at(i).getXPos())+ " , " + intToString(Foods.at(i).getYPos()),cv::Point(Foods.at(i).getXPos(),Foods.at(i).getYPos()+20),1,1,Scalar(0,255,0));
	cv::putText(frame,Foods.at(i).getType(),cv::Point(Foods.at(i).getXPos(),Foods.at(i).getYPos()-30),1,2,Foods.at(i).getColor());
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

					Food spatula;

					spatula.setXPos(moment.m10/area);	
					spatula.setYPos(moment.m01/area);
				
					Ingredients.push_back(spatula);

					objectFound = true;

				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				putText(liveFeed, "Tracking", Point(0, 25), 1, 1, Scalar(0,0,255), 2);
				drawObject(Ingredients,liveFeed);}

		}else putText(liveFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

void trackingObject(Food Foods, Mat thresholdImg,Mat HSV, Mat &liveFeed){

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

					spatula.setXPos(moment.m10/area);	
					spatula.setYPos(moment.m01/area);
					spatula.setType(Foods.getType());
					spatula.setColor(Foods.getColor());
				
					Ingredients.push_back(spatula);

					objectFound = true;

				}else objectFound = false;
				if(objectFound ==true){
					putText(liveFeed, "Tracking", Point(0, 25), 1, 1, Scalar(0,0,255), 2);
					drawObject(Ingredients,liveFeed);
					spatula.setXPos(moment.m10/area);	
					spatula.setYPos(moment.m01/area);
					spatula.setType(Foods.getType());
					spatula.setColor(Foods.getColor());}


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