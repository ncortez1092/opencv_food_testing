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
int cropHeight = 280, cropWidth = 210, cropX = 180, cropY = 80; // Used for the ROI crop

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



	while(1)
	{
// ------------- We capture the video, crop it, and only focus on the pot -----------------------
		capture.read(temp);
		Rect myCropROI(cropX, cropY , cropHeight , cropWidth);
		cv::Mat mask = cv::Mat::zeros( temp.rows, temp.cols, CV_8UC1 );
		Point center = Point(320, 180);
		float radius = 95;
		circle( mask, center, radius, Scalar(255,255,255), -1, 8, 0 ); //-1 means filled
		temp.copyTo( liveFeed, mask );
		liveFeed = liveFeed(myCropROI);
		cvtColor(liveFeed,HSV,COLOR_BGR2HSV);
		float ratio = FRAME_HEIGHT/cropHeight;
//--------------------------------------------------------------------------------------------------
		if(doCalibrate==true){
		// if doCalibrate == true, we go into calibration mode with sliders to find values

//------------------- This is used for finding the center point and the radius ----------------
		vector<Vec3f> circles;
		HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, gray.rows/8, 200, 100, 0, 0 );
		GaussianBlur(gray,gray,Size(8,8),2,2);
//----- we can access these numbers using center = circles[0][0], radius = circles[0][1]----------
		
		inRange(HSV,Scalar(Hmin,Smin,Vmin),Scalar(Hmax,Smax,Vmax),thresholdImg);
		morphOps(thresholdImg);
		imshow(windowHSV,thresholdImg);
		trackingObjectCalibration(thresholdImg,HSV,liveFeed);

	 	}else{
//-------- Here we do shape recognition before moving on -------------------------------
	 	vector<string> shapes = shapeDetection();
	 	int rectCount=0,circleCount=0,pentaCount=0,triCount=0;
	 	for (int k = 0; k < shapes.size(); k++)
		{
			if(shapes[k] == "Rectangle") rectCount += 1;
			if(shapes[k] == "Circle") circleCount += 1;
			if(shapes[k] == "Triangle") triCount += 1;
			if(shapes[k] == "Pentagon") pentaCount += 1;
	 	}
//-------------------------------------------------------------------------------------------------
	 	if(rectCount > circleCount || rectCount > triCount || rectCount > pentaCount)
	 	{ // If we have more rectangles than anything else, lets find out what they are
			tcflush(serialPort, TCIOFLUSH);
			inRange(HSV,Bread.getHSVmin1(), Bread.getHSVmax1(),thresholdImg1);
			inRange(HSV,Bread.getHSVmin2(), Bread.getHSVmax2(),thresholdImg2);
			bitwise_or(thresholdImg1,thresholdImg2, thresholdImg);
			morphOps(thresholdImg);
			imshow(windowThresh,thresholdImg);
			//theFood = &Bread;
			trackingObject(Bread,thresholdImg,HSV,liveFeed);
			Bread.setBoarder();
			char const* BreadChar = relayCoords(Bread, liveFeed);
			if (counttest % 60 == 0 && sizeof(BreadChar) > 6)
			{
			write(serialPort, BreadChar, 62);
			cout << BreadChar << endl;
			}
		}
			/*tcflush(serialPort, TCIOFLUSH);
			cvtColor(liveFeed,HSV,COLOR_BGR2HSV);
			inRange(HSV,Carrot.getHSVmin(), Carrot.getHSVmax(),thresholdImg);
			morphOps(thresholdImg);
			imshow(windowThresh,thresholdImg);
			trackingObjectCarrot(Carrot,thresholdImg,HSV,liveFeed);
			char const* CarrotChar = relayCoords(Carrot, liveFeed);
			if (counttest % 60 == 0 && sizeof(CarrotChar) > 6)
			{
				write(serialPort, CarrotChar, 62);
				cout << CarrotChar << endl;
			}	*/
					counttest += 1;
			}
			imshow(windowOriginal1,liveFeed);
		//	imshow(windowOriginal2,liveFeed);

			waitKey(50); // Wait .5 seconds before relaying the coords again. Will not work without this,
						  // Make 0 to run forever and not wait

		}

		return 0;
} 	
