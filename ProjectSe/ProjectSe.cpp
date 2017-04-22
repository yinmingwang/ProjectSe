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
vector<Mat> horizontalProjectionMat(Mat srcImg)//水平投影  
{
	Mat binImg = srcImg;
	int perPixelValue = 0;
	int width = srcImg.cols;
	int height = srcImg.rows;
	int* projectValArry = new int[height]; 
	memset(projectValArry, 0, height * 4);
	for (int col = 0; col < height; col++)
	{
		for (int row = 0; row < width; row++)
		{
			perPixelValue = binImg.at<uchar>(col, row);
			if (perPixelValue == 255)
			{
				projectValArry[col]++;
			}
		}
	}
	vector<Mat> roiList;
	int startIndex = 0;
	int endIndex = 0;
	bool inBlock = false;
	for (int i = 0; i <srcImg.rows; i++)
	{
		if (!inBlock && projectValArry[i] != 0)  
		{
			inBlock = true;
			startIndex = i;
		}
		else if (inBlock && projectValArry[i] == 0)
		{
			endIndex = i;
			inBlock = false;
			if (endIndex - startIndex > 4) {
				Mat roiImg = srcImg(Range(startIndex, endIndex + 1), Range(0, srcImg.cols));
				Mat timg = paint(roiImg);
				Mat img;
				resize(timg, img, Size(32, 32), 0, 0, CV_INTER_LINEAR);
				roiList.push_back(img);
			}
		}
	}
	delete[] projectValArry;
	return roiList;
}
vector<Mat> verticalProjectionMat(Mat srcImg)//垂直投影  
{
	Mat binImg = srcImg;
	//imshow("src", srcImg);
	int perPixelValue;
	int width = srcImg.cols;
	int height = srcImg.rows;
	int* projectValArry = new int[width];
	memset(projectValArry, 0, width * 4); 
	for (int col = 0; col < width; col++)
	{
		for (int row = 0; row < height; row++)
		{
			perPixelValue = binImg.at<uchar>(row, col);
			if (perPixelValue == 255)  
			{
				projectValArry[col]++;
			}
		}
	}
	vector<Mat> roiList;
	int startIndex = 0;
	int endIndex = 0; 
	bool inBlock = false; 
	for (int i = 0; i < srcImg.cols; i++)
	{
		if (!inBlock && projectValArry[i] != 0) 
		{
			inBlock = true;
			startIndex = i;
		}
		else if (projectValArry[i] == 0 && inBlock)
		{
			endIndex = i;
			inBlock = false;
			if (endIndex - startIndex > 6) {
				Mat roiImg = srcImg(Range(0, srcImg.rows), Range(startIndex, endIndex + 1));
				roiList.push_back(roiImg);
			}
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
Mat  filter(Mat srcimage, int size, int num) {
	for (int i = size / 2; i < srcimage.rows - size / 2; i++) {
		for (int j = size / 2; j <srcimage.cols - size / 2; j++) {
			int count = 0;
			for (int f = -size / 2; f <= size / 2; f++) {
				for (int g = -size / 2; g <= size / 2; g++) {
					if (srcimage.at<uchar>(i + f, j + g) > 200) {
						count++;
					}
				}
			}
			if (count < num) {
				srcimage.at<uchar>(i, j) = 0;
			}
		}
	}
	return srcimage;
}
int main()
{
	char buffer[40];
	for (r = 1; r < 20000; r++) {
		sprintf(buffer, "C:\\Users\\yinmw\\Desktop\\zp\\%d.jpg", r);
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
		Mat Mimage = Dilation(Mmimage, 2);
		//Mat s = filter(binimage, 5, 5);
		//medianBlur(s, Mimage, 3);
		//Mimage = Erosion(Mimage, 1);
		//Mimage = Dilation(Mimage, 1);
		vector<Mat> b = verticalProjectionMat(Mimage);//先进行垂直投影
		for (int i = 0; i < b.size(); i++)
		{
			stringstream streami;
			string  stri;
			streami << i;
			streami >> stri;
			vector<Mat> a = horizontalProjectionMat(b[i]);//水平投影  
			for (int j = 0; j < a.size(); j++)
			{
				stringstream streamj;
				string  strj;
				streamj << j;
				streamj >> strj;
				string name = str + "_"+ stri + "_" + strj + ".jpg";
				string path = "C:\\Users\\yinmw\\Desktop\\picbinaryseg\\" + name;
				imwrite(path, a[j]);
				//imshow(name, a[j]);
			}
		}
		//cvWaitKey(0);
		
	}
	return 0;
}