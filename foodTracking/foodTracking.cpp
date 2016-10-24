/* We attempImgt to do several things in this code. In the end, we want to be tracking a piece of food in the pot and relaying the (x,y) coordinates to the robot.
Steps:
1. Find the HSV values of each of the pieces of food in there
	1. We can use Auto Filtering to find the value of each piece (later)
2. Assign each HSV value to a food class of each one
	1 Use a class header file to store all of this
3. Track each piece of food and ensure movement
4. Use no-color movement tracking to ensure movement of the food at all times
5. Relay the (x,y) coordinates of each piece of food to the robot whenever called upon
	1. We will need to transform coordinates from the image to the robot somehow

*/

// The below includes are for server stuff
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>


// The below includes are normal includes plus one for our Food class
#include <sstream>
#include <string>
#include <iostream>
#include "Food.h"
#include "Food.cpp"

using namespace std;
using namespace cv;

// Initialize values used throughout the program

static const char* portName = "/dev/ttyACM0"; // This is used for accessing our port connected to gantry

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
const string windowName1 = "Original";
const string windowName2 = "HSV image";
const string windowName3 = "Thresholded Image";
const string windowName4 = "Morphological Operations";
const string windowName5 = "Controls";

// All classes ( foods )
Food lemon("lemon");


/*
bool doCalibrate;
bool mouseIsDragging;
bool mouseMove;
bool rectangleSelected;
Point initialclickPoint, currentMousePoint;
Rect rectangleROI;
vector<int> H_ROI, S_ROI, V_ROI; */ // This is all used for dragging HSV detection, will implement later

// Function Prototypes

void on_trackbar(int, void*){}
string intToString(int);
void createTrackbars();
void drawObject(vector<Food>, Mat &);
void morphOps(Mat&);

// TODO: find a way to put int main in front of these and make it work through the function overloads
void trackingObject(Mat threshold,Mat HSV, Mat &liveFeed){

	vector<Food> Ingredients;

	Mat tempImg;
	threshold.copyTo(tempImg);
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

					Food lemon;

					lemon.setXPos(moment.m10/area);	
					lemon.setYPos(moment.m01/area);
				
					Ingredients.push_back(lemon);

					objectFound = true;

				}else objectFound = false;


			}
			//let user know you found an object
			if(objectFound ==true){
				//draw object location on screen
				drawObject(Ingredients,liveFeed);}

		}else putText(liveFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

void trackingObject(Food Foods, Mat threshold,Mat HSV, Mat &liveFeed){

	vector<Food> Ingredients;

	Mat tempImg;
	threshold.copyTo(tempImg);
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

					lemon.setXPos(moment.m10/area);	
					lemon.setYPos(moment.m01/area);
					lemon.setType(Foods.getType());
					lemon.setColor(Foods.getColor());
				
					Ingredients.push_back(lemon);

					objectFound = true;

				}else objectFound = false;
				if(objectFound ==true){
					//draw object location on screen
					drawObject(Ingredients,liveFeed);
					lemon.setXPos(moment.m10/area);	
					lemon.setYPos(moment.m01/area);
					lemon.setType(Foods.getType());
					lemon.setColor(Foods.getColor());}


			}
			//let user know you found an object


		}else putText(liveFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}


int main(int argc, char* argv[])
{

	// All the stuff below is for server stuff
	int serialPort;
	struct termios portOptions;
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

	// If we would like to calibrate our filter values, set to true.
	bool doCalibrate = false;
	
	// Matrix to store each frame of the webcam feed
	Mat liveFeed;
	Mat threshold;
	Mat HSV;

	if(doCalibrate){
		//create slider bars for HSV filtering
		createTrackbars();
	}
	//video capture object to acquire webcam feed
	VideoCapture capture;
	capture.open(0);
	//open capture object at location zero (default location for webcam)
	//capture.open(0);
	//set height and width of capture frame
	//if (!capture.isOpened()){cout << "Couldn't open camera" << endl; return;}
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to liveFeed matrix
	//all of our operations will be performed within this loop
	int counter = 0;
	while(1){
		//store image to matrix
		capture.read(liveFeed);
		//convert frame from BGR to HSV colorspace
		cvtColor(liveFeed,HSV,COLOR_BGR2HSV);

		if(doCalibrate==true){
		//if in calibration mode, we track objects based on the HSV slider values.
		cvtColor(liveFeed,HSV,COLOR_BGR2HSV);
		inRange(HSV,Scalar(Hmin,Smin,Vmin),Scalar(Hmax,Smax,Vmax),threshold);
		morphOps(threshold);
		imshow(windowName2,threshold);
		trackingObject(threshold,HSV,liveFeed);
		}else{

		cvtColor(liveFeed,HSV,COLOR_BGR2HSV);
		inRange(HSV,lemon.getHSVmin(), lemon.getHSVmax(),threshold);
		morphOps(threshold);
		imshow(windowName2,threshold);
		trackingObject(lemon,threshold,HSV,liveFeed);
//		std::cout << lemon.getXPos() << endl;
		string message = intToString(lemon.getXPos())+ ", " + intToString(lemon.getYPos());
		char const* pchar = message.c_str();
		write(serialPort, pchar, 50);
		cout << pchar << endl;


	/*	cvtColor(liveFeed,HSV,COLOR_BGR2HSV);
		inRange(HSV,banana.getHSVmin(), banana.getHSVmax(),threshold);
		morphOps(threshold);
		imshow(windowName2,threshold);
		trackingObject(banana,threshold,HSV,liveFeed);

		cvtColor(liveFeed,HSV,COLOR_BGR2HSV);
		inRange(HSV,cherry.getHSVmin(), cherry.getHSVmax(),threshold);
		morphOps(threshold);
		imshow(windowName2,threshold)	;
		trackingObject(cherry,threshold,HSV,liveFeed);*/
		}

		//show frames 
		//imshow(windowName2,threshold);

		imshow(windowName1,liveFeed);
		//imshow(windowName1,HSV);


		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(500);
	}

	return 0;
}

string intToString(int number){
	stringstream s;
	s << number;
	return s.str();
}
void createTrackbars(){

	namedWindow(windowName5,0);

	char TrackbarName[50]; // Create memory
	sprintf(TrackbarName, "Hmin", Hmin);
	sprintf(TrackbarName, "Hmax", Hmax);
	sprintf(TrackbarName, "Smin", Smin); 
	sprintf(TrackbarName, "Smax", Smax); 
	sprintf(TrackbarName, "Vmin", Vmin);
	sprintf(TrackbarName, "Vmax", Vmax);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("Hmin", windowName5, &Hmin, 255, on_trackbar);
	createTrackbar("Hmax", windowName5, &Hmax, 255, on_trackbar);
	createTrackbar("Smin", windowName5, &Smin, 255, on_trackbar);
	createTrackbar("Smax", windowName5, &Smax, 255, on_trackbar);
	createTrackbar("Vmin", windowName5, &Vmin, 255, on_trackbar);
	createTrackbar("Vmax", windowName5, &Vmax, 255, on_trackbar);
}
void drawObject(vector<Food> Foods, Mat &frame){

	for(int i=0; i < Foods.size(); i++)
	{

	cv::circle(frame,cv::Point(Foods.at(i).getXPos(),Foods.at(i).getYPos()),10,cv::Scalar(0,0,255));
	cv::putText(frame,intToString(Foods.at(i).getXPos())+ " , " + intToString(Foods.at(i).getYPos()),cv::Point(Foods.at(i).getXPos(),Foods.at(i).getYPos()+20),1,1,Scalar(0,255,0));
	cv::putText(frame,Foods.at(i).getType(),cv::Point(Foods.at(i).getXPos(),Foods.at(i).getYPos()-30),1,2,Foods.at(i).getColor());
	}


}
void morphOps(Mat& threshold){

	// This will dilate then erode the image as to eliminate noise

	Mat Ele_erode = getStructuringElement( MORPH_RECT,Size(3,3));
	Mat Ele_dilate = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(threshold,threshold,Ele_erode);
	erode(threshold,threshold,Ele_erode);


	dilate(threshold,threshold,Ele_dilate);
	dilate(threshold,threshold,Ele_dilate);
}
