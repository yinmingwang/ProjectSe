#include <iostream>  
#include <fstream>  
#include <cstring>
#include <algorithm> 
#include <sstream>
#include "opencv\cv.h"
#include "opencv2\core\core.hpp"  
#include "opencv2\highgui\highgui.hpp"  
#include "opencv2\imgproc\imgproc.hpp"  
#include "opencv2\contrib\contrib.hpp" 
using namespace std;
using namespace cv;
int r = 1;
Mat paint(Mat srcimg) {
	int width = srcimg.cols;
	int height = srcimg.rows;
	if (width > height) {
		Mat img(width, width, srcimg.type(), Scalar(0, 0, 0));
		int diff = width - height;
		int d = diff / 2;
		for (int i = d; i < width - d - (diff % 2); i++) {
			for (int j = 0; j < width; j++) {
				img.at<uchar>(i, j) = srcimg.at<uchar>(i - d, j);
			}
		}
		return img;
	}
	else if (width < height) {
		Mat img(height, height, srcimg.type(), Scalar(0, 0, 0));
		int diff = height - width;
		int d = diff / 2;
		for (int i = 0; i < height; i++) {
			for (int j = d; j < height - d - (diff % 2); j++) {
				img.at<uchar>(i, j) = srcimg.at<uchar>(i, j - d);
			}
		}
		return img;
	}
	return srcimg;

}
vector<Mat> horizontalProjectionMat(Mat srcImg)//ˮƽͶӰ  
{
	Mat binImg = srcImg;
	int perPixelValue = 0;//ÿ�����ص�ֵ  
	int width = srcImg.cols;
	int height = srcImg.rows;
	int* projectValArry = new int[height];//����һ������ÿ�а�ɫ���ظ���������  
	memset(projectValArry, 0, height * 4);//��ʼ������  
	for (int col = 0; col < height; col++)//����ÿ�����ص�  
	{
		for (int row = 0; row < width; row++)
		{
			perPixelValue = binImg.at<uchar>(col, row);
			if (perPixelValue == 255)//����ǰ׵׺���  
			{
				projectValArry[col]++;
			}
		}
	}
	/*
	Mat horizontalProjectionMat(height, width, CV_8UC1);//��������  
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			perPixelValue = 255;
			horizontalProjectionMat.at<uchar>(i, j) = perPixelValue;//���ñ���Ϊ��ɫ  
		}
	}
	for (int i = 0; i < height; i++)//ˮƽֱ��ͼ  
	{
		for (int j = 0; j < projectValArry[i]; j++)
		{
			perPixelValue = 0;
			horizontalProjectionMat.at<uchar>(i, width - 1 - j) = perPixelValue;//����ֱ��ͼΪ��ɫ  
		}
	}*/
	vector<Mat> roiList;//���ڴ���ָ������ÿ���ַ�  
	int startIndex = 0;//��¼�����ַ���������  
	int endIndex = 0;//��¼����հ����������  
	bool inBlock = false;//�Ƿ���������ַ�����  
	for (int i = 0; i <srcImg.rows; i++)
	{
		if (!inBlock && projectValArry[i] != 0)//�����ַ���  
		{
			inBlock = true;
			startIndex = i;
		}
		else if (inBlock && projectValArry[i] == 0)//����հ���  
		{
			endIndex = i;
			inBlock = false;
			Mat roiImg = srcImg(Range(startIndex, endIndex + 1), Range(0, srcImg.cols));//��ԭͼ�н�ȡ��ͼ�������
			//Mat timg = paint(roiImg);
			//Mat img;
			//resize(timg, img, Size(32, 32), 0, 0, CV_INTER_LINEAR);
			roiList.push_back(roiImg);
		}
	}
	delete[] projectValArry;
	return roiList;
}
vector<Mat> verticalProjectionMat(Mat srcImg)//��ֱͶӰ  
{
	Mat binImg = srcImg;
	imshow("bin", binImg);
	int perPixelValue;//ÿ�����ص�ֵ  
	int width = srcImg.cols;
	int height = srcImg.rows;
	int* projectValArry = new int[width];//�������ڴ���ÿ�а�ɫ���ظ���������  
	memset(projectValArry, 0, width * 4);//��ʼ������  
	for (int col = 0; col < width; col++)
	{
		for (int row = 0; row < height; row++)
		{
			perPixelValue = binImg.at<uchar>(row, col);
			if (perPixelValue == 255)//����ǰ׵׺���  
			{
				projectValArry[col]++;
			}
		}
	}
	/*
	Mat verticalProjectionMat(height, width, CV_8UC1);//��ֱͶӰ�Ļ���  
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			perPixelValue = 255;  //��������Ϊ��ɫ  
			verticalProjectionMat.at<uchar>(i, j) = perPixelValue;
		}
	}
	for (int i = 0; i < width; i++)//��ֱͶӰֱ��ͼ  
	{
		for (int j = 0; j < projectValArry[i]; j++)
		{
			perPixelValue = 0;  //ֱ��ͼ����Ϊ��ɫ    
			verticalProjectionMat.at<uchar>(height - 1 - j, i) = perPixelValue;
		}
	}
	imshow("��ֱͶӰ", verticalProjectionMat);
	cvWaitKey(0);
	*/
	vector<Mat> roiList;//���ڴ���ָ������ÿ���ַ�  
	int startIndex = 0;//��¼�����ַ���������  
	int endIndex = 0;//��¼����հ����������  
	bool inBlock = false;//�Ƿ���������ַ�����  
	for (int i = 0; i < srcImg.cols; i++)//cols=width  
	{
		if (!inBlock && projectValArry[i] != 0)//�����ַ���  
		{
			inBlock = true;
			startIndex = i;
		}
		else if (projectValArry[i] == 0 && inBlock)//����հ���  
		{
			endIndex = i;
			inBlock = false;
			Mat roiImg = srcImg(Range(0, srcImg.rows), Range(startIndex, endIndex + 1));
			roiList.push_back(roiImg);
		}
	}
	delete[] projectValArry;
	return roiList;
}
Mat Dilation(Mat srcimg, int size)
{
	Mat dilatimg;
	Mat element = getStructuringElement(MORPH_RECT, Size(size, size));
	dilate(srcimg, dilatimg, element);
	return dilatimg;
}
Mat Erosion(Mat srcimg, int size) {
	Mat erodeimg;
	Mat element = getStructuringElement(MORPH_RECT, Size(size, size));
	erode(srcimg, erodeimg, element);
	return erodeimg;
}
int main()
{
	char buffer[40];
	for (r = 1; r < 500; r++) {
		sprintf(buffer, "C:\\Users\\yinmw\\Desktop\\pic\\%d.jpg", r);
		Mat srcimage = imread(buffer);
		stringstream stream;
		string str;
		stream << r;
		stream >> str;
		string filename = str + ".jpg";
		cout << filename << endl;
		Mat grayimg;
		Mat binimage;
		cv::cvtColor(srcimage, grayimg, CV_BGR2GRAY);
		adaptiveThreshold(grayimg, binimage, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 17, 33);
		Mat Mmimage = Erosion(binimage, 2);
		//Mat Dimage = Dilation(Mmimage, 2);
		Mat Mimage = Dilation(Mmimage, 2);
		vector<Mat> b = verticalProjectionMat(Mimage);//�Ƚ��д�ֱͶӰ
		for (int i = 0; i < b.size(); i++)
		{
			stringstream streami;
			string  stri;
			streami << i;
			streami >> stri;
			vector<Mat> a = horizontalProjectionMat(b[i]);//ˮƽͶӰ  
			for (int j = 0; j < a.size(); j++)
			{
				stringstream streamj;
				string  strj;
				streamj << j;
				streamj >> strj;
				string name = str + "_"+ stri + "_" + strj + ".jpg";
				//string path = "C:\\Users\\yinmw\\Desktop\\picbinaryseg\\" + name;
				//imwrite(path, a[j]);
				imshow(name, a[j]);
			}
		}
		cvWaitKey(0);
		
	}
	return 0;
}