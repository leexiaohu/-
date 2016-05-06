// ImageReconition.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;
//均值滤波
void imageBlurring(IplImage *src, IplImage *dst,int size) {
	CvScalar pixl;
	for (size_t i = 0; i < src->height; i++)
	{
		for (size_t j = 0; j < src->width; j++)
		{
			CvScalar temp = { 0,0,0 };
			int num = 0;
			for (size_t k = i - size; k <= i + size; k++)
			{
				for (size_t l = j - size; l <= j + size; l++)
				{
					if ((k >= 0 && k < src->height) && (l >= 0 && l < src->width)) {
						pixl = cvGet2D(src, k, l);
						temp.val[0] += pixl.val[0];
						num++;
					}
				}
			}
			if (num != 0) {
				pixl.val[0] = temp.val[0] / num;
			}
			else {
				pixl = temp;
			}
			cvSet2D(dst, i, j, pixl);
		}
	}
}

//二值化
void imageBinaryzation(IplImage *src, IplImage *dst, int threaHold) {
	CvScalar pixl;
	for (size_t i = 0; i < src->height; i++)
	{
		for (size_t j = 0; j < src->width; j++)
		{
			CvScalar temp;
			pixl = cvGet2D(src, i, j);
			if (pixl.val[0] > threaHold) {
				pixl.val[0] = 0;
			}
			else {
				pixl.val[0] = 255;
			}			
			cvSet2D(dst, i, j, pixl);
		}
	}
}
int main()
{
	Mat img = imread("000.jpg",0);
	IplImage ipl_img = img;
	IplImage *ipl_dst1 = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, (&ipl_img)->nChannels);
	IplImage *ipl_binary = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, (&ipl_img)->nChannels);
	IplImage *img_erode = cvCreateImage(cvGetSize(&ipl_img), 8, 1);
	IplImage *img_dilate = cvCreateImage(cvGetSize(&ipl_img), 8, 1);
	IplImage *makers = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, (&ipl_img)->nChannels);
	//imageBlurring(&ipl_img, ipl_dst1,1);
	imageBinaryzation(&ipl_img, ipl_binary,200);
	//cvErode(ipl_binary, img_erode,NULL,1);
	//cvDilate(img_erode, img_dilate,NULL,1);
	//cvWatershed(&ipl_img, makers);
	//imwrite();
	imshow("src",Mat(&ipl_img));
	//imshow("去噪", Mat(ipl_dst1));
	imshow("二值化", Mat(ipl_binary));
	//imshow("腐蚀后", Mat(img_erode));
	//imshow("膨胀后", Mat(img_dilate));
	//imshow("分割后", Mat(makers));
	waitKey();
    return 0;
}

