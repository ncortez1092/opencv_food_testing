

//============================ Declarations =====================================================


// The below includes are normal includes plus one for our Food class
#include <sstream>
#include <string>
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "snap.h"

using namespace std;
using namespace cv;

//========================= End Declarations =========================================================




//==================================== Initializations ===================================================

	Mat temp;
	Mat mask;
	Mat snap;
	Mat snapHSV;

//====================== End Initializations =====================================================






int main(int argc, char* argv[])
{

	VideoCapture cap(1);

	if(!cap.isOpened())
	{
		cout << "Camera could not be found..." << endl;
		return -1;
	}


	while(1)
	{
		cap >> temp;

		Rect myCropROI(185, 60 , 280 , 210);




		cv::Mat mask = cv::Mat::zeros( temp.rows, temp.cols, CV_8UC1 );
		Point center = Point(330, 170);
		float radius = 100;
		circle( mask, center, radius, Scalar(255,255,255), -1, 8, 0 ); //-1 means filled
		namedWindow("test"); // debug
		imshow("test", mask); // debug
		temp.copyTo( snap, mask );
		snap = snap(myCropROI);
		cvtColor(snap,snapHSV,COLOR_BGR2HSV); 

// --------------------------- This is where we make our HSV image ---------------------------------------------------------

		cvtColor(snap, snapHSV, COLOR_BGR2HSV);

// ----------------------------------------------------------------------------------------------------------------------------

		imshow("Original", snap);
		imshow("HSV", snapHSV);

		imwrite("finishedpotStickerLight_11am_3.png",snap);
		//imwrite("snapHSV_7pmnolight.png",snapHSV);
		if(waitKey(30) >= 0) break;
	}










return 0;
}