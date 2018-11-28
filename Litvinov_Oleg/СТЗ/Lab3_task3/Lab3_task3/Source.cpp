#include <stdlib.h>
#include <stdio.h>
#include <opencv2\core\core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgcodecs.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

class Swarm_with_thresholds
{
public:
	//contains vehicle images
	Mat *img = new Mat[2];

	//values of hsv thresholds
	int Hue_down;
	int Hue_up;
	int Saturation_down;
	int Saturation_up;
	int Value_down;
	int Value_up;

	//dilate and erode kernel
	int kernel_size=5;

	//min and max points for contour
	int min_points_cnt = 70;
	int max_points_cnt = 167;

	//Constructor
	Swarm_with_thresholds(Mat *imgs,
		int input_Hue_down, int input_Hue_up,
		int input_Saturation_down, int input_Saturation_up,
		int input_Value_down, int input_Value_up)
	{
		for (int i = 0; i < 2; i++)
		{
			img[i] = imgs[i];
		}

		Hue_down = input_Hue_down;
		Hue_up = input_Hue_up;
		Saturation_down = input_Saturation_down;
		Saturation_up = input_Saturation_up;
		Value_down = input_Value_down;
		Value_up = input_Value_up;
	}

	//Constructor from existing instance
	Swarm_with_thresholds(const Swarm_with_thresholds &existing_instance)
	{
		for (int i = 0; i < 2; i++)
		{
			img[i] = existing_instance.img[i].clone();
		}

		Hue_down = existing_instance.Hue_down;
		Hue_up = existing_instance.Hue_up;
		Saturation_down = existing_instance.Saturation_down;
		Saturation_up = existing_instance.Saturation_up;
		Value_down = existing_instance.Value_down;
		Value_up = existing_instance.Value_up;
	}

	//Default constructor
	Swarm_with_thresholds()
	{
	}

	//Default destructor
	~Swarm_with_thresholds()
	{
	}

	void convert_rgb2hsv()
	{
		for (int i = 0; i < 2; i++)
		{
			cvtColor(img[i], img[i], CV_BGR2HSV);
		}
	}
};

void Trackbar_Callback(int pos, void* userdata);
void show_target(int kernel_size, Mat &img, int Hue_down, int Hue_up, int Saturation_down, int Saturation_up, int Value_down, int Value_up, int min_points_cnt, int max_points_cnt, string str);
int distance(int X1, int Y1, int X2, int  Y2);
Mat selecting_robots(Mat &img, Mat &target,
	int Hue_down, int Hue_up, int Saturation_down, int Saturation_up, int Value_down, int Value_up,
	int min_points_cnt, int max_points_cnt, int kernel_size, int Xlamp, int Ylamp,
	Scalar colour);

int main(int argc, char** argv)
{
	Mat *imgs = new Mat[2];
	imgs[0] = imread("roi_robotov.jpg");//0
	imgs[1] = imread("roi_robotov_1.jpg");//1

	Swarm_with_thresholds *enemy_vehicles = new Swarm_with_thresholds(imgs, 0, 13, 35, 255, 0, 245);
	enemy_vehicles->convert_rgb2hsv();

	namedWindow("Threshold window");
	createTrackbar("Hue down", "Threshold window", &(enemy_vehicles->Hue_down), 179, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Hue up", "Threshold window", &(enemy_vehicles->Hue_up), 179, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Saturation down", "Threshold window", &(enemy_vehicles->Saturation_down), 255, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Saturation up", "Threshold window", &(enemy_vehicles->Saturation_up), 255, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Value down", "Threshold window", &(enemy_vehicles->Value_down), 255, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Value up", "Threshold window", &(enemy_vehicles->Value_up), 255, Trackbar_Callback, enemy_vehicles);
	//createTrackbar("min points ", "Threshold window", &(enemy_vehicles->min_points_cnt), 300, Trackbar_Callback, enemy_vehicles);
	//createTrackbar("max points ", "Threshold window", &(enemy_vehicles->max_points_cnt), 300, Trackbar_Callback, enemy_vehicles);
	//createTrackbar("kernel size ", "Threshold window", &(enemy_vehicles->kernel_size), 15, Trackbar_Callback, enemy_vehicles);

	waitKey(0);
	return 0;
}

void Trackbar_Callback(int pos, void* userdata)
{
	Swarm_with_thresholds *swarm_with_thresholds = (Swarm_with_thresholds*)userdata;

	cout << "Hue down " << swarm_with_thresholds->Hue_down << '\n';
	cout << "Hue up " << swarm_with_thresholds->Hue_up << '\n' << '\n';
	cout << "Saturation down " << swarm_with_thresholds->Saturation_down << '\n';
	cout << "Saturation up " << swarm_with_thresholds->Saturation_up << '\n' << '\n';
	cout << "Value down " << swarm_with_thresholds->Value_down << '\n';
	cout << "Value up " << swarm_with_thresholds->Value_up << '\n' << '\n';
	cout << "min points in contour " << swarm_with_thresholds->min_points_cnt << '\n';
	cout << "max points in contour " << swarm_with_thresholds->max_points_cnt << '\n';
	cout << "--------------------\n";

	for (int i = 0; i < 2; i++)
	{
		show_target(swarm_with_thresholds->kernel_size, swarm_with_thresholds->img[i],
			swarm_with_thresholds->Hue_down, swarm_with_thresholds->Hue_up,
			swarm_with_thresholds->Saturation_down, swarm_with_thresholds->Saturation_up,
			swarm_with_thresholds->Value_down, swarm_with_thresholds->Value_up,
			swarm_with_thresholds->min_points_cnt, swarm_with_thresholds->max_points_cnt, 
			to_string(i + 1));
	}
}

void show_target(int kernel_size, Mat &img, int Hue_down, int Hue_up, int Saturation_down, int Saturation_up, int Value_down, int Value_up, int min_points_cnt, int max_points_cnt, string str)
{
	Mat target = img.clone();

	//Selecting lamp and finding center point
	Mat lamp_pixels;
	inRange(img, Vec3b(0, 0, 248), Vec3b(179, 10, 255), lamp_pixels);
	vector<vector<Point>> lamp_cnts;
	findContours(lamp_pixels.clone(), lamp_cnts, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	int Xlamp, Ylamp;
	for (int i = 0; i < lamp_cnts.size(); i++)
	{
		if (lamp_cnts[i].size() > 70)
		{
			Moments mnts = moments(lamp_cnts[i]);
			double m00 = mnts.m00;
			double m01 = mnts.m01;
			double m10 = mnts.m10;
			Xlamp = int(m10 / m00);
			Ylamp = int(m01 / m00);
			circle(target, { Xlamp, Ylamp }, 5, Scalar(0, 0, 0), -1);
		}
	}


	//Selecting blue robots and finding the nearest of them to the lamp
	target = selecting_robots(img, target,
		90, 107, 0, 255, 0, 255,
		70, 500, kernel_size, Xlamp, Ylamp, Scalar(120, 255, 255));	
	
	//Selecting red robots and finding the nearest of them to the lamp
	target = selecting_robots(img, target,
		Hue_down, Hue_up, Saturation_down, Saturation_up, Value_down, Value_up,
		70, 167, kernel_size, Xlamp, Ylamp, Scalar(0, 255, 255));
	
	//Selecting green robots and finding the nearest of them to the lamp
	target = selecting_robots(img, target,
		54, 82, 59, 166, 143, 255,
		49, 500, kernel_size, Xlamp, Ylamp, Scalar(60, 255, 255));


	cvtColor(target, target, CV_HSV2BGR);//потом закинуть это под конец перед отрисовкой точек и контуров
	imshow("Target point " + str, target);
}

int distance(int X1, int Y1, int X2, int  Y2)
{
	return int(sqrt(pow((X1 - X2), 2) + pow((Y1 - Y2), 2)));
}

Mat selecting_robots(Mat &img, Mat &target,
	int Hue_down, int Hue_up, int Saturation_down, int Saturation_up, int Value_down, int Value_up, 
	int min_points_cnt, int max_points_cnt, int kernel_size, int Xlamp, int Ylamp,
	Scalar colour)
{
	Mat colour_pixels;
	inRange(img, Vec3b(Hue_down, Saturation_down, Value_down), Vec3b(Hue_up, Saturation_up, Value_up), colour_pixels);
	dilate(colour_pixels, colour_pixels, getStructuringElement(MORPH_ELLIPSE, Size(kernel_size, kernel_size)), Point(-1, -1), 1);
	erode(colour_pixels, colour_pixels, getStructuringElement(MORPH_ELLIPSE, Size(kernel_size, kernel_size)), Point(-1, -1), 1);
	if (colour == Scalar(0, 255, 255))
	{
		//Mat imrow = colour_pixels.row(i);
		bool flag_begin = 0;
		int counter = 0;
		int min = 10;
		int max = 20;
		for (int j = 0; j < colour_pixels.cols; j++)
		{
			counter += flag_begin * (1);
			if (colour_pixels.at<uchar>(colour_pixels.rows-1, j) == 255)
			{
				if (flag_begin == 0)
				{
					flag_begin = 1;
					counter = 0;
				}
				else
				{
					if ((counter > min) && (counter < max))
					{
						//imshow("selection before", colour_pixels.row(i));
						for (int k = counter; k > 0; k--)
						{
							colour_pixels.at<uchar>(colour_pixels.rows-1, j - k) = 255;
						}
						//imshow("selection after", colour_pixels.row(i));
					}
					else
					{
						flag_begin = 1;
						counter = 0;
					}
				}
			}
		}
	}
	if (colour == Scalar(120, 255, 255))
	{
		for (int i = 0; i < colour_pixels.rows; i++)
		{
			//Mat imrow = colour_pixels.row(i);
			bool flag_begin = 0;
			int counter = 0;
			int min = 10;
			int max = 20;
			for (int j = 0; j < colour_pixels.cols; j++)
			{
				counter += flag_begin * (1);
				if (colour_pixels.at<uchar>(i, j) == 255)
				{
					if (flag_begin == 0)
					{
						flag_begin = 1;
						counter = 0;
					}
					else
					{
						if ((counter > min) && (counter < max))
						{
							//imshow("selection before", colour_pixels.row(i));
							for (int k = counter; k > 0; k--)
							{
								colour_pixels.at<uchar>(i, j - k) = 255;
							}
							//imshow("selection after", colour_pixels.row(i));
						}
						else
						{
							flag_begin = 1;
							counter = 0;
						}
					}
				}
			}
		}
	}
	imshow("after", colour_pixels);
	vector<vector<Point>> colour_cnts;
	findContours(colour_pixels.clone(), colour_cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	int min_red_dist, Xcolour=0, Ycolour=0;
	for (int i = 0; i < colour_cnts.size(); i++)
	{
		if ((colour_cnts[i].size() < max_points_cnt) && (colour_cnts[i].size() > min_points_cnt))
		{
			Moments mnts = moments(colour_cnts[i]);
			double m00 = mnts.m00;
			double m01 = mnts.m01;
			double m10 = mnts.m10;
			int Xc = int(m10 / m00);
			int Yc = int(m01 / m00);
			int dist = distance(Xc, Yc, Xlamp, Ylamp);
			if ((i == 0) || (dist < min_red_dist))
			{
				min_red_dist = dist;
				Xcolour = Xc;
				Ycolour = Yc;
			}
		}
		else
		{
			colour_cnts.erase(colour_cnts.begin() + i);
			i--;
		}
	}
	circle(target, { Xcolour, Ycolour }, 5, colour, -1);
	drawContours(target, colour_cnts, -1, colour, 2);
	return target;
}
