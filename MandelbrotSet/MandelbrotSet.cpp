// MandelbrotSet.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <iostream>
#include<cstdlib>
#include<ctime>
#include <opencv2\opencv.hpp>
//#include<opencv\cv.h>
//#include<opencv\highgui.h>
//#include<opencv\cxcore.h>

using namespace std;

#define MAX_COLOR 256

using namespace std;

IplImage* pImg;
IplImage* fcopy;
unsigned int IMAGE_WIDTH = 600;
unsigned int IMAGE_HEIGHT = 400;
double XMax = 2.0;
double XMin = -2.0;
double YMax = 2.0;
double YMin = -2.0;
double XStep, YStep;

CvPoint pt1 = { -1,-1 };
CvPoint pt2 = { -1,-1 };
CvPoint pt3 = { -1,-1 };
//颜色
CvScalar colortab[256];
int randomColor[3] = { 0,0,0 };

void initColor()
{
	for (int i = 0;i < MAX_COLOR;i++)
	{
		colortab[i].val[0] = (i * 3 + randomColor[0]) % 255;        //配色好看的要领是用小质数
		colortab[i].val[1] = (i * 5 + randomColor[1]) % 255;
		colortab[i].val[2] = (i * 11 + randomColor[2]) % 255;
	}
}
int mSetTest(double c_re, double c_im)
{
	int flag = 1;
	size_t i;
	double init_re = 0, init_im = 0;
	double tmp_re, tmp_im, module;
	for (i = 1;i <= 200;i++)
	{
		tmp_re = init_re*init_re - init_im*init_im;
		tmp_im = 2 * init_re*init_im;
		init_re = tmp_re + c_re;
		init_im = tmp_im + c_im;
		module = sqrt(init_re*init_re + init_im*init_im);
		if (module>2.0)
		{
			flag = 0;
			break;
		}
	}

	return flag == 0 ? i : 0;
}
void mapping(size_t x, size_t y, double &re, double &im)
{
	re = XStep*x + XMin;
	im = YStep*y + YMax;
}
void draw_img()
{
	double re = 0, im = 0;
	XStep = (XMax - XMin) / IMAGE_WIDTH;
	YStep = (YMin - YMax) / IMAGE_HEIGHT;
	for (size_t x = 0;x<IMAGE_WIDTH;x++)
	{
		for (size_t y = 0;y<IMAGE_HEIGHT;y++)
		{
			mapping(x, y, re, im);
			int colorIndex = (mSetTest(re, im)) % 255;
			cv::Vec3b color;
			color.val[0] = colortab[colorIndex].val[0];
			color.val[1] = colortab[colorIndex].val[1];
			color.val[2] = colortab[colorIndex].val[2];
			cvSet2D(pImg, y, x, color);
		}
	}
	cvShowImage("mandbrot", pImg);
	cvWaitKey(0);
}

void on_mouse(int event, int x, int y, int flags, void *param = NULL)
{
	if (!pImg)
		return;

	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
	{
		pt1 = cvPoint(x, y);
		cout << "key down." << endl;
		break;
	}
	case CV_EVENT_LBUTTONDBLCLK:
	{
		pt3 = cvPoint(x, y);
		double re, im;
		mapping(pt3.x, pt3.y, re, im);
		XMax = XMax - re;
		XMin = XMin - re;
		//XMax > 2.0 ? 2.0:XMax;
		//XMin < -2.0 ? -2.0:XMin;
		YMax = YMax - im;
		YMin = YMin - im;
		//YMax > 2.0 ? 2.0:YMax;
		//YMin < -2.0 ? -2.0:YMin;
		draw_img();
		cout << "key click." << endl;
		break;
	}
	case CV_EVENT_LBUTTONUP:
	{
		pt2 = cvPoint(x, y);
		int dx = abs(pt2.x - pt1.x);
		int dy = abs(pt2.y - pt1.y);

		if (pt1.x > 0 && pt1.y > 0 && pt2.x > 0 && pt2.y > 0 && dx > 4 && dy > 4)
		{
			double Lre, Lim, Rre, Rim;
			mapping(pt1.x, pt1.y, Lre, Lim);
			mapping(pt2.x, pt2.y, Rre, Rim);
			XMin = Lre<Rre ? Lre : Rre;
			XMax = Lre>Rre ? Lre : Rre;
			YMin = Lim<Rim ? Lim : Rim;
			YMax = Lim>Rim ? Lim : Rim;
			cvCopy(pImg, fcopy);
			cvRectangle(fcopy, pt1, pt2, cvScalar(255, 255, 255));
			draw_img();
			pt1 = cvPoint(-1, -1);
			pt2 = cvPoint(-1, -1);
			cout << "key move" << endl;
		}
		cout << "key up" << endl;
		break;
	}
	case CV_EVENT_MOUSEMOVE:
	{
		if ((flags&CV_EVENT_FLAG_LBUTTON))
		{
			pt2 = cvPoint(x, y);
			if (pt1.x > 0 && pt1.y > 0
				&& pt2.x > 0 && pt2.y > 0
				&& abs(pt2.x - pt1.x) > 5
				&& abs(pt2.y - pt1.y) > 5)
			{
				//cvCopy(pImg,fcopy);
				//cvRectangle(fcopy,pt1,pt2,cvScalar(255,255,255));
				//cout << "key move" <<endl;
			}
		}

		break;
	}

	default:
		break;
	}
}
int main()
{
	srand(time(nullptr));
	cvNamedWindow("mandbrot", 1);
	pImg = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 3);
	fcopy = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), 8, 3);
	cvSetMouseCallback("mandbrot", on_mouse, 0);
	//cvShowImage ("mandbrot", pImg);
	initColor();
	draw_img();
	cvWaitKey(0);
	cvReleaseImage(&pImg);
	cvReleaseImage(&fcopy);
	cvDestroyWindow("mandlbrot");
	return 0;
}

