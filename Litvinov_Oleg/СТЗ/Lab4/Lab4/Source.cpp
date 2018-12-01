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

complex<double>** Fourier(Mat &img, Mat &fourier_img);
void idft_cust(complex<double>** dft, Mat &img, int rows, int cols);
void show_my_fourier(Mat &magI, String name);

void show_fourier(Mat &complexI, String name);
void shift_rect(Mat& fourier_img);
void correlation(Mat img, Mat sign, String str);


int main()
{
    namedWindow("main");

    /*
    Press 1 after launching to use "your" dft

    There are 3 (named from 2 to 4) tasks in this lab:
    2: Perform a convolution of an image with such filter kernels:
    Sobel(horizontal and vertical), Laplace, averaging filter

    3: Take some image and in its spectrum cut off in one case the elements
    of the spectrum with high frequencies, in the other - low. And then perform
    the inverse transform based on the spectra obtained

    4: Perform correlation (comparison) images of car numbers by turns with 3 symbols and
    by making idft find left-top point of the symbols on the car number
    */
    char task = waitKey(0);
    switch (task)
    {
    case '1':
    {
        Mat img = imread("Lenna.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        imshow("original", img);
        Mat fourier(img.rows, img.cols, CV_32FC1);

        complex<double>** dft = Fourier(img, fourier);
        show_my_fourier(fourier, "dft");

        Mat idftimg(img.rows, img.cols, CV_8UC1);
        idft_cust(dft, idftimg, img.rows, img.cols);

        imshow("idft", idftimg);
        waitKey();

        break;
    }
    case '2':
    {
        Mat img = imread("Lenna.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        imshow("original", img);

        Mat dft_img(img.size(), CV_32FC2);
        img.convertTo(img, CV_32FC1);
        dft(img, dft_img, DFT_COMPLEX_OUTPUT);
        show_fourier(dft_img, "img");
        char action = waitKey(0);
        float kernel[3][3];
        switch (action)
        {
        case 'h':
        {
            float Sobel_hor[][3] = { -1, -2, -1,
                                     0, 0, 0,
                                     1, 2, 1
                                   };
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    kernel[i][j] = Sobel_hor[i][j];
                }
            }
            break;
        }
        case 'v':
        {
            float Sobel_vert[][3] = { -1, 0, 1,
                                      -2, 0, 2,
                                      -1, 0, 1
                                    };
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    kernel[i][j] = Sobel_vert[i][j];
                }
            }
            break;
        }
        case 'l':
        {
            float Laplace[][3] = { 0, 1, 0,
                                   1, -4, 1,
                                   0, 1, 0
                                 };
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    kernel[i][j] = Laplace[i][j];
                }
            }
            break;
        }
        case 'a':
        {
            float AverBF[][3] = { 1, 1, 1,
                                  1, 1, 1,
                                  1, 1, 1
                                };
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    kernel[i][j] = AverBF[i][j] / 9;
                }
            }
            break;
        }
        default:
            break;
        }

        Mat mat_kernel(img.size(), CV_32FC1, Scalar(0));
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                mat_kernel.at<float>(i, j) = kernel[i][j];
            }
        }

        Mat dft_kernel(img.size(), CV_32FC2);
        dft(mat_kernel, dft_kernel, DFT_COMPLEX_OUTPUT);
        show_fourier(dft_kernel, "kernel");

        Mat res(img.size(), CV_32FC2);
        mulSpectrums(dft_kernel, dft_img, res, 0, 0);
        show_fourier(res, "mulSpectrums");

        //inverse dft for mulSpectrums
        Mat idft_img(img.size(), CV_32FC1);
        dft(res, idft_img, DFT_INVERSE | DFT_REAL_OUTPUT);
        normalize(idft_img, idft_img, 0.0, 255, CV_MINMAX);
        idft_img.convertTo(idft_img, CV_8UC1);
        imshow("result", idft_img);
        break;
    }
    case '3':
    {
        Mat img = imread("Lenna.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        imshow("original", img);

        Mat dft_img(img.size(), CV_32FC2);
        img.convertTo(img, CV_32FC1);
        dft(img, dft_img, DFT_COMPLEX_OUTPUT);
        show_fourier(dft_img, "img");

        Mat mat_kernel(img.size(), CV_32FC2, Scalar(0));
        circle(mat_kernel, { img.rows / 2, img.cols / 2 }, 60, Scalar(1, 0), -1);

        Mat mat_kernel2(img.size(), CV_32FC2, Scalar(1));
        circle(mat_kernel2, { img.rows / 2, img.cols / 2 }, 60, Scalar(0, 0), -1);

        Mat mul_spec;
        shift_rect(dft_img);//shift is needed because we need to cut high freqs
        mulSpectrums(dft_img, mat_kernel, mul_spec, 0);
        shift_rect(mul_spec);
        show_fourier(mul_spec, "mulSpectrums");

        Mat mul_spec2;
        //shift_rect(dft_img);//shift is needed because we need to cut high freqs
        mulSpectrums(dft_img, mat_kernel2, mul_spec2, 0);
        shift_rect(mul_spec2);
        show_fourier(mul_spec2, "mulSpectrums2");

        Mat idft_img(img.size(), CV_32FC1);
        dft(mul_spec, idft_img, DFT_INVERSE | DFT_REAL_OUTPUT);
        normalize(idft_img, idft_img, 0.0, 255, CV_MINMAX);
        idft_img.convertTo(idft_img, CV_8UC1);
        imshow("result", idft_img);

        Mat idft_img2(img.size(), CV_32FC1);
        dft(mul_spec2, idft_img2, DFT_INVERSE | DFT_REAL_OUTPUT);
        normalize(idft_img2, idft_img2, 0.0, 255, CV_MINMAX);
        idft_img2.convertTo(idft_img2, CV_8UC1);
        imshow("result2", idft_img2);
        break;
    }
    case '4':
    {
        Mat img = imread("table.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        imshow("original", img);

        Mat A = imread("symbol_A.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        //imshow("A", A);

        Mat seven = imread("symbol_7.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        //imshow("seven", seven);

        Mat zero = imread("symbol_0.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        //imshow("zero", zero);

        correlation(img.clone(), A.clone(), "A");
        correlation(img.clone(), seven.clone(), "7");
        correlation(img.clone(), zero.clone(), "0");

        break;
    }
    default:
        break;
    }
    waitKey();
    return 0;
}

void show_fourier(Mat &complexI, String name)
{
    //dft output is a dual channel image so we need to split it
    Mat planes[2];
    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    //after splitting it on real and imagine parts we can calcilate magnitude
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    Mat magI = planes[0];
    Mat fourier_img = magI.clone();

    //add 1 to every pixel to avoid	infitinies in lob
    fourier_img += Scalar::all(1);
    log(fourier_img, fourier_img);
    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    shift_rect(fourier_img);

    normalize(fourier_img, fourier_img, 0.0, 1.0, CV_MINMAX);
    imshow("dft " + name, fourier_img);
}

//This is krasivSpektr function with nice name
void shift_rect(Mat& fourier_img)
{
    int cx = fourier_img.cols / 2;
    int cy = fourier_img.rows / 2;

    Mat q0(fourier_img, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(fourier_img, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(fourier_img, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(fourier_img, Rect(cx, cy, cx, cy)); // Bottom-Right

    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

void correlation(Mat img, Mat ch, String str)
{

    //dual channel img for dft
    Mat dft_img(img.size(), CV_32FC2);
    img.convertTo(img, CV_32FC1);
    //handle normarization
    Mat mean_img, std_img;
    meanStdDev(img, mean_img, std_img);
    img -= mean_img;
    img /= std_img;

    dft(img, dft_img, DFT_COMPLEX_OUTPUT);
    show_fourier(dft_img, "img");

    //resizing original symbol to the size of target img
    ch.convertTo(ch, CV_32FC1);
    Mat mean_ch, std_ch;
    meanStdDev(ch, mean_ch, std_ch);
    ch -= mean_ch;
    ch /= std_ch;

    Mat sign(img.size(), CV_32FC1, Scalar(0));
    Mat roi(sign, Rect(0, 0, ch.cols, ch.rows));
    ch.copyTo(roi);

    //imshow("resized sign " + str, sign);

    Mat dft_sign(sign.size(), CV_32FC2);
    sign.convertTo(sign, CV_32FC1);
    dft(sign, dft_sign, DFT_COMPLEX_OUTPUT);
    show_fourier(dft_sign, str);

    //creating correlation of two images
    Mat img_sign;
    mulSpectrums(dft_img, dft_sign, img_sign, 0, true);
    //show_fourier(img_sign, "correlation with " + str);

    Mat idft_img(img.size(), CV_32FC1);
    dft(img_sign, idft_img, DFT_INVERSE | DFT_REAL_OUTPUT);
    normalize(idft_img, idft_img, 0.0, 255, CV_MINMAX);
    idft_img.convertTo(idft_img, CV_8UC1);
    imshow("correlation with " + str + " after idft", idft_img);

    //Finding the global minimum and maximum in an array
    double minVal;
    double maxVal;
    minMaxLoc(idft_img, &minVal, &maxVal);

    //"Cutting off" useless pixels and now we get left-top point of the symbols
    Mat idft_img_bin;
    threshold(idft_img, idft_img_bin, (maxVal-5), 255, THRESH_BINARY_INV);
    imshow("res_" + str, idft_img_bin);
}

complex<double>** Fourier(Mat &img, Mat &fourier_img)
{
    int rows = img.rows;
    int cols = img.cols;

    double phi1 = 2 * pi / rows;
    double phi2 = 2 * pi / cols;

    //Массив под прямое ДПФ
    complex<double>** dft = new complex<double>*[rows];
    complex<double>** dft_final = new complex<double>*[rows];
    for (int i = 0; i < rows; i++)
    {
        dft[i] = new complex<double>[cols];
        dft_final[i] = new complex<double>[cols];
        for (int j = 0; j < cols; j++)
        {
            dft[i][j] = 0;
            dft_final[i][j] = 0;
        }
    }

    //  Прямое ДПФ
    for (int i = 0; i < rows; i++)  //  Номер строки
    {
        for (int k2 = 0; k2 < cols; k2++)   //  Номер столбца пикселя, в который пишется прямое ДПФ (как в формуле)
        {
            double cp2 = cos(-phi2 * k2);
            double sp2 = sin(-phi2 * k2);
            complex<double> u(1, 0);
            complex<double> w(0, 0);
            for (int n2 = 0; n2 < cols; n2++)  //  Номер столбца пикселя - гармоники (т.е. номер гармоники) (как в формуле)
            {
                dft[i][k2] += complex<double>(img.at<uchar>(i, n2), 0) * u;
                w = u;
                u = complex<double>(cp2 * w.real() - sp2 * w.imag(), sp2 * w.real() + cp2 * w.imag());
            }
        }
    }

    for (int j = 0; j < cols; j++)  //  Номер столбца
    {
        for (int k1 = 0; k1 < rows; k1++)   //  Номер строки пикселя, в который пишется прямое ДПФ (как в формуле)
        {
            double cp1 = cos(-phi1 * k1);
            double sp1 = sin(-phi1 * k1);
            complex<double> u(1, 0);
            complex<double> w(0, 0);
            for (int n1 = 0; n1 < rows; n1++)  //  Номер строки пикселя - гармоники (т.е. номер гармоники) (как в формуле)
            {
                dft_final[k1][j] += dft[n1][j] * u;
                w = u;
                u = complex<double>(cp1 * w.real() - sp1 * w.imag(), sp1 * w.real() + cp1 * w.imag());
            }
            fourier_img.at<float>(k1, j) = abs(dft_final[k1][j]);
        }
    }

    for (int i = 0; i < rows; i++)
    {
        delete[] dft[i];
    }
    delete[] dft;

    return dft_final;
}

void idft_cust(complex<double>** fourier_img, Mat &img, int rows, int cols)
{

    double phi1 = 2 * pi / rows;
    double phi2 = 2 * pi / cols;

    //Массив под обратное ДПФ
    complex<double>** idft = new complex<double>*[rows];
    complex<double>** idft_final = new complex<double>*[rows];
    for (int i = 0; i < rows; i++)
    {
        idft[i] = new complex<double>[cols];
        idft_final[i] = new complex<double>[cols];
        for (int j = 0; j < cols; j++)
        {
            idft[i][j] = 0;
            idft_final[i][j] = 0;
        }
    }

    //Обратное ДПФ
    //complex<double> temp (0,0);
    for (int n1 = 0; n1 < rows; n1++)  //  Номер строки
    {
        for (int n2 = 0; n2 < cols; n2++)   //  Номер столбца пикселя, в который пишется прямое ДПФ (как в формуле)
        {
            for (int k2 = 0; k2 < cols; k2++)  //  Номер столбца пикселя - гармоники (т.е. номер гармоники) (как в формуле)
            {
                idft[n1][n2] += fourier_img[n1][k2] * complex<double>(cos(phi2 * k2*n2), sin(phi2 * k2*n2));
            }
        }
    }

    for (int n2 = 0; n2 < cols; n2++)  //  Номер столбца
    {
        for (int n1 = 0; n1 < rows; n1++)   //  Номер строки пикселя, в который пишется прямое ДПФ (как в формуле)
        {
            for (int k1 = 0; k1 < rows; k1++)  //  Номер строки пикселя - гармоники (т.е. номер гармоники) (как в формуле)
            {
                idft_final[n1][n2] += idft[k1][n2] * complex<double>(cos(phi1 * n1*k1), sin(phi1 * n1*k1));
            }
            img.at<uchar>(n1, n2) = abs(idft_final[n1][n2]) / (cols*rows);
        }
    }
    for (int i = 0; i < rows; i++)
    {
        delete[] idft[i];
        delete[] idft_final[i];
    }
    delete[] idft;
    delete[] idft_final;
}

void show_my_fourier(Mat &magI, String name)
{
    Mat fourier_img = magI.clone();
    fourier_img += Scalar::all(1);
    log(fourier_img, fourier_img);
    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = fourier_img.cols / 2;
    int cy = fourier_img.rows / 2;

    Mat q0(fourier_img, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(fourier_img, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(fourier_img, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(fourier_img, Rect(cx, cy, cx, cy)); // Bottom-Right

    Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);

    normalize(fourier_img, fourier_img, 0.0, 1.0, CV_MINMAX);
    imshow("dft" + name, fourier_img);
}