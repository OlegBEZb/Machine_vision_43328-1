#define pi 3.14159
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <iostream>
#include <complex>
#include <cmath>

using namespace cv;
using namespace std;

Mat Fourier(Mat img);
void krasivSpektr(Mat &magI);

int main()
{
	Mat img = imread("Fourier.jpg", IMREAD_GRAYSCALE);
	imshow("original", img);

	Mat dft_img = Fourier(img);
	krasivSpektr(dft_img);
	normalize(dft_img, dft_img, 255.0, 0.0, CV_MINMAX);
	imshow("fourier", dft_img);

	waitKey();
	return 0;
}

Mat Fourier(Mat img)
{
	int rows = img.rows;
	int cols = img.cols;
	Mat fourier_img(rows, cols, CV_32FC1);

	double phi1 = 2 * pi / img.rows;
	double phi2 = 2 * pi / img.cols;

	//Массив под прямое ДПФ
	//creating matrix to contain DFT
	complex<double>** dft = new complex<double>*[rows];
	complex<double>** dft2 = new complex<double>*[rows];
	for (int i = 0; i < rows; i++)
	{
		dft[i] = new complex<double>[cols];
		dft2[i] = new complex<double>[cols];
		for (int j = 0; j < cols; j++)
		{
			dft[i][j] = 0;
			dft2[i][j] = 0;
		}
	}

	//  Прямое ДПФ
	//complex<double> temp (0,0);
	for (int k1 = 0; k1 < rows; k1++)  //  Номер строки
	{
		for (int k2 = 0; k2 < cols; k2++)   //  Номер столбца пикселя, в который пишется прямое ДПФ (как в формуле)
		{
			for (int n2 = 0; n2 < cols; n2++)  //  Номер столбца пикселя - гармоники (т.е. номер гармоники) (как в формуле)
			{
				dft[k1][k2] += (double)img.at<uchar>(k1, n2) * complex<double>(cos(-phi2 * n2 * k2), sin(-phi2 * n2 * k2));
			}
		}
	}

	for (int k2 = 0; k2 < cols; k2++)  //  Номер столбца
	{
		for (int k1 = 0; k1 < rows; k1++)   //  Номер строки пикселя, в который пишется прямое ДПФ (как в формуле)
		{
			for (int n1 = 0; n1 < rows; n1++)  //  Номер строки пикселя - гармоники (т.е. номер гармоники) (как в формуле)
			{
				dft2[k1][k2] += dft[n1][k2] * complex<double>(cos(-phi1 * k1 * n1), sin(-phi1 * k1 * n1));
			}
			dft2[k1][k2] /= complex<double>(rows * cols);
			fourier_img.at<float>(k1, k2) = abs(dft2[k1][k2]);
		}
	}


	for (int i = 0; i < rows; i++)
		delete[] dft[i];
	delete[] dft;

	return fourier_img;
}

void krasivSpektr(Mat &magI)
{
	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);
}