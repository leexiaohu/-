// ImageFiltering.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

//Sobel模板算子
void m_Solbel(IplImage *src, IplImage *dst) {
	int sobel[3][3] = {
		{ 1, 2, 1 },
		{ 0, 0, 0 },
		{ -1,-2,-1 }
	};
	CvScalar pixl,pixl2;
	for (size_t i = 0; i < src->height; i++)
	{
		for (size_t j = 0; j < src->width; j++)
		{			
			double temp= 0;
			for (size_t k = i-1; k <= i+1; k++)
			{
				for (size_t l = j-1; l <= j+1; l++)
				{
					if ((k >= 0 && k < src->width) && (l >= 0 && l < src->height)) {
						pixl = cvGet2D(src, k, l);
						temp += pixl.val[0] * sobel[k-i+1][l-j+1];
					}
					
				}
			}
			pixl2.val[0] = temp;
			cvSet2D(dst,i,j, pixl2);
		}
	}

}
//直方图显示
void m_Hist(IplImage * img,size_t startX, size_t startY, size_t endX, size_t endY) {
	if (img->nChannels == 1){
		IplImage* r_dst = cvCreateImage(cvSize(400, 300), 8, 3);
		cvSet(r_dst, cvScalarAll(255), 0);
		int size = 256;
		float mHistogram[256] = {0};
		CvScalar pixl;
		for (size_t i = startY; i < endY; i++)
		{
			for (size_t j = startX; j < endX; j++)
			{
				pixl = cvGet2D(img, i, j);
				int idx = (int)pixl.val[0];
				mHistogram[idx]++;
			}
		}
		int maxIdx = 0;
		for (size_t i = 1; i < size; i++)
		{
			if (mHistogram[i] > mHistogram[maxIdx])
				maxIdx = i;
		}
		double r_bin_width = (double)r_dst->width / size;
		double r_bin_unith = (double)r_dst->height / mHistogram[maxIdx];
		for (int i = 0;i<size;i++)
		{
			CvPoint p0 = cvPoint(i*r_bin_width, r_dst->width);
			CvPoint p1 = cvPoint((i + 1)*r_bin_width, r_dst->height - mHistogram[i] *r_bin_unith);
			cvRectangle(r_dst, p0, p1, cvScalar(255, 0, 0), -1, 8, 0);
		}
		imshow("单通道图像直方图统计", Mat(r_dst));

	}
	else {
		int size = 256;
		float mHistogram[3][256] = {0};
		int maxIdx[3] = { 0 };
		double r_bin_width[3];
		double r_bin_unith[3];
		IplImage *dst[4];
		dst[0] = cvCreateImage(cvSize(400, 300), 8, 3);
		cvSet(dst[0], cvScalarAll(255), 0);
		dst[1] = cvCreateImage(cvSize(400, 300), 8, 3);
		cvSet(dst[1], cvScalarAll(255), 0);
		dst[2] = cvCreateImage(cvSize(400, 300), 8, 3);
		cvSet(dst[2], cvScalarAll(255), 0);
		dst[3] = cvCreateImage(cvSize(800, 600), 8, 3);
		cvSet(dst[3], cvScalarAll(255), 0);
		CvScalar pixl;
		for (size_t i = startY; i < endY; i++)
		{
			for (size_t j = startX; j < endX; j++)
			{
				pixl = cvGet2D(img, i, j);
				int idx_b = (int)pixl.val[0];
				int idx_g = (int)pixl.val[1];
				int idx_r = (int)pixl.val[2];
				mHistogram[0][idx_b]++;
				mHistogram[1][idx_g]++;
				mHistogram[2][idx_r]++;
			}
		}
		for (size_t i = 0; i < 3; i++)
		{
			maxIdx[i] = 0;
			for (size_t j = 1; j < size; j++)
			{
				if (mHistogram[i][j]>mHistogram[i][maxIdx[i]])
					maxIdx[i] = j;
			}
		}
		for (size_t i = 0; i < 3; i++)
		{
			r_bin_width[i]= dst[i]->width/size;
			r_bin_unith[i]= dst[i]->height/ mHistogram[i][maxIdx[i]];
		}
		CvScalar color[3];
		color[0] = cvScalar(255, 0, 0);
		color[1] = cvScalar(0, 255, 0);
		color[2] = cvScalar(0, 0, 255);
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < size; j++)
			{
				CvPoint p0 = cvPoint(j*r_bin_width[i], dst[i]->height);
				CvPoint p1 = cvPoint((j + 1)*r_bin_width[i], dst[i]->height - mHistogram[i][j] * r_bin_unith[i]);
				cvRectangle(dst[i], p0, p1, color[i], -1, 8, 0);
			}
		}
		CvRect rect=cvRect(0, 0, 400, 300);
		cvSetImageROI(dst[3], rect);
		cvCopy(dst[0], dst[3]);
		rect = cvRect(400,0,400,300);
		cvSetImageROI(dst[3], rect);
		cvCopy(dst[1], dst[3]);
		rect = cvRect(0, 300, 400, 300);
		cvSetImageROI(dst[3], rect);
		cvCopy(dst[2], dst[3]);
		cvResetImageROI(dst[3]);
		imshow("彩色图像直方图统计",Mat(dst[3]));
	}
}

int main()
{
	Mat img = imread("lena.bmp");
	IplImage ipl_img = img;
	IplImage * sobel_b = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 1);
	IplImage * sobel_g = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 1);
	IplImage * sobel_r = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 1);

	IplImage * sobel_b1 = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 1);
	IplImage * sobel_g1 = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 1);
	IplImage * sobel_r1 = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 1);

	IplImage * sobel_dst = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 3);	 
	cvSplit(&ipl_img, sobel_b, sobel_g, sobel_r, NULL);
	m_Solbel(sobel_b, sobel_b1);
	m_Solbel(sobel_g, sobel_g1);
	m_Solbel(sobel_r, sobel_r1);
	cvMerge(sobel_b1, sobel_g1, sobel_r1, NULL, sobel_dst);
	int x, y,width,height;
	cout << "请输入图像左上角的坐标x，y:";
	cin >> x >> y;
	cout << "请输入图像大小width，height:";
	cin >> width >> height;
	m_Hist(&ipl_img, x, y, x + width, y + height);
	imshow("原图像", img);
	imshow("sobel处理后",Mat(sobel_dst));
	
	waitKey(0);
    return 0;
}

