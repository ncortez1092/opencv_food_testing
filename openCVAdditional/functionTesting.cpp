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
Mat detected_edges, dst, liveFeed_gray;



//==================================== Initializations ===================================================
int circleCount=0,triCount=0,rectCount=0,pentaCount=0;
int edgeThresh = 1;
int lowThreshold=100;
int const max_lowThreshold=100;
int ratio = 3;
int kernel_size = 5;
char* window_name = "Edge Map";
//====================== End Initializations =====================================================

void CannyThreshold(int, void*)
{
	threshold(liveFeed_gray, liveFeed_gray, 55, 255, THRESH_BINARY);
	blur(liveFeed_gray, detected_edges, Size(kernel_size,kernel_size));
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
	dst = Scalar::all(0);
	liveFeed.copyTo(dst, detected_edges);
	imshow(window_name, dst);
}





int main(int argc, char* argv[])
{
	calibrateVideo();

	while(1)
	{
// ------------- We capture the video, crop it, and only focus on the pot -----------------------
		makeCropAndCircle();
		dst.create(liveFeed.size(), liveFeed.type());
		cvtColor(liveFeed, liveFeed_gray, CV_BGR2GRAY);
		namedWindow(window_name, CV_WINDOW_AUTOSIZE);
		CannyThreshold(0,0);
		waitKey(0);
		return 0;










//-------- Here we do shape recognition before moving on -------------------------------
	 	//vector<string> shapes = shapeDetection();
	 	//shapeCount(shapes,circleCount,triCount,rectCount,pentaCount);
//-------------------------------------------------------------------------------------------------
	 	//if(rectCount > circleCount || rectCount > triCount || rectCount > pentaCount)
	 	//{ // If we have more rectangles than anything else, lets find out what they are
		/*tcflush(serialPort, TCIOFLUSH);
		for (static bool first = true; first; first = false)
		{
			getCurrentFoods(foodValues, gline);
			int i = 0;
			while (i < Foodies.size())
			{
				loadLocalHSV();
				storeNewFoods(foodValues, i);
				setClassValues(Foodies, i);
				i++;
			}
		}

		if (counttest % 10 == 0)
		{
			ThresholdImgs1.clear();
			for (int i = 0; i < Foodies.size(); i++)
			{
				recreateMasks(Foodies, i);
				waitKey(10);

			}
		}
		trackingObjects(Foodies, ThresholdImgs1, HSV, liveFeed);
		vector<string> messages = relayCoords(Foodies, liveFeed);
		/*if (counttest % 40 == 0)
		{
			string tempString = convertBufToString(buf);
			if (tempString.size() != 0) cout << "String received from Serial...: " << tempString << endl;
			int FoodNum = 10000;
			if(tempString == "numFood") STOP_NUM = false;
			else if(tempString == ">") STOP = false;
			else if(tempString == "which") STOP_STRINGS = false;
			else
			{
				for (int i = 0; i < LocalNames.size(); i++)
				{
					cout << LocalNames[i];
					FoodNum = (tempString == LocalNames[i]) ? i : 10000;
					if (FoodNum != 10000) break;
					cout << FoodNum;
				}
				if (FoodNum != 10000) // 10000 arbitrary large num that theres no way we would cook this much foods
				{
					//cout << "Looking for food..: " << Foodies.at(FoodNum) << endl;
					string tempString2 = relayCoord(Foodies.at(FoodNum), liveFeed);
					char const* myChar = tempString2.c_str();
					write(serialPort, myChar, 62);
					cout << "Sending robot..: " << myChar << endl;
				}
			}
		}
        while (STOP==false)
		{
			for (int i = 0; i < messages.size(); i++)
			{
				char const* myChar = messages.at(i).c_str();
				write(serialPort, myChar, 62);
				cout << "Sending robot..: " << myChar << endl;
			}
			if (tcdrain(serialPort) != 0) perror("tcdrain() error");
			STOP = true;
		}
		while (STOP_NUM == false)
		{
			char const* numFood = intToString(ingredientNum).c_str();
			write(serialPort, numFood, 62);
			cout << "Sending robot numFood... " << numFood << endl;
			STOP_NUM = true;
		}
		while (STOP_STRINGS == false)
		{
			char const* whichString = listOfStrings(ingredientNum).c_str();
			write(serialPort, whichString, 62);
			cout << "Sending robot strings... " << whichString << endl;
			STOP_STRINGS = true;
		}*/
		counttest += 1;
		imshow(windowOriginal1,liveFeed);
		int key = waitKey(5);
		if (key == 27) break;
	}
	return 0;
} 	