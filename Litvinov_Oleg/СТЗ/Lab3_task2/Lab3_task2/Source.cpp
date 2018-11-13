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

class Enemy_vehicles
{
public:
    //contains vehicle images
    Mat *img = new Mat[5];

    //values of hsv thresholds
    int Hue_down;
    int Hue_up;
    int Saturation_down;
    int Saturation_up;
    int Value_down;
    int Value_up;

	//min points for contour
    int Points_cnt = 15;

    //Constructor
    Enemy_vehicles(Mat *imgs,
                   int input_Hue_down, int input_Hue_up,
                   int input_Saturation_down, int input_Saturation_up,
                   int input_Value_down, int input_Value_up)
    {
        for (int i = 0; i < 5; i++)
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
    Enemy_vehicles(const Enemy_vehicles &existing_instance)
    {
        for (int i = 0; i < 5; i++)
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
    Enemy_vehicles()
    {
    }

    //Default destructor
    ~Enemy_vehicles()
    {
    }

    void convert_rgb2hsv()
    {
        for (int i = 0; i < 5; i++)
        {
            cvtColor(img[i], img[i], CV_BGR2HSV);
        }
    }
};

void Trackbar_Callback(int pos, void* userdata);
void show_target(Mat &img, int Hue_down, int Hue_up, int Saturation_down, int Saturation_up, int Value_down, int Value_up, int Points_cnt, string str);

int main(int argc, char** argv)
{
    Mat *imgs = new Mat[5];
    imgs[0] = imread("ig_1.jpg");
    imgs[1] = imread("ig_2.jpg");
    imgs[2] = imread("ig_3.jpg");
    imgs[3] = imread("ig_4.jpg");
    imgs[4] = imread("ig_5.jpg");

    Enemy_vehicles *enemy_vehicles = new Enemy_vehicles(imgs, 0, 16, 0, 255, 0, 255);
	enemy_vehicles->convert_rgb2hsv();

    namedWindow("Threshold window");
    createTrackbar("Hue down", "Threshold window", &(enemy_vehicles->Hue_down), 179, Trackbar_Callback, enemy_vehicles);
    createTrackbar("Hue up", "Threshold window", &(enemy_vehicles->Hue_up), 179, Trackbar_Callback, enemy_vehicles);
    createTrackbar("Saturation down", "Threshold window", &(enemy_vehicles->Saturation_down), 255, Trackbar_Callback, enemy_vehicles);
    createTrackbar("Saturation up", "Threshold window", &(enemy_vehicles->Saturation_up), 255, Trackbar_Callback, enemy_vehicles);
    createTrackbar("Value down", "Threshold window", &(enemy_vehicles->Value_down), 255, Trackbar_Callback, enemy_vehicles);
    createTrackbar("Value up", "Threshold window", &(enemy_vehicles->Value_up), 255, Trackbar_Callback, enemy_vehicles);
    createTrackbar("Point in contour", "Threshold window", &(enemy_vehicles->Points_cnt), 100, Trackbar_Callback, enemy_vehicles);

    waitKey(0);
    return 0;
}

void Trackbar_Callback(int pos, void* userdata)
{
    Enemy_vehicles *enemy_vehicles = (Enemy_vehicles*)userdata;

    cout << "Hue down " << enemy_vehicles->Hue_down << '\n';
    cout << "Hue up " << enemy_vehicles->Hue_up << '\n' << '\n';
    cout << "Saturation down " << enemy_vehicles->Saturation_down << '\n';
    cout << "Saturation up " << enemy_vehicles->Saturation_up << '\n' << '\n';
    cout << "Value down " << enemy_vehicles->Value_down << '\n';
    cout << "Value up " << enemy_vehicles->Value_up << '\n' << '\n';
    cout << "Points in contour " << enemy_vehicles->Points_cnt << '\n';
    cout << "--------------------\n";

    for (int i = 0; i < 5; i++)
    {
        show_target(enemy_vehicles->img[i],
                    enemy_vehicles->Hue_down, enemy_vehicles->Hue_up,
                    enemy_vehicles->Saturation_down, enemy_vehicles->Saturation_up,
                    enemy_vehicles->Value_down, enemy_vehicles->Value_up,
                    enemy_vehicles->Points_cnt,
                    to_string(i+1));
    }
}

void show_target(Mat &img, int Hue_down, int Hue_up, int Saturation_down, int Saturation_up, int Value_down, int Value_up, int Points_cnt, string str)
{
	//Selecting "near red" and bright pixels
    Mat pixels_in_range;
    inRange(img, Vec3b(Hue_down, Saturation_down, Value_down), Vec3b(Hue_up, Saturation_up, Value_up), pixels_in_range);

    vector<vector<Point>> img_cnts;
    findContours(pixels_in_range.clone(), img_cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	Mat target = img.clone();
	//Converting image back to RGB
    cvtColor(target, target, CV_HSV2BGR);
	
    //Image moments
    for (int i = 0; i < img_cnts.size(); i++)
    {
        if (img_cnts[i].size() > Points_cnt)
        {
            Moments mnts = moments(img_cnts[i]);
            double m00 = mnts.m00;
            double m01 = mnts.m01;
            double m10 = mnts.m10;
            int Xc = int(m10 / m00);
            int Yc = int(m01 / m00);
            circle(target, { Xc, Yc }, 7, Scalar(0, 0, 0), -1);
        }
    }
    imshow("Target point " + str, target);
}