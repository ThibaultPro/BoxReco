#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


Mat getColorRangeImage(Mat);


/**
Mat getColorRangeImage(Mat color_img)
{
	Mat range_img = Mat(color_img.size(), CV_8UC3);

	inRange(color_img, Scalar(80, 130, 150), Scalar(210, 215, 230), range_img);

	return range_img;
}**/
Mat getColorRangeImage(Mat color_img)
{
	Mat range_img = Mat(color_img.size(), CV_8UC3);

	inRange(color_img, Scalar(150, 130, 102), Scalar(215, 165, 120), range_img);

	return range_img;
}

vector<Point> findPointsNoTransfo(string image)
{
	Mat source, hsv, proc_img, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2;
	string directory = "goodPictures";

	vector<vector<Point>> contours;
	vector<Point> cnts_poly;

	if (!source.empty())
	{

		proc_img = Mat(source.size(), CV_8UC3);

		inRange(source, Scalar(80, 130, 150), Scalar(210, 215, 230), proc_img);


		struct_element0 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));
		struct_element1 = getStructuringElement(MORPH_RECT, Size(15, 15), Point(3, 3));

		struct_element2 = getStructuringElement(MORPH_RECT, Size(12, 12), Point(3, 3));

		//morphological operations to eliminate most noise on the box 
		morphologyEx(proc_img, proc_img, MORPH_CLOSE, struct_element0);

		dilate(proc_img, proc_img, struct_element2);

		erode(proc_img, proc_img, struct_element1);
		Canny(proc_img, canny, 100, 200);

		findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1);
		dest1 = Mat::zeros(source.size(), CV_8UC3);
		dest2 = Mat::zeros(source.size(), CV_8UC3);

		cv::Rect rect = boundingRect(contours[0]);
		double area = rect.area();
		double maxArea = area;
		int indexMax = 0;

		for (int i = 0; i < contours.size(); i++)
		{
			//the area of the boundingRect is a better estimation than contourArea
			rect = boundingRect(contours[i]);
			area = rect.area();
			if (area > maxArea)
			{
				indexMax = i;
				maxArea = area;

			}
		}

		//this function approximate points of a polygon for the contour
		approxPolyDP(Mat(contours[indexMax]), cnts_poly, 3, true);
	}

	return cnts_poly;
}

vector<Point> findPointsWithTransfo(string image)
{
	Mat source, hsv, proc_img, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2;
	string directory = "goodPictures";

	vector<vector<Point>> contours;
	vector<Point> cnts_poly;

	if (!source.empty())
	{
		cerr << "couldn't open the image";


		cvtColor(source, source, COLOR_BGR2YCrCb); //2Luv, 2XYZ, 2YUV

		proc_img = Mat(source.size(), CV_8UC3);

		inRange(source, Scalar(150, 130, 102), Scalar(215, 165, 120), proc_img);


		struct_element0 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));
		struct_element1 = getStructuringElement(MORPH_RECT, Size(15, 15), Point(3, 3));

		struct_element2 = getStructuringElement(MORPH_RECT, Size(12, 12), Point(3, 3));

		//morphological operations to eliminate most noise on the box 
		morphologyEx(proc_img, proc_img, MORPH_CLOSE, struct_element0);

		dilate(proc_img, proc_img, struct_element2);

		erode(proc_img, proc_img, struct_element1);
		Canny(proc_img, canny, 100, 200);

		findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1);
		dest1 = Mat::zeros(source.size(), CV_8UC3);
		dest2 = Mat::zeros(source.size(), CV_8UC3);

		cv::Rect rect = boundingRect(contours[0]);
		double area = rect.area();
		double maxArea = area;
		int indexMax = 0;

		for (int i = 0; i < contours.size(); i++)
		{
			//the area of the boundingRect is a better estimation than contourArea
			rect = boundingRect(contours[i]);
			area = rect.area();
			if (area > maxArea)
			{
				indexMax = i;
				maxArea = area;

			}
		}

		//this function approximate points of a polygon for the contour
		approxPolyDP(Mat(contours[indexMax]), cnts_poly, 3, true);
	}
	return cnts_poly;
}

int main()
{
	Mat source, hsv, proc_img, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2;
	string file = "goodPictures/carton_5_pos2.jpg";

	string directory = "goodPictures"; 
	
	for (int i = 0; i < 8; i++)
	{
		if (i != 5)
		{
			string image1 = "carton_" + to_string(i+1) + "_pos1.jpg";
			vector<Point> foundPoints1, foundPoints2;
			foundPoints1 = findPointsNoTransfo(image1);

			string image2 = "carton_" + to_string(i + 1) + "_pos2.jpg";
			foundPoints2 = findPointsWithTransfo(image2);
		}
	}

	string w1 = "source";
	string w2 = "color range image";
	string w3 = "canny";
	string w4 = "segmented";
	string w5 = "contours poly";

	vector<vector<Point>> contours;
	RNG rng(1);

	namedWindow(w1, WINDOW_AUTOSIZE);

	namedWindow(w2, WINDOW_AUTOSIZE);

	namedWindow(w3, WINDOW_AUTOSIZE);

	namedWindow(w4, WINDOW_AUTOSIZE);

	namedWindow(w5, WINDOW_AUTOSIZE);


	source = imread(file);

	if (source.empty())
	{
		cerr << "couldn't open the image";
		return 0;
	}

	cvtColor(source, source, COLOR_BGR2YCrCb); //2Luv, 2XYZ, 2YUV


	proc_img = getColorRangeImage(source);

	struct_element0 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));
	struct_element1 = getStructuringElement(MORPH_RECT, Size(15, 15), Point(3, 3));

	struct_element2 = getStructuringElement(MORPH_RECT, Size(12, 12), Point(3, 3));

	//morphological operations to eliminate most noise on the box 
	morphologyEx(proc_img, proc_img, MORPH_CLOSE, struct_element0);

	dilate(proc_img, proc_img, struct_element2);

	erode(proc_img, proc_img, struct_element1);
	Canny(proc_img, canny, 100, 200);

	findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_TC89_L1);
	dest1 = Mat::zeros(source.size(), CV_8UC3);
	dest2 = Mat::zeros(source.size(), CV_8UC3);

	cv::Rect rect = boundingRect(contours[0]);
	double area = rect.area();
	double maxArea = area;
	int indexMax = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

		//drawContours(dest1, contours, (int)i, color, 2);

		//the area of the boundingRect is a better estimation than contourArea
		rect = boundingRect(contours[i]);
		area = rect.area();
		if (area > maxArea)
		{
			indexMax = i;
			maxArea = area;

		}
	}

	vector<vector<Point>> cnts;
	vector<Point> cnts_poly;

	//this function approximate points of a polygon for the contour
	approxPolyDP(Mat(contours[indexMax]), cnts_poly, 3, true);
	cnts.push_back(cnts_poly);

	fillPoly(dest1, contours[indexMax], Scalar(255, 255, 255));

	drawContours(dest2, cnts, 0, Scalar(0, 0, 255), 5);

	//cornerHarris(dest2, corners, 2, 3, 0.04);
	//goodFeaturesToTrack(dest2, corners, 1, 0.5, 50, noArray(), 3, true);
	
	//display the points of the polygons. This is what we use to estimate the dimensions
	for (int j = 0; j < cnts_poly.size(); j++)
	{
		circle(dest2, cnts_poly[j], 5, Scalar(0, 255, 0), 3);
	}

	imshow(w1, source);
	imshow(w2, proc_img);
	imshow(w3, canny);
	imshow(w4, dest1);
	imshow(w5, dest2);

	waitKey();

	return 0;
}