#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>

class SignRecogniserPedestrianCrossing
{
private:
	cv::Mat input, hsv, roi, output;
	cv::Mat blue, mask, maskOpened, white, whiteClosed;

	bool isFound;

	int counterToResetIsFound;
public:
	SignRecogniserPedestrianCrossing();
	~SignRecogniserPedestrianCrossing();

	cv::Mat start(cv::Mat &input, cv::Mat &output);
	void preprocessInput();
	void contoursFiltration();
	void conditionChecking();
	void drawContours();
	bool getIsFound();
	void resetIsFound();

	std::vector<std::vector<cv::Point>> contours, contoursSize, contoursTriangles, contoursSignTriangles, roiContours;
	std::vector<cv::Point> contoursPoint, roiContoursPoint;

	std::vector<cv::Rect> signRois;

	struct MySort {
		bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.y < pt2.y); }
	} mySort;
};

