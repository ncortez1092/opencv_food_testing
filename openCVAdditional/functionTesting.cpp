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



// The below includes are normal includes plus one for our Food class
#include "FoodAdditional.h"
#include "FoodAdditional.cpp"

using namespace std;
using namespace cv;

//========================= End Declarations =========================================================




//==================================== Initializations ===================================================

// If we would like to calibrate our filter values, set to true.
bool doCalibrate = false; // 280, 210, 180, 80 works
int cropHeight = 280, cropWidth = 210, cropX = 185, cropY = 60; // Used for the ROI crop

//====================== End Initializations =====================================================






int main(int argc, char* argv[])
{
	calibrateVideo();
	while(1)
	{
		makeCropAndCircle();
		for (int i = 0; i < 10; i++)
		{
			Mat threshTemp;
			inRange(HSV, Scalar(3,159,39), Scalar(20,255,197), threshTemp);
			morphOps(threshTemp);
			imshow("ThreshTemp_1", threshTemp);


		}
					int c = waitKey(30);
					if (c == 27 | c == 'q' | c == 'Q') break;
		
	} 	
cout << "We broke out!" << endl;
}