#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


Mat getColorRangeImage(Mat);
vector<Point2f> findPointsWithTransfo(Mat source);
vector<Point2f> selectGoodPointsCam1(vector<Point> cnts_poly);

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

vector<Point2f> findPointsWithTransfo(Mat source)
{
	Mat hsv, proc_img, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2;
	string directory = "goodPictures";

	vector<vector<Point>> contours;
	vector<Point> cnts_poly;
	vector<Point2f> goodPoints;

	if (!source.empty())
	{

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
		goodPoints = selectGoodPointsCam1(cnts_poly);
	}
	return goodPoints;
}

vector<Point2f> selectGoodPointsCam1(vector<Point> cnts_poly)
{
	double MaxY = cnts_poly[0].y;
	double MinY = cnts_poly[0].y;
	double MinX = cnts_poly[0].x;
	double MaxX = cnts_poly[0].x;

	vector<Point2f> good_points;

	for (int i = 0; i < cnts_poly.size(); i++)
	{
		if (cnts_poly[i].y >= MaxY)
		{
			MaxY = cnts_poly[i].y;
		}
		if (cnts_poly[i].y <= MinY)
		{
			MinY = cnts_poly[i].y;
		}
		if (cnts_poly[i].x >= MaxX)
		{
			MaxX = cnts_poly[i].x;
		}
		if (cnts_poly[i].x <= MinX)
		{
			MinX = cnts_poly[i].x;
		}
	}

	double minx_local = cnts_poly[0].x;
	double maxx_local = cnts_poly[0].x;
	double index_p11 = 0;
	double index_p12 = 0;
	double index_p21 = 0;
	double index_p22 = 0;
	double index_p31 = 0;
	double index_p32 = 0;
	double max_width = 0;

	for (int i = 0; i < cnts_poly.size(); i++)
	{
		if (abs(cnts_poly[i].y - MaxY) < 10)
		{
			for (int j = 0; j < cnts_poly.size(); j++)
			{
				if ((abs(cnts_poly[j].y - MaxY) < 15) && (cnts_poly[j].x < minx_local))
				{
					minx_local = cnts_poly[j].x;
					index_p11 = j;
				}
				if ((abs(cnts_poly[j].y - MaxY) < 15) && (cnts_poly[j].x > maxx_local))
				{
					maxx_local = cnts_poly[j].x;
					index_p12 = j;
				}
			}
		}

		if (abs(cnts_poly[i].y - MinY) < 15)
		{
			for (int j = 0; j < cnts_poly.size(); j++)
			{
				if ((abs(cnts_poly[j].y - MinY) < 15) && (cnts_poly[j].x < minx_local))
				{
					minx_local = cnts_poly[j].x;
					index_p31 = j;
				}
				if ((abs(cnts_poly[j].y - MinY) < 15) && (cnts_poly[j].x > maxx_local))
				{
					maxx_local = cnts_poly[j].x;
					index_p32 = j;
				}
			}
		}

		for (int j = 0; j < cnts_poly.size(); j++)
		{
			if ((abs(cnts_poly[i].x - cnts_poly[j].x) > max_width) && (abs(cnts_poly[i].y -cnts_poly[j].y)<15)) 
			{
				max_width = abs(cnts_poly[i].x - cnts_poly[j].x);
				index_p21 = i;
				index_p22 = j;
			}
		}
		
	}
	good_points.push_back(Point2f(cnts_poly[index_p31].x, cnts_poly[index_p31].y));
	good_points.push_back(Point2f(cnts_poly[index_p32].x, cnts_poly[index_p32].y));

	good_points.push_back(Point2f(cnts_poly[index_p21].x, cnts_poly[index_p21].y));
	good_points.push_back(Point2f(cnts_poly[index_p22].x, cnts_poly[index_p22].y));
	good_points.push_back(Point2f(cnts_poly[index_p11].x, cnts_poly[index_p11].y));
	good_points.push_back(Point2f(cnts_poly[index_p12].x, cnts_poly[index_p12].y));

	return good_points;
}

vector<Point> selectGoodPointsCam2(vector<Point> cnts_poly)
{
	double MaxY = cnts_poly[0].y;
	double MinY = cnts_poly[0].y;
	double MinX = cnts_poly[0].x;
	double MaxX = cnts_poly[0].x;

	vector<Point> good_points;

	for (int i = 0; i < cnts_poly.size(); i++)
	{
		if (cnts_poly[i].y >= MaxY)
		{
			MaxY = cnts_poly[i].y;
		}
		if (cnts_poly[i].y <= MinY)
		{
			MinY = cnts_poly[i].y;
		}
		if (cnts_poly[i].x >= MaxX)
		{
			MaxX = cnts_poly[i].x;
		}
		if (cnts_poly[i].x <= MinX)
		{
			MinX = cnts_poly[i].x;
		}
	}

	double minx_local = cnts_poly[0].x;
	double maxx_local = cnts_poly[0].x;
	double index_p11 = 0;
	double index_p12 = 0;
	double index_p21 = 0;
	double index_p22 = 0;
	double max_width = 0;

	for (int i = 0; i < cnts_poly.size(); i++)
	{
		cout << MaxY << endl;
		if (abs(cnts_poly[i].y - MaxY) < 20)
		{
			for (int j = 0; j < cnts_poly.size(); j++)
			{
				if ((abs(cnts_poly[j].y - MaxY) < 15) && (cnts_poly[j].x < minx_local))
				{
					minx_local = cnts_poly[j].x;
					index_p11 = j;
				}
				if ((abs(cnts_poly[j].y - MaxY) < 15) && (cnts_poly[j].x > maxx_local))
				{
					maxx_local = cnts_poly[j].x;
					index_p12 = j;
				}
			}
		}

		if ((abs(cnts_poly[i].x - cnts_poly[index_p11].x) < 20) && (abs(cnts_poly[i].y - cnts_poly[index_p11].y) > 50))
		{
			index_p21 = i;
		}
		if ((abs(cnts_poly[i].x - cnts_poly[index_p12].x) < 20) && (abs(cnts_poly[i].y - cnts_poly[index_p12].y) > 50))
		{
			index_p22 = i;
		}
	}

	good_points.push_back(cnts_poly[index_p21]);
	good_points.push_back(cnts_poly[index_p22]);
	good_points.push_back(cnts_poly[index_p11]);
	good_points.push_back(cnts_poly[index_p12]);

	return good_points;
}

void work_on_points()
{
	for (int i = 0; i < 8; i++)
	{
		if (i != 5)
		{
			string image1 = "carton_" + to_string(i + 1) + "_pos1.jpg";
			vector<Point> foundPoints1;
			vector<Point> foundPoints2;
			foundPoints1 = findPointsNoTransfo(image1);

			string image2_string = "carton_" + to_string(i + 1) + "_pos2.jpg";
			Mat image2 = imread(image2_string);
			//foundPoints2 = findPointsWithTransfo(image2);

		}
	}
}

int treat_and_display_with_transfo()
{

	Mat source, hsv, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2;
	string file = "goodPictures/carton_1_pos2.jpg";

	string directory = "goodPictures";

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


	Mat proc_img = Mat(source.size(), CV_8UC3);

	inRange(source, Scalar(150, 130, 102), Scalar(215, 165, 120), proc_img);

	struct_element0 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(1, 1));
	struct_element1 = getStructuringElement(MORPH_RECT, Size(15, 15), Point(3, 3));

	struct_element2 = getStructuringElement(MORPH_RECT, Size(11, 11), Point(3, 3));

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

	vector<Point2f> gPoints;
	gPoints = selectGoodPointsCam1(cnts_poly);
	for (int j = 0; j < gPoints.size(); j++)
	{
		circle(dest2, gPoints[j], 5, Scalar(0, 255, 0), 3);
	}
	//circle(dest2, Point(230, 394), 5, Scalar(0, 255, 0), 3);

	imshow(w1, source);
	imshow(w2, proc_img);
	imshow(w3, canny);
	imshow(w4, dest1);
	imshow(w5, dest2);

	waitKey();
	return 0;

}

int treat_and_display_with_no_transfo()
{
	Mat source, hsv, proc_img, canny, dest1, dest2;
	Mat struct_element0, struct_element1, struct_element2, struct_element3;
	string directory = "goodPictures";
	string file = "goodPictures/carton_1_pos1.jpg";

	vector<vector<Point>> contours;
	vector<Point> cnts_poly;

	string w1 = "source";
	string w2 = "color range image";
	string w3 = "canny";
	string w4 = "segmented";
	string w5 = "contours poly";

	namedWindow(w1, WINDOW_AUTOSIZE);

	namedWindow(w2, WINDOW_AUTOSIZE);

	namedWindow(w3, WINDOW_AUTOSIZE);

	namedWindow(w4, WINDOW_AUTOSIZE);

	namedWindow(w5, WINDOW_AUTOSIZE);

	source = imread(file);


	if (!source.empty())
	{
		cvtColor(source, source, COLOR_BGR2YCrCb);
		proc_img = Mat(source.size(), CV_8UC3);

		inRange(source, Scalar(150, 110, 85), Scalar(240, 210, 130), proc_img);

		struct_element0 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(3, 3));

		struct_element1 = getStructuringElement(MORPH_RECT, Size(9, 9), Point(3, 3));

		struct_element2 = getStructuringElement(MORPH_RECT, Size(7, 7), Point(3, 3));
		struct_element3 = getStructuringElement(MORPH_RECT, Size(7, 7), Point(3, 3));

		//morphological operations to eliminate most noise on the box 
		morphologyEx(proc_img, proc_img, MORPH_CLOSE, struct_element0);

		dilate(proc_img, proc_img, struct_element2);
		erode(proc_img, proc_img, struct_element1);

		//dilate(proc_img, proc_img, struct_element3);
		
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
		vector<vector<Point>> cnts;
		vector<Point> cnts_poly;

		//this function approximate points of a polygon for the contour
		approxPolyDP(Mat(contours[indexMax]), cnts_poly, 3, true);
		cnts.push_back(cnts_poly);

		fillPoly(dest1, contours[indexMax], Scalar(255, 255, 255));
		imwrite("masks/maskscarton_5_pos1.jpg", dest1);

		drawContours(dest2, cnts, 0, Scalar(0, 0, 255), 5);

		//display the points of the polygons. This is what we use to estimate the dimensions

		vector<Point> gPoints;
		//gPoints = selectGoodPointsCam2(cnts_poly);
		for (int j = 0; j < cnts_poly.size(); j++)
		{
			circle(dest2, cnts_poly[j], 5, Scalar(0, 255, 0), 3);
			cout << cnts_poly[j];
		}
		//circle(dest2, Point(230, 394), 5, Scalar(0, 255, 0), 3);

		imshow(w1, source);
		imshow(w2, proc_img);
		imshow(w3, canny);
		imshow(w4, dest1);
		imshow(w5, dest2);

		waitKey();
		return 0;
	}
	return 0;
}

int homographyPlane1()
{
	Mat image1, image2, image3;
	image1 = imread("goodPictures/carton_1_pos1.jpg");
	image2 = imread("goodPictures/carton_1_pos2.jpg");
	image3 = imread("goodPictures/carton_3_pos1.jpg");

	//cropping image2
	Mat ROI(image2, Rect(0, 120, 640, 360));
	ROI.copyTo(image2);


	//To make the homography and calculate dimensions of plan 1
	//Because Homography is the same, you can keep the matrix for the boxes. Just initialize on the biggest Box.

	vector<Point3f> p1Points;

	vector<Point2f> img1_pts;
	vector<Point2f> img2_pts;

	img2_pts.push_back(Point2f(0, 0));

	img2_pts.push_back(Point2f(240, 0));
	img2_pts.push_back(Point2f(0, 160));
	img2_pts.push_back(Point2f(240, 160));

	img1_pts.push_back(Point2f(344, 209));
	img1_pts.push_back(Point2f(457, 197));
	img1_pts.push_back(Point2f(350, 287));
	img1_pts.push_back(Point2f(453, 306));

	Mat H = findHomography(img1_pts, img2_pts);
	cout << H;

	Mat img_wrp, img_wrp2;

	warpPerspective(image1, img_wrp, H, image1.size());
	warpPerspective(image3, img_wrp2, H, image3.size());

	imwrite("image_wrap.jpg", img_wrp);

	namedWindow("display 1");
	namedWindow("display 2");

	imshow("display 1", img_wrp2);
	imshow("display 2", img_wrp);

	waitKey();
	return 0;
}

void computeC2MC1(const Mat& R1, const Mat& tvec1, const Mat& R2, const Mat& tvec2,
	Mat& R_1to2, Mat& tvec_1to2)
{
	//c2Mc1 = c2Mo * oMc1 = c2Mo * c1Mo.inv()
	R_1to2 = R2 * R1.t();
	tvec_1to2 = R2 * (-R1.t() * tvec1) + tvec2;
}

int homograpyPlane2()
{
	Mat image1, image2, image3;
	image1 = imread("goodPictures/carton_1_pos1.jpg");
	image2 = imread("goodPictures/carton_1_pos2.jpg");
	image3 = imread("goodPictures/carton_3_pos1.jpg");

	//cropping image2
	Mat ROI(image2, Rect(0, 120, 640, 360));
	ROI.copyTo(image2);

	Mat cameraMatrix = (Mat_<float>(3, 3) << 605.57, 0, 334.90, 0, 591.46, 238.77, 0, 0, 1);
	//To make the homography and calculate dimensions of plan 1
	//Because Homography is the same, you can keep the matrix for the boxes. Just initialize on the biggest Box.

	vector<Point3f> p1Points;

	vector<Point2f> img1_pts;
	vector<Point2f> img2_pts;

	img2_pts.push_back(Point2f(0, 0));

	img2_pts.push_back(Point2f(160, 0));
	img2_pts.push_back(Point2f(0, 120));
	img2_pts.push_back(Point2f(160, 120));

	img1_pts.push_back(Point2f(457, 197));
	img1_pts.push_back(Point2f(587, 198));
	img1_pts.push_back(Point2f(453, 306));
	img1_pts.push_back(Point2f(590, 283));


	Mat H = findHomography(img1_pts, img2_pts);
	cout << H;

	Mat img_wrp, img_wrp2;

	warpPerspective(image3, img_wrp, H, image1.size());
	//warpPerspective(image3, img_wrp2, H, image3.size());

	imwrite("image_wrap.jpg", img_wrp);

	namedWindow("display 1");
	namedWindow("display 2");

	imshow("display 1", image3);
	imshow("display 2", img_wrp);

	waitKey();
	//homographyPlane1();

	cout << "Execution complete" << endl;

	//treat_and_display_with_no_transfo();

	return 0;
}

static void mouse_call(int event, int x, int y, int, void* param)
{
	if (event == cv::EVENT_LBUTTONDOWN) {
		std::vector<cv::Point2f>* ptPtr = (std::vector<cv::Point2f>*)param;

		ptPtr->push_back(cv::Point2f(x, y));

	}
}

Mat computeHomography(const Mat& R_1to2, const Mat& tvec_1to2, const double d_inv, const Mat& normal)
{
	Mat homography = R_1to2 + d_inv * tvec_1to2 * normal.t();
	return homography;
}

int findHomographyAndDisplay()
{

	Mat image1, image2, image3;
	image1 = imread("goodPictures/carton_1_pos1.jpg");
	image2 = imread("goodPictures/carton_1_pos2.jpg");
	image3 = imread("goodPictures/carton_3_pos1.jpg");

	//cropping image2
	Mat ROI(image2, Rect(0, 120, 640, 360));
	ROI.copyTo(image2);

	//To make the homography and calculate dimensions of plan 1
	//Because Homography is the same, you can keep the matrix for the boxes. Just initialize on the biggest Box.

	vector<Point3f> p1Points;

	vector<Point2f> img1_pts;
	vector<Point2f> img2_pts;

	img2_pts.push_back(Point2f(0, 0));

	img2_pts.push_back(Point2f(59 * 4, 0));
	img2_pts.push_back(Point2f(0, (29.5 - 4.5) * 4));
	img2_pts.push_back(Point2f(59 * 4, (29.5 - 4.5) * 4));

	namedWindow("select points");
	setMouseCallback("select points", mouse_call, (void*)&img1_pts);

	imshow("select points", image3);
	waitKey();

	cout << img1_pts;

	for (int i = 0; i < img1_pts.size(); i++)
	{
		circle(image3, img1_pts[i], 3, Scalar(0, 255, 0), 2);
	}

	namedWindow("with points");
	imshow("with points", image3);
	waitKey();

	FileStorage fs("out_camera_pos1_data.xml", FileStorage::READ);

	if (fs.isOpened())
	{
		cout << "File is opened\n";
	}
	Mat cameraMatrix, distCoeffs;

	fs["Camera_Matrix"] >> cameraMatrix;

	cout << cameraMatrix << endl;

	fs["Distortion_Coefficients"] >> distCoeffs;

	vector<Point3f> objectPoints;
	objectPoints.push_back(Point3f(0, 0, 0));
	objectPoints.push_back(Point3f(39, 0, 0));
	objectPoints.push_back(Point3f(0, 29.5, 0));
	objectPoints.push_back(Point3f(39, 29.5, 0));

	Mat rvec1, tvec1;
	solvePnP(objectPoints, img1_pts, cameraMatrix, distCoeffs, rvec1, tvec1);
	Mat rvec2, tvec2;
	solvePnP(objectPoints, img2_pts, cameraMatrix, distCoeffs, rvec2, tvec2);

	cout << "R : " << rvec1 << endl;
	cout << "T : " << tvec1 << endl;

	Mat R1, R2, R_1to2, tvec_1to2;
	Rodrigues(rvec1, R1);
	Rodrigues(rvec2, R2);

	computeC2MC1(R1, tvec1, R2, tvec2, R_1to2, tvec_1to2);

	//Mat normal = (Mat(3, 1, CV_64F, { 0,0,1 }));
	Mat normal = (Mat_<double>(3, 1) << 0, 0, 1);

	Mat normal1;
	normal1 = R1 * normal;

	Mat origin(3, 1, CV_64F, Scalar(0));
	Mat origin1 = R1 * origin + tvec1;
	double d_inv1 = 1.0 / normal1.dot(origin1);

	Mat H, H_euclidian;
	H_euclidian = computeHomography(R1, tvec1, d_inv1, normal1);

	H = cameraMatrix * H_euclidian * cameraMatrix.inv();

	//Mat H = findHomography(img1_pts, img2_pts);

	Mat img_wrp, img_wrp2;

	warpPerspective(image3, img_wrp2, H, image1.size());
	//warpPerspective(image1, img_wrp, H, image1.size());

	//imwrite("image_wrap.jpg", img_wrp);

	namedWindow("display 1");
	//namedWindow("display 2");

	imshow("display 1", img_wrp2);
	//imshow("display 2", img_wrp);

	waitKey();
	return 0;
}

int Homography1to2()
{
	Mat image1, image2, image3, image4;
	image1 = imread("goodPictures/carton_3_pos1.jpg");
	image2 = imread("goodPictures/carton_3_pos2.jpg");
	image3 = imread("goodPictures/carton_2_pos2.jpg");
	image4 = imread("goodPictures/carton_2_pos1.jpg");

	//cropping image2
	Mat ROI(image2, Rect(0, 120, 640, 360));
	ROI.copyTo(image2);

	Mat ROI2(image3, Rect(0, 120, 640, 360));
	ROI2.copyTo(image3);


	//To make the homography and calculate dimensions of plan 1
	//Because Homography is the same, you can keep the matrix for the boxes. Just initialize on the biggest Box

	vector<Point2f> img1_pts;
	vector<Point2f> img2_pts;
	vector<Point2f> img3_pts;

	namedWindow("select points pos1");
	setMouseCallback("select points pos1", mouse_call, (void*)&img1_pts);

	imshow("select points pos1", image1);
	waitKey();

	namedWindow("select points pos2");
	setMouseCallback("select points pos2", mouse_call, (void*)&img2_pts);

	imshow("select points pos2", image2);
	waitKey();

	cout << img1_pts;
	cout << img2_pts;

	for (int i = 0; i < img1_pts.size(); i++)
	{
		circle(image1, img1_pts[i], 3, Scalar(0, 255, 0), 2);
	}
	for (int i = 0; i < img2_pts.size(); i++)
	{
		circle(image2, img2_pts[i], 3, Scalar(0, 255, 0), 2);
	}

	namedWindow("with points pos1");
	namedWindow("with points pos2");

	imshow("with points pos1", image1);

	imshow("with points pos2", image2);
	waitKey();

	Mat H = findHomography(img2_pts, img1_pts);

	cout << H;

	cv::FileStorage file("homography2to1.xml", cv::FileStorage::WRITE);
	file << "H" << H;
	file.release();

	namedWindow("add points");
	setMouseCallback("add points", mouse_call, (void*)&img3_pts);
	imshow("add points", image3);
	waitKey();

	cout << img3_pts;
	cout << endl;
	for (int i = 0; i < img3_pts.size(); i++)
	{
		circle(image3, img3_pts[i], 3, Scalar(0, 0, 255), 2);
		Mat pt1 = (Mat_<double>(3, 1) << img3_pts[i].x, img3_pts[i].y, 1);
		Mat pt2 = H * pt1;
		pt2 /= pt2.at<double>(2);
		Point2f end((int)(pt2.at<double>(0)), (int)pt2.at<double>(1));
		cout << end;
		circle(image4, end, 3, Scalar(0, 0, 255), 2);
	}

	cout << endl;
	namedWindow("added points");
	imshow("added points", image3);
	waitKey();

	namedWindow("result");
	imshow("result", image4);
	waitKey();

	return 0;
}

int HomographyDistance()
{
	Mat image1, image2, image3, image4;
	image1 = imread("goodPictures/carton_9_pos1.jpg");
	//image2 = imread("goodPictures/carton_3_pos2.jpg");
	//image3 = imread("goodPictures/carton_2_pos2.jpg");
	//image4 = imread("goodPictures/carton_2_pos1.jpg");

	//cropping image2
	/**
	Mat ROI(image2, Rect(0, 120, 640, 360));
	ROI.copyTo(image2);

	Mat ROI2(image3, Rect(0, 120, 640, 360));
	ROI2.copyTo(image3);
	**/

	//To make the homography and calculate dimensions of plan 1
	//Because Homography is the same, you can keep the matrix for the boxes. Just initialize on the biggest Box

	vector<Point2f> img1_pts;
	vector<Point2f> img2_pts;
	vector<Point3f> obj_pts;
	img2_pts.push_back(Point2f(0, 360));
	img2_pts.push_back(Point2f(61, 360));
	img2_pts.push_back(Point2f(0, 360 - 57));
	img2_pts.push_back(Point2f(61, 360 - 57));

	obj_pts.push_back(Point3f(0, 0, 0));
	obj_pts.push_back(Point3f(30.5, 0, 0));
	obj_pts.push_back(Point3f(0, 28.5, 0));
	obj_pts.push_back(Point3f(30.5, 28.5, 0));

	/**
	works carton 3
		img2_pts.push_back(Point2f(29, 360 -10));
	img2_pts.push_back(Point2f(58, 360-10));
	img2_pts.push_back(Point2f(29, 360 - 50));
	img2_pts.push_back(Point2f(58, 360 - 50));

	obj_pts.push_back(Point3f(0, 0, 0));
	obj_pts.push_back(Point3f(58, 0, 0));
	obj_pts.push_back(Point3f(0, 40, 0));
	obj_pts.push_back(Point3f(58, 40, 0));
	**/
 
	namedWindow("select points pos1");
	setMouseCallback("select points pos1", mouse_call, (void*)&img1_pts);

	imshow("select points pos1", image1);
	waitKey();
	cout << img1_pts;

	for (int i = 0; i < img1_pts.size(); i++)
	{
		circle(image1, img1_pts[i], 3, Scalar(0, 255, 0), 2);
	}

	namedWindow("with points pos1");

	imshow("with points pos1", image1);

	waitKey();

	FileStorage fs1("out_camera_pos1_data.xml", FileStorage::READ);
	FileStorage fs2("out_camera_pos2_data.xml", FileStorage::READ);

	if (fs1.isOpened() && fs2.isOpened())
	{
		cout << "Files are opened\n";
	}
	Mat cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2;

	fs1["Camera_Matrix"] >> cameraMatrix1;
	fs2["Camera_Matrix"] >> cameraMatrix2;

	fs1["Distortion_Coefficients"] >> distCoeffs1;
	fs2["Distortion_Coefficients"] >> distCoeffs2;

	Mat rvec1, tvec1;
	solvePnP(obj_pts, img1_pts, cameraMatrix1, distCoeffs1, rvec1, tvec1);
	Mat rvec2, tvec2;
	solvePnP(obj_pts, img2_pts, cameraMatrix2, distCoeffs2, rvec2, tvec2);

	Mat R1, R2, R_1to2, tvec_1to2;
	Rodrigues(rvec1, R1);
	Rodrigues(rvec2, R2);

	computeC2MC1(R1, tvec1, R2, tvec2, R_1to2, tvec_1to2);

	//Mat normal = (Mat(3, 1, CV_64F, { 0,0,1 }));
	Mat normal = (Mat_<double>(3, 1) << 0, 0, 1);

	Mat normal1;
	normal1 = R1 * normal;

	Mat origin(3, 1, CV_64F, Scalar(0));
	Mat origin1 = R1 * origin + tvec1;
	double d_inv1 = 1.0 / normal1.dot(origin1);

	Mat H, H_euclidian;
	H_euclidian = computeHomography(R_1to2, tvec_1to2, d_inv1, normal1);

	Mat cameraMatrix;
	cameraMatrix = (cameraMatrix1 + cameraMatrix2) / 2;

	H = cameraMatrix * H_euclidian * cameraMatrix.inv();

	//Mat H = findHomography(img1_pts, img2_pts);

	Mat img_wrp, img_wrp2;

	warpPerspective(image1, img_wrp2, H, image1.size());
	//warpPerspective(image1, img_wrp, H, image1.size());

	//imwrite("image_wrap.jpg", img_wrp);

	namedWindow("display 1");
	//namedWindow("display 2");

	imshow("display 1", img_wrp2);

	waitKey();

	return 0;
}

int HomographyPlan2()
{
	Mat image1, image2, image3, image4;
	image1 = imread("goodPictures/carton_9_pos1.jpg");
	//image2 = imread("goodPictures/carton_3_pos2.jpg");
	//image3 = imread("goodPictures/carton_2_pos2.jpg");
	image2 = imread("goodPictures/carton_2_pos1.jpg");

	//cropping image2
	/**
	Mat ROI(image2, Rect(0, 120, 640, 360));
	ROI.copyTo(image2);

	Mat ROI2(image3, Rect(0, 120, 640, 360));
	ROI2.copyTo(image3);
	**/

	//To make the homography and calculate dimensions of plan 1
	//Because Homography is the same, you can keep the matrix for the boxes. Just initialize on the biggest Box

	vector<Point2f> img1_pts;
	vector<Point2f> img2_pts;
	vector<Point3f> obj_pts;
	img2_pts.push_back(Point2f(0, 360 - 4.5));
	img2_pts.push_back(Point2f(81, 360));
	img2_pts.push_back(Point2f(0, 360 - 57));
	img2_pts.push_back(Point2f(81, 360 - 57));

	obj_pts.push_back(Point3f(0, 4.5, 11));
	obj_pts.push_back(Point3f(40.5, 0, 11));
	obj_pts.push_back(Point3f(0, 28.5, 11));
	obj_pts.push_back(Point3f(40.5, 28.5, 11));

	/**
	works carton 3
	img2_pts.push_back(Point2f(29, 360 -10));
	img2_pts.push_back(Point2f(58, 360-10));
	img2_pts.push_back(Point2f(29, 360 - 50));
	img2_pts.push_back(Point2f(58, 360 - 50));

	obj_pts.push_back(Point3f(0, 0, 0));
	obj_pts.push_back(Point3f(58, 0, 0));
	obj_pts.push_back(Point3f(0, 40, 0));
	obj_pts.push_back(Point3f(58, 40, 0));
	**/

	namedWindow("select points pos1");
	setMouseCallback("select points pos1", mouse_call, (void*)&img1_pts);

	imshow("select points pos1", image1);
	waitKey();

	for (int i = 0; i < img1_pts.size(); i++)
	{
		circle(image1, img1_pts[i], 3, Scalar(0, 255, 0), 2);
		cout << img1_pts[i] << endl;
	}

	namedWindow("with points pos1");

	imshow("with points pos1", image1);

	waitKey();

	FileStorage fs1("out_camera_pos1_datamore.xml", FileStorage::READ);
	FileStorage fs2("out_camera_pos2_data.xml", FileStorage::READ);

	if (fs1.isOpened() && fs2.isOpened())
	{
		cout << "Files are opened\n";
	}
	Mat cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2;

	fs1["Camera_Matrix"] >> cameraMatrix1;
	fs2["Camera_Matrix"] >> cameraMatrix2;

	fs1["Distortion_Coefficients"] >> distCoeffs1;
	fs2["Distortion_Coefficients"] >> distCoeffs2;

	Mat rvec1, tvec1;
	solvePnP(obj_pts, img1_pts, cameraMatrix1, distCoeffs1, rvec1, tvec1);
	Mat rvec2, tvec2;
	solvePnP(obj_pts, img2_pts, cameraMatrix1, distCoeffs1, rvec2, tvec2);

	Mat R1, R2, R_1to2, tvec_1to2;
	Rodrigues(rvec1, R1);
	Rodrigues(rvec2, R2);

	computeC2MC1(R1, tvec1, R2, tvec2, R_1to2, tvec_1to2);

	//Mat normal = (Mat(3, 1, CV_64F, { 0,0,1 }));
	Mat normal = (Mat_<double>(3, 1) << 0, 0, 1);

	Mat normal1;
	normal1 = R1 * normal;

	Mat origin(3, 1, CV_64F, Scalar(0));
	Mat origin1 = R1 * origin + tvec1;
	double d_inv1 = 1.0 / normal1.dot(origin1);

	Mat H, H_euclidian;
	H_euclidian = computeHomography(R_1to2, tvec_1to2, d_inv1, normal1);

	Mat cameraMatrix;
	cameraMatrix = (cameraMatrix1 + cameraMatrix2) / 2;

	H = cameraMatrix1 * H_euclidian * cameraMatrix1.inv();

	/**
	vector<Point2f> img2_pts;
	//calcul de homographie avec les points du plan, selectionner les points sur carton 2 et reporter gr�ce � l'homographie dans le plan
	namedWindow("select points carton2");
	setMouseCallback("select points carton2", mouse_call, (void*)&img2_pts);

	imshow("select points carton2", image2);
	waitKey();
	cout << img2_pts;

	for (int i = 0; i < img2_pts.size(); i++)
	{
		circle(image1, img2_pts[i], 3, Scalar(0, 255, 0), 2);
	}

	namedWindow("with points carton2");

	imshow("with points carton2", image2);

	waitKey();
	Mat pt1 = (Mat_<double>(3, 1) << img1_pts[1].x, img1_pts[1].y, 1);

	Mat pt2 = H * pt1;
	pt2 /= pt2.at<double>(2);
	Point2f end((int)(pt2.at<double>(0)), (int)pt2.at<double>(1));
	cout << end;
	**/
	//Mat H = findHomography(img1_pts, img2_pts);

	Mat img_wrp, img_wrp2;

	warpPerspective(image2, img_wrp2, H, image2.size());
	//warpPerspective(image1, img_wrp, H, image1.size());

	//imwrite("image_wrap.jpg", img_wrp);

	namedWindow("display 1");
	//namedWindow("display 2");

	imshow("display 1", img_wrp2);

	waitKey();

	return 0;
}

int calculateHomography1to2Mean()
{
	Mat image1, image2, image3, image4;
	Mat m_Homography = Mat::zeros(Size(3, 3), CV_64F);

	vector<Point2f> img1_pts;
	vector<Point2f> img2_pts;
	vector<Point2f> img3_pts;
	vector<Point2f> img4_pts;
	String w1name;
	String w2name;

	for (int i = 1; i < 9; i++)
	{
		if (i != 6)
		{
			String file1 = "goodPictures/carton_" + to_string(i) + "_pos1.jpg";
			String file2 = "goodPictures/carton_" + to_string(i) + "_pos2.jpg";
			image1 = imread(file1);
			image2 = imread(file2);

			//cropping image2
			Mat ROI(image2, Rect(0, 120, 640, 360));
			ROI.copyTo(image2);
			Mat H_temp = Mat::zeros(Size(3, 3), CV_64F);

			w1name = "image n� " + to_string(i) + "select points pos1";
			namedWindow(w1name);
			setMouseCallback(w1name, mouse_call, (void*)&img1_pts);

			imshow(w1name, image1);
			waitKey();

			w2name = "image n� " + to_string(i) + "select points pos2";
			namedWindow(w2name);
			setMouseCallback(w2name, mouse_call, (void*)&img2_pts);

			imshow(w2name, image2);
			waitKey();

			H_temp = findHomography(img2_pts, img1_pts);
			add(H_temp, m_Homography, m_Homography);
		}
	}
	m_Homography /= 8;

	image3 = imread("goodPictures/carton_3_pos2.jpg");
	image4 = imread("goodPictures/carton_3_pos1.jpg");

	Mat ROI2(image3, Rect(0, 120, 640, 360));
	ROI2.copyTo(image3);

	//To make the homography and calculate dimensions of plan 1
	//Because Homography is the same, you can keep the matrix for the boxes. Just initialize on the biggest Box

	cv::FileStorage file("homographyMean2to1.xml", cv::FileStorage::WRITE);
	file << "H_mean" << m_Homography;
	file.release();

	namedWindow("add points");
	setMouseCallback("add points", mouse_call, (void*)&img3_pts);
	imshow("add points", image3);
	waitKey();

	for (int i = 0; i < img3_pts.size(); i++)
	{
		circle(image3, img3_pts[i], 3, Scalar(0, 0, 255), 2);
		Mat pt1 = (Mat_<double>(3, 1) << img3_pts[i].x, img3_pts[i].y, 1);
		Mat pt2 = m_Homography * pt1;
		pt2 /= pt2.at<double>(2);
		Point2f end((int)(pt2.at<double>(0)), (int)pt2.at<double>(1));
		circle(image4, end, 3, Scalar(0, 0, 255), 2);
	}

	cout << endl;
	namedWindow("added points");
	imshow("added points", image3);
	waitKey();

	namedWindow("result");
	imshow("result", image4);
	waitKey();

	return 0;
}

int main()
{
	Mat image1, image2, image3, image4;
	Mat m_Homography = Mat::zeros(Size(3, 3), CV_64F);

	vector<Point2f> img1_pts;
	vector<Point2f> img2_pts;
	vector<Point2f> img3_pts;
	vector<Point2f> img4_pts;
	String w1name;
	String w2name;

	for (int i = 1; i < 8; i++)
	{
		if (i != 6)
		{
			String file1 = "goodPictures/carton_" + to_string(i) + "_pos2.jpg";
			image1 = imread(file1);

			Mat ROI2(image1, Rect(0, 120, 640, 360));
			ROI2.copyTo(image1);

			w1name = "image n� " + to_string(i) + "select points pos2";
			namedWindow(w1name);
			setMouseCallback(w1name, mouse_call, (void*)&img1_pts);

			imshow(w1name, image1);
			waitKey();

			double l, h;
			Mat H_temp = Mat::zeros(Size(3, 3), CV_64F);

			cout << "carton n�" + to_string(i) + " : " << endl;
			cout << "l : ";
			cin >> l;
			cout << endl;
			cout << "H :";
			cin >> h;
			cout << endl;

			img2_pts.push_back(Point2f(0, 360));
			img2_pts.push_back(Point2f(4*l, 360));
			img2_pts.push_back(Point2f(0, 360 - 4*h));
			img2_pts.push_back(Point2f(4*l, 360 - 4*h));

			H_temp = findHomography(img1_pts, img2_pts);
			add(H_temp, m_Homography, m_Homography);
		}
	}
	m_Homography /= 6;

	image4 = imread("goodPictures/carton_3_pos2.jpg");

	//To make the homography and calculate dimensions of plan 1
	//Because Homography is the same, you can keep the matrix for the boxes. Just initialize on the biggest Box

	cv::FileStorage file("Hv1_2_mean.xml", cv::FileStorage::WRITE);
	file << "H_mean" << m_Homography;
	file.release();

	warpPerspective(image4, image3, m_Homography, image4.size());

	namedWindow("result");
	imshow("result", image3);
	waitKey();

	return 0;
}

