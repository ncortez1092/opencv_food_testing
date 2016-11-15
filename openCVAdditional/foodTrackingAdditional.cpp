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
	fstream foodValues;
	foodValues.open("foodValues.txt", ios::in | ios::app | ios::out);
	setUpSerial();
	if (!foodValues.is_open()) 
	{
		cout << "File not opened " << endl;
		 return -1;
	}
	cout << "How many ingredients?" << endl;
	cin >> ingredientNum;
	for (int i=0; i< ingredientNum; i++)
	{
		cout << "Name of ingredient number "<< (i+1) << ": " << endl;
		cin >> Name;
		LocalNames.push_back(Name);
		LocalHSVMins.push_back(Scalar(0,0,0));
		LocalHSVMaxs.push_back(Scalar(0,0,0));
	}
	if(doCalibrate){
		//create slider bars for HSV filtering
		createTrackbars();
	}


	//video capture object to acquire webcam feed
	VideoCapture capture(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH); // Set width
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT); // Set height
	namedWindow(windowOriginal1);
	setMouseCallback(windowOriginal1, makeCalibrationRectangle, &liveFeed);
	ifClicked = false; 
	mouseHasMoved = false; 
	rectangleIsSelected = false;

	while(1)
	{
// ------------- We capture the video, crop it, and only focus on the pot -----------------------
		capture.read(temp);
		cv::Mat mask = cv::Mat::zeros( temp.rows, temp.cols, CV_8UC1 );
		Point center = Point(330, 177);
		float radius = 80;
		cropX = 330 - radius;
		cropY = 177 - radius;
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
			while( getline (foodValues,gline) )
			{

				sscanf(gline.c_str(), "%s [%f, %f, %f] [%f, %f, %f]", Name, &HSVmintemp1, &HSVmintemp2, &HSVmintemp3, &HSVmaxtemp1, &HSVmaxtemp2, &HSVmaxtemp3);
				cout << Name << HSVmintemp1 << HSVmintemp2 << HSVmintemp3;
				Names.push_back(Name);
				HSVMins.push_back(Scalar(HSVmintemp1, HSVmintemp2, HSVmintemp3));
				HSVMaxs.push_back(Scalar(HSVmaxtemp1, HSVmaxtemp2, HSVmaxtemp3));
				foodValues.close();
				/*for (int i = 0; i < Names.size(); i++)
				{
					if (Name == Names[i]) checkIfOnList[i] = true;
					else checkIfOnList[i] = false;
					cout << checkIfOnList[i];
				}
				for (int i = 0; i < Names.size(); i++)
					{
						if (checkIfOnList[i]) 
						{
							HSVMins[i] = Scalar(HSVmintemp1, HSVmintemp2, HSVmintemp3);
							HSVMaxs[i] = Scalar(HSVmaxtemp1, HSVmaxtemp2, HSVmaxtemp3);
						}
					}*/
			}
			for (static bool first =true; first; first = false)
			{
				int i = 0;
				while (i < ingredientNum)
				{
					for (int k = 0; k < LocalNames.size(); k++)
					{
						for (int j = 0; j < Names.size(); j++)
						{
							if (LocalNames[k] == Names[j])
							{
								LocalHSVMins[k] = HSVMins[j];
								cout << "Debug.." << LocalHSVMins[k] << endl;
							}
						}
					}

				if(LocalHSVMins[i] == Scalar(0,0,0))
				{	cout << "Make rectangle in center of food " << LocalNames[i] << " until it tracks, then hit the Q key" << endl;
					unsigned long now = clock();
					while ((clock()- now)/CLOCKS_PER_SEC <= .01000)
					{
								//capture.read(liveFeed);
								//circle( mask, center, radius, Scalar(255,255,255), -1, 8, 0 ); //-1 means filled
								//temp.copyTo( liveFeed, mask );
								//liveFeed = liveFeed(myCropROI);
								//resize(liveFeed, liveFeed, Size(400,400));
						imshow(windowOriginal1,liveFeed);
						recordHSV(liveFeed, HSV);
						inRange(HSV, Scalar(Hmin, Smin, Vmin), Scalar(Hmax,Smax,Vmax), thresholdImg);
						//trackingObject(PotSticker,thresholdImg,HSV,liveFeed);
						int key = waitKey(300);
						if (key == 27 | key == 'q' | key == 'Q') break;
					}
					cout << LocalHSVMins[i];
					cout << HSVMINS[HSVMINS.size()-1];
					LocalHSVMins[i] = HSVMINS[HSVMINS.size()-1];
					LocalHSVMaxs[i] = HSVMAXS[HSVMAXS.size()-1];

					foodValues.open("foodValues.txt", ios::in | ios::app | ios::out);
					cout << "Adding.. " << endl;
					foodValues << LocalNames[i];
					cout << LocalNames[i] << " ";
					foodValues << " ";
					foodValues << LocalHSVMins[i];
					cout << LocalHSVMins[i] << " ";
					foodValues << " ";
					foodValues << LocalHSVMins[i] << endl;
					cout << LocalHSVMaxs[i] << endl;
					foodValues.close();
				}

				//imshow(windowOriginal1,liveFeed);
				//waitKey(50);
				i++;
				//waitKey(5000);
				}
			}

			//inRange(HSV,Bread.getHSVmin1(), Bread.getHSVmax1(),thresholdImg1);
			//inRange(HSV,Bread.getHSVmin3(), Bread.getHSVmax3(),thresholdImg2);
			//inRange(HSV,PotSticker.getHSVmin1(), PotSticker.getHSVmax1(),thresholdImg3);
			//inRange(HSV,PotSticker.getHSVmin2(), PotSticker.getHSVmax2(),thresholdImg4);
			//inRange(HSV,Meat.getHSVmin1(), Meat.getHSVmax1(),thresholdImg5);
			//inRange(HSV,Meat.getHSVmin3(), Meat.getHSVmax3(),thresholdImg6);
			//inRange(HSV,Broccoli.getHSVmin1(), Broccoli.getHSVmax1(),thresholdImg7);
			//inRange(HSV,Broccoli.getHSVmin2(), Broccoli.getHSVmax2(),thresholdImg8);
			//inRange(HSV,Carrot.getHSVmin1(), Carrot.getHSVmax1(),thresholdImg1);
			//inRange(HSV,Carrot.getHSVmin3(), Carrot.getHSVmax3(),thresholdImg2);
			//bitwise_or(thresholdImg1,thresholdImg2,thresholdImg);
			//bitwise_or(thresholdImg3,thresholdImg4,thresholdImg9);
			//bitwise_or(thresholdImg5,thresholdImg6,thresholdImg10);
			//bitwise_or(thresholdImg7,thresholdImg8,thresholdImg11);
			//morphOps(thresholdImg);
			//morphOps(thresholdImg9); 	
			//morphOps(thresholdImg10);
			//morphOps(thresholdImg11);
			//imshow(windowThresh,thresholdImg5);
			//trackingObject(Bread,thresholdImg,HSV,liveFeed);
			//trackingObject(Meat,thresholdImg10,HSV2,liveFeed);
			//trackingObject(TriangleMeat,thresholdImg11,HSV,liveFeed);
			//trackingObject(Broccoli,thresholdImg11,HSV,liveFeed);
			//trackingObject(Carrot,thresholdImg,HSV,liveFeed);
			//PotSticker.setBoarder();
			//Bread.setBoarder();
			//Meat.setBoarder();
			//TriangleMeat.setBoarder();
			//char const* BreadChar = relayCoords(Bread, liveFeed);
			/*char const* BroccoliChar = relayCoords(Broccoli, liveFeed);
			if(counttest % 150 == 0 && sizeof(BroccoliChar) > 4)
			{
				write(serialPort, BroccoliChar, 62);
				cout << BroccoliChar << endl;
			}
			char const* CarrotChar = relayCoords(Carrot,liveFeed);
			if(counttest % 160 == 0 && sizeof(CarrotChar) > 4)
			{
				write(serialPort, CarrotChar, 62);
				cout << CarrotChar << endl;
			}*/
			
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
			//char const* TriangleMeatChar = relayCoords(TriangleMeat, liveFeed);
			/*if (counttest % 60 == 0 && sizeof(BreadChar) > 4)
			{
			write(serialPort, BreadChar, 62);
			cout << BreadChar << endl;
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
			//video.write(liveFeed);
			imshow(windowOriginal1,liveFeed);
		//	imshow(windowOriginal2,liveFeed);

        
			int key = waitKey(30);
			if (key == 27) break; // Wait .5 seconds before relaying the coords again. Will not work without this,
						  // Make 0 to run forever and not wait

		}
	}

		return 0;
} 	
