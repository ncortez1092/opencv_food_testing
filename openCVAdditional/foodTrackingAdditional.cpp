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

Instructions:
Ensure that the center of the pot is in the center of your cropped image and that the middle of the pot registers a (0,0) coordinate.

	THIS IS A TESTING CODE! DO NOT USE FOR PRODUCTION!!

*/
//============================ Declarations =====================================================
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
#include "FoodAdditional.h"
#include "FoodAdditional.cpp"

using namespace std;
using namespace cv;

//========================= End Declarations =========================================================




//==================================== Initializations ===================================================

// If we would like to calibrate our filter values, set to true.
bool doCalibrate = false;

//====================== End Initializations =====================================================






int main(int argc, char* argv[])
{
	setUpSerial();

	if(doCalibrate){
		//create slider bars for HSV filtering
		createTrackbars();
	}


	//video capture object to acquire webcam feed
	VideoCapture capture(1);
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH); // Set width
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT); // Set height



	while(1){
		// Store what is read from camera
		capture.read(temp);
		 //Get an HSV image (hue, saturation, value)
		Rect myCropROI(200,55,250,250);


		/* // This is all used for finding the center point and the radius
		vector<Vec3f> circles;
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows/8, 200, 100, 0, 0 );
		//GaussianBlur(gray,gray,Size(8,8),2,2);*/


		cv::Mat mask = cv::Mat::zeros( temp.rows, temp.cols, CV_8UC1 );
		Point center = Point(320, 180);
		float radius = 115;
		circle( mask, center, radius, Scalar(255,255,255), -1, 8, 0 ); //-1 means filled
	//	namedWindow("test"); // debug
	//	imshow("test", mask); // debug
		temp.copyTo( liveFeed, mask );
		liveFeed = liveFeed(myCropROI);
		cvtColor(liveFeed,HSV,COLOR_BGR2HSV);
		liveFeed.copyTo(liveFeed1);
		liveFeed.copyTo(liveFeed2);
		if(doCalibrate==true){
		// if doCalibrate == true, we go into calibration mode with sliders to find values
		cvtColor(liveFeed,HSV,COLOR_BGR2HSV);
		inRange(HSV,Scalar(Hmin,Smin,Vmin),Scalar(Hmax,Smax,Vmax),thresholdImg);
		morphOps(thresholdImg);
		imshow(windowHSV,thresholdImg);
		trackingObjectCalibration(thresholdImg,HSV,liveFeed);
	 	}else{
		// If we aren't in calibrate mode, these foods will be found.
		cvtColor(liveFeed1,HSV1,COLOR_BGR2HSV);
		inRange(HSV1,Broccoli.getHSVmin(), Broccoli.getHSVmax(),thresholdImg1);
		morphOps(thresholdImg1);
		imshow(windowThresh1,thresholdImg1);
		trackingObject(Broccoli,thresholdImg1,HSV1,liveFeed1);
		char const* BroccoliChar = relayCoords(Broccoli, liveFeed1);

		cvtColor(liveFeed2,HSV2,COLOR_BGR2HSV);
		inRange(HSV2,Carrot.getHSVmin(), Carrot.getHSVmax(),thresholdImg2);
		morphOps(thresholdImg2);
		imshow(windowThresh2,thresholdImg2);
		trackingObject(Carrot,thresholdImg2,HSV2,liveFeed2);
		char const* CarrotChar = relayCoords(Carrot, liveFeed2);

		if (counttest % 100 == 0 && sizeof(BroccoliChar) > 6)
		{
		write(serialPort, BroccoliChar, 62);
		cout << BroccoliChar << endl;
		}
		if (counttest % 70 == 0 && sizeof(CarrotChar) > 6)
		{
			write(serialPort, CarrotChar, 62);
			cout << CarrotChar << endl;
		}	
				counttest += 1;
		}

		imshow(windowOriginal1,liveFeed1);
		imshow(windowOriginal2,liveFeed2);

		waitKey(50); // Wait .5 seconds before relaying the coords again. Will not work without this,
					  // Make 0 to run forever and not wait

	}

	return 0;
} 	
