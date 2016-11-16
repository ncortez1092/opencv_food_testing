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
bool doCalibrate = false;

//====================== End Initializations =====================================================






int main(int argc, char* argv[])
{
	setUpSerial();
	getInfoFromUser();
	if(doCalibrate)
	{
		//create slider bars for HSV filtering
		createTrackbars();
	}

	calibrateVideo();

	while(1)
	{
// ------------- We capture the video, crop it, and only focus on the pot -----------------------
		makeCropAndCircle();
//--------------------------------------------------------------------------------------------------
		if(doCalibrate==true)
		{
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

	 	}
	 	else
	 	{
//-------- Here we do shape recognition before moving on -------------------------------
	 	/*vector<string> shapes = shapeDetection();
	 	int rectCount=0,circleCount=0,pentaCount=0,triCount=0;
	 	for (int k = 0; k < shapes.size(); k++)
		{
			if(shapes[k] == "Rectangle") rectCount += 1;
			if(shapes[k] == "Circle") circleCount += 1;
			if(shapes[k] == "Triangle") triCount += 1;
			if(shapes[k] == "Pentagon") pentaCount += 1;
	 	}*/
//-------------------------------------------------------------------------------------------------
	 	//if(rectCount > circleCount || rectCount > triCount || rectCount > pentaCount)
	 	//{ // If we have more rectangles than anything else, lets find out what they are


	 		/*
I want to implement here with the record is something where the system asks:
"How many ingredients?" makes a vector for each of them. Then it will ask the user to drag
a small rectangle in the center and when its stable, hit the middle mouse button. That will
save the first HSV range. Then the system will ask the user to draw another rectangle in
another area of the food 2 more times. Ill make a mask for all of these bitwise_or manner.
It will save the values under the food group entered.

We can have it save the values entered into a file, and have that same file being read everytime
and it will contain all the info on all of our foods we have used before.

In the future: We can have it continuously recalibrate itself over time/3 intervals. Enter it
as Raw, medium, and finished. Then we can save it and have 3 sub-classes of each food class.
	 		*/
			tcflush(serialPort, TCIOFLUSH);
			foodValues.open("foodValues.txt", ios::in | ios::app | ios::out);
			if(checkIfFileOpen(foodValues)) return -1;
			for (static bool first =true; first; first = false)
			{
				getCurrentFoods(foodValues, gline);
				foodValues.close();
				int i = 0;
				while (i < Foodies.size())
				{
					loadLocalHSV();
					storeNewFoods(i);
					setClassValues(Foodies, i);
					//cout << "Out of setClassValues: ";
					//cout << Foodies.at(i).getHSVmin1() << ", " << Foodies.at(i).getHSVmax1() << ", " << Foodies.at(i).getType()<< endl;
					createMasks(Foodies, i);
					i++;
				}
			}

			if (counttest % 70 == 0)
			{
				for (int i = 0; i < Foodies.size(); i++)
				{
					recreateMasks(Foodies, i);
					cout << "In counttest loop: ";
					cout << Foodies.at(i).getHSVmin1() << ", " << Foodies.at(i).getHSVmax1() << endl;
					cout << "Iterration: " << i << endl;
					waitKey(1000);

				}
			}
			trackingObjects(Foodies, ThresholdImgs1, HSV, liveFeed);
			inRange(HSV,PotSticker.getHSVmin1(), PotSticker.getHSVmax1(),thresholdImg3);
			inRange(HSV,PotSticker.getHSVmin2(), PotSticker.getHSVmax2(),thresholdImg4);
			bitwise_or(thresholdImg3,thresholdImg4,thresholdImg9);
			morphOps(thresholdImg9); 	
			trackingObject(PotSticker, thresholdImg9, HSV, liveFeed);
			//PotSticker.setBoarder();
			char const* PotStickerChar = relayCoords(PotSticker, liveFeed);
						if(counttest % 150 == 0 && sizeof(PotStickerChar) > 4)
			{
				write(serialPort, PotStickerChar, 62);
				cout << PotStickerChar << endl;
			}/*
			char const* MeatChar = relayCoords(Meat, liveFeed);
			if(counttest % 200 == 0 && sizeof(MeatChar) > 4)
			{
				write(serialPort, MeatChar, 62);
				cout << MeatChar << endl;
			}*/
		//}
		//if(circleCount > rectCount || circleCount > triCount || circleCount > pentaCount)
		//{

		//}
		//else if(triCount > circleCount || triCount > rectCount || triCount > pentaCount)
		//{

		//}
		//else
		//{

		//}
			counttest += 1;
			imshow(windowOriginal1,liveFeed);
			int key = waitKey(30);
			if (key == 27) break; // Wait .5 seconds before relaying the coords again. Will not work without this,
						  // Make 0 to run forever and not wait

		}
	}
	return 0;
} 	




//======================= FROM FoodAdditional.h ====================================
void trackingObjectCalibration(Mat, Mat, Mat &);
void createTrackbars();

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