#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<iostream>
#include <stdlib.h>//for rand
#include<stdio.h>//for sprintf
//#include<conio.h>
using namespace cv;
using namespace std;

Mat handle_blur(Mat original, int width, int height);
Mat similarity(Mat blurred, Mat handle_blurred);

Mat handle_blur(Mat original, int width, int height)
{
	int nRows = original.rows;
    int nCols = original.cols;

	Mat blurred(nRows, nCols, CV_8UC1);

    for (int i = 0; i < nRows-height; i++)
    {
        for (int j = 0; j < nCols-width; j++)
        {
            Rect rp(i, j, width, height);
            Mat roi = original(rp);
            Scalar roi_avg = mean(roi);
			blurred.at<uchar>(Point(i+width / 2, j +height / 2)) = floor(roi_avg[0]+0.5);
        }
    }

	return blurred;
}
Mat similarity(Mat blurred, Mat handle_blurred)
{
	int pixels = 0;
	int correct = 0;

	if ((blurred.rows == handle_blurred.rows) && (blurred.cols == handle_blurred.cols))
	{
		cout << "Image sizes are the same\n";
	}
	else
	{
		cout << "Problem with image sizes. I don't want to deal with it";
	}

	for (int i = 0; i < blurred.rows; i++)
	{
		for (int j = 0; j < blurred.cols; j++)
		{
			if (int(blurred.at<uchar>(i, j)) == int(handle_blurred.at<uchar>(i, j)))
			{
				correct++;
			}
			pixels++;
		}
	}
	cout << correct * 100.0 / pixels << " % of similarity";

	Mat sub;
	subtract(handle_blurred, blurred, sub);
	return sub;
}
class CV_EXPORTS TickMeter
{
public:
	TickMeter();
	void start();
	void stop();

	int64 getTimeTicks() const;
	double getTimeMicro() const;
	double getTimeMilli() const;
	double getTimeSec()   const;
	int64 getCounter() const;

	void reset();
private:
	int64 counter;
	int64 sumTime;
	int64 startTime;
};

std::ostream& operator << (std::ostream& out, const TickMeter& tm);

TickMeter::TickMeter() { reset(); }
int64 TickMeter::getTimeTicks() const { return sumTime; }
double TickMeter::getTimeMicro() const { return  getTimeMilli()*1e3; }
double TickMeter::getTimeMilli() const { return getTimeSec()*1e3; }
double TickMeter::getTimeSec() const { return (double)getTimeTicks() / cv::getTickFrequency(); }
int64 TickMeter::getCounter() const { return counter; }
void TickMeter::reset() { startTime = 0; sumTime = 0; counter = 0; }

void TickMeter::start(){ startTime = cv::getTickCount(); }
void TickMeter::stop()
{
	int64 time = cv::getTickCount();
	if (startTime == 0)
		return;
	++counter;
	sumTime += (time - startTime);
	startTime = 0;
}

std::ostream& operator << (std::ostream& out, const TickMeter& tm) { return out << tm.getTimeSec() << "sec"; }

int main()
{
	Mat original = imread("Lenna.png", CV_LOAD_IMAGE_GRAYSCALE);
	imshow("Original", original);

	Mat blurred;
	TickMeter timer;
	timer.start();
	blur(original, blurred, Size(3, 3));
	timer.stop();
	cout << timer.getTimeMilli() << " ms for original blur\n";
	imshow("Blurred", blurred);

	Mat handle_blurred;
	timer.reset();
	timer.start();
	handle_blurred = handle_blur(original, 3, 3);
	timer.stop();
	cout << timer.getTimeMilli() << " ms for handle blur\n";
	imshow("Handle blurred", handle_blurred);

	Mat sub = similarity(blurred, handle_blurred);
	imshow("Subtraction", sub);

	waitKey(0);
	return 1;
}