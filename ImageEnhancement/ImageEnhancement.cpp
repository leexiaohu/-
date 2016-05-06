// ImageEnhancement.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include<iostream>
#include<opencv2\opencv.hpp>
#include<time.h>
using namespace std;
using namespace cv;
#define PI 3.14159
#define TWO_PI 6.283185307
void addSaltAndPepperNoise(IplImage *src, IplImage *dst, double SNR) {
	int NoiseNum = src->imageSize*(1- SNR);
	for (size_t i = 0; i < NoiseNum; i++)
	{
		int row = rand() % src->width;
		int col = rand() % src->height;
		int blackOrwhite = rand() % 2;
		CvScalar pixl;
		if (blackOrwhite) {
			pixl = { 0,0,0,0 };
			cvSet2D(dst, row, col, pixl);
		}
		else {
			pixl = { 255,255,255,255 };
			cvSet2D(dst, row, col, pixl);
		}
	}

}
double generateGaussianNoise()
{
	static bool hasSpare = false;
	static double rand1, rand2;
	if (hasSpare)
	{
		hasSpare = false;
		return sqrt(rand1) * sin(rand2);
	}
	hasSpare = true;
	rand1 = rand() / ((double)RAND_MAX);
	if (rand1 < 1e-100) rand1 = 1e-100;
	rand1 = -2 * log(rand1);
	rand2 = (rand() / ((double)RAND_MAX)) * TWO_PI;
	return sqrt(rand1) * cos(rand2);
}

void addGaussianNoise(IplImage *src, IplImage *dst) {

	srand((unsigned)time(NULL));	
	CvScalar pixl;
	for (size_t i = 0; i < src->height; i++)
	{
		for (size_t j = 0; j < src->width; j++)
		{
			int x = rand() % 256 - 128;
			pixl=cvGet2D(src, i, j);
			pixl.val[0] += generateGaussianNoise() * 128;			
			pixl.val[1] += generateGaussianNoise() * 128;			
			pixl.val[2] += generateGaussianNoise() * 128;			
			cvSet2D(dst, i, j, pixl);
		}
	}
	
}
void imageBlurring(IplImage *src, IplImage *dst) {
	CvScalar pixl;
	for (size_t i = 0; i < src->height; i++)
	{
		for (size_t j = 0; j < src->width; j++)
		{		
			CvScalar temp = {0,0,0};
			int num = 0;
			for (size_t k = i-5; k <= i+5; k++)
			{
				for (size_t l = j-5; l <= j+5; l++)
				{
					if ((k >= 0 && k < src->height) && (l >= 0 && l < src->width)) {
						pixl = cvGet2D(src, k, l);
						temp.val[0] += pixl.val[0];
						temp.val[1] += pixl.val[1];
						temp.val[2] += pixl.val[2];
						num++;
					}
				}
			}
			if (num != 0) {
				pixl.val[0] = temp.val[0] / num;
				pixl.val[1] = temp.val[1] / num;
				pixl.val[2] = temp.val[2] / num;
			}
			else {
				pixl = temp;
			}
			cvSet2D(dst, i, j, pixl);
		}
	}
}
int cmp(const void *a, const void *b)
{
	return *((double *)b) - *((double*)a);
}
double Median(double *A,size_t size) {
	int t = (size - 1) / 2;
	qsort(A, size*size, sizeof(double), cmp);	
	return *(A + size-1);
}
void MedianFiltering(IplImage *src, IplImage *dst,int size) {
	int t = (size - 1) / 2;
	double *Median_B = new double[size * size];
	double *Median_G = new double[size * size];
	double *Median_R = new double[size * size];
	CvScalar pixl;
	for (size_t i = t; i < src->height - t; i++)
	{
		for (size_t j = t; j < src->width - t; j++)
		{
			for (size_t k = i - t,m = 0; k <= i + t; k++,m++)
			{
				for (size_t l = j - t,n = 0; l <= j + t; l++,n++)
				{
					if (m==t || n==t) {
						pixl = cvGet2D(src, k, l);
						*(Median_B + m * size + n) = pixl.val[0];
						*(Median_G + m * size + n) = pixl.val[1];
						*(Median_R + m * size + n) = pixl.val[2];
					}
					else {
						*(Median_B + m * size + n) = 0;
						*(Median_G + m * size + n) = 0;
						*(Median_R + m * size + n) = 0;
					}
				}
			}
			pixl.val[0] = Median(Median_B,size);
			pixl.val[1] = Median(Median_G,size);
			pixl.val[2] = Median(Median_R,size);
			cvSet2D(dst, i, j, pixl);
		}
	}
}
int main()
{
	Mat img = imread("lena.bmp");
	IplImage ipl_img;
	ipl_img= img;
	IplImage *ipl_dst1 = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 3);
	IplImage *ipl_dst2 = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 3);
	IplImage *ipl_dst3 = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 3);
	IplImage *ipl_dst4 = cvCreateImage(cvGetSize(&ipl_img), (&ipl_img)->depth, 3);
	cvCopy(&ipl_img, ipl_dst1);
	addSaltAndPepperNoise(&ipl_img, ipl_dst1,0.99);
	addGaussianNoise(&ipl_img, ipl_dst2);
	imageBlurring(&ipl_img, ipl_dst3);
	MedianFiltering(ipl_dst1, ipl_dst4,5);
	imshow("原图像", Mat(&ipl_img));
	imshow("椒盐噪声", Mat(ipl_dst1));
	imshow("高斯噪声", Mat(ipl_dst2));
	imshow("模糊图像", Mat(ipl_dst3));
	imshow("使用中值滤波处理椒盐噪声图像", Mat(ipl_dst4));
	waitKey();
    return 0;
}

