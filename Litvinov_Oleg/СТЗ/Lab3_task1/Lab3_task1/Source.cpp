#include <stdlib.h>
#include <stdio.h>
#include <opencv2\core\core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgcodecs.hpp>
#include <iostream>
#include <tuple>
#include <string>

using namespace std;
using namespace cv;

class Allababah
{
public:
	//Constructor
	Allababah(const Mat &input1, const Mat &input2, const Mat&input3, int input_thresh1, int input_thresh2, int input_thresh3)
	{
		img1 = input1.clone();
		img2 = input2.clone();
		img3 = input3.clone();
		thresh1 = input_thresh1;
		thresh2 = input_thresh2;
		thresh3 = input_thresh3;
	}

	//Constructor from existing instance
	Allababah(const Allababah &existing_instance)
	{
		img1 = existing_instance.img1.clone();
		img2 = existing_instance.img2.clone();
		img3 = existing_instance.img3.clone();
		thresh1 = existing_instance.thresh1;
		thresh2 = existing_instance.thresh2;
		thresh3 = existing_instance.thresh3;
	}

	//Default constructor
	Allababah()
	{
	}

	//Default destructor
	~Allababah()
	{
	}

	Mat img1;
	Mat img2;
	Mat img3;
	int thresh1;
	int thresh2;
	int thresh3;
};

void Trackbar_Callback(int pos, void* userdata);
void show_target(const Mat &img, int binary_thresh, int down_thresh, int up_thresh, string str);

int main(int argc, char** argv)
{
	Mat img1 = imread("ig_0.jpg");
	Mat img2 = imread("ig_1.jpg");
	Mat img3 = imread("ig_2.jpg");
	Allababah *babah = new Allababah(img1, img2, img3, 200, 100, 200);

	namedWindow("Threshold window");
	
	createTrackbar("Binary_threshold", "Threshold window", &(babah->thresh1), 255, Trackbar_Callback, babah);
	createTrackbar("Canny_down_threshold", "Threshold window", &(babah->thresh2), 255, Trackbar_Callback, babah);
	createTrackbar("Canny_up_threshold", "Threshold window", &(babah->thresh3), 255, Trackbar_Callback, babah);

	waitKey(0);
	return 0;
}

void Trackbar_Callback(int pos, void* userdata)
{
	Allababah *babah = (Allababah*)userdata;

	cout << "Binary_threshold " << babah->thresh1 << '\n';
	cout << "Canny_down_threshold " << babah->thresh2 << '\n';
	cout << "Canny_up_threshold " << babah->thresh3 << '\n' << '\n';

	show_target(babah->img1, babah->thresh1, babah->thresh2, babah->thresh3, "ig_0");
	show_target(babah->img2, babah->thresh1, babah->thresh2, babah->thresh3, "ig_1");
	show_target(babah->img3, babah->thresh1, babah->thresh2, babah->thresh3, "ig_2");
}

void show_target(const Mat &img, int binary_thresh, int down_thresh, int up_thresh, string str)
{
	Mat img_binary;
	threshold(img, img_binary, binary_thresh, 255, THRESH_BINARY);

	Mat img_canny;
	Canny(img_binary, img_canny, down_thresh, up_thresh, 3, true);

	vector<vector<Point>> img_cnts;
	findContours(img_canny.clone(), img_cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	Mat img_filled_cnts = img.clone();
	drawContours(img_filled_cnts, img_cnts, -1, Scalar(0, 0, 255), CV_FILLED);
	imshow("Image " + str + " with filled contours", img_filled_cnts);

	Mat target = img.clone();
	//Image moments
	for (int i = 0; i < img_cnts.size(); i++)
	{
		if (img_cnts[i].size() > 41)
		{
			Moments mnts = moments(img_cnts[i]);
			double m00 = mnts.m00;
			double m01 = mnts.m01;
			double m10 = mnts.m10;
			int Xc = int(m10 / m00);
			int Yc = int(m01 / m00);
			circle(target, {Xc, Yc}, 4, Scalar(0, 255, 0), -1);
		}
	}
	imshow("target point " + str, target);
}