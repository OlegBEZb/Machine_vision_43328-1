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

/*
Function to search contours via canny algorithm. Before using this algo sample may be 
binarized by using "sample_to_binary" flag
*/
vector<vector<Point>> search_contours(Mat sample, int down_thresh, int up_thresh, bool sample_to_binary = 0);

int main(int argc, char** argv)
{
    Mat wrench_sample = imread("gk_tmplt.jpg");
    Mat wrenches = imread("gk.jpg");
    double similarity_threshold = 0.5;

    vector<vector<Point>> sample_cnts = search_contours(wrench_sample, 100, 200);
    Mat sample_with_cnts = wrench_sample.clone();
    drawContours(sample_with_cnts, sample_cnts, -1, Scalar(0, 0, 255), 3);
    imshow("sample cnts", sample_with_cnts);

    vector<vector<Point>> target_cnts = search_contours(wrenches, 100, 200, 1);
    Mat target_with_cnts = wrenches.clone();
    drawContours(target_with_cnts, target_cnts, -1, Scalar(0, 0, 255), 3);
    imshow("target cnts", target_with_cnts);

    for (int i = 0; i < target_cnts.size(); i++)
    {
        double similarity = matchShapes(sample_cnts[0], target_cnts[i], CV_CONTOURS_MATCH_I2, 0);
        if (similarity > similarity_threshold)
        {
			//deleting non-similar contours from production vector
            target_cnts.erase(target_cnts.begin() + i);
            i--;
        }
    }
	//similar forms will be circled רע green
    Mat prod = target_with_cnts.clone();
    drawContours(prod, target_cnts, -1, Scalar(0, 255, 0), 3);
    imshow("production cnts", prod);

    waitKey(0);
    return 0;
}

vector<vector<Point>> search_contours(Mat sample, int down_thresh, int up_thresh, bool sample_to_binary)
{
    if (sample_to_binary)
    {
		//invariation uses because targets are on the white background
        threshold(sample, sample, 200, 255, THRESH_BINARY_INV);
    }

    Mat sample_canny;
    Canny(sample, sample_canny, down_thresh, up_thresh, 3, true);

    vector<vector<Point>> sample_cnts;
    findContours(sample_canny.clone(), sample_cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    return sample_cnts;
}