// DCT变换.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <ctime>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <limits>
#include <complex>
using namespace cv;
using namespace std;

#define MAX_NUM 2147483647

#define N 8
#define PI 3.14159

typedef struct zigzag {
	int row;
	int col;
}zigzag;
zigzag Zigzag[64] = {
	{ 0,0 },
	{ 0,1 },{ 1,0 },
	{ 2,0 },{ 1,1 },{ 0,2 },
	{ 0,3 },{ 1,2 },{ 2,1 },{ 3,0 },
	{ 4,0 },{ 3,1 },{ 2,2 },{ 1,3 },{ 0,4 },
	{ 0,5 },{ 1,4 },{ 2,3 },{ 3,2, },{ 4,1 },{ 5,0 },
	{ 6,0 },{ 5,1 },{ 4,2 },{ 3,3 },{ 2,4 },{ 1,5 },{ 0,6 },
	{ 0,7 },{ 1,6 },{ 2,5 },{ 3,4 },{ 4,3 },{ 5,2 },{ 6,1 },{ 7,0 },
	{ 7,1 },{ 6,2 },{ 5,3 },{ 4,4 },{ 3,5 },{ 2,6 },{ 1,7 },
	{ 2,7 },{ 3,6 },{ 4,5 },{ 5,4 },{ 4,6 },{ 3,7 },
	{ 7,3 },{ 6,4 },{ 5,5 },{ 4,6 },{ 3,7 },
	{ 4,7 },{ 5,6 },{ 6,5 },{ 7,4 },
	{ 7,5 },{ 6,6 },{ 5,7 },
	{ 6,7 },{ 7,6 },
	{ 7,7 }
};
IplImage * BGR2YCbCr(IplImage * img_bgr)
{	
	IplImage *img_YCbCr = cvCreateImage(cvGetSize(img_bgr), img_bgr->depth, 3);
	IplImage *img_Y = cvCreateImage(cvGetSize(img_bgr), img_bgr->depth, 1);
	CvScalar bgr, YCC, Y;
	for (int rows = 0; rows < img_bgr->height; ++rows) {
		for (int cols = 0; cols < img_bgr->width; ++cols){
			bgr = cvGet2D(img_bgr, rows, cols);
			double B = bgr.val[0];
			double G = bgr.val[1];
			double R = bgr.val[2];
			YCC.val[0] = 0.257*R + 0.564*G + 0.098*B + 16;
			YCC.val[1] = -0.148*R - 0.291*G + 0.439*B + 128;
			YCC.val[2] = 0.439*R - 0.368*G - 0.071*B + 128;
			Y.val[0] = YCC.val[0];
			cvSet2D(img_YCbCr, rows, cols, YCC);
			cvSet2D(img_Y, rows, cols, Y);
		}
	}
	imshow("YCbCr空间图像",Mat(img_YCbCr));
	return img_Y;
}
//计算DCT系数
void DCT(double f[N][N], double F[N][N]) {
	//每个像素值减去128
	for (int i = 0;i < N;i++) {
		for (int j = 0;j < N;j++) {
			f[i][j] = f[i][j] - 128;
		}
	}
	//计算E（u）E（V）
	double c[N][N];
	for (int i = 1; i < N; ++i)
		for (int j = 1; j < N; ++j)
			c[i][j] = 1.0 / 4;

	for (int i = 0; i < N; ++i)
	{
		c[i][0] = c[0][i] = sqrt(2.0) / 8;
	}
	c[0][0] = 1.0 / 8;
	//根据公式进行DCT变换
	for (int u = 0;u < N;u++) {
		for (int v = 0;v < N;v++) {
			double temp = 0;
			for (int x = 0;x < N;x++) {
				for (int y = 0;y < N;y++) {
					temp += f[x][y] * cos(((2 * x + 1)*u*PI) / 16.0)*cos(((2 * y + 1)*v*PI) / 16.0);
				}
			}
			F[u][v] = temp*c[u][v];
		}		
	}
	//cout << F << endl;
}

//量化DCT系数
void QuanDCT(double F[][N], int QuanTable[][N], int QF[N*N], zigzag Zigzag[N*N])
{
	for (int i = 0; i < N*N; ++i)
	{
		int rows = Zigzag[i].row;
		int cols = Zigzag[i].col;
		QF[i] = (int)round(F[rows][cols] / QuanTable[rows][cols]);
	}
	return;
}
//反量化系数
int Rebuild(int QF[N], double F[N][N], zigzag Zigzag[N*N], int QuanTable[][N], int number)
{
	//int rows,cols;
	for (int rows = 0; rows < N; ++rows)
		for (int cols = 0; cols < N; ++cols)
			F[rows][cols] = 0;
	int i = 0;
	while (QF[i] != MAX_NUM && i <number)
	{
		int rows = Zigzag[i].row;
		int cols = Zigzag[i].col;
		F[rows][cols] = QF[i] * QuanTable[rows][cols];
		++i;
	}
	return 0;
}
//重构DCT系数
IplImage *RE_DCT(double f[N][N], double F[N][N]) {
	IplImage *retImg = cvCreateImage(cvSize(64,64), IPL_DEPTH_8U,1);
	CvScalar basicScalar;
	//计算E（u）E（V）
	double c[N][N];
	for (int i = 1; i < N; ++i)
		for (int j = 1; j < N; ++j)
			c[i][j] = 1.0 / 4;

	for (int i = 0; i < N; ++i)
	{
		c[i][0] = c[0][i] = sqrt(2.0) / 8;
	}
	c[0][0] = 1.0 / 8;
	//根据公式进行反DCT变换
	for (int x = 0,x1=0;x < N;x++,x1+=8) {
		for (int y = 0,y1=0;y < N;y++,y1+=8) {
			f[x][y] = 0;
			IplImage *basicImg = cvCreateImage(cvSize(8, 8), IPL_DEPTH_8U, 1);
			for (int u = 0;u < N;u++) {
				for (int v = 0;v < N;v++) {
					double BasicImage= c[u][v] * cos(((2 * x + 1)*u*PI) / 16.0)*cos(((2 * y + 1)*v*PI) / 16.0);
					f[x][y] += F[u][v] * BasicImage;
					basicScalar.val[0] = BasicImage*100;//扩大100倍有利于显示
					cvSet2D(basicImg, u, v, basicScalar);
				}
			}
			CvRect rect = cvRect(x1, y1, basicImg->width, basicImg->height);
			cvSetImageROI(retImg, rect);
			cvCopy(basicImg, retImg);
			cvResetImageROI(retImg);
		}
	}
	for (int i = 0;i < N;i++) {
		for (int j = 0;j < N;j++) {
			f[i][j] = f[i][j] + 128;
		}
	}
	//Mat t = Mat(retImg);
	return retImg;
}

//DFT变换
void DFT(Complex<double> f[N][N],Complex<double> F[N][N]) {
	
	Complex<double> temp;
	Complex<double> wn;
	//根据公式进行DFT变换
	for (size_t u = 0;u < N;u++) {
		for (size_t v = 0;v < N;v++) {
			F[u][v] = { 0,0 };
			for (size_t x = 0;x < N;x++) {
				for (size_t y = 0;y < N;y++) {
					wn= {cos(-2.0*PI*(u*x/(double)N+v*y/(double)N)),sin(-2.0 * PI*(u*x /(double) N + v*y /(double) N))};
					F[u][v] += f[x][y] * wn;
				}
			}
		}
	}
}
int main()
{
	Mat lena = imread("lena.bmp");
	IplImage lena_bgr = lena;
	IplImage *img_y = BGR2YCbCr(&lena_bgr);
	IplImage *dtc_img = cvCreateImage(cvGetSize(img_y), img_y->depth,1);
	IplImage *basic_img= cvCreateImage(cvSize(N*N, N*N), img_y->depth, 1);
	imshow("YCbCr--Y分量图像", Mat(img_y));
	double f[N][N];
	double F[N][N];

	Complex<double> fCom[N][N];
	Complex<double> FCom[N][N];

	int QF[N*N];
	//量化表
	int QuanTable[8][8] = 
	  { 16,11,10,16,24,40,51,61,
		12,12,14,19,26,58,60,55,
		14,13,16,24,40,57,69,56,
		14,17,22,29,51,87,80,62,
		18,22,37,56,68,109,103,77,
		24,35,55,64,81,104,113,92,
		49,64,78,87,103,121,120,101,
		72,92,95,98,112,100,103,99
	};

	Mat dft_img(lena.rows,lena.cols,CV_64FC1);
	time_t t_start,t_end;
	t_start = time(NULL);
	for (size_t i = 0; i < img_y->width /N; i++)
	{
		for (size_t j = 0; j < img_y->height/N; j++)
		{
			CvScalar yScalar,dctScalar,dftScalar;
			for (size_t k = i*N; k < (i+1)*N; k++)
			{
				for (size_t l = j*N; l < (j + 1)*N; l++)
				{
					yScalar = cvGet2D(img_y, k, l);
					f[k-i*N][l-j*N] = yScalar.val[0];

					fCom[k - i*N][l - j*N].re= yScalar.val[0];
					fCom[k - i*N][l - j*N].im = 0;
				}
			}
			
			DFT(fCom, FCom);
			
			for (size_t k = i*N; k < (i + 1)*N; k++)
			{
				for (size_t l = j*N; l < (j + 1)*N; l++)
				{
					double re = FCom[k - i*N][l - j*N].re;
					double im = FCom[k - i*N][l - j*N].im;
					double temp = sqrt(re*re+im*im);
					dft_img.at<double>(k,l)= temp;
				}
			}
			
			DCT(f,F);//计算DCT系数			
			QuanDCT(F, QuanTable, QF, Zigzag);//量化			
			Rebuild(QF, F, Zigzag, QuanTable, 5) ;//反量化	

			basic_img = RE_DCT(f,F);//重构出DCT系数，并画出基图像
			
			for (size_t k = i*N; k < (i + 1)*N; k++)
			{
				for (size_t l = j*N; l < (j + 1)*N; l++)
				{
					dctScalar.val[0]=f[k - i*N][l - j*N];
					cvSet2D(dtc_img, k, l, dctScalar);
				}
			}
		}
	}
	t_end = time(NULL);
	double diff=difftime(t_end, t_start);
	cout << "用时：" << diff << "秒" << endl;
	Mat basic_mat_img = Mat(basic_img);
	Mat basic_mat_large(basic_mat_img.rows*3, basic_mat_img.cols*3, basic_mat_img.type());//放大5倍
	resize(basic_mat_img, basic_mat_large, basic_mat_large.size(), CV_INTER_LINEAR);
	normalize(dft_img, dft_img, 0, 1, CV_MINMAX);
	normalize(basic_mat_large, basic_mat_large, 0, 255, CV_MINMAX);
	imshow("DFT图像", dft_img);
	imshow("重构后DCT图像",Mat(dtc_img));
	imshow("DCT基图像", basic_mat_large);
	waitKey(0);
    return 0;
}

