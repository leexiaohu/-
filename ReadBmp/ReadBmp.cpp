// ReadBmp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <opencv2\opencv.hpp>
#define PI 3.1415926
using namespace std;
using namespace cv;
Mat lena;
double Min(double x, double y, double z) {
	double minTmp = x;
	if (y<minTmp)
		minTmp = y;
	if (z<minTmp)
		minTmp = z;
	return minTmp;
}
void showBRG(int height, int width) {
	Mat RMat, GMat, BMat;
	RMat = Mat(width, height, CV_8UC3);
	GMat = Mat(width, height, CV_8UC3);
	BMat = Mat(width, height, CV_8UC3);
	Vec3b b, g, r, pixel;
	for (int i = 0;i < height;i++) {
		for (int j = 0;j < width;j++) {
			pixel = lena.at<Vec3b>(i, j);
			b[0] = pixel[0];
			g[1] = pixel[1];
			r[2] = pixel[2];
			RMat.at<Vec3b>(i, j) = r;
			GMat.at<Vec3b>(i, j) = g;
			BMat.at<Vec3b>(i, j) = b;
		}
	}
	imshow("R分量", RMat);
	imshow("G分量", GMat);
	imshow("B分量", BMat);
}
//BRG转换为HSI空间并显示
void BGR2HSI(IplImage *img) {
	int width, height;
	width = img->width;
	height = img->height;
	IplImage *HImage = cvCreateImage(cvSize(width, height), img->depth, 1);
	IplImage *SImage = cvCreateImage(cvSize(width, height), img->depth, 1);
	IplImage *IImage = cvCreateImage(cvSize(width, height), img->depth, 1);
	CvScalar pixel,HScalar, SScalar,IScalar;
	double H, S, I;
	for (int i = 0;i < height;i++) {
		for (int j = 0;j < width;j++) {
			//pixel = lena.at<Vec3b>(i, j);
			pixel = cvGet2D(img,i,j);
			double B = pixel.val[0];
			double G = pixel.val[1];
			double R = pixel.val[2];
			
			double sum = R + G + B;
			I = sum / 3.0f;
			if (sum != 0)
				S = 1 - 3 * Min(R, G, B) / (R + G + B);
			double numerator = 0.5*(2 * R - G - B);
			double denominator = (R - G)*(R - G) + (R - B)*sqrt(G - B);
			if (denominator == 0) 
				denominator = 0.01f;
			double angle = acos(numerator / denominator);
			if (B <= G)
				H = angle;
			else
				H = 2*PI - angle;
			HScalar.val[0] = H;
			SScalar.val[0] = S;
			IScalar.val[0] = I;
			cvSet2D(HImage, i, j, HScalar);
			cvSet2D(SImage, i, j, SScalar);
			cvSet2D(IImage, i, j, IScalar);
		}
	}
	imshow("H分量", Mat(HImage));
	imshow("S分量", Mat(SImage));
	imshow("I分量", Mat(IImage));
}
void BGR2YIQ(IplImage *img) {
	int width, height;
	width = img->width;
	height = img->height;
	IplImage *YImage = cvCreateImage(cvSize(width, height), img->depth, 1);
	IplImage *IImage = cvCreateImage(cvSize(width, height), img->depth, 1);
	IplImage *QImage = cvCreateImage(cvSize(width, height), img->depth, 1);
	CvScalar pixel, Y, I, Q;
	for (int i = 0;i < height;i++) {
		for (int j = 0;j < width;j++) {
			pixel = cvGet2D(img, i, j);
			int B = pixel.val[0];
			int G = pixel.val[1];
			int R = pixel.val[2];
			Y.val[0] = 0.299*R + 0.587*G + 0.114*B;
			I.val[0] = 0.596*R - 0.274*G - 0.322*B;
			Q.val[0] = 0.211*R - 0.523*G + 0.312*B;

			cvSet2D(YImage, i, j, Y);
			cvSet2D(IImage, i, j, I);
			cvSet2D(QImage, i, j, Q);
		}
	}
	imshow("Y分量", Mat(YImage));
	imshow("I分量", Mat(IImage));
	imshow("Q分量", Mat(QImage));
}
void BGR2XYZ(IplImage *img) {
	int width, height;
	width = img->width;
	height = img->height;
	IplImage *XImage = cvCreateImage(cvSize(width, height), img->depth, 1);
	IplImage *YImage = cvCreateImage(cvSize(width, height), img->depth, 1);
	IplImage *ZImage = cvCreateImage(cvSize(width, height), img->depth, 1);
	CvScalar pixel, X, Y, Z;
	for (int i = 0;i < height;i++) {
		for (int j = 0;j < width;j++) {
			pixel = cvGet2D(img,i,j);
			int B = pixel.val[0];
			int G = pixel.val[1];
			int R = pixel.val[2];
			X.val[0] = 0.490*R + 0.310*G + 0.200*B;
			Y.val[0] = 0.177*R + 0.813*G + 0.011*B;
			Z.val[0] = 0.000*R + 0.010*G + 0.990*B;
			
			cvSet2D(XImage, i, j, X);
			cvSet2D(YImage, i, j, Y);
			cvSet2D(ZImage, i, j, Z);
		}
	}
	imshow("X分量", Mat(XImage));
	imshow("Y分量", Mat(YImage));
	imshow("Z分量", Mat(ZImage));
}
int main()
{
	lena = imread("lena.bmp");
	IplImage iplimg = lena;
	imshow("lena_bmp显示",lena);
	int x, y,width,height;
	width = lena.cols;
	height = lena.rows;
	
	Vec3b pixel;
	cout << "Input the position x y:";
	cin >> x >> y;
	pixel = lena.at<Vec3b>(x, y);
	cout << "The value of pixel is:" << pixel << endl;
	
	showBRG(height, width);
	BGR2HSI(&iplimg);
	BGR2YIQ(&iplimg);
	BGR2XYZ(&iplimg);
	waitKey(0);
    return 0;
}

