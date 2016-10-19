#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/stitching.hpp"
#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;

const int NUM_SECONDS = 3; // The amount of seconds before it checks for new images
bool try_use_gpu = false; // Do you want to use GPU? False is faster
vector<Mat> imgs; // Make a vector of images. Used for stitching
Mat Pano, img1, img2; // Pano - Our stitched image. img1,img2 - our Left and Right images

int main(int argc, char* argv[])
{
  Stitcher stitcher = Stitcher::createDefault(try_use_gpu); // Create a default stitcher
 //   namedWindow("Left Video",CV_WINDOW_AUTOSIZE); //create a window called "Left Video"
//    namedWindow("Right Video", CV_WINDOW_AUTOSIZE);
    namedWindow("Stitched Video"); // Stream a window


clock_t this_time = clock(); // This is all used for ensuring we check every NUM_SECONDS time
clock_t last_time = this_time;
double time_counter = 0;

while(true)
{
    this_time = clock();
    time_counter += (double)(this_time-last_time);
    last_time = this_time;
    if ((time_counter) > (double)(NUM_SECONDS * CLOCKS_PER_SEC))
    {
        VideoCapture Left(0); // open the video camera no. 1
        VideoCapture Right(1); // open the video camera no. 2
        if (!Left.isOpened() || !Right.isOpened())  // if not success, exit program
    {
        cout << "Cannot open one of the video cam" << endl;
        return -1;
    }
    double LWidth = Left.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double LHeight = Left.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

    cout << "Frame size : " << LWidth << " x " << LHeight << endl; // Mainly used to see how often we are getting an image, but useful info

        vector<Mat> imgs; // redeclared since we release/clear all images
        Mat Pano, img1, img2;// redeclared since we release/clear all images
        time_counter -=(double)(NUM_SECONDS * CLOCKS_PER_SEC); // Make it 0
        Left >> img1; // Put our left camera frame into img1
        Right >> img2; // Put our right camera frame into img2
        imgs.push_back(img1); // This pushes back our imgs into other vector spots
        imgs.push_back(img2); // This pushes back our imgs into other vector spots
        //imshow("Left Video", img1); //show the frame in "Left Video" window
        //imshow("Right Video", img1); //show the frame in the "Right Video" window'

        Stitcher::Status status = stitcher.stitch(imgs, Pano); // Stitches all images in imgs vector and outputs it into Pano

        if (status != Stitcher::OK)
        {
            cout << "Can't stitch images, error code = " << int(status) << endl;
            return -1;
        }

        resize(Pano, Pano, Size(640,360),0,0,INTER_CUBIC); // Resize our pano image so its a bit smaller

        imshow("Stitched Video", Pano); // Show it

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            cout << "esc key is pressed by user" << endl;
            break; 
       }
    }
    else
    {// Need to release them all for data issues
        imgs.clear();
        img1.release();
        img2.release();
        Pano.release();
    }

}
return 0;
}