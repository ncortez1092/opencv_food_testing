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
int circleCount=0,triCount=0,rectCount=0,pentaCount=0;

//====================== End Initializations =====================================================






int main(int argc, char* argv[])
{
	setUpSerial();
	getInfoFromUser();
	calibrateVideo();

	while(1)
	{
// ------------- We capture the video, crop it, and only focus on the pot -----------------------
		makeCropAndCircle();
//-------- Here we do shape recognition before moving on -------------------------------
	 	//vector<string> shapes = shapeDetection();
	 	//shapeCount(shapes,circleCount,triCount,rectCount,pentaCount);
//-------------------------------------------------------------------------------------------------
	 	//if(rectCount > circleCount || rectCount > triCount || rectCount > pentaCount)
	 	//{ // If we have more rectangles than anything else, lets find out what they are
		tcflush(serialPort, TCIOFLUSH);
		for (static bool first = true; first; first = false)
		{
			getCurrentFoods(foodValues, gline);
			int i = 0;
			while (i < Foodies.size())
			{
				loadLocalHSV();
				//cout << "after loadLocalHSV";
				//cout << LocalHSVMins[i];
				//cout << LocalBGRVals[i] << endl;
				//cout << LocalHSVMins.size() << LocalBGRVals.size();
				storeNewFoods(foodValues, i);
				setClassValues(Foodies, i);
				//createMasks(Foodies, i);
				i++;
			}
		}

		if (counttest % 10 == 0)
		{
			ThresholdImgs1.clear();
			for (int i = 0; i < Foodies.size(); i++)
			{
				recreateMasks(Foodies, i);
				//cout << "In counttest loop: ";
				//cout << Foodies.at(i).getHSVmin1() << ", " << Foodies.at(i).getHSVmax1() << endl;
				//cout << "Iterration: " << i << endl;
				waitKey(10);

			}
		}
		trackingObjects(Foodies, ThresholdImgs1, HSV, liveFeed);
		//inRange(HSV,PotSticker.getHSVmin1(), PotSticker.getHSVmax1(),thresholdImg3);
		//inRange(HSV,PotSticker.getHSVmin2(), PotSticker.getHSVmax2(),thresholdImg4);
		//bitwise_or(thresholdImg3,thresholdImg4,thresholdImg9);
		//morphOps(thresholdImg9); 	
		//trackingObject(PotSticker, thresholdImg9, HSV, liveFeed);
		//PotSticker.setBoarder();
		//char const* PotStickerChar = relayCoords(Foodies, liveFeed);
		char thisChar[64];
		tcflush(serialPort, TCIOFLUSH);
		read(serialPort, thisChar, 62);
		vector<string> messages = relayCoords(Foodies, liveFeed);
		//cout << messages; 	
		if(*thisChar == '>')
		{
			for (int i = 0; i < messages.size(); i++)
			{
				char const* myChar = messages.at(i).c_str();
				write(serialPort, myChar, 62);
				cout << "Sending robot..: " << myChar << endl;
			}
		}

		counttest += 1;
		imshow(windowOriginal1,liveFeed);
		int key = waitKey(50);
		if (key == 27) break;
	}
	return 0;
} 	