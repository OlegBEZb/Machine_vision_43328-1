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
void show_target(Mat &img, int Hue_down, int Hue_up, int Saturation_down, int Saturation_up, int Value_down, int Value_up, int min_points_cnt, int max_points_cnt, string str);
int distance(int X1, int Y1, int X2, int  Y2);

int main(int argc, char** argv)
{
	Mat *imgs = new Mat[2];
	imgs[0] = imread("roi_robotov.jpg");
	imgs[1] = imread("roi_robotov_1.jpg");

	Swarm_with_thresholds *enemy_vehicles = new Swarm_with_thresholds(imgs, 0, 13, 35, 255, 0, 245);
	enemy_vehicles->convert_rgb2hsv();

	namedWindow("Threshold window");
	createTrackbar("Hue down", "Threshold window", &(enemy_vehicles->Hue_down), 179, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Hue up", "Threshold window", &(enemy_vehicles->Hue_up), 179, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Saturation down", "Threshold window", &(enemy_vehicles->Saturation_down), 255, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Saturation up", "Threshold window", &(enemy_vehicles->Saturation_up), 255, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Value down", "Threshold window", &(enemy_vehicles->Value_down), 255, Trackbar_Callback, enemy_vehicles);
	createTrackbar("Value up", "Threshold window", &(enemy_vehicles->Value_up), 255, Trackbar_Callback, enemy_vehicles);
	createTrackbar("min points ", "Threshold window", &(enemy_vehicles->min_points_cnt), 300, Trackbar_Callback, enemy_vehicles);
	//createTrackbar("max points ", "Threshold window", &(enemy_vehicles->max_points_cnt), 300, Trackbar_Callback, enemy_vehicles);

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
		show_target(swarm_with_thresholds->img[i],
			swarm_with_thresholds->Hue_down, swarm_with_thresholds->Hue_up,
			swarm_with_thresholds->Saturation_down, swarm_with_thresholds->Saturation_up,
			swarm_with_thresholds->Value_down, swarm_with_thresholds->Value_up,
			swarm_with_thresholds->min_points_cnt, swarm_with_thresholds->max_points_cnt, 
			to_string(i + 1));
	}
}

void show_target(Mat &img, int Hue_down, int Hue_up, int Saturation_down, int Saturation_up, int Value_down, int Value_up, int min_points_cnt, int max_points_cnt, string str)
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


	//Selecting robots and finding the nearest of each colour
	Mat blue_pixels;
	inRange(img, Vec3b(90, 0, 0), Vec3b(107, 255, 255), blue_pixels);
	dilate(blue_pixels, blue_pixels, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 1);
	erode(blue_pixels, blue_pixels, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 1);
	vector<vector<Point>> blue_cnts;
	findContours(blue_pixels.clone(), blue_cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	int min_blue_dist, Xblue, Yblue;
	for (int i = 0; i < blue_cnts.size(); i++)
	{
		if (blue_cnts[i].size() < 70)
		{
			//blue_cnts.erase(blue_cnts[i]);//убрать вектора с малым колвом точек
			blue_cnts.erase (blue_cnts.begin()+i);
			i--;
		}
		else
		{
			Moments mnts = moments(blue_cnts[i]);
			double m00 = mnts.m00;
			double m01 = mnts.m01;
			double m10 = mnts.m10;
			int Xc = int(m10 / m00);
			int Yc = int(m01 / m00);
			int dist = distance(Xc, Yc, Xlamp, Ylamp);
			if ((i == 0) || (dist < min_blue_dist))
			{
				min_blue_dist = dist;
				Xblue = Xc;
				Yblue = Yc;
			}
		}
	}
	circle(target, { Xblue, Yblue }, 5, Scalar(120, 255, 255), -1);
	drawContours(target, blue_cnts, -1, Scalar(120, 255, 255), 2);


	//Selecting red robots
	Mat red_pixels;
	//inRange(img, Vec3b(Hue_down, Saturation_down, Value_down), Vec3b(Hue_up, Saturation_up, Value_up), red_pixels); for debug
	inRange(img, Vec3b(0, 35, 0), Vec3b(13, 255, 245), red_pixels);
	dilate(red_pixels, red_pixels, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 1);
	erode(red_pixels, red_pixels, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 1);
	imshow("after both " + str, red_pixels);
	vector<vector<Point>> red_cnts;
	findContours(red_pixels.clone(), red_cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	int min_red_dist, Xred, Yred;
	for (int i = 0; i < red_cnts.size(); i++)
	{
		if ((red_cnts[i].size() < 167) && (red_cnts[i].size() > min_points_cnt))
		{
			Moments mnts = moments(red_cnts[i]);
			double m00 = mnts.m00;
			double m01 = mnts.m01;
			double m10 = mnts.m10;
			int Xc = int(m10 / m00);
			int Yc = int(m01 / m00);
			int dist = distance(Xc, Yc, Xlamp, Ylamp);
			if ((i == 0) || (dist < min_red_dist))
			{
				min_red_dist = dist;
				Xred = Xc;
				Yred = Yc;
			}
		}
		else
		{
			red_cnts.erase(red_cnts.begin() + i);
			i--;
		}
	}
	circle(target, { Xred, Yred }, 5, Scalar(0, 255, 255), -1);
	drawContours(target, red_cnts, -1, Scalar(0, 255, 255), 2);


	//Selecting green robots
	Mat green_pixels;
	inRange(img, Vec3b(54, 59, 143), Vec3b(82, 166, 255), green_pixels);
	dilate(green_pixels, green_pixels, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 1);
	erode(green_pixels, green_pixels, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)), Point(-1, -1), 1);
	vector<vector<Point>> green_cnts;
	findContours(green_pixels.clone(), green_cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	int min_green_dist, Xgreen, Ygreen;
	for (int i = 0; i < green_cnts.size(); i++)
	{
		if (green_cnts[i].size() > 49)
		{
			Moments mnts = moments(green_cnts[i]);
			double m00 = mnts.m00;
			double m01 = mnts.m01;
			double m10 = mnts.m10;
			int Xc = int(m10 / m00);
			int Yc = int(m01 / m00);
			int dist = distance(Xc, Yc, Xlamp, Ylamp);
			if ((i == 0) || (dist < min_green_dist))
			{
				min_green_dist = dist;
				Xgreen = Xc;
				Ygreen = Yc;
			}
		}
		else
		{
			green_cnts.erase(green_cnts.begin() + i);
			i--;
		}
	}
	circle(target, { Xgreen, Ygreen }, 5, Scalar(60, 255, 255), -1);
	drawContours(target, green_cnts, -1, Scalar(60, 255, 255), 2);


	cvtColor(target, target, CV_HSV2BGR);//потом закинуть это под конец перед отрисовкой точек и контуров
	imshow("Target point " + str, target);
}

int distance(int X1, int Y1, int X2, int  Y2)
{
	return int(sqrt(pow((X1 - X2), 2) + pow((Y1 - Y2), 2)));
}