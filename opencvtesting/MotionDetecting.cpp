// We attempt motion detection




#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/stitching.hpp"
#include <iostream>
#include <time.h>
#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;
using namespace std;


const int i = 5; // Blur ammount
Mat old,frame,temp,gray1,blob,delta1,gray2,imgThresholded, imgHSV, imgTmp; // All images we use
int counter = 0;
vector<KeyPoint> keypoints;

int main(int argc, char* argv[])
{
	VideoCapture cap(2);
//	namedWindow("Camera", CV_WINDOW_AUTOSIZE);
	namedWindow("Thresh", CV_WINDOW_AUTOSIZE);
	namedWindow("Control");
	namedWindow("Keypoints");

	SimpleBlobDetector::Params params;


	// Change thresholds
	params.minThreshold = 10;
	params.maxThreshold = 200;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 1500;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01;
	if (!cap.isOpened())
	{
		cout << "Couldn't find camera" << endl;
		return -1;
	}
//	Point2f posX[5];
//	Point2f posY[5];

	int iLowH = 170;
	int iHighH = 179;

	int iLowS = 150;
 	int iHighS = 255;

 	int iLowV = 60;
 	int iHighV = 255;

 	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
 	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
 	createTrackbar("HighS", "Control", &iHighS, 255);

 	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
 	createTrackbar("HighV", "Control", &iHighV, 255);



	int iLastX = -1;
	int iLastY = -1;


	cap >> imgTmp;

	while(true)
	{

		cap >> frame;


	//	resize(frame,frame,Size(640,480),0,0,INTER_CUBIC);
		cvtColor(frame, imgHSV, COLOR_BGR2HSV);
		inRange(imgHSV,Scalar(iLowH,iLowS,iLowV),Scalar(iHighH,iHighS,iHighV),imgThresholded);

 		 //morphological opening (removes small objects from the foreground)
  		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

  		//morphological closing (removes small holes from the foreground)
  		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
  		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

 		 //Calculate the moments of the thresholded image
 		 Moments oMoments = moments(imgThresholded);



 		 int minHessian = 400;

		// Set up detector with params
		Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);   

		// Detect blob
		detector->detect( imgThresholded, keypoints);
/*
 		 for (int k = 1; k < 5; k++)
 		 {
 		 	posX[k] = keypoints[k].pt;
 		 	posY[k] = keypoints[k].pt;

 		 	cout << "Position for the " << k << "'th iteration is" << posX[k] << " " << posY[k] << endl;
 		 }*/

 		 Mat kpImg;
 		 drawKeypoints(imgThresholded,kpImg,Scalar(0,0,225),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
 		 imshow("Camera", frame);
 		 imshow("Threshold", imgThresholded);
 		 imshow("Keypoints", kpImg);

 		 		if (waitKey(30) == 27)
		{
			cout << "esc pressd by user" << endl;
			break;
		}
 		
 	}
 		return 0;
 }




 		 /*

 		 double m01 = oMoments.m01;
 		 double m10 = oMoments.m10;
 		 double mArea = oMoments.m00;

 		 if(mArea > 100)
 		 {
 		 	int posX = m10 / mArea;
 		 	int posY = m01 / mArea;
 		 }



		if(temp.empty())
		{

			frame.copyTo(temp);

		}
		else
		{
			temp = old;
		}


		cvtColor(temp, gray1, CV_BGR2GRAY);
		cvtColor(frame, gray2, CV_BGR2GRAY);
		absdiff(gray1,gray2,delta1);
		threshold(delta1,thresh, 200, 255, CV_THRESH_BINARY);
		blur(thresh, blob, Size(i,i));

		imshow("Blob", gray2);
		imshow("Camera",delta1);
		imshow("Thresh", gray1);
		old = frame;
		if (waitKey(30) == 27)
		{
			cout << "esc pressd by user" << endl;
			break;
		}


	}



	return 0;
}

*/