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

string field_choosing();
string tank_choosing();
tuple<int, int, Mat> Keyboard_move(char c, Mat TankImg, Mat TankOriented, int Tank_x, int Tank_y, int field_width, int field_height);
void fire(Mat TankOriented, int Tank_x, int Tank_y, Mat Production, int field_width, int field_height);

char orient = 'l'; //fire orientation : l-left, r-right and so on

int main(int argc, char* argv[])
{
    //loading field image
    string field_name = field_choosing();
    Mat FieldImg = imread(field_name, IMREAD_COLOR);
    int field_width = FieldImg.cols;
    int field_height = FieldImg.rows;

    //loading agent image and configuring its sizes
    string tank_name = tank_choosing();
    Mat TankImg = imread(tank_name, IMREAD_COLOR);
    resize(TankImg, TankImg, Size(TankImg.cols / 2, TankImg.rows / 2));
    int Tank_x = 0;
    int Tank_y = 0;
    //TankImg contains native orient of the image (left turn)
    //TankOriented will be changed every time we will press the control button
    Mat TankOriented = TankImg.clone();

    //This matrix will contain field and one tank on it
    Mat Production;
    namedWindow("Production", WINDOW_AUTOSIZE);
    moveWindow("Production", 500, 150);

    while (1)
    {
        //clearing field
        Production = FieldImg.clone();

        //configuring ROI parameters and selecting ROI on the field
        Rect RoiTank(Tank_x, Tank_y, TankOriented.cols, TankOriented.rows);
        Mat RoiPepe = Production(RoiTank);

        //insert agent into field ROI
        TankOriented.copyTo(RoiPepe);

        imshow("Production", Production);

        //make a move, a shoot or quit the program
        char c = waitKey(3);
        if (c == 'c')
        {
            break;
        }
        else if(c == 32)
        {
            fire(TankOriented, Tank_x, Tank_y, Production, field_width, field_height);
        }
        else
        {
            tie(Tank_x, Tank_y, TankOriented) = Keyboard_move(c, TankImg, TankOriented, Tank_x, Tank_y, field_width, field_height);
        }
    }

    return 0;
}

string field_choosing()
{
    namedWindow("Field choosing", WINDOW_AUTOSIZE);
    moveWindow("Field choosing", 500, 200);
    int x = 500;
    int y = 500;
    Mat field(x, y, CV_8UC3, Scalar(120, 120, 120));
    putText(field, "Choose the field!", Point(40, 50), FONT_ITALIC, 1.5, Scalar(0), 4);
    putText(field, "Lenna(18+) - press q", Point(5, 120), FONT_ITALIC, 0.9, Scalar(0), 4);
    putText(field, "Pepe - press w", Point(5, 160), FONT_ITALIC, 0.9, Scalar(0), 4);
    putText(field, "Arnold - press e", Point(5, 200), FONT_ITALIC, 0.9, Scalar(0), 4);
    imshow("Field choosing", field);
    char c = waitKey(0);
    switch (c)
    {
    case 'q':
        {
            destroyAllWindows();
            return "Lena.jpg";
        }
    case 'w':
        {
            destroyAllWindows();
            return "Pepe.jpg";
        }
    case 'e':
        {
            destroyAllWindows();
            return "arnold.jpg";
        }
    }
}
string tank_choosing()
{
    namedWindow("Tank choosing", WINDOW_AUTOSIZE);
    moveWindow("Tank choosing", 500, 200);
    int x = 500;
    int y = 500;
    Mat field(x, y, CV_8UC3, Scalar(120, 120, 120));
    putText(field, "Choose the tank!", Point(40, 50), FONT_ITALIC, 1.5, Scalar(0), 4);
    putText(field, "T-34 - press q", Point(5, 120), FONT_ITALIC, 0.9, Scalar(0), 4);
    putText(field, "T-44 - press w", Point(5, 160), FONT_ITALIC, 0.9, Scalar(0), 4);
    putText(field, "Luchs - press e", Point(5, 200), FONT_ITALIC, 0.9, Scalar(0), 4);
    imshow("Tank choosing", field);
    char c = waitKey(0);
    switch (c)
    {
    case 'q':
        {
            destroyAllWindows();
            return "T_34.png";
        }
    case 'w':
        {
            destroyAllWindows();
            return "T_44.jpg";
        }
    case 'e':
        {
            destroyAllWindows();
            return "Luchs.jpg";
        }
    }
}
tuple<int, int, Mat> Keyboard_move(char c, Mat TankImg, Mat TankOriented, int Tank_x, int Tank_y, int field_width, int field_height)
{
    static int min_len = min(TankImg.cols, TankImg.rows);
    static int max_len = max(TankImg.cols, TankImg.rows);
    static Point2f min_len_center(min_len / 2., min_len / 2.);
    static Point2f max_len_center(max_len / 2., max_len / 2.);

    static Mat M_up = getRotationMatrix2D(min_len_center, -90, 1);
    static Mat M_down = getRotationMatrix2D(max_len_center, 90, 1);
    static Mat M_left = getRotationMatrix2D(Point(TankImg.cols / 2, TankImg.rows / 2), 0, 1);
    static Mat M_right = getRotationMatrix2D(Point(TankImg.cols / 2, TankImg.rows / 2), 180, 1);

    switch (c)
    {
    case 'w':
        {
            warpAffine(TankImg, TankOriented, M_up, Size(TankImg.rows, TankImg.cols));

            Tank_y -= 10;
            if (Tank_y < 0)
            {
                Tank_y = 0;
            }
            orient = 'u';
            break;
        }
    case 's':
        {
            warpAffine(TankImg, TankOriented, M_down, Size(TankImg.rows, TankImg.cols));

            Tank_y += 10;
            if (Tank_y > (field_height - TankOriented.rows))
            {
                Tank_y = field_height - TankOriented.rows;
            }
            orient = 'd';
            break;
        }
    case 'a':
        {
            warpAffine(TankImg, TankOriented, M_left, Size(TankImg.cols, TankImg.rows));

            Tank_x -= 10;
            if (Tank_x<0)
            {
                Tank_x = 0;
            }
            orient = 'l';
            break;
        }
    case 'd':
        {
            warpAffine(TankImg, TankOriented, M_right, Size(TankImg.cols, TankImg.rows));

            Tank_x += 10;
            if (Tank_x>(field_width - TankImg.cols))
            {
                Tank_x = field_width - TankImg.cols;
            }
            orient = 'r';
            break;
        }
    }

    return make_tuple(Tank_x, Tank_y, TankOriented);
}
void fire(Mat TankOriented, int Tank_x, int Tank_y, Mat Production, int field_width, int field_height)
{
    // special image for firing
    Mat fire_production;

    //bullet's params and initial coordinates
    int bullet_speed = 10;
    int bullet_length = 20;
    int x, y;
    switch (orient)
    {
    case 'l':
        {
            x = Tank_x;
            y = Tank_y + TankOriented.rows / 2;
            break;
        }
    case 'r':
        {
            x = Tank_x + TankOriented.cols;
            y = Tank_y + TankOriented.rows / 2;
            break;
        }
    case 'u':
        {
            x = Tank_x + TankOriented.cols / 2;
            y = Tank_y;
            break;
        }
    case 'd':
        {
            x = Tank_x + TankOriented.cols / 2;
            y = Tank_y + TankOriented.rows;
            break;
        }
    }

    //while bullet is in the window
    while ((x > 0 && x < field_width) && (y > 0 && y < field_height))
    {
        //clearing field
        fire_production = Production.clone();

        //drawing line in a needed direction
        switch (orient)
        {
        case 'l':
            x -= bullet_speed;
            line(fire_production, Point(x, y), Point(x - bullet_length, y), Scalar(0, 0, 255), 2);
            break;
        case 'r':
            x += bullet_speed;
            line(fire_production, Point(x, y), Point(x + bullet_length, y), Scalar(0, 0, 255), 2);
            break;
        case 'u':
            y -= bullet_speed;
            line(fire_production, Point(x, y), Point(x, y + bullet_length), Scalar(0, 0, 255), 2);
            break;
        case 'd':
            y += bullet_speed;
            line(fire_production, Point(x, y), Point(x, y - bullet_length), Scalar(0, 0, 255), 2);
            break;
        }

        imshow("Production", fire_production);
        waitKey(30);
    }
}